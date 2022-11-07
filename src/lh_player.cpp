#include "lh_player.h"
#include "lh_input.h"
#include "lh_static_entity.h"
#include "lh_memory.h"

internal
void CameraInitialize(Camera *camera, vec3 position, f32 fov, f32 aspect) {
    camera->position = position;
    camera->front = {0, 0, 1};
    camera->up    = {0, 1, 0};
    camera->right = {1, 0, 0};
    camera->pitch = 0.0f;
    camera->yaw = RAD(90.0f);
    camera->view = Mat4LookAt(camera->position, camera->position + camera->front, camera->up);
    camera->proj = Mat4Perspective(fov, aspect, 0.01f, 100.0f);
    camera->fov = fov;
}

void PlayerInitialize(Player *player, vec3 position) {
    player->position = position;
    player->newPosition = position;
    player->direction = {0, 0, 1};
    player->speed = 3.5f;
    player->gravity = 9.8f;
    player->verticalVelocity = 0;
    player->horizontalVelocity = 0;
    player->grounded = false;
    player->joystickSensitivity = 2.5f;
    player->mouseSensitivity = 0.001f;
    player->mouseDefaultScreenX = 0;
    player->mouseDefaultScreenY = 0;
    player->collider.a = position;
    player->collider.a.y += 0.2f;
    player->collider.b = position;
    player->collider.b.y -= 0.6f;
    player->collider.r = 0.3f;
    player->down.o = player->collider.b;
    player->down.d = {0, (-player->collider.r) - 0.1f, 0};
    player->up.o = player->collider.a;
    player->up.d = {0, player->collider.r + 0.1f, 0};
    CameraInitialize(&player->camera, position, 60.0f, (f32)WINDOW_WIDTH/(f32)WINDOW_HEIGHT);
}

void PlayerUpdateCollisionData(Player *player, vec3 position) {
    player->collider.a = position;
    player->collider.a.y += 0.2f;
    player->collider.b = position;
    player->collider.b.y -= 0.6f;
    player->down.o = player->collider.b;
    player->up.o = player->collider.a;
}

void PlayerProcessMovement(Player *player, f32 dt) { 
    // Mouse and Right Stick movement
    f32 leftStickX = JoysickGetLeftStickX();
    f32 leftStickY = JoysickGetLeftStickY();
    f32 rightStickX = JoysickGetRightStickX();
    f32 rightStickY = JoysickGetRightStickY();
    
    if(MouseGetButtonJustDown(MOUSE_BUTTON_RIGHT)) {
        MouseShowCursor(false);
        player->mouseDefaultScreenX = MouseGetScreenX();
        player->mouseDefaultScreenY = MouseGetScreenY();
    }
    if(MouseGetButtonJustUp(MOUSE_BUTTON_RIGHT)) {
        MouseShowCursor(true);
    }
    if(MouseGetButtonDown(MOUSE_BUTTON_RIGHT)) {
        f32 deltaMouseX = (f32)(MouseGetScreenX() - player->mouseDefaultScreenX);
        f32 deltaMouseY = (f32)(MouseGetScreenY() - player->mouseDefaultScreenY);
        MouseSetCursor(player->mouseDefaultScreenX, player->mouseDefaultScreenY);
        player->camera.yaw -= (deltaMouseX * player->mouseSensitivity);
        player->camera.pitch -= (deltaMouseY * player->mouseSensitivity); 
    }

    player->camera.yaw -= (rightStickX * player->joystickSensitivity) * dt;
    player->camera.pitch += (rightStickY * player->joystickSensitivity) * dt;

    f32 maxPitch = RAD(89.0f);
    if(player->camera.pitch > maxPitch) {
        player->camera.pitch = maxPitch;
    }
    else if(player->camera.pitch < -maxPitch) {
        player->camera.pitch = -maxPitch;
    }
    player->camera.front.x = cosf(player->camera.yaw) * cosf(player->camera.pitch);
    player->camera.front.y = sinf(player->camera.pitch);
    player->camera.front.z = sinf(player->camera.yaw) * cosf(player->camera.pitch);
    normalize(&player->camera.front);
    player->camera.right = normalized(cross(player->camera.up, player->camera.front));
    // TODO: this is a easy way of doing this... implement it better
    vec3 worldFront = normalized(cross(player->camera.right, player->camera.up));
    
    // Keyboard and Left Stick movement
    if(KeyboardGetKeyDown(KEYBOARD_KEY_W)) {
        player->newPosition = player->newPosition + (worldFront * player->speed) * dt;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_S)) {
        player->newPosition = player->newPosition - (worldFront * player->speed) * dt;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_D)) {
        player->newPosition = player->newPosition + (player->camera.right * player->speed) * dt;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_A)) {
        player->newPosition = player->newPosition - (player->camera.right * player->speed) * dt;
    }
    player->newPosition = player->newPosition + (player->camera.right * (leftStickX * player->speed)) * dt;
    player->newPosition = player->newPosition + (worldFront  * (leftStickY * player->speed)) * dt; 
    
    // Jump
    if((JoysickGetButtonJustDown(JOYSTICK_BUTTON_A) || KeyboardGetKeyJustDown(KEYBOARD_KEY_SPACE)) &&
        player->grounded) {
        player->verticalVelocity = 5;
    }
    if(!player->grounded) {
        player->verticalVelocity += -player->gravity * dt;
    }
    player->newPosition.y += player->verticalVelocity * dt; 
}


void PlayerProcessCollision(Player *player, OctreeNode *tree, Arena *arena) {
    OBB obb;
    obb.c = player->position;
    obb.u[0] = {1, 0, 0};
    obb.u[1] = {0, 1, 0};
    obb.u[2] = {0, 0, 1};
    obb.e = {1, 1, 1};
    StaticEntityNode *entitiesToProcess = NULL;
    i32 entitiesToProcessCount = 0;  
    OctreeOBBQuery(tree, &obb, &entitiesToProcess, &entitiesToProcessCount, arena);
    entitiesToProcess = entitiesToProcess - (entitiesToProcessCount - 1);
    
    // ray floor test
    bool flag = false;
    for(i32 i = 0; i < entitiesToProcessCount; ++i) {
        StaticEntityNode *entityNode = entitiesToProcess + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            OBB *obb = staticEntity->obbs + j;
            f32 t = 0;
            if(RaycastOBB(obb, &player->down, &t) && t <= 1.0f) {
                flag = true;
                if(player->verticalVelocity < 0) 
                    player->verticalVelocity = 0;
            }
        }
    }
    player->grounded = flag;

    // capsule obb test
    for(i32 i = 0; i < entitiesToProcessCount; ++i) {
        StaticEntityNode *entityNode = entitiesToProcess + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            OBB *obb = staticEntity->obbs + j;
            vec3 closestPoint = ClosestPtPointOBB(player->position, obb);
            vec3 testPosition = ClosestPtPointSegment(closestPoint, player->collider.a, player->collider.b);

            Sphere sphere = {};
            sphere.c = testPosition;
            sphere.r = player->collider.r;
            vec3 d = player->newPosition - testPosition;

            f32 t = 0.0f;
            if(IntersectMovingSphereOBB(sphere, d, *obb, &t)) {
                vec3 hitPoint =  player->position + d * t;
                vec3 normal = normalized(testPosition - closestPoint);
#if 0
                Plane collisionPlane;
                collisionPlane.p = hitPoint;
                collisionPlane.n = normal; 
                player->newPosition = ClosestPtPointPlane(player->newPosition, collisionPlane) +
                                                         (collisionPlane.n * 0.002f);
#else
                f32 penetration = fabsf(dot(player->newPosition - hitPoint, normal));
                player->newPosition = player->newPosition + (normal * penetration) + (normal * 0.002f);
                PlayerUpdateCollisionData(player, player->newPosition);
#endif
            }
        }
    }
}

internal
void PlayerUpdateCamera(Player *player) {
    player->camera.view = Mat4LookAt(player->camera.position, player->camera.position + player->camera.front, player->camera.up);
}

void PlayerUpdate(Player *player, OctreeNode *tree, Arena *arena, f32 dt) {
    player->newPosition = player->position;
    
    PlayerProcessMovement(player, dt);
    PlayerProcessCollision(player, tree, arena);
    
    player->position = player->newPosition;
    player->camera.position = player->position;
    PlayerUpdateCollisionData(player, player->position);

    PlayerUpdateCamera(player);
}


#if 0
void PlayerCapsuleOBBsArray(Player *player, StaticEntityNode *entities, i32 count) {
    for(i32 i = 0; i < count; ++i) {
        StaticEntityNode *entityNode = entities + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            OBB *obb = staticEntity->obbs + j;
            vec3 closestPoint = ClosestPtPointOBB(player->position, obb);
            vec3 testPosition = ClosestPtPointSegment(closestPoint, player->collider.a, player->collider.b);
            f32 distanceSq = lenSq(closestPoint - testPosition);
            if(distanceSq > player->collider.r * player->collider.r) {
                obb->color = 0xFF00FF00;
                continue;
            }
            obb->color = 0xFFFF0000;
            vec3 normal = {0, 1, 0};
            if(CMP(distanceSq, 0.0f)) {
                f32 mSq = lenSq(closestPoint - obb->c);
                if(CMP(mSq, 0.0f)) {
                }
                else {
                    normal = normalized(closestPoint - obb->c);
                }
            }
            else {
                normal = normalized(testPosition - closestPoint);
            }
            
            vec3 outsidePoint = testPosition - normal * player->collider.r;
            f32 distance = len(closestPoint - outsidePoint);
            player->position = player->position + normal * distance;
            player->camera.position = player->position;
            PlayerUpdateCollisionData(player);
        }
    }
}
#endif


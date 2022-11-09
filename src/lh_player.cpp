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
    player->potentialPosition = position;
    player->velocity = {};
    player->acceleration = {};
    player->direction = {0, 0, 1};
    player->speed = 3.5f;
    player->gravity = 9.8f;
    player->verticalVelocity = 0;
    player->horizontalVelocity = 0;
    player->grounded = false;
    player->frameCount = 5;
    player->frame = 0;
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
    player->direction = player->camera.front;
    
    // Keyboard and Left Stick movement
    player->acceleration = {};
    if(KeyboardGetKeyDown(KEYBOARD_KEY_W)) {
        player->acceleration = player->acceleration + worldFront;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_S)) {
        player->acceleration = player->acceleration - worldFront;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_D)) {
        player->acceleration = player->acceleration + player->camera.right;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_A)) {
        player->acceleration = player->acceleration - player->camera.right;
    }
    player->acceleration = player->acceleration + (player->camera.right * leftStickX);
    player->acceleration = player->acceleration + (worldFront * leftStickY);

    if(lenSq(player->acceleration) > 1.0f) {
        player->acceleration = normalized(player->acceleration);
    }

    if(player->grounded) {
        player->speed = 40;
        player->acceleration = player->acceleration * player->speed;
        player->acceleration = player->acceleration - player->velocity*10;
    }
    else {
        player->speed = 40.0f/10.0f;
        player->acceleration = player->acceleration * player->speed;
        player->acceleration = player->acceleration - player->velocity*10.0f/10.0f;
    }

    player->potentialPosition = player->acceleration*0.5f*(dt*dt) +
                                player->velocity*dt +
                                player->position;
    player->velocity = player->acceleration*dt + player->velocity;
    player->potentialPosition = player->position + player->velocity * dt;    
         
    // Jump
    if((JoysickGetButtonJustDown(JOYSTICK_BUTTON_A) || KeyboardGetKeyJustDown(KEYBOARD_KEY_SPACE)) &&
        player->grounded) {
        player->verticalVelocity = 4.5f;
    }
    if(!player->grounded) {
        player->verticalVelocity += -player->gravity * dt;
    }
    if(player->verticalVelocity < -6.0f) {
        player->verticalVelocity = -6.0f;
    }
    player->potentialPosition.y += player->verticalVelocity * dt; 
    PlayerUpdateCollisionData(player, player->potentialPosition);
    
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

    // TODO: fix this to work on low frame rates
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

    for(i32 i = 0; i < entitiesToProcessCount; ++i) {
        StaticEntityNode *entityNode = entitiesToProcess + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            OBB *obb = staticEntity->obbs + j;
#if 0
            Sphere sphere = {};
            sphere.c = player->position;
            sphere.r = player->collider.r;
            vec3 d = player->potentialPosition - player->position;

            f32 t = 0.0f;
            if(IntersectMovingSphereOBB(sphere, d, *obb, &t)) {
                vec3 hitPoint = player->position + d * t;
                vec3 closestPoint = ClosestPtPointOBB(hitPoint, obb);
                vec3 normal = normalized(hitPoint - closestPoint);
                vec3 extraVelocity = player->potentialPosition - hitPoint;
                player->potentialPosition = hitPoint;
                f32 penetration1 = fabsf(dot(player->potentialPosition - hitPoint, normal));
                //f32 penetration2 = player->collider.r - len(closestPoint - player->potentialPosition);
                player->potentialPosition = player->potentialPosition + (normal * penetration1);
            }

#else
            i32 iterations = 10;

            vec3 testPotentialPos = player->position;
            for(i32 iter = 0; iter < iterations; ++iter) {
                vec3 step = (player->potentialPosition - player->position) / iterations;
                testPotentialPos = testPotentialPos + step;
                vec3 closestPoint = ClosestPtPointOBB(testPotentialPos, obb);

                PlayerUpdateCollisionData(player, testPotentialPos);
                vec3 capsulePosition = ClosestPtPointSegment(closestPoint, player->collider.a, player->collider.b);
                vec3 rayToClosest = closestPoint - capsulePosition; 
                f32 penetration = player->collider.r - len(rayToClosest);
                if(isnan(penetration)) penetration = 0;
                if(penetration > 0) {
                    vec3 velocity = player->potentialPosition - testPotentialPos; 
                    Plane collisionPlane;
                    collisionPlane.p = testPotentialPos;
                    collisionPlane.n = normalized(testPotentialPos - closestPoint); 
                    vec3 finalPosition = ClosestPtPointPlane(player->potentialPosition, collisionPlane) - normalized(rayToClosest) * penetration;
                    player->potentialPosition = testPotentialPos - normalized(rayToClosest) * penetration;
                    vec3 offset = finalPosition - player->potentialPosition;
                    player->potentialPosition = player->potentialPosition + offset;
                    break;
                }
            }
#endif


        }
    }
}


internal
void PlayerUpdateCamera(Player *player) {
    player->camera.view = Mat4LookAt(player->camera.position, player->camera.position + player->camera.front, player->camera.up);
}

void PlayerProcessGun(Player *player, OctreeNode *tree, Arena *arena, f32 dt) {
    if((MouseGetButtonDown(MOUSE_BUTTON_LEFT) || JoysickGetButtonDown(JOYSTICK_RIGHT_TRIGGER)) && !player->playAnimation) {
        player->playAnimation = true; 
        player->animationTimer = 0.0f;
        
        OBB obb;
        obb.c = player->position;
        obb.u[0] = {1, 0, 0};
        obb.u[1] = {0, 1, 0};
        obb.u[2] = {0, 0, 1};
        obb.e = {20, 1, 20};
        StaticEntityNode *entitiesToProcess = NULL;
        i32 entitiesToProcessCount = 0;  
        OctreeOBBQuery(tree, &obb, &entitiesToProcess, &entitiesToProcessCount, arena);
        entitiesToProcess = entitiesToProcess - (entitiesToProcessCount - 1); 
        Ray bullet;
        bullet.o = player->position;
        bullet.d = player->direction;
        f32 tMin = FLT_MAX;
        for(i32 i = 0; i < entitiesToProcessCount; ++i) {
            StaticEntityNode *entityNode = entitiesToProcess + i;
            StaticEntity *staticEntity = entityNode->object;
            for(i32 j = 0; j < staticEntity->meshCount; ++j) {
                OBB *obb = staticEntity->obbs + j; 
                f32 t = 0;
                if(RaycastOBB(obb, &bullet, &t)) {
                    if(t < tMin) {
                        tMin = t;
                    }
                }
            }
        }
        vec3 hitPoint = bullet.o + bullet.d * tMin;
        player->bulletBuffer[player->bulletBufferCount++] = hitPoint;
        player->bulletBufferCount %= 10; 
        

    }
    
    if(player->playAnimation) {
        player->frame = (i32)player->animationTimer;
        player->animationTimer += 15.0f * dt;
        
        if(player->frame >= player->frameCount) {
            player->playAnimation = false;
            player->animationTimer = 0.0f;
            player->frame = 0;
        }

    }
}

void PlayerUpdate(Player *player, OctreeNode *tree, Arena *arena, f32 dt) {
    PlayerProcessMovement(player, dt);
    PlayerProcessCollision(player, tree, arena);
    player->position = player->potentialPosition;
    player->camera.position = player->position;
    PlayerUpdateCamera(player);
    PlayerProcessGun(player, tree, arena, dt);
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


#include "lh_player.h"
#include "lh_input.h"
#include "lh_static_entity.h"
#include "lh_memory.h"
#include "lh_enemy.h"

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
    player->physicId = PhysicAddObject(&player->physic);
    player->physic->position = position;
    player->physic->potentialPosition = position;
    player->physic->collider.a = position;
    player->physic->collider.a.y += 0.2f;
    player->physic->collider.b = position;
    player->physic->collider.b.y -= 0.8f;
    player->physic->collider.r = 0.3f;
    player->physic->down.o = player->physic->collider.b;
    player->physic->down.d = {0, (-player->physic->collider.r) - 0.1f, 0};

    player->direction = {0, 0, 1};
    player->speed = 24.0f;
    player->gravity = 9.8f*1.5f;
    player->frameCount = 5;
    player->frame = 0;
    player->joystickSensitivity = 2.5f;
    player->mouseSensitivity = 0.001f;
    player->mouseDefaultScreenX = 0;
    player->mouseDefaultScreenY = 0;

    CameraInitialize(&player->camera, position, 60.0f, (f32)WINDOW_WIDTH/(f32)WINDOW_HEIGHT);
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
    vec3 playerMovement = {};
    if(KeyboardGetKeyDown(KEYBOARD_KEY_W)) {
        playerMovement = playerMovement + worldFront;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_S)) {
        playerMovement = playerMovement - worldFront;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_D)) {
        playerMovement = playerMovement + player->camera.right;
    }
    if(KeyboardGetKeyDown(KEYBOARD_KEY_A)) {
        playerMovement = playerMovement - player->camera.right;
    }
    playerMovement = playerMovement + (player->camera.right * leftStickX);
    playerMovement = playerMovement + (worldFront * leftStickY);

    if(lenSq(playerMovement) > 1.0f) {
        playerMovement = normalized(playerMovement);
    }
    
    playerMovement = playerMovement * player->speed; 
    if(!player->physic->grounded) {
        // TODO: improve this magic number
        playerMovement.x *= 0.1f;
        playerMovement.z *= 0.1f;
    }
    PhysicAddForce(player->physicId, playerMovement);

    if(!player->physic->grounded) {
        vec3 gravityVector = {0, -player->gravity, 0.0f};
        PhysicAddForce(player->physicId, gravityVector);
    }

    // Jump
    if((JoysickGetButtonJustDown(JOYSTICK_BUTTON_A) || KeyboardGetKeyJustDown(KEYBOARD_KEY_SPACE)) &&
        player->physic->grounded) {
        vec3 jumpForce = {0, 6, 0};
        PhysicAddImpulse(player->physicId, jumpForce);
    }    
}

void PlayerProcessGun(Player *player, OctreeNode *tree, Arena *arena, Enemy *enemies, i32 enemyCount, f32 dt) {
    if((MouseGetButtonDown(MOUSE_BUTTON_LEFT) || JoysickGetButtonDown(JOYSTICK_RIGHT_TRIGGER)) && !player->playAnimation) {
        player->playAnimation = true; 
        player->animationTimer = 0.0f;
        
        OBB obb;
        obb.c = player->physic->position;
        obb.u[0] = {1, 0, 0};
        obb.u[1] = {0, 1, 0};
        obb.u[2] = {0, 0, 1};
        obb.e = {20, 1, 20};
        StaticEntityNode *entitiesToProcess = NULL;
        i32 entitiesToProcessCount = 0;  
        OctreeOBBQuery(tree, &obb, &entitiesToProcess, &entitiesToProcessCount, arena);
        entitiesToProcess = entitiesToProcess - (entitiesToProcessCount - 1); 
        Ray bullet;
        bullet.o = player->physic->position;
        bullet.d = player->direction;

        // test ray against status meshes
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

        // test ray against enemies
        // pistol have a range of fire of 20 units
        f32 shootRange = 30.0f;
        vec3 p1 = player->physic->position;
        vec3 q1 = player->physic->position + (player->direction * shootRange);

        f32 enemyTMin = FLT_MAX;
        Enemy *enemyHit = NULL;
        for(i32 i = 0; i < enemyCount; ++i) {
            Enemy *enemy = enemies + i;
            vec3 p2 = enemy->collider.b;
            vec3 q2 = enemy->collider.b + (enemy->collider.a - enemy->collider.b);
            f32 s, t;
            vec3 c1, c2;
            f32 sqDist = ClosestPtSegmentSegment(p1, q1, p2, q2, s, t, c1, c2); 
            if((sqDist <= (enemy->collider.r * enemy->collider.r))) {
                if(s < enemyTMin) {
                    enemyHit = enemy;
                    enemyTMin = s;
                }
            }
        }
        if((enemyTMin*shootRange) < tMin) {
            EnemyWasShoot(enemyHit); 
        }
        else {
            vec3 hitPoint = bullet.o + bullet.d * tMin;
            player->bulletBuffer[player->bulletBufferCount++] = hitPoint;
            player->bulletBufferCount %= 10; 
        }
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

void PlayerUpdate(Player *player, OctreeNode *tree, Arena *arena, Enemy *enemy, i32 enemyCount, f32 dt) {
    PlayerProcessMovement(player, dt);
    PlayerProcessGun(player, tree, arena, enemy, enemyCount, dt);
}

void PlayerFixUpdate(Player *player, f32 dt) {
    player->lastPhysicState = *player->physic;
}

void PlayerPostUpdate(Player *player, f32 t) {
    vec3 interpolatedPosition = PhysicInterpolatePosition(&player->lastPhysicState, player->physic, t);
    player->camera.position = interpolatedPosition;
    player->camera.view = Mat4LookAt(player->camera.position,
                                     player->camera.position + player->camera.front,
                                     player->camera.up);
}

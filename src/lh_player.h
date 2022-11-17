#ifndef _LH_PLAYER_H_
#define _LH_PLAYER_H_

#include "lh_defines.h"
#include "lh_math.h"
#include "lh_physics.h"

struct OctreeNode;
struct Arena;

struct Camera {
    vec3 position;
    vec3 front;
    vec3 right;
    vec3 up;
    f32 pitch;
    f32 yaw;
    mat4 view;
    mat4 proj;
    f32 fov;
};

#if 1

struct Player {
    PhysicObject *physic;
    PhysicObject lastPhysicState;
    SlotmapKey physicId;
    vec3 direction;
    f32 speed;
    f32 gravity;
    
    i32 frameCount;
    i32 frame;
    bool playAnimation;
    f32 animationTimer;
    vec3 bulletBuffer[10];
    i32 bulletBufferCount;
    
    f32 joystickSensitivity;
    f32 mouseSensitivity;
    i32 mouseDefaultScreenX;
    i32 mouseDefaultScreenY;

    Camera camera;
};


#else
struct Player {
    vec3 position;
    vec3 potentialPosition;
    vec3 velocity;
    vec3 acceleration;
    vec3 direction;
    f32 speed;
    f32 gravity;
    bool grounded;
    
    i32 frameCount;
    i32 frame;
    bool playAnimation;
    f32 animationTimer;
    vec3 bulletBuffer[10];
    i32 bulletBufferCount;
    
    f32 joystickSensitivity;
    f32 mouseSensitivity;
    i32 mouseDefaultScreenX;
    i32 mouseDefaultScreenY;

    Camera camera;
    Capsule collider;
    Ray down;
    Ray up;
};
#endif

#if 1
void PlayerInitialize(Player *player, vec3 position);
void PlayerProcessMovement(Player *player, f32 dt);
void PlayerProcessGun(Player *player, OctreeNode *tree, Arena *arena, f32 dt);
void PlayerUpdate(Player *player, OctreeNode *tree, Arena *arena, f32 dt);
void PlayerFixUpdate(Player *player, f32 dt);
void PlayerPostUpdate(Player *player, f32 t);

#else

void PlayerInitialize(Player *player, vec3 position);
void PlayerUpdateCollisionData(Player *player, vec3 position, f32 dt);
void PlayerProcessMovement(Player *player, f32 dt);
void PlayerProcessCollision(Player *player, OctreeNode *tree, Arena *arena, f32 dt);
void PlayerUpdate(Player *player, OctreeNode *tree, Arena *arena, f32 dt);
void PlayerFixUpdate(Player *player, OctreeNode *tree, Arena *arena, f32 dt);

#endif

#endif

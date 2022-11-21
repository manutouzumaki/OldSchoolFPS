#ifndef _LH_ENEMY_H_
#define _LH_ENEMY_H_

#include "lh_defines.h"
#include "lh_math.h"
#include "lh_renderer.h"
#include "lh_physics.h"

struct Texture;
struct OctreeNode;
struct Arena;

struct Enemy {
    PhysicObject *physic;
    PhysicObject lastPhysicState;
    SlotmapKey physicId;
    vec3 direction;
    Mesh mesh;
    Texture *texture;
    Texture *hitTexture;
    Texture *currentTexture;
    f32 hitTimer;
};

void EnemyInitialize(Enemy *enemy, vec3 position, Texture *texture, Texture *hitTexture);
void EnemyUpdate(Enemy *enemy, OctreeNode *tree, Arena *arena, f32 dt);
void EnemyFixUpdate(Enemy *enemy, f32 dt);
void EnemyPostUpdate(Enemy *enemy, f32 cameraYaw, f32 t);


void EnemyWasShoot(Enemy *enemy);

#endif

#ifndef _LH_ENEMY_H_
#define _LH_ENEMY_H_

#include "lh_defines.h"
#include "lh_math.h"
#include "lh_collision.h"
#include "lh_renderer.h"

struct Texture;
struct OctreeNode;
struct Arena;

struct Enemy {
    vec3 position;
    vec3 direction;
    Mesh mesh;
    Texture *texture;
    Capsule collider;
};

void EnemyInitialize(Enemy *enemy, vec3 position, Texture *texture);
void EnemyUpdate(Enemy *enemy, OctreeNode *tree, Arena *arena, f32 cameraYaw, vec3 playerPosition, f32 dt);

#endif

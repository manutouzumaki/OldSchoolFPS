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
    Texture *hitTexture;
    Texture *currentTexture;
    Capsule collider;
    f32 hitTimer;
};

void EnemyInitialize(Enemy *enemy, vec3 position, Texture *texture, Texture *hitTexture);
void EnemyUpdate(Enemy *enemy, OctreeNode *tree, Arena *arena, f32 cameraYaw, f32 dt);
void EnemyWasShoot(Enemy *enemy);

#endif

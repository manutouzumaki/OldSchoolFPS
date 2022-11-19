#include "lh_enemy.h"
#include "lh_static_entity.h"
#include "lh_memory.h"

/*
struct Enemy {
    vec3 position;
    Mesh mesh;
    Texture *texture;
    Capsule collider;
};
*/

global_variable Vertex vertices[] = {
    // position           // uv        // normal
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f
};

global_variable u32 indices[] = {
    0, 1, 3,
    3, 1, 2
};

void EnemyUpdateCollisionData(Enemy *enemy, vec3 position) {
    enemy->collider.a = position;
    enemy->collider.a.y += 0.4f;
    enemy->collider.b = position;
    enemy->collider.b.y -= 0.4f;
}

void EnemyInitialize(Enemy *enemy, vec3 position, Texture *texture, Texture *hitTexture) {
    enemy->position = position;
    enemy->direction = {0, 0, 1};
    enemy->mesh.vertices = vertices;
    enemy->mesh.indices = indices;
    enemy->mesh.verticesCount = 0;
    enemy->mesh.indicesCount = ARRAY_LENGTH(indices);
    enemy->mesh.transform.position = position;
    f32 ratio = 32.0f/58.0f;
    f32 size = 0.7f;
    enemy->mesh.transform.scale = {size*ratio, size, 1};
    enemy->mesh.transform.rotation = {0, 180.0f, 0};
    enemy->mesh.world = TransformToMat4(enemy->mesh.transform.position,
                                        enemy->mesh.transform.rotation,
                                        enemy->mesh.transform.scale);
    enemy->texture = texture;
    enemy->hitTexture = hitTexture;
    enemy->currentTexture = texture;
    enemy->collider.a = position;
    enemy->collider.a.y += 0.4f;
    enemy->collider.b = position;
    enemy->collider.b.y -= 0.4f;
    enemy->collider.r = 0.3f;
}

#include "windows.h"
#include "stdio.h"

void EnemyUpdate(Enemy *enemy, OctreeNode *tree, Arena *arena, f32 cameraYaw, f32 dt) {
#if 1
    // DOOM use this kind of rotation
    enemy->mesh.transform.rotation.y = -DEG(cameraYaw) + 90;
    enemy->mesh.world = TransformToMat4(enemy->mesh.transform.position,
                                        enemy->mesh.transform.rotation,
                                        enemy->mesh.transform.scale);

    if(enemy->hitTimer > 0.0f) {
        enemy->hitTimer -= dt;
    }
    if(enemy->hitTimer < 0.0f) {
        enemy->hitTimer = 0.0f;
        enemy->currentTexture = enemy->texture;
    }
    


#else
    vec2 planeEnemyFront = {enemy->direction.x, enemy->direction.z};
    vec2 planeEnemyRight = {enemy->direction.z, -enemy->direction.x};
    vec2 planeEnemyPosition = {enemy->position.x, enemy->position.z};
    vec2 planePlayerPosition = {playerPosition.x, playerPosition.z};
    vec2 relativePosition = normalized(planePlayerPosition - planeEnemyPosition);
    f32 angleSide = dot(planeEnemyRight, relativePosition);
    f32 angle = acosf(dot(planeEnemyFront, relativePosition));
    if(angleSide < 0.0f) {
        angle = (RAD(360.0f) - angle);
    }
    enemy->mesh.transform.rotation.y = DEG(angle) + 180.0f;
    enemy->mesh.world = TransformToMat4(enemy->mesh.transform.position,
                                        enemy->mesh.transform.rotation,
                                        enemy->mesh.transform.scale);
#endif
}

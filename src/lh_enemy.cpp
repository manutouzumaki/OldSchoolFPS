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

void EnemyInitialize(Enemy *enemy, vec3 position, Texture *texture, Texture *hitTexture) {
    enemy->physicId = PhysicAddObject(&enemy->physic);
    enemy->physic->position = position;
    enemy->physic->potentialPosition = position;
    enemy->physic->collider.a = position;
    enemy->physic->collider.a.y += 0.4f;
    enemy->physic->collider.b = position;
    enemy->physic->collider.b.y -= 0.4f;
    enemy->physic->collider.r = 0.3f;
    enemy->physic->down.o = enemy->physic->collider.b;
    enemy->physic->down.d = {0, (-enemy->physic->collider.r) - 0.1f, 0};

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

}

void EnemyWasShoot(Enemy *enemy) {
    enemy->currentTexture = enemy->hitTexture;
    enemy->hitTimer = 0.2f;
}

void EnemyUpdate(Enemy *enemy, OctreeNode *tree, Arena *arena, f32 dt) {
    if(!enemy->physic->grounded) {
        vec3 gravityVector = {0, -9.8f*1.5f, 0.0f};
        PhysicAddForce(enemy->physicId, gravityVector);
    }

    
    if(enemy->hitTimer > 0.0f) {
        enemy->hitTimer -= dt;
    }
    if(enemy->hitTimer < 0.0f) {
        enemy->hitTimer = 0.0f;
        enemy->currentTexture = enemy->texture;
    }
}

void EnemyFixUpdate(Enemy *enemy, f32 dt) {
    enemy->lastPhysicState = *enemy->physic;
}

void EnemyPostUpdate(Enemy *enemy, f32 cameraYaw, f32 t) {
#if 1
    // DOOM use this kind of rotation
    vec3 interpolatedPosition = PhysicInterpolatePosition(&enemy->lastPhysicState, enemy->physic, t);
    interpolatedPosition.y -= 0.4f;
    enemy->mesh.transform.position = interpolatedPosition;
    enemy->mesh.transform.rotation.y = -DEG(cameraYaw) + 90;
    enemy->mesh.world = TransformToMat4(enemy->mesh.transform.position,
                                        enemy->mesh.transform.rotation,
                                        enemy->mesh.transform.scale);
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

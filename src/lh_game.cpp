#include "lh_game.h"
#include "lh_platform.h"
#include "lh_renderer.h"
#include "lh_sound.h"
#include "lh_input.h"
#include "lh_static_entity.h"
#include "lh_physics.h"

//////////////////////////////////////////////////////////////////////
// TODO (manuto):
//////////////////////////////////////////////////////////////////////
// ...
// ...
// ...
//////////////////////////////////////////////////////////////////////

global_variable Vertex verticesCube[] = {
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f,  0.0f,  1.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,-1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,-1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,-1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,-1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,-1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,-1.0f,  0.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f
};

global_variable Vertex verticesCube2[] = {
    -1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
     1.0f,  1.0f, -1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
     1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
     1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
     1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
     1.0f, -1.0f, -1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
     1.0f,  1.0f, -1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
     1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
     1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f
};

global_variable u32 indicesCube2[] =
{
    3,1,0,2,1,3,
    6,4,5,7,4,6,
    11,9,8, 10,9, 11,
    14, 12, 13, 15, 12, 14,
    19, 17, 16, 18, 17, 19,
    22, 20, 21, 23, 20, 22
};


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

global_variable Light lights[12] = {
    {3, 1, 4},
    {3, 1, 8},
    {3, 1, 12},
    {3, 1, 16},
    {3, 1, 25},
    {6, 1, 25},
    {9, 1, 25},
    {12, 1,25},
    {16, 1, 4},
    {16, 1, 8},
    {16, 1, 12},
    {16, 1, 16}
};

global_variable vec3 lights2[12] = {
    {3, 1, 4},
    {3, 1, 8},
    {3, 1, 12},
    {3, 1, 16},
    {3, 1, 25},
    {6, 1, 25},
    {9, 1, 25},
    {12, 1,25},
    {16, 1, 4},
    {16, 1, 8},
    {16, 1, 12},
    {16, 1, 16}
};


#include "lh_map.h"

internal
void SortEnemies(Enemy *enemies, vec3 position, i32 count) {
    for(i32 j = 1;
        j < count;
        ++j)
    {
        Enemy key = enemies[j];
        f32 keyDistance = lenSq(key.physic->position - position);
        i32 i = j - 1;
        
        Enemy src = enemies[i];
        f32 srcDistance = lenSq(src.physic->position - position);
        while(i >= 0 && srcDistance < keyDistance)
        {
            enemies[i + 1] = enemies[i];
            --i;
            if(i >= 0) {
                srcDistance = lenSq(src.physic->position - position);
            }
        }
        enemies[i + 1] = key;
    }
}

internal
void DrawStaticEntityArray(StaticEntityNode *entities, i32 count,
                           LightNode *lightsToRendere, i32 lightsToRenderCount,
                           GameState *gameState, vec3 cameraPosition, Arena *frameArena) {

    vec3 *lightsArray = ArenaPushArray(frameArena, lightsToRenderCount, vec3);
    for(i32 i = 0; i < lightsToRenderCount; ++i) {
        LightNode *light = lightsToRendere + i;
        lightsArray[i] = light->object->position;
    }

    for(i32 i = 0; i < count; ++i) {
        StaticEntityNode *entityNode = entities + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            Mesh *mesh = staticEntity->meshes + j;
            f32 repeatV = staticEntity->transform.scale.y;
            f32 repeatU = staticEntity->transform.scale.x > staticEntity->transform.scale.z ? staticEntity->transform.scale.x : staticEntity->transform.scale.z;
            RendererDrawMesh(mesh, mesh->world, staticEntity->bitmap, lightsArray, lightsToRenderCount,
                             cameraPosition, true, repeatU, repeatV,
                             &gameState->constBuffer, gameState->shader);
        }
    }
}

internal
void DrawSkybox(vec3 cameraPosition, GameState *gameState) {
    mat4 world;
    Mesh *mesh = gameState->quadMesh;
    // FRONT
    world = Mat4Translate(cameraPosition.x, cameraPosition.y, cameraPosition.z + 1.0f);
    RendererDrawMesh(mesh, world, gameState->skybox[2], NULL, 0, cameraPosition, false, 1, 1,
                     &gameState->constBuffer, gameState->skyboxShader);
// BACK
    world = Mat4Translate(cameraPosition.x, cameraPosition.y, cameraPosition.z - 1.0f) * Mat4RotateY(RAD(180.0f));
    RendererDrawMesh(mesh, world, gameState->skybox[3], NULL, 0, cameraPosition, false, 1, 1,
                     &gameState->constBuffer, gameState->skyboxShader);


// LEFT
    world = Mat4Translate(cameraPosition.x - 1, cameraPosition.y, cameraPosition.z) * Mat4RotateY(RAD(90.0f));
    RendererDrawMesh(mesh, world, gameState->skybox[5], NULL, 0, cameraPosition, false, 1, 1,
                     &gameState->constBuffer, gameState->skyboxShader);

// RIGHT
    world = Mat4Translate(cameraPosition.x + 1, cameraPosition.y, cameraPosition.z) * Mat4RotateY(RAD(-90.0f));
    RendererDrawMesh(mesh, world, gameState->skybox[4], NULL, 0, cameraPosition, false, 1, 1,
                     &gameState->constBuffer, gameState->skyboxShader);

// UP
    world = Mat4Translate(cameraPosition.x, cameraPosition.y + 1, cameraPosition.z) * Mat4RotateX(RAD(-90.0f)) * Mat4RotateZ(RAD(-90.0f));
    RendererDrawMesh(mesh, world, gameState->skybox[0], NULL, 0, cameraPosition, false, 1, 1,
                     &gameState->constBuffer, gameState->skyboxShader);
}

void GameInit(Memory *memory) {
    // The GameState has to be the first element on the memory
    ASSERT(memory->used + sizeof(GameState) <= memory->size);
    GameState *gameState = (GameState *)memory->data;
    memory->used += sizeof(GameState);

    //WindowSystemInitialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Last Hope 3D");
    WindowSystemInitialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Minecraft");
    RendererSystemInitialize();
    SoundSystemInitialize();
    PhysicSystemInitialize();
    
    gameState->dataArena = ArenaCreate(memory, Megabytes(500));
    gameState->frameArena = ArenaCreate(memory, Megabytes(10));
    gameState->textureArena = ArenaCreate(memory, Megabytes(1));
    gameState->soundArena = ArenaCreate(memory, Megabytes(1));
    gameState->staticEntitiesArena = ArenaCreate(memory, Megabytes(1));

    // Load Assets
    gameState->bitmaps[0] = RendererCreateTexture("../assets/test.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[1] = RendererCreateTexture("../assets/grass.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[2] = RendererCreateTexture("../assets/hand.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[3] = RendererCreateTexture("../assets/crosshair.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[4] = RendererCreateTexture("../assets/shootSpritesheet.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[5] = RendererCreateTexture("../assets/enemy.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[6] = RendererCreateTexture("../assets/enemyHit.bmp", &gameState->textureArena, &gameState->dataArena);
    
    gameState->skybox[0] = RendererCreateTexture("../assets/skyUp.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->skybox[1] = RendererCreateTexture("../assets/skyDown.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->skybox[2] = RendererCreateTexture("../assets/skyFront.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->skybox[3] = RendererCreateTexture("../assets/skyBack.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->skybox[4] = RendererCreateTexture("../assets/skyLeft.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->skybox[5] = RendererCreateTexture("../assets/skyRight.bmp", &gameState->textureArena, &gameState->dataArena);
    
    gameState->chocolate = SoundCreate("../assets/chocolate.wav", &gameState->soundArena, &gameState->dataArena);
    gameState->music     = SoundCreate("../assets/lugia.wav", &gameState->soundArena, &gameState->dataArena);
    gameState->shoot     = SoundCreate("../assets/shoot.wav", &gameState->soundArena, &gameState->dataArena);

    gameState->shader = RendererCreateShader("../src/shaders/vertex.hlsl", 
                                             "../src/shaders/pixel.hlsl",
                                             &gameState->dataArena);
    gameState->skyboxShader = RendererCreateShader("../src/shaders/vertex.hlsl", 
                                                   "../src/shaders/skybox.hlsl",
                                                   &gameState->dataArena);

    gameState->quadMesh = RendererCreateMesh(vertices, ARRAY_LENGTH(vertices), indices, ARRAY_LENGTH(indices), Mat4Identity(), &gameState->dataArena); 
    gameState->cubeMesh = RendererCreateMesh(verticesCube2, ARRAY_LENGTH(verticesCube2), indicesCube2, ARRAY_LENGTH(indicesCube2), Mat4Identity(), &gameState->dataArena); 

    // InitializeMap
    StaticEntitiesInitialized(&gameState->entities, &gameState->entitiesCount, &gameState->staticEntitiesArena,
                              vertices, indices, ARRAY_LENGTH(indices), gameState->bitmaps, gameState->quadMesh);
    Mesh *cubeMesh = gameState->cubeMesh;

    StaticEntity *entity = ArenaPushStruct(&gameState->staticEntitiesArena, StaticEntity);
    gameState->entitiesCount++;
    entity->bitmap = gameState->bitmaps[1];
    Transform *absTransform = &entity->transform;
    Mesh *mesh1 = &entity->meshes[entity->meshCount];
    OBB *obb1 = &entity->obbs[entity->meshCount++];
    Transform *relTransform = &mesh1->transform;
    mat4 *world1 = &mesh1->world;

    absTransform->position = {5, -1.0f, 22};
    absTransform->scale = {2, 2, 1};
    absTransform->rotation = {0, 0, 0};

    relTransform->position = {};
    relTransform->scale = {};
    relTransform->rotation = {};
    
    *obb1 = CreateOBB({5, -1.0f, 22}, {0, 0, 0}, {2, 2, 1});
    *world1 = TransformToMat4(absTransform->position + relTransform->position,
                             absTransform->rotation + relTransform->rotation,
                             absTransform->scale + relTransform->scale);
    mesh1->vertices = verticesCube2;
    mesh1->verticesCount = 0;
    mesh1->indices = indicesCube2;
    mesh1->indicesCount = ARRAY_LENGTH(indicesCube2);
    mesh1->gpuVertex = cubeMesh->gpuVertex;
    mesh1->gpuIndices = cubeMesh->gpuIndices;


    entity = ArenaPushStruct(&gameState->staticEntitiesArena, StaticEntity);
    gameState->entitiesCount++;
    entity->bitmap = gameState->bitmaps[1];
    absTransform = &entity->transform;
    mesh1 = &entity->meshes[entity->meshCount];
    obb1 = &entity->obbs[entity->meshCount++];
    relTransform = &mesh1->transform;
    world1 = &mesh1->world;

    absTransform->position = {0.7f, -1.0f, 22};
    absTransform->scale = {2, 1, 1};
    absTransform->rotation = {0, 0, 0};

    relTransform->position = {};
    relTransform->scale = {};
    relTransform->rotation = {};
    
    *obb1 = CreateOBB({0.7f, -1.0f, 22}, {0, 0, 0}, {2, 1, 1});
    *world1 = TransformToMat4(absTransform->position + relTransform->position,
                             absTransform->rotation + relTransform->rotation,
                             absTransform->scale + relTransform->scale);
    mesh1->vertices = verticesCube2;
    mesh1->verticesCount = 0;
    mesh1->indices = indicesCube2;
    mesh1->indicesCount = ARRAY_LENGTH(indicesCube2);
    mesh1->gpuVertex = cubeMesh->gpuVertex;
    mesh1->gpuIndices = cubeMesh->gpuIndices;


    entity = ArenaPushStruct(&gameState->staticEntitiesArena, StaticEntity);
    gameState->entitiesCount++;
    entity->bitmap = gameState->bitmaps[1];
    absTransform = &entity->transform;
    mesh1 = &entity->meshes[entity->meshCount];
    obb1 = &entity->obbs[entity->meshCount++];
    relTransform = &mesh1->transform;
    world1 = &mesh1->world;

    absTransform->position = {16, -1.0f, 12};
    absTransform->scale = {2, 1, 1};
    absTransform->rotation = {0, 0, 0};

    relTransform->position = {};
    relTransform->scale = {};
    relTransform->rotation = {};
    
    *obb1 = CreateOBB({16, -1.0f, 12}, {0, 0, 0}, {2, 1, 1});
    *world1 = TransformToMat4(absTransform->position + relTransform->position,
                             absTransform->rotation + relTransform->rotation,
                             absTransform->scale + relTransform->scale);
    mesh1->vertices = verticesCube2;
    mesh1->verticesCount = 0;
    mesh1->indices = indicesCube2;
    mesh1->indicesCount = ARRAY_LENGTH(indicesCube2);
    mesh1->gpuVertex = cubeMesh->gpuVertex;
    mesh1->gpuIndices = cubeMesh->gpuIndices;

    entity = ArenaPushStruct(&gameState->staticEntitiesArena, StaticEntity);
    gameState->entitiesCount++;
    entity->bitmap = gameState->bitmaps[1];
    absTransform = &entity->transform;
    mesh1 = &entity->meshes[entity->meshCount];
    obb1 = &entity->obbs[entity->meshCount++];
    relTransform = &mesh1->transform;
    world1 = &mesh1->world;

    absTransform->position = {9, -1.0f, 18};
    absTransform->scale = {2, 2, 2};
    absTransform->rotation = {0, 45, 0};

    relTransform->position = {};
    relTransform->scale = {};
    relTransform->rotation = {};
    
    *obb1 = CreateOBB({9, -1.0f, 18}, {0, 45, 0}, {2, 2, 2});
    *world1 = TransformToMat4(absTransform->position + relTransform->position,
                             absTransform->rotation + relTransform->rotation,
                             absTransform->scale + relTransform->scale);
    mesh1->vertices = verticesCube2;
    mesh1->verticesCount = 0;
    mesh1->indices = indicesCube2;
    mesh1->indicesCount = ARRAY_LENGTH(indicesCube2);
    mesh1->gpuVertex = cubeMesh->gpuVertex;
    mesh1->gpuIndices = cubeMesh->gpuIndices;



    gameState->entities = entity;
    gameState->entities = gameState->entities - (gameState->entitiesCount - 1);

    // TODO: test octree
    f32 mapWidth = mapCountX*2.0f;
    f32 mapHeight = mapCountY*2.0f; 
    gameState->tree = OctreeCreate({mapWidth*0.5f, -2.0f, mapHeight*0.5f},
                                    mapWidth*0.5f,
                                    2,
                                    &gameState->dataArena);

    // add the StaticEntity to the octree
    for(i32 i = 0; i < gameState->entitiesCount; ++i) {
        StaticEntity *object = gameState->entities + i;
        OctreeInsertObject(gameState->tree, object, &gameState->dataArena);
    }
    // add lights to the octree
    for(i32 i = 0; i < ARRAY_LENGTH(lights); ++i) {
        Light *object = lights + i;
        OctreeInsertLight(gameState->tree, object, &gameState->dataArena);
    }

    PlayerInitialize(&gameState->player, {2, 4, 4});
    RendererSetProj(gameState->player.camera.proj);
    RendererSetView(gameState->player.camera.view);

    gameState->constBuffer.proj = gameState->player.camera.proj;
    gameState->constBuffer.view = gameState->player.camera.view;
    RendererUpdateShaderData(gameState->shader, &gameState->constBuffer);

    EnemyInitialize(&gameState->enemy[0], {2,  2, 5}, gameState->bitmaps[5], gameState->bitmaps[6]);
    EnemyInitialize(&gameState->enemy[1], {3,  2, 10}, gameState->bitmaps[5], gameState->bitmaps[6]);
    EnemyInitialize(&gameState->enemy[2], {3,  2, 25}, gameState->bitmaps[5], gameState->bitmaps[6]);
    EnemyInitialize(&gameState->enemy[3], {12, 2, 20}, gameState->bitmaps[5], gameState->bitmaps[6]);
    EnemyInitialize(&gameState->enemy[4], {16, 2, 10}, gameState->bitmaps[5], gameState->bitmaps[6]);
    EnemyInitialize(&gameState->enemy[5], {17, 2, 15}, gameState->bitmaps[5], gameState->bitmaps[6]);
    gameState->enemyCount = 6;
    //SoundPlay(gameState->music, true); 
}

void GameUpdate(Memory *memory, f32 dt) {

    if(KeyboardGetKeyJustDown(KEYBOARD_KEY_1) || JoysickGetButtonJustDown(JOYSTICK_BUTTON_B)) {
        RendererType type = GetRendererType();
        if(type == RENDERER_DIRECTX) {
            RendererSetMode(RENDERER_CPU);
        }
        if(type == RENDERER_CPU) {
            RendererSetMode(RENDERER_DIRECTX);
        }
    }

    PhysicClearForces();
    GameState *gameState = (GameState *)memory->data;
    if((MouseGetButtonDown(MOUSE_BUTTON_LEFT) || JoysickGetButtonDown(JOYSTICK_RIGHT_TRIGGER)) && !gameState->player.playAnimation) {
        SoundPlay(gameState->shoot, false); 
    }
    PlayerUpdate(&gameState->player, gameState->tree, &gameState->frameArena, gameState->enemy, gameState->enemyCount, dt);
    for(i32 i = 0; i < gameState->enemyCount; ++i) {
        Enemy *enemy = gameState->enemy + i;
        EnemyUpdate(enemy, gameState->tree, &gameState->frameArena, dt);
    }    
}

void GameFixUpdate(Memory *memory, f32 dt) {
    GameState *gameState = (GameState *)memory->data;
    PlayerFixUpdate(&gameState->player, dt);
    for(i32 i = 0; i < gameState->enemyCount; ++i) {
        Enemy *enemy = gameState->enemy + i;
        EnemyFixUpdate(enemy, dt);
    }
    PhysicStep(gameState->tree, &gameState->frameArena, dt); 
}

void GamePostUpdate(Memory *memory, f32 t) {
    GameState *gameState = (GameState *)memory->data;
    PlayerPostUpdate(&gameState->player, t);
    for(i32 i = 0; i < gameState->enemyCount; ++i) {
        Enemy *enemy = gameState->enemy + i;
        EnemyPostUpdate(enemy, gameState->player.camera.yaw, t);
    }

}

void GameRender(Memory *memory) {
    GameState *gameState = (GameState *)memory->data;
    RendererSetView(gameState->player.camera.view);
    RendererClearBuffers(0xFFFFFFEE, 0.0f);
    gameState->constBuffer.view = gameState->player.camera.view;
    RendererUpdateShaderData(gameState->shader, &gameState->constBuffer);
    RendererUpdateShaderData(gameState->skyboxShader, &gameState->constBuffer);

    // SKYBOX
    DrawSkybox(gameState->player.camera.position, gameState);
    RendererFlushWorkQueue(); 

    OBB obb;
    obb.c = gameState->player.camera.position;
    obb.u[0] = {1, 0, 0};
    obb.u[1] = {0, 1, 0};
    obb.u[2] = {0, 0, 1};
    obb.e = {20, 1, 20};
    StaticEntityNode *entitiesToRender = NULL;
    i32 entitiesToRenderCount = 0;  
    OctreeOBBQuery(gameState->tree, &obb, &entitiesToRender, &entitiesToRenderCount, &gameState->frameArena);
    entitiesToRender = entitiesToRender - (entitiesToRenderCount - 1);

    obb.e = {10, 1, 10};
    LightNode *lightsToRender = NULL;
    i32 lightsToRenderCount = 0;
    OctreeOBBQueryLights(gameState->tree, &obb, &lightsToRender, &lightsToRenderCount, &gameState->frameArena);
    lightsToRender = lightsToRender - (lightsToRenderCount - 1);

    DrawStaticEntityArray(entitiesToRender, entitiesToRenderCount, lightsToRender, lightsToRenderCount,
                          gameState, gameState->player.camera.position, &gameState->frameArena);
    for(i32 i = 0; i < 10; ++i) {
        vec3 position = gameState->player.bulletBuffer[i];
        mat4 world = Mat4Translate(position.x, position.y, position.z) * Mat4Scale(0.05f, 0.05f, 0.05f);
        RendererDrawMesh(gameState->cubeMesh, world, gameState->bitmaps[3], NULL, 0, gameState->player.camera.position, true, 1, 1,
                         &gameState->constBuffer, gameState->skyboxShader);
    }
    RendererFlushWorkQueue(); 

    // render the enemy
    // TODO: mabye copy the enemy to a tmp Arena
    SortEnemies(gameState->enemy, gameState->player.camera.position, gameState->enemyCount);
    for(i32 i = 0 ; i < gameState->enemyCount; ++i) {
        Enemy *enemy = gameState->enemy + i;
        RendererDrawMesh(gameState->quadMesh, enemy->mesh.world, enemy->currentTexture, NULL, 0, gameState->player.camera.position, true, 1, 1,
                         &gameState->constBuffer, gameState->skyboxShader);
    }
    RendererFlushWorkQueue(); 

    RendererDrawAnimatedRect((WINDOW_WIDTH/2) - ((78*2)/2) , 0, 78*2, 128*2, gameState->bitmaps[4], 78, 128, gameState->player.frame);
    RendererDrawRect((WINDOW_WIDTH/2) - ((16)/2), (WINDOW_HEIGHT/2) - ((16)/2), 16, 16, gameState->bitmaps[3]);

    RendererPresent();
    ArenaReset(&gameState->frameArena);
}

void GameShutdown(Memory * memory) {
    GameState *gameState = (GameState *)memory->data;
    SoundDestroy(gameState->shoot);
    SoundDestroy(gameState->music);
    SoundDestroy(gameState->chocolate);

    PhysicSystemShutdown();
    SoundSystemShudown();
    RendererSystemShutdown();
    WindowSystemShutdown();
}

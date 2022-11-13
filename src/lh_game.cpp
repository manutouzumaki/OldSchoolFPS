#include "lh_game.h"
#include "lh_platform.h"
#include "lh_renderer.h"
#include "lh_sound.h"
#include "lh_texture.h"
#include "lh_input.h"
#include "lh_static_entity.h"

//////////////////////////////////////////////////////////////////////
// TODO (manuto):
//////////////////////////////////////////////////////////////////////
// - fix jump VERY IMPORTANT
// - add some kind of enemy
// - add aplha blending to the 3d software renderer
// - improve the physic of the player
// - add font
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
    -1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
     1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f, -1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
     1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
    -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
     1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
     1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f
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

global_variable vec3 lights[12] = {
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
void DrawStaticEntityArray(StaticEntityNode *entities, i32 count, GameState *gameState, vec3 cameraPosition) {
    for(i32 i = 0; i < count; ++i) {
        StaticEntityNode *entityNode = entities + i;
        StaticEntity *staticEntity = entityNode->object;
        for(i32 j = 0; j < staticEntity->meshCount; ++j) {
            Mesh *mesh = staticEntity->meshes + j;
            OBB *obb = staticEntity->obbs + j;
            f32 repeatV = staticEntity->transform.scale.y;
            f32 repeatU = staticEntity->transform.scale.x > staticEntity->transform.scale.z ? staticEntity->transform.scale.x : staticEntity->transform.scale.z;
            RendererPushWorkToQueue(mesh->vertices, mesh->indices, mesh->indicesCount,
                                    staticEntity->bitmap, lights, ARRAY_LENGTH(lights),
                                    cameraPosition, mesh->world, true, repeatU, repeatV);
            //DEBUG_RendererDrawWireframeBuffer(verticesCube, ARRAY_LENGTH(verticesCube), obb->color, obb->world);
        }
    }
}

internal
void DrawSkybox(vec3 cameraPosition, GameState *gameState) {
// FRONT
    mat4 skyboxWorld = Mat4Translate(cameraPosition.x, cameraPosition.y, cameraPosition.z + 1.0f);
    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                            gameState->skybox[2], NULL, 0,
                            cameraPosition, skyboxWorld, false, 1, 1);
// BACK
    skyboxWorld = Mat4Translate(cameraPosition.x, cameraPosition.y, cameraPosition.z - 1.0f) * 
                  Mat4RotateY(RAD(180.0f));
    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                            gameState->skybox[3], NULL, 0,
                            cameraPosition, skyboxWorld, false, 1, 1);
// LEFT
    skyboxWorld = Mat4Translate(cameraPosition.x - 1, cameraPosition.y, cameraPosition.z) * 
                  Mat4RotateY(RAD(90.0f));
    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                            gameState->skybox[5], NULL, 0,
                            cameraPosition, skyboxWorld, false, 1, 1);
// RIGHT
    skyboxWorld = Mat4Translate(cameraPosition.x + 1, cameraPosition.y, cameraPosition.z) * 
                  Mat4RotateY(RAD(-90.0f));
    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                            gameState->skybox[4], NULL, 0,
                            cameraPosition, skyboxWorld, false, 1, 1);
// UP
    skyboxWorld = Mat4Translate(cameraPosition.x, cameraPosition.y + 1, cameraPosition.z) * 
                  Mat4RotateX(RAD(-90.0f)) * Mat4RotateZ(RAD(-90.0f));
    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                            gameState->skybox[0], NULL, 0,
                            cameraPosition, skyboxWorld, false, 1, 1);
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
    
    gameState->dataArena = ArenaCreate(memory, Megabytes(500));
    gameState->frameArena = ArenaCreate(memory, Megabytes(10));
    gameState->textureArena = ArenaCreate(memory, Megabytes(1));
    gameState->soundArena = ArenaCreate(memory, Megabytes(1));
    gameState->staticEntitiesArena = ArenaCreate(memory, Megabytes(1));

    // Load Assets
    gameState->bitmaps[0] = TextureCreate("../assets/test.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[1] = TextureCreate("../assets/grass.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[2] = TextureCreate("../assets/hand.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[3] = TextureCreate("../assets/crosshair.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[4] = TextureCreate("../assets/shootSpritesheet.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->bitmaps[5] = TextureCreate("../assets/enemy.bmp", &gameState->textureArena, &gameState->dataArena);
    
    gameState->skybox[0] = TextureCreate("../assets/skyUp.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->skybox[1] = TextureCreate("../assets/skyDown.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->skybox[2] = TextureCreate("../assets/skyFront.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->skybox[3] = TextureCreate("../assets/skyBack.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->skybox[4] = TextureCreate("../assets/skyLeft.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->skybox[5] = TextureCreate("../assets/skyRight.bmp", &gameState->textureArena, &gameState->dataArena);
    
    gameState->chocolate = SoundCreate("../assets/chocolate.wav", &gameState->soundArena, &gameState->dataArena);
    gameState->music     = SoundCreate("../assets/lugia.wav", &gameState->soundArena, &gameState->dataArena);
    gameState->shoot     = SoundCreate("../assets/shoot.wav", &gameState->soundArena, &gameState->dataArena);

    
    // InitializeMap
    StaticEntitiesInitialized(&gameState->entities, &gameState->entitiesCount, &gameState->staticEntitiesArena,
                              vertices, indices, ARRAY_LENGTH(indices), gameState->bitmaps);

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


    gameState->entities = entity;
    gameState->entities = gameState->entities - (gameState->entitiesCount - 1);

    // TODO: test octree
    f32 mapWidth = mapCountX*2.0f;
    f32 mapHeight = mapCountY*2.0f; 
    gameState->tree = OctreeCreate({mapWidth*0.5f, -2.0f, mapHeight*0.5f},
                                    mapWidth*0.5f,
                                    1,
                                    &gameState->dataArena);

    // add the StaticEntity to the octree
    for(i32 i = 0; i < gameState->entitiesCount; ++i) {
        StaticEntity *object = gameState->entities + i;
        OctreeInsertObject(gameState->tree, object, &gameState->dataArena);
    }

    PlayerInitialize(&gameState->player, {2, 4, 4});
    RendererSetProj(gameState->player.camera.proj);
    RendererSetView(gameState->player.camera.view);


    EnemyInitialize(&gameState->enemy, {2, -0.3, 10}, gameState->bitmaps[5]);
    //SoundPlay(gameState->music, true); 

}

void GameUpdate(Memory *memory, f32 dt) {
    GameState *gameState = (GameState *)memory->data;
    if((MouseGetButtonDown(MOUSE_BUTTON_LEFT) || JoysickGetButtonDown(JOYSTICK_RIGHT_TRIGGER)) && !gameState->player.playAnimation) {
        SoundPlay(gameState->shoot, false); 
    }
    EnemyUpdate(&gameState->enemy, gameState->tree, &gameState->frameArena, gameState->player.camera.yaw, gameState->player.position, dt);
    PlayerUpdate(&gameState->player, gameState->tree, &gameState->frameArena, dt);
}

void GameFixUpdate(Memory *memory, f32 dt) {
    GameState *gameState = (GameState *)memory->data;
    PlayerFixUpdate(&gameState->player, gameState->tree, &gameState->frameArena, dt);
}

void GameRender(Memory *memory) {
    GameState *gameState = (GameState *)memory->data;
    RendererSetView(gameState->player.camera.view);
    RendererClearBuffers(0xFFFFFFEE, 0.0f);

    // SKYBOX
    DrawSkybox(gameState->player.position, gameState);
    RendererFlushWorkQueue(); 

    OBB obb;
    obb.c = gameState->player.position;
    obb.u[0] = {1, 0, 0};
    obb.u[1] = {0, 1, 0};
    obb.u[2] = {0, 0, 1};
    obb.e = {20, 1, 20};
    StaticEntityNode *entitiesToRender = NULL;
    i32 entitiesToRenderCount = 0;  
    OctreeOBBQuery(gameState->tree, &obb, &entitiesToRender, &entitiesToRenderCount, &gameState->frameArena);
    entitiesToRender = entitiesToRender - (entitiesToRenderCount - 1);

    DrawStaticEntityArray(entitiesToRender, entitiesToRenderCount, gameState, gameState->player.position);

    for(i32 i = 0; i < 10; ++i) {
        vec3 position = gameState->player.bulletBuffer[i];
        mat4 world = Mat4Translate(position.x, position.y, position.z) * Mat4Scale(0.05f, 0.05f, 0.05f);
        RendererPushWorkToQueue(verticesCube2, indicesCube2, ARRAY_LENGTH(indicesCube2),
                                gameState->bitmaps[3], NULL, 0,
                                gameState->player.position, world, true, 1, 1);
    }

    RendererFlushWorkQueue(); 

    // TODO: render the enemy
    Enemy *enemy = &gameState->enemy;
    RendererPushWorkToQueue(enemy->mesh.vertices, enemy->mesh.indices, enemy->mesh.indicesCount, enemy->texture,
                            NULL, 0, gameState->player.position, enemy->mesh.world, true, 1, 1);

    RendererFlushWorkQueue(); 

    //RendererDrawRectFast((WINDOW_WIDTH/2) - ((192)/2) - 20, 0, 192, 192, gameState->bitmaps[2]);
    RendererDrawAnimatedRectFast((WINDOW_WIDTH/2) - ((78*2)/2) , 0, 78*2, 128*2, gameState->bitmaps[4], 78, 128, gameState->player.frame);
    RendererDrawRectFast((WINDOW_WIDTH/2) - ((16)/2),
                         (WINDOW_HEIGHT/2) - ((16)/2), 
                          16, 16, gameState->bitmaps[3]);
    
    RendererPresent();

    ArenaReset(&gameState->frameArena);
}

void GameShutdown(Memory * memory) {
    GameState *gameState = (GameState *)memory->data;
    SoundDestroy(gameState->shoot);
    SoundDestroy(gameState->music);
    SoundDestroy(gameState->chocolate);

    SoundSystemShudown();
    RendererSystemShutdown();
    WindowSystemShutdown();
}

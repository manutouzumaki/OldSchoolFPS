#ifndef _LH_GAME_H_
#define _LH_GAME_H_

#include "lh_defines.h"
#include "lh_memory.h"
#include "lh_math.h"
#include "lh_player.h"
#include "lh_enemy.h"

struct Sound;
struct Texture;
struct StaticEntity;
struct OctreeNode;

struct Mesh;
struct Shader;

struct GameState {
    Arena dataArena;
    Arena frameArena;
    Arena textureArena; 
    Arena soundArena;
    Arena staticEntitiesArena;
    
    Texture *bitmaps[7];
    Texture *skybox[6];

    Sound *chocolate;
    Sound *music;    
    Sound *shoot;

    Player player;
    Enemy enemy[6];
    i32 enemyCount;
    
    OctreeNode *tree;
    StaticEntity *entities;
    i32 entitiesCount;

    // TODO: test directx renderer
    Mesh *cubeMesh;
    Mesh *quadMesh;
    Shader *shader;
    Shader *skyboxShader;
    ConstantBuffer constBuffer;
};

void GameInit(Memory *memory);
void GameUpdate(Memory *memory, f32 dt);
void GameFixUpdate(Memory *memory, f32 dt);
void GamePostUpdate(Memory *memory, f32 t);
void GameRender(Memory *memory);
void GameShutdown(Memory *memory);

#endif

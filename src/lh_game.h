#ifndef _LH_GAME_H_
#define _LH_GAME_H_

#include "lh_defines.h"
#include "lh_memory.h"
#include "lh_math.h"
#include "lh_player.h"

struct Sound;
struct Texture;
struct StaticEntity;
struct OctreeNode;

struct GameState {
    Arena dataArena;
    Arena frameArena;
    Arena textureArena; 
    Arena soundArena;
    Arena staticEntitiesArena;
    
    Texture *bitmaps[4];
    Texture *skybox[6];

    Sound *chocolate;
    Sound *music;    
    Sound *shoot;

    Player player;
    
    OctreeNode *tree;
    StaticEntity *entities;
    i32 entitiesCount;
};

void GameInit(Memory *memory);
void GameUpdate(Memory *memory, f32 dt);
void GameRender(Memory *memory);
void GameShutdown(Memory *memory);

#endif

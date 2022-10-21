#ifndef _LH_GAME_H_
#define _LH_GAME_H_

#include "lh_defines.h"
#include "lh_memory.h"

struct Sound;
struct Texture;

struct GameState {
    Arena dataArena;
    Arena textureArena; 
    Arena soundArena;
    
    Texture *bitmap;
    Sound *chocolate;
    Sound *music;    
    Sound *shoot; 
};

void GameInit(Memory *memory);
void GameUpdate(Memory *memory, f32 dt);
void GameRender(Memory *memory);
void GameShutdown(Memory *memory);

#endif

#ifndef _LH_GAME_H_
#define _LH_GAME_H_

#include "lh_platform.h"
#include "lh_renderer.h"
#include "lh_sound.h"
#include "lh_input.h"
#include "lh_memory.h"

struct GameState {
    Arena bitmapArena;
    BMP bitmap;

    Sound *chocolate;
    Sound *music;    
    Sound *shoot; 
};

void GameInit(Memory *memory);
void GameUpdate(Memory *memory, f32 dt);
void GameRender(Memory *memory);
void GameShutdown(Memory *memory);

#endif

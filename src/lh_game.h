#ifndef _LH_GAME_H_
#define _LH_GAME_H_

#include "lh_platform.h"

struct GameState {
    Arena bitmapArena;
    BMP bitmap;
};

void GameInit(Memory *memory);
void GameUpdate(f32 dt);
void GameRender();
void GameShutdown();

#endif

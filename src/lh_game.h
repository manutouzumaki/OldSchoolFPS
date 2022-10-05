#ifndef _LH_GAME_H_
#define _LH_GAME_H_

struct GameState {
    Arena bitmapArena;
    Bitmap bitmap;
};

void GameInit(Platform *platform);
void GameUpdate(Platform *platform);
void GameRender(Platform *platform);
void GameShutdown(Platform *platform);

#endif

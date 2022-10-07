#include "lh_game.h"

global_variable vec3 vertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

global_variable vec2 uvs[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f
};

global_variable f32 vertices0[] = {
//  Position               UV
    -1.0f,  1.0f, -1.0f,   0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 1.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 1.0f,
     1.0f,  1.0f, -1.0f,   0.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,   0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,   0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,   1.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,   1.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,   0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,   0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 1.0f,
     1.0f,  1.0f, -1.0f,   0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,   1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,   1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,   0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,   0.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,   1.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 1.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 0.0f,
    -1.0f,  1.0f,  1.0f,   0.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,   0.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,   0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,   1.0f, 1.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f,   0.0f, 0.0f,
     1.0f, -1.0f,  1.0f,   0.0f, 1.0f
};

global_variable GameState *gGameState;
global_variable Window *gWindow;
global_variable Renderer *gRenderer;

void GameInit(Memory *memory) {
    // The GameState has to be the first element on the memory
    ASSERT(memory->used + sizeof(GameState) <= memory->size);
    gGameState = (GameState *)((u8 *)memory->data + memory->used);
    memory->used += sizeof(GameState);

    gWindow = WindowCreate(800, 600, "Last Hope 3D");

    gRenderer = RendererCreate(gWindow);
    RendererSetProj(gRenderer, Mat4Perspective(60.0f, 800.0f/600.0f, 0.1f, 100.0f));
    RendererSetView(gRenderer, Mat4LookAt({6, 0, -8}, {6, 0, 0}, {0, 1, 0}));

    gGameState->bitmapArena = ArenaCreate(memory, Megabytes(1));
    gGameState->bitmap = LoadTexture("../assets/test.bmp", &gGameState->bitmapArena);
}

void GameUpdate(f32 dt) {
}

void GameRender() {
    RendererClearBuffers(gRenderer, 0xFF000022, 0.0f);
    
    
    RenderBufferTextureClipping(gRenderer, vertices, uvs,
             ARRAY_LENGTH(vertices), gGameState->bitmap);
    
    
    RendererPresent(gRenderer);
}

void GameShutdown() {
    RendererDestroy(gRenderer);
    WindowDestroy(gWindow);
}


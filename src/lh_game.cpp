#include "lh_game.h"

global_variable GameState *gGameState;
global_variable Window *gWindow;
global_variable Renderer *gRenderer;

#if 0
global_variable Vertex vertices[] = {
    // positions           uvs           normals
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f
};
#else
Vertex vertices[] = {
    -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
     1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    -1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    
    -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    
    1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
     1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
     1.0f,  1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f
};

u32 indices[] =
{
    3,1,0,2,1,3,
    6,4,5,7,4,6,
    11,9,8, 10,9, 11,
    14, 12, 13, 15, 12, 14,
    19, 17, 16, 18, 17, 19,
    22, 20, 21, 23, 20, 22
};
#endif


void GameInit(Memory *memory) {
    // The GameState has to be the first element on the memory
    ASSERT(memory->used + sizeof(GameState) <= memory->size);
    gGameState = (GameState *)((u8 *)memory->data + memory->used);
    memory->used += sizeof(GameState);

    gWindow = WindowCreate(800, 600, "Last Hope 3D");

    gRenderer = RendererCreate(gWindow);
    RendererSetProj(gRenderer, Mat4Perspective(60.0f, 800.0f/600.0f, 0.1f, 100.0f));
    RendererSetView(gRenderer, Mat4LookAt({0, 0, -8}, {0, 0, 0}, {0, 1, 0}));

    gGameState->bitmapArena = ArenaCreate(memory, Megabytes(1));
    gGameState->bitmap = LoadTexture("../assets/test.bmp", &gGameState->bitmapArena);
}

void GameUpdate(f32 dt) {
}

void GameRender() {
    RendererClearBuffers(gRenderer, 0xFF000000, 0.0f);
#if 0 
    RenderBuffer(gRenderer, vertices, ARRAY_LENGTH(vertices),
                                                gGameState->bitmap, {0.5f, 0.2f, -1});
#else
    RenderBuffer(gRenderer, vertices, indices, ARRAY_LENGTH(indices),
                 gGameState->bitmap, {0.5f, 0.2f, -1});
#endif 
    RendererPresent(gRenderer);
}

void GameShutdown() {
    RendererDestroy(gRenderer);
    WindowDestroy(gWindow);
}

#include "lh_game.h"

global_variable GameState *gGameState;
global_variable Window *gWindow;
global_variable Renderer *gRenderer;
global_variable PlatformWorkQueue *gQueue;
Vertex vertices[] = {
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

u32 indices[] =
{
    3,1,0,2,1,3,
    6,4,5,7,4,6,
    11,9,8, 10,9, 11,
    14, 12, 13, 15, 12, 14,
    19, 17, 16, 18, 17, 19,
    22, 20, 21, 23, 20, 22
};


void GameInit(Memory *memory, PlatformWorkQueue *queue) {
    // The GameState has to be the first element on the memory
    ASSERT(memory->used + sizeof(GameState) <= memory->size);
    gGameState = (GameState *)((u8 *)memory->data + memory->used);
    memory->used += sizeof(GameState);

    gWindow = WindowCreate(960, 540, "Last Hope 3D");
    gQueue = queue;

    gRenderer = RendererCreate(gWindow);
    RendererSetProj(gRenderer, Mat4Perspective(60.0f, 960.0f/540.0f, 0.1f, 100.0f));
    //RendererSetView(gRenderer, Mat4LookAt({-1, 0, -2}, {-1, 0, 0}, {0, 1, 0}));
    RendererSetView(gRenderer, Mat4LookAt({0, 0, -5}, {0, 0, 0}, {0, 1, 0}));

    gGameState->bitmapArena = ArenaCreate(memory, Megabytes(1));
    gGameState->bitmap = LoadTexture("../assets/test.bmp", &gGameState->bitmapArena);
}

global_variable f32 gAngle = 0.0f;
void GameUpdate(f32 dt) {
    gAngle += 20.0f * dt;
}

void GameRender() {
    RendererClearBuffers(gRenderer, 0xFF000000, 0.0f);
   

#if 1
    mat4 rotY = Mat4RotateY(RAD(gAngle));
    mat4 rotX = Mat4RotateX(RAD(gAngle));
    mat4 rotZ = Mat4RotateZ(RAD(gAngle));
    mat4 world = rotY * rotX * rotZ;
    RenderBuffer(gQueue,
                 gRenderer, vertices, indices, ARRAY_LENGTH(indices),
                 gGameState->bitmap, {0.5f, 0.2f, -1}, world);
#else 
    mat4 rotY = Mat4RotateY(RAD(gAngle));
    mat4 rotX = Mat4RotateX(RAD(gAngle));
    mat4 rotZ = Mat4RotateZ(RAD(gAngle));
    for(i32 y = -1; y < 1; y++) {
        for(i32 x = -2; x < 2; x++) {
            mat4 translation = Mat4Translate(x*3, y*3,  0);
            mat4 world = translation * rotY * rotX * rotZ;
            RenderBuffer(gQueue,
                         gRenderer, vertices, indices, ARRAY_LENGTH(indices),
                         gGameState->bitmap, {0.5f, 0.2f, -1}, world);
        }
    }
#endif
    
    RendererPresent(gRenderer);
}

void GameShutdown() {
    RendererDestroy(gRenderer);
    WindowDestroy(gWindow);
}

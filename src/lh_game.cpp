#include "lh_game.h"
#include "lh_platform.h"
#include "lh_renderer.h"
#include "lh_sound.h"
#include "lh_texture.h"
#include "lh_input.h"

//////////////////////////////////////////////////////////////////////
// TODO (manuto):
//////////////////////////////////////////////////////////////////////
// - Fix XAudio bug (closing the game fast make it go boom)
// - Create FPS camera
// - Create a simple tile base Scene
// - Try collision with the Scene
// - ...
// - ...
//////////////////////////////////////////////////////////////////////

#if 0
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
#else
Vertex vertices[] = {
    // positio           // uv        // normal
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f
};

u32 indices[] = {
    0, 1, 3,
    3, 1, 2
};

#endif


// TODO: FPS camera
vec3 cameraPosition = {2, 0, 1};
vec3 cameraFront = {0, 0, 1};
vec3 cameraRight = {1, 0, 0};
vec3 cameraUp = {0, 1, 0};
f32 cameraPitch = 0;
f32 cameraYaw = RAD(90.0f);

f32 playerSpeed = 2.0f;
f32 sensitivity = 2.0f;

void UpdateCamera(f32 dt) {
    f32 leftStickX = JoysickGetLeftStickX();
    f32 leftStickY = JoysickGetLeftStickY();
    f32 rightStickX = JoysickGetRightStickX();
    f32 rightStickY = JoysickGetRightStickY();
    
    // Right Stick movement
    cameraYaw -= (rightStickX * sensitivity) * dt;
    cameraPitch += (rightStickY * sensitivity) * dt;
    f32 maxPitch = RAD(89.0f);
    if(cameraPitch > maxPitch) {
        cameraPitch = maxPitch;
    }
    else if(cameraPitch < -maxPitch) {
        cameraPitch = -maxPitch;
    }
    cameraFront.x = cosf(cameraYaw) * cosf(cameraPitch);
    cameraFront.y = sinf(cameraPitch);
    cameraFront.z = sinf(cameraYaw) * cosf(cameraPitch);
    cameraRight = cross(cameraUp, cameraFront);


    // Left Stick movement
    cameraPosition = cameraPosition + (cameraRight * (leftStickX * playerSpeed)) * dt;
    cameraPosition = cameraPosition + (cameraFront * (leftStickY * playerSpeed)) * dt;
    
    RendererSetView(Mat4LookAt(cameraPosition, cameraPosition + cameraFront, cameraUp));
}

// TODO: map test

const i32 mapCountX = 16;
const i32 mapCountY = 16;
i32 map[mapCountY][mapCountX] = {
    0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 1, 4, 0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 6, 3, 3, 3, 0,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 8, 5, 9, 1, 4,
    2, 1, 1, 4, 2, 1, 1, 1, 1, 1, 1, 4, 0, 2, 1, 4,
    2, 1, 1, 6, 7, 1, 1, 1, 8, 5, 5, 0, 0, 2, 1, 4,
    2, 1, 1, 1, 1, 1, 1, 8, 0, 0, 0, 0, 0, 2, 1, 4,
    2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 2, 1, 4,
    2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 0, 5, 0,
    2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void DrawMap(GameState *gameState) {
    for(i32 y = 0; y < mapCountY; ++y) {
        for(i32 x = 0; x < mapCountX; ++x) {
            i32 tile = map[y][x];
            mat4 world = Mat4Identity();
            switch(tile) {
                case 0: {
                    continue; 
                } break;
                case 1: {
                    mat4 translation = Mat4Translate(x, -0.5f, y);
                    mat4 rotX = Mat4RotateX(RAD(90.0f));
                    world = translation * rotX; 
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                } break;
                case 2: {
                    mat4 translation = Mat4Translate(x + 0.5f, 0, y);
                    mat4 rotY = Mat4RotateY(RAD(-90.0f));
                    world = translation * rotY;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                } break;
                case 4: {
                    mat4 translation = Mat4Translate(x - 0.5f, 0, y);
                    mat4 rotY = Mat4RotateY(RAD(90.0f));
                    world = translation * rotY;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                } break;
                case 5: {
                    mat4 translation = Mat4Translate(x, 0, y - 0.5f);
                    world = translation;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                } break;
                case 3: {
                    mat4 translation = Mat4Translate(x, 0, y + 0.5f);
                    mat4 rotY = Mat4RotateY(RAD(180.0f));
                    world = translation * rotY;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                } break;
                case 6: {
                    mat4 translation = Mat4Translate(x - 0.5f, 0, y);
                    mat4 rotY = Mat4RotateY(RAD(90.0f));
                    world = translation * rotY;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                    translation = Mat4Translate(x, 0, y + 0.5f);
                    rotY = Mat4RotateY(RAD(180.0f));
                    world = translation * rotY;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                } break;
                case 7: {
                    mat4 translation = Mat4Translate(x + 0.5f, 0, y);
                    mat4 rotY = Mat4RotateY(RAD(-90.0f));
                    world = translation * rotY;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                    translation = Mat4Translate(x, 0, y + 0.5f);
                    rotY = Mat4RotateY(RAD(180.0f));
                    world = translation * rotY;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);

                } break;
                case 8: {
                    mat4 translation = Mat4Translate(x, 0, y - 0.5f);
                    world = translation;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                    translation = Mat4Translate(x - 0.5f, 0, y);
                    mat4 rotY = Mat4RotateY(RAD(90.0f));
                    world = translation * rotY;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                } break;
                case 9: {
                    mat4 translation = Mat4Translate(x + 0.5f, 0, y);
                    mat4 rotY = Mat4RotateY(RAD(-90.0f));
                    world = translation * rotY;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                    translation = Mat4Translate(x, 0, y - 0.5f);
                    world = translation;    
                    RendererPushWorkToQueue(vertices, indices, ARRAY_LENGTH(indices),
                                            gameState->bitmap, {0.5f, 0.2f, -1}, world);
                } break;
            }; 

        }
    }
}

void GameInit(Memory *memory) {
    // The GameState has to be the first element on the memory
    ASSERT(memory->used + sizeof(GameState) <= memory->size);
    GameState *gameState = (GameState *)memory->data;
    memory->used += sizeof(GameState);

    WindowSystemInitialize(960, 540, "Last Hope 3D");
    RendererSystemInitialize();
    SoundSystemInitialize();

    gameState->dataArena = ArenaCreate(memory, Megabytes(500));
    gameState->textureArena = ArenaCreate(memory, Megabytes(1));
    gameState->soundArena = ArenaCreate(memory, Megabytes(1));


    RendererSetProj(Mat4Perspective(60.0f, 960.0f/540.0f, 0.1f, 100.0f));
    RendererSetView(Mat4LookAt(cameraPosition, cameraPosition + cameraFront, cameraUp));

    // Load Assets
    gameState->bitmap = TextureCreate("../assets/test.bmp", &gameState->textureArena, &gameState->dataArena);
    gameState->chocolate = SoundCreate("../assets/chocolate.wav", &gameState->soundArena, &gameState->dataArena);
    gameState->music     = SoundCreate("../assets/lugia.wav", &gameState->soundArena, &gameState->dataArena);
    gameState->shoot     = SoundCreate("../assets/shoot.wav", &gameState->soundArena, &gameState->dataArena);

    SoundPlay(gameState->music, true);
}

void GameUpdate(Memory *memory, f32 dt) {
    GameState *gameState = (GameState *)memory->data;

    UpdateCamera(dt);

    if(JoysickGetButtonJustDown(JOYSTICK_BUTTON_A)) {
        SoundPlay(gameState->shoot, false);
    }
}

void GameRender(Memory *memory) {
    GameState *gameState = (GameState *)memory->data;
    RendererClearBuffers(0xFF021102, 0.0f);

    DrawMap(gameState);
    
    RendererPresent();
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

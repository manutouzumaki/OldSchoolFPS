#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "lh_defines.h"
#include "lh_memory.h"
#include "lh_math.h"
#include "lh_input.h"
#include "lh_sound.h"

struct ReadFileResult {
    void *data;
    size_t size;
};

ReadFileResult ReadFile(char *path, Arena *arena);
bool WriteFile(char *path, void *data, size_t size);

struct BMP
{
    void *data;
    u32 width;
    u32 height;
};

BMP LoadTexture(char *path, Arena *arena);


struct Renderer;

struct PlatformWorkQueue;
typedef void PlatformWorkQueueCallback(PlatformWorkQueue *queue, void *data);
void PlatformAddEntry(PlatformWorkQueue *queue, PlatformWorkQueueCallback *callback, void *data);
void PlatformCompleteAllWork(PlatformWorkQueue *queue);


void WindowSystemInitialize(i32 width, i32 height, char *title);
void WindowSystemShutdown();
void WindowSetSize(i32 width, i32 height);

struct Vertex {
    vec3 position;
    vec2 uv;
    vec3 normal;
};

struct Mesh;

void RendererSystemInitialize();
void RendererSystemShutdown();
void RendererClearBuffers(u32 color, f32 depth);
void RendererPushWorkToQueue(PlatformWorkQueue *queue, Vertex *vertices, u32 *indices,
                             i32 indicesCount, BMP bitmap, vec3 lightDir, mat4 world);
void RendererPresent(PlatformWorkQueue *queue);
void RendererSetProj(mat4 proj);
void RendererSetView(mat4 view);


struct Counter {
    u64 count;
    u64 hit;
    u64 cyclesPerHit;
};

enum CycleCounter
{
    CycleCounter_TriangleRasterizer,
    CYCLECOUNTER_COUNT 
};

extern Counter *DEBUG_counters;

#define START_CYCLE_COUNTER(id) u64 StartCycleCounter_##id = __rdtsc()
#define END_CYCLE_COUNTER(id) DEBUG_counters[CycleCounter_##id].count += (__rdtsc() - StartCycleCounter_##id); DEBUG_counters[CycleCounter_##id].hit++

#endif

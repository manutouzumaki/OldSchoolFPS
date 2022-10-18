#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "lh_defines.h"
#include "lh_memory.h"
#include "lh_math.h"


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


struct PlatformWorkQueue;
typedef void PlatformWorkQueueCallback(PlatformWorkQueue *queue, void *data);
void PlatformAddEntry(PlatformWorkQueue *queue, PlatformWorkQueueCallback *callback, void *data);
void PlatformCompleteAllWork(PlatformWorkQueue *queue);

struct Window;

Window *WindowCreate(i32 width, i32 height, char *title);
void WindowDestroy(Window *window);
void WindowSetSize(i32 width, i32 height);

struct Vertex {
    vec3 position;
    vec2 uv;
    vec3 normal;
};

struct Renderer;
struct Mesh;

Renderer *RendererCreate(Window *window);
void RendererDestroy(Renderer *renderer);
void RendererClearBuffers(Renderer *renderer, u32 color, f32 depth);
void RendererPresent(Renderer *renderer);
void RendererSetProj(Renderer *renderer, mat4 proj);
void RendererSetView(Renderer *renderer, mat4 view);
void RenderMesh(Renderer *renderer, Mesh *mesh, BMP bitmap, vec3 lightDir);
void RenderBuffer(Renderer *renderer, Vertex *vertices, i32 verticesCount, BMP bitmap, vec3 lightDir);
void RenderBuffer(PlatformWorkQueue *queue, Renderer *renderer, Vertex *vertices, u32 *indices,
                  i32 indicesCount, BMP bitmap, vec3 lightDir, mat4 world); 
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

#ifndef _LH_RENDERER_H_
#define _LH_RENDERER_H_

#include "lh_defines.h"
#include "lh_math.h"

struct Texture;

struct Vertex {
    vec3 position;
    vec2 uv;
    vec3 normal;
};

void RendererSystemInitialize();
void RendererSystemShutdown();
void RendererClearBuffers(u32 color, f32 depth);
void RendererPushWorkToQueue(Vertex *vertices, u32 *indices,
                             i32 indicesCount, Texture *bitmap, vec3 lightDir, mat4 world);
void RendererPresent();
void RendererSetProj(mat4 proj);
void RendererSetView(mat4 view);

#endif

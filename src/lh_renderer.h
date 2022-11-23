#ifndef _LH_RENDERER_H_
#define _LH_RENDERER_H_

#include "lh_header_renderer.h"
#include "lh_gpu_renderer.h"
#include "lh_cpu_renderer.h"

struct Renderer {
    RendererType type;
    CPURenderer cpuRenderer;
    GPURenderer gpuRenderer;
    
    ID3D11Device *device;
    ID3D11DeviceContext *deviceContext;
    IDXGISwapChain *swapChain;
    ID3D11RenderTargetView *renderTargetView;
};

void RendererSystemInitialize();
void RendererSystemShutdown();
void RendererClearBuffers(u32 color, f32 depth);
void RendererDrawRectFast(i32 xPos, i32 yPos, i32 width, i32 height, Texture *bitmap);
void RendererDrawAnimatedRectFast(i32 x, i32 y, i32 width, i32 height, Texture *bitmap, i32 spriteW, i32 spriteH, i32 frame);
void RendererFlushWorkQueue();
void RendererPresent();
void RendererSetProj(mat4 proj);
void RendererSetView(mat4 view);

Shader *RendererCreateShader(char *vertexPath, char *pixelPath, Arena *arena);
void RendererUpdateShaderData(Shader *shader, ConstantBuffer *buffer);
Mesh *RendererCreateMesh(Vertex *vertices, i32 verticesCount, u32 *indices, i32 indicesCount, mat4 world, Arena *arena);
Texture *RendererCreateTexture(char *path, Arena *objArena, Arena *dataArena);

void RendererSetShader(Shader *shader);

void RendererDrawMesh(Mesh *mesh, Texture *texture, vec3 *lights, i32 lightsCount,
                      vec3 viewPos, bool writeDepthBuffer, f32 repeatU, f32 repeatV,
                      ConstantBuffer *constBuffer, Shader *shader);


void RendererSetDepthBuffer(bool value);
void RendererSetLighting(bool value);

#endif

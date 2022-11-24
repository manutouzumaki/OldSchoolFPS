#ifndef _LH_CPU_RENDERER_H_
#define _LH_CPU_RENDERER_H_

#include "lh_header_renderer.h"

struct RenderWork {
    Vertex *vertices;
    i32 verticesCount;
    u32 *indices;
    i32 indicesCount;
    Texture *bitmap;
    vec3 *lights;
    i32 lightsCount;
    vec3 viewPos;
    mat4 world;
    bool writeDepthBuffer;
    f32 repeatU;
    f32 repeatV;
};

struct CPURenderer {
    u32 *colorBuffer;
    f32 *depthBuffer;
    i32 bufferWidth;
    i32 bufferHeight;
    mat4 view;
    mat4 proj;
    RenderWork *workArray;
    i32 workCount;

    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader  *pixelShader;
    ID3D11InputLayout  *inputLayout;
    ID3D11Buffer *vertexBuffer;

    ID3D11Texture2D *backBuffer;
    ID3D11ShaderResourceView *colorMap;
    ID3D11SamplerState *colorMapSampler;
};


void CPURendererInitialize();
void CPURendererShutdown();

void CPURendererDrawMesh(Mesh *mesh, mat4 world, Texture *texture, vec3 *lights, i32 lightsCount,
                         vec3 viewPos, bool writeDepthBuffer, f32 repeatU, f32 repeatV);
void CPUDrawRectFast(i32 x, i32 y, i32 width, i32 height, Texture *bitmap);
void CPUDrawAnimatedRectFast(i32 x, i32 y, i32 width, i32 height, Texture *bitmap, i32 spriteW, i32 spriteH, i32 frame);
void CPUDrawRect(i32 xPos, i32 yPos, i32 width, i32 height, Texture *bitmap);


#endif

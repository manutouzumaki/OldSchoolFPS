#ifndef _LH_GPU_RENDERER_H_
#define _LH_GPU_RENDERER_H_

#include "lh_header_renderer.h"

struct ConstantBuffer2D {
    mat4 world;
    mat4 view;
    mat4 proj;
    f32 ratioU;
    f32 ratioV;
    f32 frameX;
    f32 frameY;
};

struct GPURenderer {
    i32 bufferWidth;
    i32 bufferHeight;
    // 2d rendering
    ID3D11Buffer *quad;
    ID3D11Buffer *buffer;
    ConstantBuffer2D constBuffer;

    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader  *pixelShader;
    ID3D11InputLayout  *inputLayout;
    
    ID3D11DepthStencilView *depthStencilView;
    ID3D11RasterizerState *wireFrameRasterizer;
    ID3D11RasterizerState *fillRasterizerCullBack;
    ID3D11RasterizerState *fillRasterizerCullFront;
    ID3D11RasterizerState *fillRasterizerCullNone;
    ID3D11DepthStencilState *depthStencilOn;
    ID3D11DepthStencilState *depthStencilOff;
    ID3D11BlendState *alphaBlendEnable;
    ID3D11BlendState *alphaBlendDisable;
};

void GPURendererInitialize();
void GPURendererShutdown();
void GPURendererDrawMesh(Mesh *mesh, Texture *texture, vec3 *lights, i32 lightsCount,
                         vec3 viewPos, bool writeDepthBuffer, f32 repeatU, f32 repeatV);

void GPURendererDrawRect(i32 xPos, i32 yPos, i32 width, i32 height, Texture *bitmap);
void GPUDrawAnimatedRect(i32 x, i32 y, i32 width, i32 height, Texture *bitmap, i32 spriteW, i32 spriteH, i32 frame);

#endif

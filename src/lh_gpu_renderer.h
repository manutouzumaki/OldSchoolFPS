#ifndef _LH_GPU_RENDERER_H_
#define _LH_GPU_RENDERER_H_

#include "lh_header_renderer.h"

struct GPURenderer {
    i32 bufferWidth;
    i32 bufferHeight;
    
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

#endif

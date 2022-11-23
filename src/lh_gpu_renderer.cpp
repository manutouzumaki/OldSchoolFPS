#include "lh_gpu_renderer.h"
#include "lh_renderer.h"
#include "lh_platform.h"

extern Window gWindow;
extern Renderer gRenderer;

void GPURendererInitialize() {
    GPURenderer *gpuRenderer = &gRenderer.gpuRenderer;

    gpuRenderer->bufferWidth = gWindow.width;
    gpuRenderer->bufferHeight = gWindow.height;
  
    HRESULT result;
    // create the depth stencil texture
    ID3D11Texture2D *depthStencilTexture = 0;
    D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
    ZeroMemory(&depthStencilTextureDesc, sizeof(depthStencilTextureDesc));
    depthStencilTextureDesc.Width = gpuRenderer->bufferWidth;
    depthStencilTextureDesc.Height = gpuRenderer->bufferHeight;
    depthStencilTextureDesc.MipLevels = 1;
    depthStencilTextureDesc.ArraySize = 1;
    depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilTextureDesc.SampleDesc.Count = 1;
    depthStencilTextureDesc.SampleDesc.Quality = 0;
    depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilTextureDesc.CPUAccessFlags = 0;
    depthStencilTextureDesc.MiscFlags = 0;

    // create depth stencil states
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    // Depth test parameters
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    // Stencil test parameters
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    
    // Stencil operations if pixel is front-facing
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    gRenderer.device->CreateDepthStencilState(&depthStencilDesc, &gpuRenderer->depthStencilOn);
    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.StencilEnable = false;
    gRenderer.device->CreateDepthStencilState(&depthStencilDesc, &gpuRenderer->depthStencilOff);
    
    // TODO: check if this is necesary as this point in time
    //gRenderer.deviceContext->OMSetDepthStencilState(gpuRenderer->depthStencilOn, 1);

    result = gRenderer.device->CreateTexture2D(&depthStencilTextureDesc, NULL, &depthStencilTexture);
    
    // create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    result = gRenderer.device->CreateDepthStencilView(depthStencilTexture, &descDSV, &gpuRenderer->depthStencilView);
    if(depthStencilTexture)
    {
        depthStencilTexture->Release();
    }

    // TODO: check if this is necesary as this point in time
    //gRenderer.deviceContext->OMSetRenderTargets(1, &gRenderer.renderTargetView, gpuRenderer->depthStencilView);

    // Alpha blending
    D3D11_BLEND_DESC blendStateDesc = {};
    blendStateDesc.RenderTarget[0].BlendEnable = true;
    blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    gRenderer.device->CreateBlendState(&blendStateDesc, &gpuRenderer->alphaBlendEnable);

    blendStateDesc.RenderTarget[0].BlendEnable = false;
    blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    gRenderer.device->CreateBlendState(&blendStateDesc, &gpuRenderer->alphaBlendDisable);

    // TODO: check if this is necesary as this point in time
    //gRenderer.deviceContext->OMSetBlendState(gpuRenderer->alphaBlendDisable, 0, 0xffffffff);

    // Create Rasterizers Types
    D3D11_RASTERIZER_DESC fillRasterizerFrontDesc = {};
    fillRasterizerFrontDesc.FillMode = D3D11_FILL_SOLID;
    fillRasterizerFrontDesc.CullMode = D3D11_CULL_FRONT;
    fillRasterizerFrontDesc.DepthClipEnable = true;
    gRenderer.device->CreateRasterizerState(&fillRasterizerFrontDesc, &gpuRenderer->fillRasterizerCullFront);

    D3D11_RASTERIZER_DESC fillRasterizerBackDesc = {};
    fillRasterizerBackDesc.FillMode = D3D11_FILL_SOLID;
    fillRasterizerBackDesc.CullMode = D3D11_CULL_BACK;
    fillRasterizerBackDesc.DepthClipEnable = true;
    gRenderer.device->CreateRasterizerState(&fillRasterizerBackDesc, &gpuRenderer->fillRasterizerCullBack);

    D3D11_RASTERIZER_DESC fillRasterizerNoneDesc = {};
    fillRasterizerNoneDesc.FillMode = D3D11_FILL_SOLID;
    fillRasterizerNoneDesc.CullMode = D3D11_CULL_NONE;
    fillRasterizerNoneDesc.DepthClipEnable = true;
    gRenderer.device->CreateRasterizerState(&fillRasterizerNoneDesc, &gpuRenderer->fillRasterizerCullNone);

    D3D11_RASTERIZER_DESC wireFrameRasterizerDesc = {};
    wireFrameRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    wireFrameRasterizerDesc.CullMode = D3D11_CULL_NONE;
    wireFrameRasterizerDesc.DepthClipEnable = true;
    gRenderer.device->CreateRasterizerState(&wireFrameRasterizerDesc, &gpuRenderer->wireFrameRasterizer);
}

void GPURendererShutdown() {
    GPURenderer *gpuRenderer = &gRenderer.gpuRenderer;
    gpuRenderer->depthStencilView->Release();
    gpuRenderer->wireFrameRasterizer->Release();
    gpuRenderer->fillRasterizerCullBack->Release();
    gpuRenderer->fillRasterizerCullFront->Release();
    gpuRenderer->fillRasterizerCullNone->Release();
    gpuRenderer->depthStencilOn->Release();
    gpuRenderer->depthStencilOff->Release();
    gpuRenderer->alphaBlendEnable->Release();
    gpuRenderer->alphaBlendDisable->Release();
}

void GPURendererDrawMesh(Mesh *mesh, Texture *texture, vec3 *lights, i32 lightsCount,
                         vec3 viewPos, bool writeDepthBuffer, f32 repeatU, f32 repeatV) {
    u32 stride = sizeof(Vertex);
    u32 offset = 0;
    gRenderer.deviceContext->IASetVertexBuffers(0, 1, &mesh->gpuVertex, &stride, &offset);
    gRenderer.deviceContext->IASetIndexBuffer(mesh->gpuIndices, DXGI_FORMAT_R32_UINT, 0);
    gRenderer.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gRenderer.deviceContext->PSSetShaderResources(0, 1, &texture->colorMap);
    gRenderer.deviceContext->PSSetSamplers(0, 1, &texture->colorMapSampler);
    gRenderer.deviceContext->DrawIndexed(mesh->indicesCount, 0, 0);
}

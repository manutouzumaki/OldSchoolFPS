#include "lh_gpu_renderer.h"
#include "lh_renderer.h"
#include "lh_platform.h"
#include <d3dcompiler.h>

extern Window gWindow;
extern Renderer gRenderer;

// Vertex Shader
global_variable char *vertexShaderSource =
"cbuffer CBuffer : register(b0)\n"
"{\n"
"   matrix world;\n"
"   matrix view;\n"
"   matrix proj;\n"
"   float ratioU;\n"
"   float ratioV;\n"
"   float frameX;\n"
"   float frameY;\n"
"}\n"
"struct VS_Input\n"
"{\n"
"   float4 pos : POSITION;\n"
"   float2 tex0 : TEXCOORD0;\n"
"};\n"
"struct PS_Input\n"
"{\n"
"   float4 pos : SV_POSITION;\n"
"   float2 tex0 : TEXCOORD0;\n"
"   float ratioU : TEXCOORD1;\n"
"   float ratioV : TEXCOORD2;\n"
"   float frameX : TEXCOORD3;\n"
"   float frameY : TEXCOORD4;\n"
"};\n"
"PS_Input VS_Main( VS_Input vertex )\n"
"{\n"
"   PS_Input vsOut = ( PS_Input )0;\n"
"   vsOut.pos = mul(vertex.pos, world);\n"
"   vsOut.pos = mul(vsOut.pos, view);\n"
"   vsOut.pos = mul(vsOut.pos, proj);\n"
"   vsOut.tex0 = vertex.tex0;\n"
"   vsOut.ratioU = ratioU;\n"
"   vsOut.ratioV = ratioV;\n"
"   vsOut.frameX = frameX;\n"
"   vsOut.frameY = frameY;\n"
"   return vsOut;\n"
"}\0";

// Pixel Shader
global_variable char *pixelShaderSource  =
"Texture2D colorMap : register( t0 );\n"
"SamplerState colorSampler : register( s0 );\n"
"struct PS_Input\n"
"{\n"
"   float4 pos : SV_POSITION;\n"
"   float2 tex0 : TEXCOORD0;\n"
"   float ratioU : TEXCOORD1;\n"
"   float ratioV : TEXCOORD2;\n"
"   float frameX : TEXCOORD3;\n"
"   float frameY : TEXCOORD4;\n"
"};\n"
"float4 PS_Main( PS_Input frag ) : SV_TARGET\n"
"{\n"
"   float2 tex;"
"   tex.x = (frag.tex0.x*frag.ratioU) + frag.frameX;\n"
"   tex.y = (frag.tex0.y*frag.ratioV) + frag.frameY;\n"
"   float4 color = colorMap.Sample(colorSampler, tex.xy);\n"
"   return color;\n"
"}\0";

internal
i32 StringLength(char * String)
{
    i32 Count = 0;
    while(*String++)
    {
        ++Count;
    }
    return Count;
}

void GPURendererInitialize() {
    GPURenderer *gpuRenderer = &gRenderer.gpuRenderer;

    gpuRenderer->bufferWidth = gWindow.width;
    gpuRenderer->bufferHeight = gWindow.height;
    gpuRenderer->constBuffer.world = Mat4Identity();
    gpuRenderer->constBuffer.view = Mat4Identity();
    gpuRenderer->constBuffer.proj = Mat4Ortho(-gWindow.width/2, gWindow.width/2,
                                              -gWindow.height/2, gWindow.height/2,
                                              0, 100);
    gpuRenderer->constBuffer.ratioU = 1.0f;
    gpuRenderer->constBuffer.ratioV = 1.0f;
    gpuRenderer->constBuffer.frameX = 0;
    gpuRenderer->constBuffer.frameY = 0;
  
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

    // Create a quad buffer for draw the ui
    ID3DBlob *vertexShaderCompiled = 0;
    ID3DBlob *errorVertexShader    = 0;
    result = D3DCompile((void *)vertexShaderSource,
                        (SIZE_T)StringLength(vertexShaderSource),
                        0, 0, 0, "VS_Main", "vs_4_0",
                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                        &vertexShaderCompiled, &errorVertexShader);
    if(errorVertexShader != 0)
    {
        errorVertexShader->Release();
    }

    ID3DBlob *pixelShaderCompiled = 0;
    ID3DBlob *errorPixelShader    = 0;
    result = D3DCompile((void *)pixelShaderSource,
                        (SIZE_T)StringLength(pixelShaderSource),
                        0, 0, 0, "PS_Main", "ps_4_0",
                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                        &pixelShaderCompiled, &errorPixelShader);
    if(errorPixelShader != 0)
    {
        errorPixelShader->Release();
    }

    // Create the Vertex Shader.
    result = gRenderer.device->CreateVertexShader(vertexShaderCompiled->GetBufferPointer(),
                                                  vertexShaderCompiled->GetBufferSize(), 0,
                                                  &gpuRenderer->vertexShader);
    // Create the Input layout.
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
         0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
        0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    u32 totalLayoutElements = ARRAY_LENGTH(inputLayoutDesc);
    result = gRenderer.device->CreateInputLayout(inputLayoutDesc,
                                                 totalLayoutElements,
                                                 vertexShaderCompiled->GetBufferPointer(),
                                                 vertexShaderCompiled->GetBufferSize(),
                                                 &gpuRenderer->inputLayout);
    // Create Pixel Shader.
    result = gRenderer.device->CreatePixelShader(pixelShaderCompiled->GetBufferPointer(),
                                                 pixelShaderCompiled->GetBufferSize(), 0,
                                                 &gpuRenderer->pixelShader); 
    vertexShaderCompiled->Release();
    pixelShaderCompiled->Release();

    // Create Vertex Buffer
    VertexD3D11 vertices[] = 
    {
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f,  0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.0f,  0.0f, 0.0f, 0.0f, 0.0f,
         0.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };

    // buffer description
    D3D11_BUFFER_DESC vertexDesc = {};
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(VertexD3D11) * 6;
    // pass the buffer data (Vertices).
    D3D11_SUBRESOURCE_DATA resourceData = {};
    resourceData.pSysMem = vertices;
    // Create the VertexBuffer
    result = gRenderer.device->CreateBuffer(&vertexDesc, &resourceData, &gpuRenderer->quad);

    // Initialize constant buffer
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = sizeof(ConstantBuffer2D);
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    result = gRenderer.device->CreateBuffer(&constantBufferDesc, 0, &gpuRenderer->buffer);
    if(FAILED(result)) {
        OutputDebugString("ERROR Creating constant buffer\n");
        ASSERT(false);
    }

}

void GPURendererShutdown() {
    GPURenderer *gpuRenderer = &gRenderer.gpuRenderer;
    gpuRenderer->buffer->Release();
    gpuRenderer->quad->Release();
    gpuRenderer->vertexShader->Release();
    gpuRenderer->inputLayout->Release();
    gpuRenderer->pixelShader->Release();
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

void GPURendererDrawRect(i32 xPos, i32 yPos, i32 width, i32 height, Texture *bitmap) {
    GPURenderer *gpuRenderer = &gRenderer.gpuRenderer;
    xPos -= (gpuRenderer->bufferWidth/2);
    yPos -= (gpuRenderer->bufferHeight/2);
    mat4 world = Mat4Translate(xPos, yPos, 0) * Mat4Scale(width, height, 1.0f);
    gpuRenderer->constBuffer.world = world;
    gpuRenderer->constBuffer.ratioU = 1.0f;
    gpuRenderer->constBuffer.ratioV = 1.0f;
    gpuRenderer->constBuffer.frameX = 0;
    gpuRenderer->constBuffer.frameY = 0;
    gRenderer.deviceContext->UpdateSubresource(gpuRenderer->buffer, 0, 0, &gpuRenderer->constBuffer, 0, 0);
    gRenderer.deviceContext->VSSetConstantBuffers(0, 1, &gpuRenderer->buffer); 
    gRenderer.deviceContext->IASetInputLayout(gpuRenderer->inputLayout);
    gRenderer.deviceContext->VSSetShader(gpuRenderer->vertexShader, 0, 0);
    gRenderer.deviceContext->PSSetShader(gpuRenderer->pixelShader,  0, 0);
    gRenderer.deviceContext->PSSetShaderResources(0, 1, &bitmap->colorMap);
    gRenderer.deviceContext->PSSetSamplers(0, 1, &bitmap->colorMapSampler);
    u32 stride = sizeof(VertexD3D11);
    u32 offset = 0;
    gRenderer.deviceContext->IASetVertexBuffers(0, 1, &gpuRenderer->quad, &stride, &offset);
    gRenderer.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gRenderer.deviceContext->Draw(6, 0);
}


void GPUDrawAnimatedRect(i32 xPos, i32 yPos, i32 width, i32 height, Texture *bitmap, i32 spriteW, i32 spriteH, i32 frame) {
    GPURenderer *gpuRenderer = &gRenderer.gpuRenderer;
    xPos -= (gpuRenderer->bufferWidth/2);
    yPos -= (gpuRenderer->bufferHeight/2);
    mat4 world = Mat4Translate(xPos, yPos, 0) * Mat4Scale(width, height, 1.0f);
    gpuRenderer->constBuffer.world = world;
    gpuRenderer->constBuffer.ratioU = (f32)spriteW/(f32)bitmap->width;
    gpuRenderer->constBuffer.ratioV = (f32)spriteH/(f32)bitmap->height;
    gpuRenderer->constBuffer.frameX = (f32)(frame % (bitmap->width / spriteW)) * gpuRenderer->constBuffer.ratioU;
    gpuRenderer->constBuffer.frameY = (f32)(frame / (bitmap->width / spriteW)) * gpuRenderer->constBuffer.ratioU;
    gRenderer.deviceContext->UpdateSubresource(gpuRenderer->buffer, 0, 0, &gpuRenderer->constBuffer, 0, 0);
    gRenderer.deviceContext->VSSetConstantBuffers(0, 1, &gpuRenderer->buffer); 
    gRenderer.deviceContext->IASetInputLayout(gpuRenderer->inputLayout);
    gRenderer.deviceContext->VSSetShader(gpuRenderer->vertexShader, 0, 0);
    gRenderer.deviceContext->PSSetShader(gpuRenderer->pixelShader,  0, 0);
    gRenderer.deviceContext->PSSetShaderResources(0, 1, &bitmap->colorMap);
    gRenderer.deviceContext->PSSetSamplers(0, 1, &bitmap->colorMapSampler);
    u32 stride = sizeof(VertexD3D11);
    u32 offset = 0;
    gRenderer.deviceContext->IASetVertexBuffers(0, 1, &gpuRenderer->quad, &stride, &offset);
    gRenderer.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gRenderer.deviceContext->Draw(6, 0);

}

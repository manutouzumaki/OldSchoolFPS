#include <windows.h>
#include "lh_renderer.h"
#include "lh_platform.h"
#include "lh_file.h"
#include "lh_memory.h"
#include <math.h>
#include <d3dcompiler.h>
#include <immintrin.h>
#include <xmmintrin.h>

extern Window gWindow;
Renderer gRenderer;

internal
void InitializeD3D11() {
    i32 clientWidth = gWindow.width;
    i32 clientHeight = gWindow.height;

    // - 1: Define the device types and feature level we want to check for.
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_SOFTWARE
    };
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    i32 driverTypesCount = ARRAY_LENGTH(driverTypes);
    i32 featureLevelsCount = ARRAY_LENGTH(featureLevels);


    // - 2: create the d3d11 device, rendering context, and swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = clientWidth;
    swapChainDesc.BufferDesc.Height = clientHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = (i32)FPS;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = gWindow.hwnd;
    swapChainDesc.Windowed = true;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    D3D_FEATURE_LEVEL featureLevel;
    D3D_DRIVER_TYPE driverType;
    HRESULT result;
    for(u32 driver = 0; driver < driverTypesCount; ++driver) {
        result = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[driver], NULL, 0, featureLevels, featureLevelsCount, D3D11_SDK_VERSION, &swapChainDesc,
                                               &gRenderer.swapChain, &gRenderer.device, &featureLevel, &gRenderer.deviceContext);
        if(SUCCEEDED(result)) {
            driverType = driverTypes[driver];
            break;
        }
    }

    // - 3: Create render target view.
    ID3D11Texture2D *backBufferTexture = NULL;
    result = gRenderer.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBufferTexture);
    result = gRenderer.device->CreateRenderTargetView(backBufferTexture, 0, &gRenderer.renderTargetView);
    if(backBufferTexture) {
        backBufferTexture->Release();
    }

    OutputDebugString("D3D11 Initialized\n");

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = gWindow.width;
    viewport.Height = gWindow.height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    gRenderer.deviceContext->RSSetViewports(1, &viewport);
}

void RendererSystemInitialize() {
    InitializeD3D11();
    GPURendererInitialize();
    CPURendererInitialize();
    gRenderer.type = RENDERER_DIRECTX;

    switch(gRenderer.type) {
        case RENDERER_CPU: {
            gRenderer.deviceContext->OMSetRenderTargets(1, &gRenderer.renderTargetView, 0);
            gRenderer.deviceContext->PSSetShaderResources(0, 1, &gRenderer.cpuRenderer.colorMap);
            gRenderer.deviceContext->PSSetSamplers(0, 1, &gRenderer.cpuRenderer.colorMapSampler);
            u32 stride = sizeof(VertexD3D11);
            u32 offset = 0;
            gRenderer.deviceContext->IASetInputLayout(gRenderer.cpuRenderer.inputLayout);
            gRenderer.deviceContext->IASetVertexBuffers(0, 1, &gRenderer.cpuRenderer.vertexBuffer, &stride, &offset);
            gRenderer.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            gRenderer.deviceContext->VSSetShader(gRenderer.cpuRenderer.vertexShader, 0, 0);
            gRenderer.deviceContext->PSSetShader(gRenderer.cpuRenderer.pixelShader,  0, 0);
                           
        }break;
        case RENDERER_DIRECTX: {
            gRenderer.deviceContext->OMSetRenderTargets(1, &gRenderer.renderTargetView, gRenderer.gpuRenderer.depthStencilView);
            gRenderer.deviceContext->OMSetDepthStencilState(gRenderer.gpuRenderer.depthStencilOn, 1);
            gRenderer.deviceContext->OMSetBlendState(gRenderer.gpuRenderer.alphaBlendEnable, 0, 0xffffffff);
            gRenderer.deviceContext->RSSetState(gRenderer.gpuRenderer.fillRasterizerCullNone);            
        }break;  
    };
}

void RendererSystemShutdown() {
    CPURendererShutdown();
    GPURendererShutdown();
    gRenderer.renderTargetView->Release();
    gRenderer.swapChain->Release();
    gRenderer.deviceContext->Release();
    gRenderer.device->Release();
}

void RendererSetDepthBuffer(bool value) {
    if(value) {
        gRenderer.deviceContext->OMSetDepthStencilState(gRenderer.gpuRenderer.depthStencilOn, 1);
    }
    else {
 
        gRenderer.deviceContext->OMSetDepthStencilState(gRenderer.gpuRenderer.depthStencilOff, 1);
    }
}

void RendererClearBuffers(u32 color, f32 depth) {

    switch(gRenderer.type) {
        case RENDERER_CPU: {
            CPURenderer *cpuRenderer = &gRenderer.cpuRenderer;
            __m128i pixelColor = _mm_set1_epi32(color);
            __m128 depthValue = _mm_set1_ps(depth);
            for(i32 y = 0; y < cpuRenderer->bufferHeight; ++y) {
                for(i32 x = 0; x < cpuRenderer->bufferWidth; x += 4) {
                    u32 *pixelPt = cpuRenderer->colorBuffer + ((y * cpuRenderer->bufferWidth) + x);
                    f32 *depthPt = cpuRenderer->depthBuffer + ((y * cpuRenderer->bufferWidth) + x);
                    _mm_storeu_si128((__m128i *)pixelPt, pixelColor);
                    _mm_storeu_ps(depthPt, depthValue);
                }
            }
        } break;
        case RENDERER_DIRECTX: {
            GPURenderer *gpuRenderer = &gRenderer.gpuRenderer;
            gRenderer.deviceContext->ClearDepthStencilView(gpuRenderer->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
        } break;
    }

    float ClearColor[4] = {1.0f, 0.5f, 1.0f, 1.0f};
    gRenderer.deviceContext->ClearRenderTargetView(gRenderer.renderTargetView, ClearColor);
}

void RendererDrawMesh(Mesh *mesh, Texture *texture, vec3 *lights, i32 lightsCount,
                      vec3 viewPos, bool writeDepthBuffer, f32 repeatU, f32 repeatV,
                      ConstantBuffer *constBuffer, Shader *shader) {
    switch(gRenderer.type) {
        case RENDERER_CPU: {
            CPURendererDrawMesh(mesh, texture, lights, lightsCount, viewPos, writeDepthBuffer, repeatU, repeatV);
        } break;
        case RENDERER_DIRECTX: {
            constBuffer->world = mesh->world;
            RendererSetShader(shader);
            RendererUpdateShaderData(shader, constBuffer);
            RendererSetDepthBuffer(writeDepthBuffer);
            GPURendererDrawMesh(mesh, texture, lights, lightsCount, viewPos, writeDepthBuffer, repeatU, repeatV);
        } break; 
    };
}

void RendererDrawRectFast(i32 xPos, i32 yPos, i32 width, i32 height, Texture *bitmap) {
    switch(gRenderer.type) {
        case RENDERER_CPU: {
            CPUDrawRectFast(xPos, yPos, width, height, bitmap);
        } break;
        case RENDERER_DIRECTX: {
        } break; 
    };
}

void RendererDrawAnimatedRectFast(i32 x, i32 y, i32 width, i32 height, Texture *bitmap, i32 spriteW, i32 spriteH, i32 frame) {
    switch(gRenderer.type) {
        case RENDERER_CPU: {
            CPUDrawAnimatedRectFast(x, y, width, height, bitmap, spriteW, spriteH, frame);
        } break;
        case RENDERER_DIRECTX: {
        } break; 
    };
}

void RendererPresent() {
    switch(gRenderer.type) {
        case RENDERER_CPU: {
            D3D11_MAPPED_SUBRESOURCE buffer;
            gRenderer.deviceContext->Map(gRenderer.cpuRenderer.backBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &buffer);
            memcpy(buffer.pData, gRenderer.cpuRenderer.colorBuffer, gRenderer.cpuRenderer.bufferWidth*gRenderer.cpuRenderer.bufferHeight*sizeof(u32));
            gRenderer.deviceContext->Unmap(gRenderer.cpuRenderer.backBuffer, 0);
            gRenderer.deviceContext->Draw(6, 0);
        } break;
        case RENDERER_DIRECTX: {

        } break;
    }
    gRenderer.swapChain->Present(1, 0);
}

void RendererSetProj(mat4 proj) {
    gRenderer.cpuRenderer.proj = proj;
} 

void RendererSetView(mat4 view) {
    gRenderer.cpuRenderer.view = view;
}

Shader *RendererCreateShader(char *vertexPath, char *pixelPath, Arena *arena) {
    Shader *shader = ArenaPushStruct(arena, Shader);
    ReadFileResult vertexSrc = ReadFile(vertexPath);
    ReadFileResult pixelSrc = ReadFile(pixelPath); 

    HRESULT result;
    ID3DBlob *vertexShaderCompiled = 0;
    ID3DBlob *errorVertexShader    = 0;
    result = D3DCompile(vertexSrc.data,
                        vertexSrc.size,
                        0, 0, 0, "VS_Main", "vs_4_0",
                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                        &vertexShaderCompiled, &errorVertexShader);
    if(errorVertexShader != 0)
    {
        OutputDebugString("ERROR COMPILING VERTEX SHADER ...\n");
        OutputDebugString((char *)errorVertexShader->GetBufferPointer());
        errorVertexShader->Release();
        ASSERT(false);
    }
    ID3DBlob *pixelShaderCompiled = 0;
    ID3DBlob *errorPixelShader    = 0;
    result = D3DCompile(pixelSrc.data,
                        pixelSrc.size,
                        0, 0, 0, "PS_Main", "ps_4_0",
                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                        &pixelShaderCompiled, &errorPixelShader);
    if(errorPixelShader != 0)
    {
        OutputDebugString("ERROR COMPILING PIXEL SHADER ...\n");
        OutputDebugString((char *)errorPixelShader->GetBufferPointer());
        errorPixelShader->Release();
        ASSERT(false);
    }

    // Create the Vertex Shader.
    result = gRenderer.device->CreateVertexShader(vertexShaderCompiled->GetBufferPointer(),
                                                  vertexShaderCompiled->GetBufferSize(), 0,
                                                  &shader->vertex);
    // Create the Input layout.
    D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
         0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
         0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,
         0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    u32 totalLayoutElements = ARRAY_LENGTH(inputLayoutDesc);
    result = gRenderer.device->CreateInputLayout(inputLayoutDesc,
                                                 totalLayoutElements,
                                                 vertexShaderCompiled->GetBufferPointer(),
                                                 vertexShaderCompiled->GetBufferSize(),
                                                 &shader->input);
    if(FAILED(result)) {
        ASSERT(!"ERROR Creating INPUT LAYOUT");
    }
    // Create Pixel Shader.
    result = gRenderer.device->CreatePixelShader(pixelShaderCompiled->GetBufferPointer(),
                                                 pixelShaderCompiled->GetBufferSize(), 0,
                                                 &shader->pixel); 
    vertexShaderCompiled->Release();
    pixelShaderCompiled->Release();
    
    free(vertexSrc.data);
    free(pixelSrc.data);

    // Initialize constant buffer
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    result = gRenderer.device->CreateBuffer(&constantBufferDesc, 0, &shader->buffer);
    if(FAILED(result)) {
        OutputDebugString("ERROR Creating constant buffer\n");
        ASSERT(false);
    }

    return shader;
}

void RendererUpdateShaderData(Shader *shader, ConstantBuffer *buffer) {
    gRenderer.deviceContext->UpdateSubresource(shader->buffer, 0, 0, buffer, 0, 0);
    gRenderer.deviceContext->VSSetConstantBuffers(0, 1, &shader->buffer);
};


Mesh *RendererCreateMesh(Vertex *vertices, i32 verticesCount, u32 *indices, i32 indicesCount, mat4 world, Arena *arena) {
    
    Mesh *mesh = ArenaPushStruct(arena, Mesh);

    mesh->vertices = vertices;
    mesh->verticesCount = 0;
    mesh->indices = indices;
    mesh->indicesCount = indicesCount;
    mesh->world = world;
    // mesh->transform = TODO: ....
   

    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(resourceData));
    resourceData.pSysMem = vertices;

    // vertex buffer initialization 
    D3D11_BUFFER_DESC vertexDesc;
    ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = sizeof(Vertex) * verticesCount;
    HRESULT result = gRenderer.device->CreateBuffer(&vertexDesc, &resourceData, &mesh->gpuVertex);
    if(FAILED(result)) {
        ASSERT(!"ERROR Creating VERTEX BUFFER");
    }
    // indices buffer initialization
    D3D11_BUFFER_DESC indexDesc;
    ZeroMemory(&indexDesc, sizeof(indexDesc));
    indexDesc.Usage = D3D11_USAGE_DEFAULT;
    indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexDesc.ByteWidth = sizeof(u32) * indicesCount;
    indexDesc.CPUAccessFlags = 0;
    resourceData.pSysMem = indices;
    result = gRenderer.device->CreateBuffer(&indexDesc, &resourceData, &mesh->gpuIndices);
    if(FAILED(result)) {
        ASSERT(!"ERROR Creating INDEX BUFFER");
    }

    return mesh;
}

internal
u32 BitScanForward(u32 mask)
{
    unsigned long shift = 0;
    _BitScanForward(&shift, mask);
    return (u32)shift;
}

Texture *RendererCreateTexture(char *path, Arena *objArena, Arena *dataArena) {
    ReadFileResult fileResult = ReadFile(path, dataArena);
    BitmapHeader *header = (BitmapHeader *)fileResult.data;
    Texture *texture = ArenaPushStruct(objArena, Texture);
    texture->data = (void *)((u8 *)fileResult.data + header->bitmapOffset);
    texture->width = header->width;
    texture->height = header->height;
    u32 redShift = BitScanForward(header->redMask);
    u32 greenShift = BitScanForward(header->greenMask);
    u32 blueShift = BitScanForward(header->blueMask);
    u32 alphaShift = BitScanForward(header->alphaMask);
    u32 *colorData = (u32 *)texture->data;
    for(u32 i = 0; i < texture->width*texture->height; ++i)
    {
        u32 red = (colorData[i] & header->redMask) >> redShift;       
        u32 green = (colorData[i] & header->greenMask) >> greenShift;       
        u32 blue = (colorData[i] & header->blueMask) >> blueShift;       
        u32 alpha = (colorData[i] & header->alphaMask) >> alphaShift;       
        colorData[i] = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
    }


    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = texture->data;
    data.SysMemPitch = texture->width*sizeof(u32);
    data.SysMemSlicePitch = 0;

    D3D11_TEXTURE2D_DESC textureDesc = {}; 
    textureDesc.Width = texture->width;
    textureDesc.Height = texture->height;
    textureDesc.MipLevels = 1; // use 0 to generate a full set of subtextures (mipmaps)
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    ID3D11Texture2D *tempTexture;
    HRESULT result = gRenderer.device->CreateTexture2D(&textureDesc, 0, &tempTexture);
    if(FAILED(result))
    {
        ASSERT("FAILED Creating texture\n");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};
    shaderResourceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// DXGI_FORMAT_R8G8B8A8_UNORM;
    shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceDesc.Texture2D.MipLevels = -1;
    shaderResourceDesc.Texture2D.MostDetailedMip = 0;
    result = gRenderer.device->CreateShaderResourceView(tempTexture, &shaderResourceDesc, &texture->colorMap);
    if(FAILED(result))
    {
        ASSERT("FAILED Creating Shader resource view\n");
    }
    gRenderer.deviceContext->UpdateSubresource(tempTexture, 0, 0, data.pSysMem, data.SysMemPitch, 0);
    gRenderer.deviceContext->GenerateMips(texture->colorMap);

    tempTexture->Release();

    D3D11_SAMPLER_DESC colorMapDesc = {};
    colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;//D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;//D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;//D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //D3D11_FILTER_MIN_MAG_MIP_LINEAR | D3D11_FILTER_MIN_MAG_MIP_POINT
    colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
    result = gRenderer.device->CreateSamplerState(&colorMapDesc, &texture->colorMapSampler);
    if(FAILED(result))
    {
        OutputDebugString("FAILED Creating sampler state\n");
    }
    return texture;
}

void RendererSetShader(Shader *shader) {
    gRenderer.deviceContext->IASetInputLayout(shader->input);
    gRenderer.deviceContext->VSSetShader(shader->vertex, 0, 0);
    gRenderer.deviceContext->PSSetShader(shader->pixel,  0, 0);
}


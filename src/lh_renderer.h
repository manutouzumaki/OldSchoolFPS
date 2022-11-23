#ifndef _LH_RENDERER_H_
#define _LH_RENDERER_H_

#include "lh_defines.h"
#include "lh_math.h"

#include <d3d11.h>

struct Texture;
struct Arena;

enum RendererType {
    RENDERER_DIRECTX,
    RENDERER_CPU,
    RENDERER_OPENGL
};

struct Vertex {
    vec3 position;
    vec2 uv;
    vec3 normal;
};

struct Mesh {
    Vertex *vertices;
    u32 *indices;
    i32 verticesCount;
    i32 indicesCount;
    Transform transform; 
    mat4 world;

    ID3D11Buffer *gpuVertex;
    ID3D11Buffer *gpuIndices;
};

struct ConstantBuffer {
    mat4 world;
    mat4 view;
    mat4 proj; 
};

struct Shader {
    ID3D11VertexShader *vertex;
    ID3D11PixelShader  *pixel;
    ID3D11InputLayout  *input;

    ID3D11Buffer *buffer;
};

#pragma pack(push, 1)
struct BitmapHeader
{
    u16 fileType;
    u32 fileSize;
    u16 reserved1;
    u16 reserved2;
    u32 bitmapOffset;
	u32 size;             
	i32 width;            
    i32 height;           
	u16 planes;           
	u16 bitsPerPixel;    
	u32 compression;      
	u32 sizeOfBitmap;     
	i32 horzResolution;  
	i32 vertResolution;  
	u32 colorsUsed;       
	u32 colorsImportant;  
	u32 redMask;          
	u32 greenMask;        
	u32 blueMask;         
	u32 alphaMask;        
};
#pragma pack(pop)

struct Texture
{
    void *data;
    u32 width;
    u32 height;

    ID3D11ShaderResourceView *colorMap;
    ID3D11SamplerState *colorMapSampler;
};

void RendererSystemInitialize();
void RendererSystemShutdown();
void RendererClearBuffers(u32 color, f32 depth);
void RendererPushWorkToQueue(Vertex *vertices, u32 *indices,
                             i32 indicesCount, Texture *bitmap, vec3 *lights, i32 lightsCount,
                             vec3 viewPos, mat4 world, bool writeDepthBuffer, f32 repeatU, f32 repeatV);
void RendererDrawRectFast(i32 xPos, i32 yPos, i32 width, i32 height, Texture *bitmap);
void RendererDrawAnimatedRectFast(i32 x, i32 y, i32 width, i32 height, Texture *bitmap, i32 spriteW, i32 spriteH, i32 frame);
void RendererFlushWorkQueue();
void RendererPresent();
void RendererSetProj(mat4 proj);
void RendererSetView(mat4 view);

void DEBUG_RendererDrawWireframeBuffer(Vertex *vertices, i32 verticesCount, u32 color, mat4 world);


Shader *RendererCreateShader(char *vertexPath, char *pixelPath, Arena *arena);
void RendererUpdateShaderData(Shader *shader, ConstantBuffer *buffer);
Mesh *RendererCreateMesh(Vertex *vertices, i32 verticesCount, u32 *indices, i32 indicesCount, mat4 world, Arena *arena);
Texture *RendererCreateTexture(char *path, Arena *objArena, Arena *dataArena);

void RendererSetShader(Shader *shader);
void RendererDrawMesh(Mesh *mesh, Texture *texture);
void RendererSetDepthBuffer(bool value);

#endif

#ifndef _LH_HEADER_RENDERER_H_
#define _LH_HEADER_RENDERER_H_

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

struct VertexD3D11 {
    vec3 position;
    vec2 uvs;
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

#endif

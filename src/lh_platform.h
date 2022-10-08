#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "lh_defines.h"
#include "lh_memory.h"
#include "lh_math.h"

struct ReadFileResult {
    void *data;
    size_t size;
};

ReadFileResult ReadFile(char *path, Arena *arena);
bool WriteFile(char *path, void *data, size_t size);

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

struct BMP
{
    void *data;
    u32 width;
    u32 height;
};

BMP LoadTexture(char *path, Arena *arena);

struct Window;

Window *WindowCreate(i32 width, i32 height, char *title);
void WindowDestroy(Window *window);
void WindowSetSize(i32 width, i32 height);

struct Renderer;

Renderer *RendererCreate(Window *window);
void RendererDestroy(Renderer *renderer);
void RendererClearBuffers(Renderer *renderer, u32 color, f32 depth);
void RendererPresent(Renderer *renderer);
void RenderBuffer(Renderer *renderer, vec3 *vertices, i32 verticesCount); 
void RenderBufferTexture(Renderer *renderer, vec3 *vertices, vec2 *uvs, i32 verticesCount, BMP bitmap);
void RenderBufferTextureClipping(Renderer *renderer, vec3 *vertices, vec2 *uvs, i32 verticesCount, BMP bitmap);
void RenderBufferTextureClippingDirectionalLight(Renderer *renderer,
                                                 vec3 *vertices, 
                                                 vec2 *uvs,
                                                 vec3 *normals,
                                                 i32 verticesCount,
                                                 BMP bitmap,
                                                 vec3 lightDir);
void RendererSetProj(Renderer *renderer, mat4 proj);
void RendererSetView(Renderer *renderer, mat4 view);

#endif

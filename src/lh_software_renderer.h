#ifndef _LH_SOFTWARE_RENDERER_3D_H_
#define _LH_SOFTWARE_RENDERER_3D_H_

struct Renderer {
    u32 *colorBuffer;
    f32 *depthBuffer;
    i32 bufferWidth;
    i32 bufferHeight;
};

typedef vec3 Point;

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

struct Bitmap
{
    void *data;
    u32 width;
    u32 height;
};

Bitmap LoadBitmap(char *path, Arena *arena);
void RenderBuffer(Renderer *renderer, vec3 *vertices, i32 verticesCount); 
void RenderBufferTexture(Renderer *renderer, vec3 *vertices, vec2 *uvs, i32 verticesCount, Bitmap bitmap);
void DrawLine(Renderer *renderer, Point a, Point b, u32 color);
void DrawLineTriangle(Renderer *renderer, Point a, Point b, Point c, u32 color);
void DrawFillTriangle(Renderer *renderer, Point a, Point b, Point c, u32 color);
void DrawTextureTriangle(Renderer *renderer, Point a, Point b, Point c,
                         vec2 aUv, vec2 bUv, vec2 cUv, Bitmap bitmap);
void DrawPoint(Renderer *renderer, Point point, u32 color);

#endif

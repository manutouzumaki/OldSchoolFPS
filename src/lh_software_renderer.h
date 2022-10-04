#ifndef _LH_SOFTWARE_RENDERER_3D_H_
#define _LH_SOFTWARE_RENDERER_3D_H_

struct Renderer {
    u32 *colorBuffer;
    f32 *depthBuffer;
    i32 bufferWidth;
    i32 bufferHeight;
};

typedef vec3 Point;

void RenderBuffer(Renderer *renderer, vec3 *vertices, i32 verticesCount); 
void DrawLine(Renderer *renderer, Point a, Point b, u32 color);
void DrawLineTriangle(Renderer *renderer, Point a, Point b, Point c, u32 color);
void DrawFillTriangle(Renderer *renderer, Point a, Point b, Point c, u32 color);
void DrawPoint(Renderer *renderer, Point point, u32 color);

#endif

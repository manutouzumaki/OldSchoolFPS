#ifndef _LH_SOFTWARE_RENDERER_3D_H_
#define _LH_SOFTWARE_RENDERER_3D_H_

typedef vec2 Point;

void DrawLine(u32 *buffer, Point a, Point b, u32 color);
void DrawLineTriangle(u32 *buffer, Point a, Point b, Point c, u32 color);
void DrawFillTriangle(u32 *buffer, Point a, Point b, Point c, u32 color);
void DrawPoint(u32 *buffer, Point point, u32 color);

#endif

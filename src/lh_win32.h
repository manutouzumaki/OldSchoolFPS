#ifndef _LH_WIND32_H_
#define _LH_WIND32_H_

struct Window {
    HWND hwnd;
    i32 width;
    i32 height;
    char *title;
};

struct Renderer {
    HBITMAP handle;
    HDC hdc;
    u32 *colorBuffer;
    f32 *depthBuffer;
    i32 bufferWidth;
    i32 bufferHeight;
    mat4 view;
    mat4 proj;
};

#endif

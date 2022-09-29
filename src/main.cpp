#include <windows.h>
#include <stdint.h>
#include <math.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef int32_t b32;
typedef int8_t b8;

#define global_variable static
#define local_persist static
#define internal static

#define TRUE 1
#define FALSE 0

#define ARRAY_LENGTH(array) (sizeof(array) / sizeof((array)[0]))
#define ASSERT(condition) if(!(condition)) {*(i32 *)0 = 0;}

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

struct vec2 {
    union {
        struct {
            f32 x;
            f32 y;
        };
        f32 v[2];
    }; 
};

typedef vec2 Point;

LRESULT WindowProcA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch(msg) {
        case WM_CLOSE: {
            PostQuitMessage(0);                   
        }break;
        default: {
            result = DefWindowProcA(hwnd, msg, wParam, lParam); 
        }
    }
    return result;
}

void DrawLine(u32 *buffer, Point a, Point b, u32 color) {
    i32 xDelta = (i32)(b.x - a.x);
    i32 yDelta = (i32)(b.y - a.y);
    i32 sideLength = abs(xDelta) >= abs(yDelta) ? abs(xDelta) : abs(yDelta);
    f32 xInc = (f32)xDelta / (f32)sideLength;
    f32 yInc = (f32)yDelta / (f32)sideLength;
    f32 x = a.x;
    f32 y = a.y;
    for(i32 i = 0; i < sideLength; ++i) {
        buffer[(i32)y * 800 + (i32)x] = color;
        x += xInc;
        y += yInc;
    }
}

void DrawLineTriangle(u32 *buffer, Point a, Point b, Point c, u32 color) {
    DrawLine(buffer, a, b, color);
    DrawLine(buffer, b, c, color);
    DrawLine(buffer, c, a, color);
}

void SwapVec2(vec2 *a, vec2 *b) {
    vec2 tmp = *a;
    *a = *b;
    *b = tmp;
}

void SwapInt(i32 *a, i32 *b) {
    i32 tmp = *a;
    *a = *b;
    *b = tmp;
}

void DrawFillTriangle(u32 *buffer, Point a, Point b, Point c, u32 color) {
    if(a.y > b.y) {
        SwapVec2(&a, &b);
    }
    if(b.y > c.y) {
        SwapVec2(&b, &c);
    }
    if(a.y > b.y) {
        SwapVec2(&a, &b);
    }
    i32 x0 = a.x;
    i32 y0 = a.y;
    i32 x1 = b.x;
    i32 y1 = b.y;
    i32 x2 = c.x;
    i32 y2 = c.y;
    f32 invSlope1 = 0;
    f32 invSlope2 = 0; 
    if(y1 - y0 != 0) invSlope1 = (f32)(x1 - x0) / abs((y1 - y0));
    if(y2 - y0 != 0) invSlope2 = (f32)(x2 - x0) / abs((y2 - y0));
    if(y1 - y0 != 0) {
        for(i32 y = y0; y < y1; y++) {
            i32 xStart = x1 + (y - y1) * invSlope1;
            i32 xEnd = x0 + (y - y0) * invSlope2;
            if(xEnd < xStart) {
                SwapInt(&xStart, &xEnd);
            }
            for(i32 x  = xStart; x <= xEnd; x++) {
                buffer[(i32)y * 800 + (i32)x] = color;
            }
        }
    }
    invSlope1 = 0;
    invSlope2 = 0;
    if(y2 - y1 != 0) invSlope1 = (f32)(x2 - x1) / abs(y2 - y1);
    if(y2 - y0 != 0) invSlope2 = (f32)(x2 - x0) / abs(y2 - y0);
    if(y2 - y1 != 0) {
        for(i32 y = y1; y < y2; y++) {
            i32 xStart = x1 + (y - y1) * invSlope1;
            i32 xEnd = x0 + (y - y0) * invSlope2;
            if(xEnd < xStart) {
                SwapInt(&xStart, &xEnd);
            }
            for(i32 x  = xStart; x <= xEnd; x++) {
                buffer[(i32)y * 800 + (i32)x] = color;
            }
        }
    }
}

void DrawPoint(u32 *buffer, Point point, u32 color) {
    i32 x = (f32)point.x;
    i32 y = (f32)point.y;
    buffer[(i32)y * 800 + (i32)x] = color;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    OutputDebugString("Hi LastHope...\n");
    
    // we have to create a window class and then register to windows.
    WNDCLASSA wndClass = {};
    wndClass.style = CS_HREDRAW|CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProcA;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(0, IDC_ARROW);
    wndClass.lpszClassName = "LastHope3D";
    RegisterClassA(&wndClass);

    HWND hwnd = CreateWindowA("LastHope3D", "Last Hope 3D",
                              WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              0, 0, hInstance, 0);
    

    // create and update a color buffer
    HDC hdc = GetDC(hwnd);
    BITMAPINFO bufferInfo = {};
    bufferInfo.bmiHeader.biSize = sizeof(bufferInfo.bmiHeader);
    bufferInfo.bmiHeader.biWidth = 800;
    bufferInfo.bmiHeader.biHeight = -600;
    bufferInfo.bmiHeader.biPlanes = 1;
    bufferInfo.bmiHeader.biBitCount = 32;
    bufferInfo.bmiHeader.biCompression = BI_RGB;
    void *colorBuffer = 0;
    HBITMAP colorBufferHandle = CreateDIBSection(hdc, &bufferInfo, DIB_RGB_COLORS, &colorBuffer, 0, 0);
    // fill the buffer with black
    memset(colorBuffer, 0, 800 * 600 * 4);

    Point c = { 200, 10 };
    Point a = { 200, 160 };
    Point b = { 10, 200 };
    DrawFillTriangle((u32 *)colorBuffer, a, b, c, 0x00FFFF00);
    DrawLineTriangle((u32 *)colorBuffer, a, b, c, 0x0000FFFF);
    DrawPoint((u32 *)colorBuffer, a, 0x00FF0000);
    DrawPoint((u32 *)colorBuffer, b, 0x00FF0000);
    DrawPoint((u32 *)colorBuffer, c, 0x00FF0000);

    b32 running = TRUE;

    // get messages and handle them
    MSG msg = {};
    while(running) {
        // flush windows messages
        while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) {
                running = FALSE;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        
        // present the color buffer to the window
        HDC colorBufferDC = CreateCompatibleDC(hdc);
        SelectObject(colorBufferDC, colorBufferHandle);
        BitBlt(hdc, 0, 0, 800, 600, colorBufferDC, 0, 0, SRCCOPY);
        DeleteDC(colorBufferDC);

    }
    return 0;
}

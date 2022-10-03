#include "lh_include.h"

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
    bufferInfo.bmiHeader.biHeight = 600;
    bufferInfo.bmiHeader.biPlanes = 1;
    bufferInfo.bmiHeader.biBitCount = 32;
    bufferInfo.bmiHeader.biCompression = BI_RGB;
    void *colorBuffer = 0;
    HBITMAP colorBufferHandle = CreateDIBSection(hdc, &bufferInfo, DIB_RGB_COLORS, &colorBuffer, 0, 0);
    f32 *depthBuffer = (f32 *)VirtualAlloc(0, 800 * 600 * sizeof(f32), MEM_COMMIT, PAGE_READWRITE);
    
    // initialize depth buffer
    for(i32 i = 0; i < 800*600; ++i) {
        depthBuffer[i] = 0.0f;
    }

    // fill the buffer with black
    memset(colorBuffer, 0, 800 * 600 * 4);

#if 0
    Point c = { 200, 10 };
    Point a = { 200, 160 };
    Point b = { 10, 200 };
    DrawFillTriangle((u32 *)colorBuffer, depthBuffer, a, b, c, 0x00FFFF00);
    DrawLineTriangle((u32 *)colorBuffer, depthBuffer, a, b, c, 0x0000FFFF);
    DrawPoint((u32 *)colorBuffer, a, 0x00FF0000);
    DrawPoint((u32 *)colorBuffer, b, 0x00FF0000);
    DrawPoint((u32 *)colorBuffer, c, 0x00FF0000);
#endif

    b32 running = TRUE;


    // TODO: test matrix multiplication
    mat4 aMat = Mat4Identity();
    mat4 bMat = Mat4Identity();
    Mat4Print((aMat * 2.0f) * (bMat * 3));
    
    // TODO: 
    // - draw a cube
    // - z buffer
    // - back face culling
    vec3 vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

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
        
        // clear color and depth buffer
        memset(colorBuffer, 0, 800 * 600 * 4);
        for(i32 i = 0; i < 800*600; ++i) {
            depthBuffer[i] = 0.0f;
        }

        RenderBuffer((u32 *)colorBuffer, depthBuffer, vertices, ARRAY_LENGTH(vertices));
        
        for(i32 y = 300; y < 332; ++y) {
            for(i32 x = 300; x < 332; ++x) {
                Point p = {(f32)x, (f32)y};
                DrawPoint((u32 *)colorBuffer, p, 0x0000FFFF);
            }
        }

        
        // present the color buffer to the window
        HDC colorBufferDC = CreateCompatibleDC(hdc);
        SelectObject(colorBufferDC, colorBufferHandle);
        BitBlt(hdc, 0, 0, 800, 600, colorBufferDC, 0, 0, SRCCOPY);
        DeleteDC(colorBufferDC);

    }
    return 0;
}

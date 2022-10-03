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


    // TODO: test matrix multiplication
    mat4 aMat = Mat4Identity();
    mat4 bMat = Mat4Identity();
    Mat4Print((aMat * 2.0f) * (bMat * 3));

    vec3 vertices[] = {
        // bottom left triangle
        {-3, -1, 1},
        {-1,  1, 1},
        { 1, -1, 1},
        // upper right triangle
        {-1,  1, 1},
        { 1,  1, 1},
        { 1, -1, 1}
    };
    RenderBuffer((u32 *)colorBuffer, vertices, ARRAY_LENGTH(vertices));

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

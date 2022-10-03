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
    
    // fill the buffer with black
    memset(colorBuffer, 0, 800 * 600 * sizeof(u32));
    memset(depthBuffer, 0, 800 * 600 * sizeof(f32));

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
    timeBeginPeriod(1);
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    f32 invFrequency = 1.0f / (f32)frequency.QuadPart;

    LARGE_INTEGER lastCounter;
    QueryPerformanceCounter(&lastCounter);

    MSG msg = {};
    while(running) {
        // if we have time left Sleep 
        LARGE_INTEGER workCounter = {};
        QueryPerformanceCounter(&workCounter);
        f32 secondsElapsed = (f32)(workCounter.QuadPart - lastCounter.QuadPart) * invFrequency;
        while(secondsElapsed < TARGET_SECONDS_PER_FRAME) {
            DWORD milisecondsToSleep = (DWORD)((TARGET_SECONDS_PER_FRAME - secondsElapsed) * 1000.0f);
            Sleep(milisecondsToSleep);
            QueryPerformanceCounter(&workCounter);
            secondsElapsed = (f32)(workCounter.QuadPart - lastCounter.QuadPart) * invFrequency;
        }

        LARGE_INTEGER currentCounter;
        QueryPerformanceCounter(&currentCounter);
        f64 fps = (f64)frequency.QuadPart / (f64)(currentCounter.QuadPart - lastCounter.QuadPart); 

        // flush windows messages
        while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) {
                running = FALSE;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        
        // clear color and depth buffer
        memset(colorBuffer, 0, 800 * 600 * sizeof(u32));
        memset(depthBuffer, 0, 800 * 600 * sizeof(f32));

        RenderBuffer((u32 *)colorBuffer, depthBuffer, vertices, ARRAY_LENGTH(vertices));
         
        // present the color buffer to the window
        HDC colorBufferDC = CreateCompatibleDC(hdc);
        SelectObject(colorBufferDC, colorBufferHandle);
        BitBlt(hdc, 0, 0, 800, 600, colorBufferDC, 0, 0, SRCCOPY);
        DeleteDC(colorBufferDC);

        lastCounter = currentCounter;
    }
    return 0;
}

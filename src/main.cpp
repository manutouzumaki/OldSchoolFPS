#include "lh_include.h"

ReadFileResult ReadFile(char *path, Arena *arena) {
    ReadFileResult result = {};
    HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(file != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize;
        GetFileSizeEx(file, &fileSize);
        result.data = ArenaPush(arena, fileSize.QuadPart);
        if(ReadFile(file, result.data, (DWORD)fileSize.QuadPart, 0, 0)) {
            result.size = fileSize.QuadPart; 
        }
        else {
            ASSERT(!"ERROR Reading the file");
        }
        CloseHandle(file);
    }
    return result;
}

bool WriteFile(char *path, void *data, size_t size) {
    bool succed = FALSE;
    HANDLE file = CreateFileA(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if(file != INVALID_HANDLE_VALUE) {
        if(WriteFile(file, data, size, 0, 0)) {
            succed = TRUE;
        }
        CloseHandle(file);
    }
    return succed;
}

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
                              800, 600,
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
    u32 *colorBuffer = 0;
    HBITMAP colorBufferHandle = CreateDIBSection(hdc, &bufferInfo, DIB_RGB_COLORS, (void **)&colorBuffer, 0, 0);
    f32 *depthBuffer = (f32 *)VirtualAlloc(0, 800 * 600 * sizeof(f32), MEM_COMMIT, PAGE_READWRITE);
    
    // fill the buffer with black
    memset(colorBuffer, 0, 800 * 600 * sizeof(u32));
    memset(depthBuffer, 0, 800 * 600 * sizeof(f32));

    // allocate memory for the entire game
    Platform platform = {};
    platform.memory = MemoryCreate(Megabytes(10));
    platform.renderer.colorBuffer = colorBuffer;
    platform.renderer.depthBuffer = depthBuffer;
    platform.renderer.bufferWidth = 800;
    platform.renderer.bufferHeight = 600;
    GameInit(&platform); 

    b32 running = TRUE;
    
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
#if 1
        LARGE_INTEGER workCounter = {};
        QueryPerformanceCounter(&workCounter);
        f32 secondsElapsed = (f32)(workCounter.QuadPart - lastCounter.QuadPart) * invFrequency;
        while(secondsElapsed < TARGET_SECONDS_PER_FRAME) {
            // TODO: look for an alternative
            DWORD milisecondsToSleep = (DWORD)((TARGET_SECONDS_PER_FRAME - secondsElapsed) * 1000.0f);
            Sleep(milisecondsToSleep);
            QueryPerformanceCounter(&workCounter);
            secondsElapsed = (f32)(workCounter.QuadPart - lastCounter.QuadPart) * invFrequency;
        }
#endif

        LARGE_INTEGER currentCounter;
        QueryPerformanceCounter(&currentCounter);
        platform.deltaTime = (f32)(currentCounter.QuadPart - lastCounter.QuadPart) * invFrequency;

        // flush windows messages
        while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) {
                running = FALSE;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
       
        GameUpdate(&platform); 

        // clear color and depth buffer
        memset(colorBuffer, 0, 800 * 600 * sizeof(u32));
        memset(depthBuffer, 0, 800 * 600 * sizeof(f32));
        
        GameRender(&platform);
         
        // present the color buffer to the window
        HDC colorBufferDC = CreateCompatibleDC(hdc);
        SelectObject(colorBufferDC, colorBufferHandle);
        BitBlt(hdc, 0, 0, 800, 600, colorBufferDC, 0, 0, SRCCOPY);
        DeleteDC(colorBufferDC);

        lastCounter = currentCounter;
    }

    GameShutdown(&platform);
    return 0;
}

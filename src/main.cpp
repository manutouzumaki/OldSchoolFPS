#include <windows.h>
#include "lh_game.h"
#include "lh_win32.h"

internal
u32 BitScanForward(u32 mask)
{
    unsigned long shift = 0;
    _BitScanForward(&shift, mask);
    return (u32)shift;
}


BMP LoadTexture(char *path, Arena *arena) {
    ReadFileResult fileResult = ReadFile(path, arena);
    BitmapHeader *header = (BitmapHeader *)fileResult.data;
    BMP bitmap;
    bitmap.data = (void *)((u8 *)fileResult.data + header->bitmapOffset);
    bitmap.width = header->width;
    bitmap.height = header->height;
    u32 redShift = BitScanForward(header->redMask);
    u32 greenShift = BitScanForward(header->greenMask);
    u32 blueShift = BitScanForward(header->blueMask);
    u32 alphaShift = BitScanForward(header->alphaMask);
    u32 *colorData = (u32 *)bitmap.data;
    for(u32 i = 0; i < bitmap.width*bitmap.height; ++i)
    {
        u32 red = (colorData[i] & header->redMask) >> redShift;       
        u32 green = (colorData[i] & header->greenMask) >> greenShift;       
        u32 blue = (colorData[i] & header->blueMask) >> blueShift;       
        u32 alpha = (colorData[i] & header->alphaMask) >> alphaShift;       
        colorData[i] = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
    }
    return bitmap;
}

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

Window *WindowCreate(i32 width, i32 height, char *title) { 
    HINSTANCE hInstance = GetModuleHandleA(0);
    Window *window = (Window *)malloc(sizeof(Window));
    WNDCLASSA wndClass = {};
    wndClass.style = CS_HREDRAW|CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProcA;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(0, IDC_ARROW);
    wndClass.lpszClassName = "LastHope3D";
    RegisterClassA(&wndClass);
    HWND hwnd = CreateWindowA("LastHope3D", title,
                              WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              width, height,
                              0, 0, hInstance, 0);
    window->width = width;
    window->height =  height;
    window->title = title;
    window->hwnd = hwnd;
    return window;
}

void WindowDestroy(Window *window) {
    ASSERT(window);
    DestroyWindow(window->hwnd);
    free(window);
    window = 0;
}

void WindowSetSize(Window *window, i32 width, i32 height) {
    RECT currentWindowDim;
    GetWindowRect(window->hwnd, &currentWindowDim);
    MoveWindow(window->hwnd,
               currentWindowDim.left,
               currentWindowDim.top,
               width, height, TRUE);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    OutputDebugString("Hi LastHope...\n"); 

    // allocate memory for the entire game
    Memory memory = MemoryCreate(Megabytes(10));
    GameInit(&memory); 

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
        f32 deltaTime = (f32)(currentCounter.QuadPart - lastCounter.QuadPart) * invFrequency;

        // flush windows messages
        while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
            if(msg.message == WM_QUIT) {
                running = FALSE;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
       
        GameUpdate(deltaTime); 
        GameRender();
         
        lastCounter = currentCounter;
    }

    GameShutdown();
    return 0;
}

#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#include "lh_game.h"

struct Window {
    HWND hwnd;
    i32 width;
    i32 height;
    char *title;
};

// TODO: start implementing multithreading code
struct PlatformWorkQueueEntry {
    PlatformWorkQueueCallback *callback;
    void *data;
};

struct PlatformWorkQueue {
    u32 volatile completitionGloal;
    u32 volatile completitionCount;
    u32 volatile nextEntryToWrite;
    u32 volatile nextEntryToRead;
    HANDLE semaphoreHandle;
    PlatformWorkQueueEntry entries[256];
};

Window gWindow = {};
PlatformWorkQueue gQueue = {};
bool gRunning;
extern Input gLastInput;
extern Input gInput;

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

void WindowSystemInitialize(i32 width, i32 height, char *title) { 
    HINSTANCE hInstance = GetModuleHandleA(0);
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
    gWindow.width = width;
    gWindow.height =  height;
    gWindow.title = title;
    gWindow.hwnd = hwnd;
    

    i32 stopHere =0;
}

void WindowSystemShutdown() {
    DestroyWindow(gWindow.hwnd);
}

void WindowSetSize(i32 width, i32 height) {
    RECT currentWindowDim;
    GetWindowRect(gWindow.hwnd, &currentWindowDim);
    MoveWindow(gWindow.hwnd,
               currentWindowDim.left,
               currentWindowDim.top,
               width, height, TRUE);
}

bool DoNextWorkQueueEntry() {
    bool weShouldSleep = false;
    u32 originalNextEntryToRead = gQueue.nextEntryToRead;
    u32 newNextEntryToRead = (originalNextEntryToRead + 1) % ARRAY_LENGTH(gQueue.entries);
    if(originalNextEntryToRead != gQueue.nextEntryToWrite) {
        u32 index = InterlockedCompareExchange((LONG volatile *)&gQueue.nextEntryToRead, newNextEntryToRead, originalNextEntryToRead);
        if(index == originalNextEntryToRead) {
            PlatformWorkQueueEntry entry = gQueue.entries[index];
            entry.callback(entry.data);
            InterlockedIncrement((LONG volatile *)&gQueue.completitionCount);
        } 
    }
    else {
        weShouldSleep = true;
    }
    return weShouldSleep;
}

void PlatformAddEntry(PlatformWorkQueueCallback *callback, void *data) {
    u32 newNextEntryToWrite = (gQueue.nextEntryToWrite + 1) % ARRAY_LENGTH(gQueue.entries);
    ASSERT(newNextEntryToWrite != gQueue.nextEntryToRead);
    PlatformWorkQueueEntry *entry = gQueue.entries + gQueue.nextEntryToWrite;
    entry->callback = callback;
    entry->data = data;
    ++gQueue.completitionGloal;
    _WriteBarrier();
    gQueue.nextEntryToWrite = newNextEntryToWrite;
    ReleaseSemaphore(gQueue.semaphoreHandle, 1, 0);
}

void PlatformCompleteAllWork() {
    while(gQueue.completitionGloal != gQueue.completitionCount) {
        DoNextWorkQueueEntry();
    }
    gQueue.completitionGloal = 0;
    gQueue.completitionCount = 0;
}

DWORD WINAPI
ThreadProc(LPVOID lpParameter)
{
    for(;;)
    {
        if(DoNextWorkQueueEntry())
        {
            WaitForSingleObjectEx(gQueue.semaphoreHandle, INFINITE, FALSE);
        }
    }
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    OutputDebugString("Hi LastHope...\n"); 

    HRESULT result = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
    if (FAILED(result)) {
        OutputDebugString("COM Initialization FAILED\n");
        return result;
    }
    u32 initialCount = 0;
    u32 threadCount = 7;
    gQueue.semaphoreHandle = CreateSemaphoreEx(0, initialCount, threadCount,
                                               0, 0, SEMAPHORE_ALL_ACCESS);

    for(u32 threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
        DWORD threadId;
        HANDLE threadHandle = CreateThread(0, 0, ThreadProc, NULL, 0, &threadId);
        CloseHandle(threadHandle);
    }
    // allocate memory for the entire game
    Memory memory = MemoryCreate(Megabytes(10));
    GameInit(&memory);
    
    // get messages and handle them
    timeBeginPeriod(1);
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    f32 invFrequency = 1.0f / (f32)frequency.QuadPart;

    LARGE_INTEGER lastCounter;
    QueryPerformanceCounter(&lastCounter);

    MSG msg = {};
    gRunning = true;
        
    while(gRunning) {
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

#if 1
        char buffer[256];
        sprintf(buffer, "MS: %f\n", deltaTime * 1000.0f);
        OutputDebugString(buffer);
        sprintf(buffer, "FPS: %d\n", (i32)(1.0f/deltaTime));
        OutputDebugString(buffer);
#endif

        // flush windows messages
        ProcessInputAndMessages();
       
        GameUpdate(&memory, deltaTime); 
        GameRender(&memory);

        gLastInput = gInput; 
        lastCounter = currentCounter;
    }

    GameShutdown(&memory);
    return 0;
}

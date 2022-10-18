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

#pragma pack(push, 1)
struct BitmapHeader
{
    u16 fileType;
    u32 fileSize;
    u16 reserved1;
    u16 reserved2;
    u32 bitmapOffset;
	u32 size;             
	i32 width;            
    i32 height;           
	u16 planes;           
	u16 bitsPerPixel;    
	u32 compression;      
	u32 sizeOfBitmap;     
	i32 horzResolution;  
	i32 vertResolution;  
	u32 colorsUsed;       
	u32 colorsImportant;  
	u32 redMask;          
	u32 greenMask;        
	u32 blueMask;         
	u32 alphaMask;        
};
#pragma pack(pop)

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

Counter *DEBUG_counters = 0;

void OutputCounter() {
#if 0
    const char *names[CYCLECOUNTER_COUNT] = {
        "TriangleRasterizer"
    };
    for(i32 i = 0; i < CYCLECOUNTER_COUNT; ++i) {
        char buffer[256];
        if(DEBUG_counters[i].hit) DEBUG_counters[i].cyclesPerHit = DEBUG_counters[i].count / DEBUG_counters[i].hit;
        sprintf(buffer, "%s: cyclesPerFrame: %lld hit: %lld cyclesPerHit: %lld\n", names[i],
                                                                                   DEBUG_counters[i].count,
                                                                                   DEBUG_counters[i].hit,
                                                                                   DEBUG_counters[i].cyclesPerHit);
        OutputDebugString(buffer);
        Counter zero = {};
        DEBUG_counters[i] = zero;
    }
#endif
}

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

struct PlatformThreadInfo {
    i32 logicalThreadIndex;
    PlatformWorkQueue *queue;
};

bool DoNextWorkQueueEntry(PlatformWorkQueue *queue) {
    bool weShouldSleep = false;
    u32 originalNextEntryToRead = queue->nextEntryToRead;
    u32 newNextEntryToRead = (originalNextEntryToRead + 1) % ARRAY_LENGTH(queue->entries);
    if(originalNextEntryToRead != queue->nextEntryToWrite) {
        u32 index = InterlockedCompareExchange((LONG volatile *)&queue->nextEntryToRead, newNextEntryToRead, originalNextEntryToRead);
        if(index == originalNextEntryToRead) {
            PlatformWorkQueueEntry entry = queue->entries[index];
            entry.callback(queue, entry.data);
            InterlockedIncrement((LONG volatile *)&queue->completitionCount);
        } 
    }
    else {
        weShouldSleep = true;
    }
    return weShouldSleep;
}

void PlatformAddEntry(PlatformWorkQueue *queue, PlatformWorkQueueCallback *callback, void *data) {
    u32 newNextEntryToWrite = (queue->nextEntryToWrite + 1) % ARRAY_LENGTH(queue->entries);
    ASSERT(newNextEntryToWrite != queue->nextEntryToRead);
    PlatformWorkQueueEntry *entry = queue->entries + queue->nextEntryToWrite;
    entry->callback = callback;
    entry->data = data;
    ++queue->completitionGloal;
    _WriteBarrier();
    queue->nextEntryToWrite = newNextEntryToWrite;
    ReleaseSemaphore(queue->semaphoreHandle, 1, 0);
}

void PlatformCompleteAllWork(PlatformWorkQueue *queue) {
    while(queue->completitionGloal != queue->completitionCount) {
        DoNextWorkQueueEntry(queue);
    }
    queue->completitionGloal = 0;
    queue->completitionCount = 0;
}

DWORD WINAPI
ThreadProc(LPVOID lpParameter)
{
    PlatformThreadInfo *threadInfo = (PlatformThreadInfo *)lpParameter;
    for(;;)
    {
        if(DoNextWorkQueueEntry(threadInfo->queue))
        {
            WaitForSingleObjectEx(threadInfo->queue->semaphoreHandle, INFINITE, FALSE);
        }
    }
}

void TestCallback(PlatformWorkQueue *queue, void *data) {
    char buffer[256];
    sprintf(buffer, "Thread: %u: %s\n", GetCurrentThreadId(), (char *)data);
    OutputDebugString(buffer);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    OutputDebugString("Hi LastHope...\n"); 

    PlatformThreadInfo threadInfo[7];
    PlatformWorkQueue queue = {};

    u32 initialCount = 0;
    u32 threadCount = ARRAY_LENGTH(threadInfo);
    queue.semaphoreHandle = CreateSemaphoreEx(0, initialCount, threadCount,
                                              0, 0, SEMAPHORE_ALL_ACCESS);

    for(u32 threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
        PlatformThreadInfo *info = threadInfo + threadIndex;
        info->queue = &queue;
        info->logicalThreadIndex = threadIndex;
        DWORD threadId;
        HANDLE threadHandle = CreateThread(0, 0, ThreadProc, info, 0, &threadId);
        CloseHandle(threadHandle);
    }

    PlatformAddEntry(&queue, TestCallback, "String A0");
    PlatformAddEntry(&queue, TestCallback, "String A1");
    PlatformAddEntry(&queue, TestCallback, "String A2");
    PlatformAddEntry(&queue, TestCallback, "String A3");
    PlatformAddEntry(&queue, TestCallback, "String A4");
    PlatformAddEntry(&queue, TestCallback, "String A5");
    PlatformAddEntry(&queue, TestCallback, "String A6");
    PlatformAddEntry(&queue, TestCallback, "String A7");
    PlatformAddEntry(&queue, TestCallback, "String A8");
    PlatformAddEntry(&queue, TestCallback, "String A9");

    PlatformAddEntry(&queue, TestCallback, "String B0");
    PlatformAddEntry(&queue, TestCallback, "String B1");
    PlatformAddEntry(&queue, TestCallback, "String B2");
    PlatformAddEntry(&queue, TestCallback, "String B3");
    PlatformAddEntry(&queue, TestCallback, "String B4");
    PlatformAddEntry(&queue, TestCallback, "String B5");
    PlatformAddEntry(&queue, TestCallback, "String B6");
    PlatformAddEntry(&queue, TestCallback, "String B7");
    PlatformAddEntry(&queue, TestCallback, "String B8");
    PlatformAddEntry(&queue, TestCallback, "String B9");

    PlatformCompleteAllWork(&queue);


    // allocate memory for the entire game
    Memory memory = MemoryCreate(Megabytes(10));
    GameInit(&memory, &queue); 
    DEBUG_counters = ((GameState *)memory.data)->counters;

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
#if 0
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
        sprintf(buffer, "FPS: %d\n", (i32)(deltaTime * 1000));
        OutputDebugString(buffer);
#endif

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
        //OutputCounter();

         
        lastCounter = currentCounter;
    }

    GameShutdown();
    return 0;
}

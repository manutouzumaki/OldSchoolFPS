#include <windows.h>
#include <windowsx.h>
#include <xinput.h>
#include <xaudio2.h>
#include <intrin.h>
#include <stdio.h>
#include "lh_game.h"

struct Window {
    HWND hwnd;
    i32 width;
    i32 height;
    char *title;
};

Window gWindow;

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

internal 
f32 ProcessXInputStick(SHORT value, i32 deadZoneValue)
{
    f32 result = 0;
    if(value < -deadZoneValue)
    {
        result = (f32)(value + deadZoneValue) / (32768.0f - deadZoneValue);
    }
    else if(value > deadZoneValue)
    {
        result = (f32)(value - deadZoneValue) / (32767.0f - deadZoneValue);
    }
    return result;
}

global_variable bool gRunning;
global_variable Input gInput;
global_variable Input gLastInput;
global_variable WORD XInputButtons[] = 
{
    XINPUT_GAMEPAD_DPAD_UP,
    XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT,
    XINPUT_GAMEPAD_DPAD_RIGHT,
    XINPUT_GAMEPAD_START,
    XINPUT_GAMEPAD_BACK,
    XINPUT_GAMEPAD_A,
    XINPUT_GAMEPAD_B,
    XINPUT_GAMEPAD_X,
    XINPUT_GAMEPAD_Y
};

internal
void ProcessInputAndMessages(Input *lastInput) {
    gInput.mouseWheel = 0;
    MSG msg = {};
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        switch(msg.message)
        {
            case WM_QUIT: {
                gRunning = false;      
            } break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP: { 
                bool wasDown = ((msg.lParam & (1 << 30)) != 0);
                bool isDown = ((msg.lParam & (1 << 31)) == 0);
                if(isDown != wasDown) {
                    DWORD vkCode = (DWORD)msg.wParam;
                    gInput.keys[vkCode].isDown = isDown;
                }
            }break;
            case WM_MOUSEMOVE: {
                gInput.mouseX = (i32)GET_X_LPARAM(msg.lParam); 
                gInput.mouseY = (i32)GET_Y_LPARAM(msg.lParam); 
            }break;
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: {
                gInput.mouseLeft.isDown = ((msg.wParam & MK_LBUTTON) != 0);
                gInput.mouseMiddle.isDown = ((msg.wParam & MK_MBUTTON) != 0);
                gInput.mouseRight.isDown = ((msg.wParam & MK_RBUTTON) != 0);
            }break;
            case WM_MOUSEWHEEL: {
                i32 zDelta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
                if (zDelta != 0) {
                    // Flatten the input to an OS-independent (-1, 1)
                    zDelta = (zDelta < 0) ? -1 : 1;
                    gInput.mouseWheel = zDelta;
                }
            } break;
            default: {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }break;
        } 
    }

    XINPUT_STATE state = {};
    if(XInputGetState(0, &state) == ERROR_SUCCESS)
    {
        XINPUT_GAMEPAD *pad = &state.Gamepad;
        for(i32 i = 0; i < ARRAY_LENGTH(gInput.joyButtons); ++i)
        {
            gInput.joyButtons[i].isDown = pad->wButtons & XInputButtons[i];
        }
        gInput.leftStickX =  ProcessXInputStick(pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        gInput.leftStickY =  ProcessXInputStick(pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        gInput.rightStickX = ProcessXInputStick(pad->sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        gInput.rightStickY = ProcessXInputStick(pad->sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    }
    else
    {
        for(i32 i = 0; i < ARRAY_LENGTH(gInput.joyButtons); ++i)
        {
            gInput.joyButtons[i].isDown = false;
        }
        gInput.leftStickX = 0.0f; 
        gInput.leftStickY = 0.0f;
        gInput.rightStickX = 0.0f;
        gInput.rightStickY = 0.0f;
    }

    
    for(i32 i = 0; i < ARRAY_LENGTH(gInput.keys); ++i) {
        if(lastInput->keys[i].isDown) {
            gInput.keys[i].wasDown = true;
        }
        else {
            gInput.keys[i].wasDown = false; 
        }
    }
    for(i32 i = 0; i < ARRAY_LENGTH(gInput.mouseButtons); ++i) {
        if(lastInput->mouseButtons[i].isDown) {
            gInput.mouseButtons[i].wasDown = true;
        }
        else {
            gInput.mouseButtons[i].wasDown = false; 
        }
    }
    for(i32 i = 0; i < ARRAY_LENGTH(gInput.joyButtons); ++i) {
        if(lastInput->joyButtons[i].isDown) {
            gInput.joyButtons[i].wasDown = true;
        }
        else {
            gInput.joyButtons[i].wasDown = false; 
        }
    }
}

// Input function implementation
bool KeyboardGetKeyDown(i32 key) {
    return gInput.keys[key].isDown;
}

bool KeyboardGetKeyJustDown(i32 key) {
    if(gInput.keys[key].isDown != gInput.keys[key].wasDown) {
        return gInput.keys[key].isDown; 
    }
    return false;
}

bool KeyboardGetKeyJustUp(i32 key) {
    if(gInput.keys[key].isDown != gInput.keys[key].wasDown) {
        return gInput.keys[key].wasDown; 
    }
    return false;
}

bool KeyboardGetKeyUp(i32 key) {
    return !gInput.keys[key].isDown;
}

bool MouseGetButtonDown(i32 button) {
    return gInput.mouseButtons[button].isDown;
}

bool MouseGetButtonJustDown(i32 button) {
    if(gInput.mouseButtons[button].isDown != gInput.mouseButtons[button].wasDown) {
        return gInput.mouseButtons[button].isDown; 
    }
    return false;
}

bool MouseGetButtonJustUp(i32 button) {
    if(gInput.mouseButtons[button].isDown != gInput.mouseButtons[button].wasDown) {
        return gInput.mouseButtons[button].wasDown; 
    }
    return false;
}

bool MouseGetButtonUp(i32 button) {
    return !gInput.mouseButtons[button].isDown;
}

i32 MouseGetCursorX() {
    return gInput.mouseX;
}
i32 MouseGetCursorY() {
    return gInput.mouseY;
}

i32 MouseGetWheel() {
    return gInput.mouseWheel;
}

i32 MouseGetLastCursorX() {
    return gLastInput.mouseX;
}

i32 MouseGetLastCursorY() {
    return gLastInput.mouseY;
}

bool JoysickGetButtonDown(i32 button) {
    return gInput.joyButtons[button].isDown;
}

bool JoysickGetButtonJustDown(i32 button) {
    if(gInput.joyButtons[button].isDown != gInput.joyButtons[button].wasDown) {
        return gInput.joyButtons[button].isDown; 
    }
    return false;
}

bool JoysickGetButtonJustUp(i32 button) {
    if(gInput.joyButtons[button].isDown != gInput.joyButtons[button].wasDown) {
        return gInput.joyButtons[button].wasDown; 
    }
    return false;
}

bool JoysickGetButtonUp(i32 button) {
    return !gInput.joyButtons[button].isDown;
}

f32 JoysickGetLeftStickX() {
    return gInput.leftStickX;
}

f32 JoysickGetLeftStickY() {
    return gInput.leftStickY;
}

f32 JoysickGetRightStickX() {
    return gInput.rightStickX;
}

f32 JoysickGetRightStickY() {
    return gInput.rightStickY;
}


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    OutputDebugString("Hi LastHope...\n"); 

    HRESULT result = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
    if (FAILED(result)) {
        OutputDebugString("COM Initialization FAILED\n");
        return result;
    }

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
    // allocate memory for the entire game
    Memory memory = MemoryCreate(Megabytes(10));
    GameInit(&memory, &queue); 
    DEBUG_counters = ((GameState *)memory.data)->counters;

    
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

#if 0
        char buffer[256];
        sprintf(buffer, "MS: %f\n", deltaTime * 1000.0f);
        OutputDebugString(buffer);
        sprintf(buffer, "FPS: %d\n", (i32)(1.0f/deltaTime));
        OutputDebugString(buffer);
#endif

        // flush windows messages
        ProcessInputAndMessages(&gLastInput);
       
        GameUpdate(deltaTime); 
        GameRender();
        //OutputCounter();

        gLastInput = gInput; 
        lastCounter = currentCounter;
    }

    GameShutdown();
    return 0;
}

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "lh_defines.h"
#include <windows.h>

struct Window {
    HWND hwnd;
    HINSTANCE instance;
    i32 width;
    i32 height;
    char *title;
};


typedef void PlatformWorkQueueCallback(void *data);
void PlatformAddEntry(PlatformWorkQueueCallback *callback, void *data);
void PlatformCompleteAllWork();

void WindowSystemInitialize(i32 width, i32 height, char *title);
void WindowSystemShutdown();
void WindowSetSize(i32 width, i32 height);
i32 WindowGetScreenWidth();
i32 WindowGetScreenHeight();

#endif

#include "lh_file.h"
#include "lh_defines.h"
#include "lh_memory.h"
#include <windows.h>
#include <memory.h>

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

ReadFileResult ReadFile(char *path) {
    ReadFileResult result = {};
    HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(file != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER fileSize;
        GetFileSizeEx(file, &fileSize);
        result.data = malloc(fileSize.QuadPart);
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

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

struct Platform {
    Memory memory;
    Renderer renderer;
    f32 deltaTime;
};

struct ReadFileResult {
    void *data;
    size_t size;
};

ReadFileResult ReadFile(char *path, Arena *arena);
bool WriteFile(char *path, void *data, size_t size);

#endif




#ifndef _LH_FILE_H_
#define _LH_FILE_H_

struct ReadFileResult {
    void *data;
    size_t size;
};

struct Arena;
ReadFileResult ReadFile(char *path, Arena *arena);
ReadFileResult ReadFile(char *path);
bool WriteFile(char *path, void *data, size_t size);

#endif

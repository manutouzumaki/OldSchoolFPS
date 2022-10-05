#ifndef _LH_MEMORY_H_
#define _LH_MEMORY_H_

struct Memory {
    size_t used;
    size_t size;
    void *data;
};

struct Arena {
    size_t used;
    size_t size;
    void *base;
};

Memory MemoryCreate(size_t size);
Arena ArenaCreate(Memory *memory, size_t size);
void *ArenaPush(Arena *arena, size_t size);
#define ArenaPushStruct(arena, type) (type *)ArenaPush(arena, sizeof(type))
#define ArenaPushArray(arena, count, type) (type *)ArenaPush(arena, sizeof(type) * count)

#endif
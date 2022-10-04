
Memory MemoryCreate(size_t size) {
    Memory memory = {};
    memory.data = VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
    memset(memory.data, 0, size);
    memory.size = size;
    memory.used = 0;
    return memory;
}

Arena ArenaCreate(Memory *memory, size_t size) {
    ASSERT(memory->used + size <= memory->size);
    Arena arena = {};
    arena.base = (void *)((u8 *)memory->data + memory->used);
    arena.size = size;
    arena.used = 0;
    memory->used += size;
    return arena;
}


void *ArenaPush(Arena *arena, size_t size) {
    ASSERT(arena->used + size <= arena->size);
    void * result = (void *)((u8 *)arena->base + arena->used);
    arena->used += size;
    return result;
}

#ifndef _LH_SLOTMAP_H_
#define _LH_SLOTMAP_H_

#include "lh_defines.h"

#define MAX_UNSIGNED_INT 0xFFFFFFFF

struct SlotmapKey {
    u32 id;
    u32 gen;
};

template<typename T, i32 Size>
struct Slotmap {
    u32 count;
    u32 generation;
    u32 freeList;

    SlotmapKey indices[Size];
    T data[Size];
    u32 erase[Size];

    void Initialize();
    SlotmapKey Add(T element);
    void Remove(SlotmapKey key);
    T Get(SlotmapKey key);
}; 

#endif

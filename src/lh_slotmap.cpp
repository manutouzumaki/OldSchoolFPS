#include "lh_slotmap.h"

template<typename T, i32 Size>
void Slotmap<T, Size>::Initialize() {
    count = 0;
    generation = 0;
    freeList = 0;
    for(u32 i = 0; i < Size; ++i) {
        SlotmapKey *indice = indices + i;
        indice->id = i + 1;
        indice->gen = MAX_UNSIGNED_INT;
    }
    memset(data, 0, Size * sizeof(T));
    memset(erase, 0, Size * sizeof(u32));
}

template<typename T, i32 Size>
SlotmapKey Slotmap<T, Size>::Add(T element) {
    ASSERT(count < Size);
    ASSERT(generation < MAX_UNSIGNED_INT);
    ASSERT(freeList < Size);
    u32 index = freeList;
    SlotmapKey *key = indices + index;
    freeList = key->id;
    key->id = count;
    key->gen = generation;
    data[key->id] = element;
    erase[key->id] = index;
    SlotmapKey outKey = {index, generation};
    generation++;
    count++;
    return outKey;
}

template<typename T, i32 Size>
void Slotmap<T, Size>::Remove(SlotmapKey key) {
    SlotmapKey internalKey = indices[key.id];
    if(internalKey.gen != key.gen) {
        ASSERT(!"try to delete a entity already gone");
    }
    
    indices[key.id].id = freeList;
    indices[key.id].gen = MAX_UNSIGNED_INT;
    freeList = key.id;

    // now we need to delete the actual data
    unsigned int dataIndex = internalKey.id;
    
    if(dataIndex != count - 1) {
        // delete the element
        data[dataIndex] = data[count - 1];
        erase[dataIndex] = erase[count - 1]; 
        // update the modify element
        indices[erase[dataIndex]].id = dataIndex;
    }
    --count;
}

template<typename T, i32 Size>
T Slotmap<T, Size>::Get(SlotmapKey key) {
    SlotmapKey indice = indices[key.id];
    if(indice.gen != key.gen) {
        ASSERT(!"ERROR wrong element!!!");
    }
    return data[indice.id];
}

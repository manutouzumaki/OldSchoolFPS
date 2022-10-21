#include "lh_texture.h"
#include "lh_file.h"
#include "lh_memory.h"

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

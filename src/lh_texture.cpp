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

Texture *TextureCreate(char *path, Arena *objArena, Arena *dataArena) {
    ReadFileResult fileResult = ReadFile(path, dataArena);
    BitmapHeader *header = (BitmapHeader *)fileResult.data;
    Texture *texture = ArenaPushStruct(objArena, Texture);
    texture->data = (void *)((u8 *)fileResult.data + header->bitmapOffset);
    texture->width = header->width;
    texture->height = header->height;
    u32 redShift = BitScanForward(header->redMask);
    u32 greenShift = BitScanForward(header->greenMask);
    u32 blueShift = BitScanForward(header->blueMask);
    u32 alphaShift = BitScanForward(header->alphaMask);
    u32 *colorData = (u32 *)texture->data;
    for(u32 i = 0; i < texture->width*texture->height; ++i)
    {
        u32 red = (colorData[i] & header->redMask) >> redShift;       
        u32 green = (colorData[i] & header->greenMask) >> greenShift;       
        u32 blue = (colorData[i] & header->blueMask) >> blueShift;       
        u32 alpha = (colorData[i] & header->alphaMask) >> alphaShift;       
        colorData[i] = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
    }
    return texture;
}

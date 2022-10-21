#ifndef _LH_SOUND_H_
#define _LH_SOUND_H_

struct Sound;
struct Arena;

bool SoundSystemInitialize();
void SoundSystemShudown();

Sound *SoundCreate(const char *fileName, Arena *objArena, Arena *dataArena);
void SoundDestroy(Sound *sound);

void SoundPlay(Sound *sound, bool loop);
void SoundStop(Sound *sound);


#endif

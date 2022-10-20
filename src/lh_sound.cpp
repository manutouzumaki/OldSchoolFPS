#include "lh_sound.h"
#include "lh_defines.h"
#include <windows.h>
#include <xaudio2.h>
#include <malloc.h>

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

global_variable IXAudio2 *gXAudio2;
global_variable IXAudio2MasteringVoice *gMasterVoice;

internal
HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD *chunkSize, DWORD *chunkDataPosition) {
    HRESULT result = S_OK;
    if(INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    DWORD chunkType;
    DWORD chunkDataSize;
    DWORD RIFFDataSize = 0;
    DWORD fileType;
    DWORD bytesRead = 0;
    DWORD offset = 0;

    while(result == S_OK) {
        DWORD read;
        if(ReadFile(hFile, &chunkType, sizeof(DWORD), &read, NULL) == 0) {
            result = HRESULT_FROM_WIN32(GetLastError());
        }
        if(ReadFile(hFile, &chunkDataSize, sizeof(DWORD), &read, NULL) == 0) {
            result = HRESULT_FROM_WIN32(GetLastError());
        }
        switch(chunkType) {
            case fourccRIFF: {
                RIFFDataSize = chunkDataSize;
                chunkDataSize = 4;
                if(ReadFile(hFile, &fileType, sizeof(DWORD), &read, NULL) == 0) {
                    result = HRESULT_FROM_WIN32(GetLastError());
                }       
            }break;
            default: {
                if( INVALID_SET_FILE_POINTER == SetFilePointer(hFile, chunkDataSize, NULL, FILE_CURRENT)) {
                    return HRESULT_FROM_WIN32( GetLastError() );    
                }
            }
        }

        offset += sizeof(DWORD) * 2;
        if(chunkType == fourcc) {
            *chunkSize = chunkDataSize;
            *chunkDataPosition = offset;
            return S_OK;
        }
        offset += chunkDataSize;
        if(bytesRead > RIFFDataSize) return S_FALSE;
    }

    return S_OK;
}

internal
HRESULT ReadChunkData(HANDLE hFile, void *buffer, DWORD bufferSize, DWORD bufferOffset) {
    HRESULT result = S_OK;
    if(INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferOffset, NULL, FILE_BEGIN)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    DWORD read;
    if(ReadFile(hFile, buffer, bufferSize, &read, NULL) == 0) {
        result = HRESULT_FROM_WIN32(GetLastError());
    }
    return result;
}

struct Sound {
    IXAudio2SourceVoice *sourceVoice;
    u8 *buffer;
    size_t bufferSize;
};

bool SoundSystemInitialize() {
    HRESULT result = {};
    result = XAudio2Create(&gXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if(FAILED(result)) {
        OutputDebugString("pXAudio2 Initialization FAILED\n");
        return false;
    }
    result = gXAudio2->CreateMasteringVoice(&gMasterVoice);
    if(FAILED(result)) {
        OutputDebugString("pMasterVoice Initialization FAILED\n");
        return false;
    }
    return true;
}

void SoundSystemShudown() {
    if(gMasterVoice) gMasterVoice->DestroyVoice();
    if(gXAudio2) gXAudio2->Release();

}

Sound *SoundCreate(const char *fileName) {
    Sound *sound = (Sound *)malloc(sizeof(Sound));
    WAVEFORMATEXTENSIBLE wfx = {0};

    // open the file
    HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(INVALID_HANDLE_VALUE == hFile) {
        OutputDebugString("Error Reading SOUND FILE\n");
    }
    if(INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN)) {
        OutputDebugString("Error SetFilePointer FILE_BEGIN\n");
        return NULL;
    }
    // locate the RIFF chunk in the audio file, and check the file type
    DWORD chunkSize;
    DWORD chunkPosition;
    FindChunk(hFile, fourccRIFF, &chunkSize, &chunkPosition);
    DWORD fileType;
    ReadChunkData(hFile, &fileType, sizeof(DWORD), chunkPosition);
    if(fileType != fourccWAVE) {
        OutputDebugString("Error Sound file Not Supported\n");
        return NULL;
    }
    // locate the FMT chunk and copy its content into WAVEFORMATEXTENSIBLE structure
    FindChunk(hFile, fourccFMT, &chunkSize, &chunkPosition);
    ReadChunkData(hFile, &wfx, chunkSize, chunkPosition);
    // locate the DATA chunk and read its contents into a buffer
    FindChunk(hFile, fourccDATA, &chunkSize, &chunkPosition);
    sound->buffer = (u8 *)malloc(chunkSize);
    sound->bufferSize = chunkSize;
    ReadChunkData(hFile, sound->buffer, chunkSize, chunkPosition);

    // Create a source voice
    HRESULT result = gXAudio2->CreateSourceVoice(&sound->sourceVoice, (WAVEFORMATEX *)&wfx,
                                                 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL,
                                                 NULL, NULL);
    if(FAILED(result)) {
        OutputDebugString("Error Initialization IXAudio2SourceVoice FAILED\n");
        return NULL;
    }

    return  sound;
}

void SoundDestroy(Sound *sound) {
    if(sound) {
        free(sound->buffer);
        sound->sourceVoice->DestroyVoice();
        free(sound);
        sound = 0;
    }
}

void SoundPlay(Sound *sound, bool loop) {
    // submit an XAUDIO2_BUFFER to the source voice using the function SubmitSourceBuffer
    sound->sourceVoice->FlushSourceBuffers();
    XAUDIO2_BUFFER buffer = {0};
    buffer.AudioBytes = sound->bufferSize;
    buffer.pAudioData = sound->buffer;
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
    HRESULT result = sound->sourceVoice->SubmitSourceBuffer(&buffer);
    if(FAILED(result)) {
        OutputDebugString("Error submit XAUDIO2_BUFFER FAILED\n");
    }
    sound->sourceVoice->Start(0);
}

void SoundStop(Sound *sound) {
    sound->sourceVoice->Stop(0);
    sound->sourceVoice->FlushSourceBuffers();
}

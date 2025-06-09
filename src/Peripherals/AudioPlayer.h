#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include "../Config/Config.h" 

typedef struct {
    char files[MAX_AUDIO_FILES_IN_PLAYLIST][MAX_AUDIO_FILENAME_LENGTH];
    int file_count;
} audio_playlist_t;


void AudioPlayer_Init();
void AudioPlayer_Handle();
bool AudioPlayer_PlayFile(const char* filepath);
void AudioPlayer_Stop();
void AudioPlayer_SetVolume(uint8_t volume_percent); // 0-100
bool AudioPlayer_IsPlaying();
int  AudioPlayer_ScanFiles(const char* directory, audio_playlist_t* playlist);

#endif // AUDIOPLAYER_H
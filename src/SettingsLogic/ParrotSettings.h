#ifndef PARROT_SETTINGS_H
#define PARROT_SETTINGS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h> 
#include "../Config/Config.h"
#include "../App/AppGlobal.h"
#include "../Peripherals/AudioPlayer.h" // 包含新的播放器头文件

typedef enum {
    PARROT_PLAY_MODE_SINGLE_ONCE,
    PARROT_PLAY_MODE_SINGLE_LOOP,
    PARROT_PLAY_MODE_FOLDER_LOOP,
    PARROT_PLAY_MODE_ALL_RANDOM,
    PARROT_PLAY_MODE_ALL_LOOP,
    PARROT_PLAY_MODE_COUNT
} parrot_play_mode_t;

typedef struct {
    bool enabled;
    uint8_t countdown_hour;
    uint8_t countdown_minute;
    uint8_t countdown_second;
} parrot_timed_play_t;

// === 接口函数保持不变 ===
void ParrotSettings_Init();
bool ParrotSettings_ScanAudioFiles();
int  ParrotSettings_GetFileCount();
const char* ParrotSettings_GetFileName(int index);
int  ParrotSettings_GetCurrentFileIndex();
void ParrotSettings_SetCurrentFileIndex(int index);
const char* ParrotSettings_GetCurrentPlayingInfoString();
void ParrotSettings_PlaySelectedFile();
void ParrotSettings_TogglePlayPause();
void ParrotSettings_Stop();
void ParrotSettings_NextTrack();
void ParrotSettings_PreviousTrack();
bool ParrotSettings_IsCurrentlyPlaying();
void ParrotSettings_SetPlayMode(parrot_play_mode_t mode);
parrot_play_mode_t ParrotSettings_GetPlayMode();
const char* ParrotSettings_GetPlayModeString(parrot_play_mode_t mode);
void ParrotSettings_SetVolume(uint8_t volume);
uint8_t ParrotSettings_GetVolume();
void ParrotSettings_SetTimedPlaySettings(const parrot_timed_play_t* timed_play_config);
const parrot_timed_play_t* ParrotSettings_GetTimedPlaySettings();
void ParrotSettings_UpdateTimedPlayState();
bool ParrotSettings_IsTimedPlaySessionActive();
void ParrotSettings_GetFormattedRemainingCountdown(char* buffer, size_t buffer_len);

typedef void (*parrot_status_update_cb_t)(void);
void ParrotSettings_RegisterStatusUpdateCallback(parrot_status_update_cb_t cb);

void ParrotSettings_Handle(); // 在主循环中调用

#endif // PARROT_SETTINGS_H
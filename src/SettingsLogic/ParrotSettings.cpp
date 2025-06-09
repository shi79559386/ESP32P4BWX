#include "ParrotSettings.h"
#include "../Peripherals/AudioPlayer.h" // 包含新的播放器头文件
#include "../Peripherals/OutputControls.h"
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

static audio_playlist_t s_playlist;
static int s_current_file_selection_index = -1;
static uint8_t s_current_volume = PARROT_DEFAULT_VOLUME;
static parrot_play_mode_t s_current_play_mode = PARROT_PLAY_MODE_FOLDER_LOOP;
static parrot_timed_play_t s_timed_play_config = {false, 0, 10, 0};
static parrot_status_update_cb_t s_status_update_callback = NULL;

static bool s_timed_play_active_session = false;
static unsigned long s_timed_play_countdown_start_millis = 0;

static void trigger_status_update() {
    if (s_status_update_callback) {
        s_status_update_callback();
    }
}

static void play_file_by_index(int index) {
    if (index < 0 || index >= s_playlist.file_count) {
        Serial.printf("ParrotSettings: Invalid index %d to play.\n", index);
        return;
    }
    
    if (AudioPlayer_PlayFile(s_playlist.files[index])) {
        s_current_file_selection_index = index;
        Serial.printf("ParrotSettings: Playing index %d: %s\n", index, s_playlist.files[index]);

        if (s_timed_play_config.enabled && !s_timed_play_active_session) {
            s_timed_play_active_session = true;
            s_timed_play_countdown_start_millis = millis();
        }
        set_parrot_state(true);
        trigger_status_update();
    } else {
        Serial.printf("ParrotSettings: Failed to play index %d\n", index);
    }
}

static void handle_playback_finished() {
    Serial.println("ParrotSettings: Playback finished, determining next action...");
    set_parrot_state(false);

    if (s_current_play_mode == PARROT_PLAY_MODE_SINGLE_ONCE) {
        ParrotSettings_Stop();
        return;
    }
    
    if(s_timed_play_active_session){
        // 在定时会话中，播放完一首就按模式继续
    }

    ParrotSettings_NextTrack();
}

void ParrotSettings_Init() {
    Serial.println("ParrotSettings: Initializing with new AudioPlayer...");
    AudioPlayer_Init();
    
    s_current_file_selection_index = -1;
    s_playlist.file_count = 0;
    
    ParrotSettings_ScanAudioFiles();
    AudioPlayer_SetVolume(s_current_volume);
    
    if (s_playlist.file_count > 0) {
        s_current_file_selection_index = 0;
    }

    Serial.println("ParrotSettings: Initialization complete.");
    trigger_status_update();
}

bool ParrotSettings_ScanAudioFiles() {
    Serial.println("ParrotSettings: Scanning audio files from SD card...");
    int count = AudioPlayer_ScanFiles(AUDIO_ROOT_PATH, &s_playlist);
    if (s_current_file_selection_index >= count) {
        s_current_file_selection_index = (count > 0) ? 0 : -1;
    }
    trigger_status_update();
    return (count > 0);
}

int ParrotSettings_GetFileCount() {
    return s_playlist.file_count;
}

const char* ParrotSettings_GetFileName(int index) {
    if (index >= 0 && index < s_playlist.file_count) {
        const char* last_slash = strrchr(s_playlist.files[index], '/');
        return last_slash ? last_slash + 1 : s_playlist.files[index];
    }
    return "文件无效";
}

int ParrotSettings_GetCurrentFileIndex() {
    return s_current_file_selection_index;
}

void ParrotSettings_SetCurrentFileIndex(int index) {
    if (index >= 0 && index < s_playlist.file_count) {
        s_current_file_selection_index = index;
    } else if (s_playlist.file_count > 0) {
        s_current_file_selection_index = 0;
    } else {
        s_current_file_selection_index = -1;
    }
    trigger_status_update();
}

const char* ParrotSettings_GetCurrentPlayingInfoString() {
    static char info_buffer[128];
    if (!AudioPlayer_IsPlaying() || s_current_file_selection_index < 0) {
        strcpy(info_buffer, "已停止");
        return info_buffer;
    }
    
    snprintf(info_buffer, sizeof(info_buffer), "播放中: %s", ParrotSettings_GetFileName(s_current_file_selection_index));
    return info_buffer;
}

void ParrotSettings_PlaySelectedFile() {
    if (s_current_file_selection_index >= 0) {
        play_file_by_index(s_current_file_selection_index);
    }
}

void ParrotSettings_TogglePlayPause() {
    if (AudioPlayer_IsPlaying()) {
        ParrotSettings_Stop();
    } else {
        ParrotSettings_PlaySelectedFile();
    }
}

void ParrotSettings_Stop() {
    AudioPlayer_Stop();
    set_parrot_state(false);
    s_timed_play_active_session = false;
    trigger_status_update();
}

void ParrotSettings_NextTrack() {
    if (s_playlist.file_count == 0) return;
    
    int next_index = 0;
    switch(s_current_play_mode) {
        case PARROT_PLAY_MODE_ALL_RANDOM:
            next_index = random(s_playlist.file_count);
            break;
        case PARROT_PLAY_MODE_SINGLE_LOOP:
            next_index = s_current_file_selection_index;
            break;
        default: // FOLDER_LOOP, ALL_LOOP
            next_index = (s_current_file_selection_index + 1) % s_playlist.file_count;
            break;
    }
    play_file_by_index(next_index);
}

void ParrotSettings_PreviousTrack() {
    if (s_playlist.file_count == 0) return;
    int prev_index = (s_current_file_selection_index - 1 + s_playlist.file_count) % s_playlist.file_count;
    play_file_by_index(prev_index);
}

bool ParrotSettings_IsCurrentlyPlaying() {
    return AudioPlayer_IsPlaying();
}

void ParrotSettings_SetPlayMode(parrot_play_mode_t mode) {
    if (mode < PARROT_PLAY_MODE_COUNT) {
        s_current_play_mode = mode;
        trigger_status_update();
    }
}

parrot_play_mode_t ParrotSettings_GetPlayMode() {
    return s_current_play_mode;
}

const char* ParrotSettings_GetPlayModeString(parrot_play_mode_t mode) {
    switch (mode) {
        case PARROT_PLAY_MODE_SINGLE_ONCE: return "单曲";
        case PARROT_PLAY_MODE_SINGLE_LOOP: return "单曲循环";
        case PARROT_PLAY_MODE_FOLDER_LOOP: return "列表循环";
        case PARROT_PLAY_MODE_ALL_RANDOM:  return "随机";
        case PARROT_PLAY_MODE_ALL_LOOP:    return "全部循环";
        default: return "未知";
    }
}

void ParrotSettings_SetVolume(uint8_t volume) {
    if (volume > 100) volume = 100;
    s_current_volume = volume;
    AudioPlayer_SetVolume(s_current_volume);
    trigger_status_update();
}

uint8_t ParrotSettings_GetVolume() {
    return s_current_volume;
}

void ParrotSettings_SetTimedPlaySettings(const parrot_timed_play_t* timed_play_config) {
    if (timed_play_config) {
        s_timed_play_config = *timed_play_config;
        if (s_timed_play_config.enabled) {
            s_timed_play_active_session = AudioPlayer_IsPlaying();
            if(s_timed_play_active_session) s_timed_play_countdown_start_millis = millis();
        } else {
            s_timed_play_active_session = false;
        }
        trigger_status_update();
    }
}

const parrot_timed_play_t* ParrotSettings_GetTimedPlaySettings() {
    return &s_timed_play_config;
}

void ParrotSettings_UpdateTimedPlayState() {
    if (!s_timed_play_config.enabled || !s_timed_play_active_session || !AudioPlayer_IsPlaying()) {
        return;
    }

    unsigned long total_countdown_ms =
        (unsigned long)s_timed_play_config.countdown_hour * 3600000UL +
        (unsigned long)s_timed_play_config.countdown_minute * 60000UL +
        (unsigned long)s_timed_play_config.countdown_second * 1000UL;

    if (total_countdown_ms == 0) return;

    if (millis() - s_timed_play_countdown_start_millis >= total_countdown_ms) {
        Serial.println("ParrotSettings: Countdown timer expired. Stopping playback.");
        ParrotSettings_Stop();
        s_timed_play_config.enabled = false;
        trigger_status_update();
    }
}

bool ParrotSettings_IsTimedPlaySessionActive() {
    return s_timed_play_config.enabled && s_timed_play_active_session && AudioPlayer_IsPlaying();
}

void ParrotSettings_GetFormattedRemainingCountdown(char* buffer, size_t buffer_len) {
    if (!buffer || buffer_len == 0) return;

    if (s_timed_play_config.enabled && s_timed_play_active_session && AudioPlayer_IsPlaying()) {
        unsigned long total_ms = (unsigned long)s_timed_play_config.countdown_hour * 3600000UL + (unsigned long)s_timed_play_config.countdown_minute * 60000UL + (unsigned long)s_timed_play_config.countdown_second * 1000UL;
        unsigned long elapsed_ms = millis() - s_timed_play_countdown_start_millis;
        if (elapsed_ms < total_ms) {
            unsigned long remaining_ms = total_ms - elapsed_ms;
            uint8_t h = remaining_ms / 3600000UL;
            remaining_ms %= 3600000UL;
            uint8_t m = remaining_ms / 60000UL;
            remaining_ms %= 60000UL;
            uint8_t s = remaining_ms / 1000UL;
            snprintf(buffer, buffer_len, "%02u:%02u:%02u", h, m, s);
        } else {
            strcpy(buffer, "00:00:00");
        }
    } else {
        if (s_timed_play_config.enabled) {
            snprintf(buffer, buffer_len, "%02u:%02u:%02u", s_timed_play_config.countdown_hour, s_timed_play_config.countdown_minute, s_timed_play_config.countdown_second);
        } else {
            strcpy(buffer, "--:--:--");
        }
    }
}

void ParrotSettings_RegisterStatusUpdateCallback(parrot_status_update_cb_t cb) {
    s_status_update_callback = cb;
}

void ParrotSettings_Handle() {
    static bool last_playing_state = false;
    bool current_playing_state = AudioPlayer_IsPlaying();
    
    if (last_playing_state && !current_playing_state) {
        handle_playback_finished();
    }
    
    last_playing_state = current_playing_state;

    ParrotSettings_UpdateTimedPlayState();
}
#include "AudioPlayer.h"
#include <Arduino.h>
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"

static AudioGeneratorWAV *wav = nullptr;
static AudioFileSourceSD *file = nullptr;
static AudioOutputI2S *out = nullptr;
static TaskHandle_t audio_task_handle = NULL;
static char current_playing_file[MAX_AUDIO_FILENAME_LENGTH] = {0};
static bool is_playing_flag = false;
static uint8_t s_volume = 100; // Store volume percentage

// 音频播放任务，在后台运行
void audio_playback_task(void *parameter) {
    is_playing_flag = true;
    Serial.printf("AudioPlayer: Task started, playing %s\n", current_playing_file);

    file = new AudioFileSourceSD(current_playing_file);
    wav = new AudioGeneratorWAV();
    out = new AudioOutputI2S(); 
    
    out->SetPinout(I2S_BCLK_PIN, I2S_LRC_PIN, I2S_DOUT_PIN); 
    AudioPlayer_SetVolume(s_volume); // Apply current volume

    if(wav->begin(file, out)){
        while (wav->isRunning()) {
            if (!wav->loop()) {
                wav->stop();
            }
            vTaskDelay(pdMS_TO_TICKS(2)); 
        }
    } else {
        Serial.println("AudioPlayer: WAV generator failed to start.");
    }

    Serial.printf("AudioPlayer: Task finished playing %s\n", current_playing_file);
    delete wav;
    delete file;
    delete out;
    wav = nullptr;
    file = nullptr;
    out = nullptr;
    is_playing_flag = false;
    current_playing_file[0] = '\0';
    audio_task_handle = NULL;
    vTaskDelete(NULL);
}

void AudioPlayer_Init() {
    Serial.println("AudioPlayer: Initialized.");
}

void AudioPlayer_Handle() {
    // Non-blocking, handled by FreeRTOS task
}

bool AudioPlayer_PlayFile(const char* filepath) {
    if (is_playing_flag) {
        AudioPlayer_Stop();
        vTaskDelay(pdMS_TO_TICKS(50)); // Wait a bit for the old task to be deleted
    }

    if (!filepath || strlen(filepath) == 0) {
        return false;
    }

    strncpy(current_playing_file, filepath, sizeof(current_playing_file) - 1);

    xTaskCreate(
        audio_playback_task,
        "AudioPlaybackTask",
        8192,
        NULL,
        5,
        &audio_task_handle
    );

    return (audio_task_handle != NULL);
}

void AudioPlayer_Stop() {
    if (is_playing_flag && wav) {
        wav->stop(); // This will terminate the loop in the task
    }
    // The task will clean itself up. To force stop:
    if(audio_task_handle != NULL){
        vTaskDelete(audio_task_handle);
        audio_task_handle = NULL;
    }
    
    if (wav) { delete wav; wav = nullptr; }
    if (file) { delete file; file = nullptr; }
    if (out) { delete out; out = nullptr; }
    is_playing_flag = false;
    current_playing_file[0] = '\0';
    Serial.println("AudioPlayer: Playback stopped by request.");
}

void AudioPlayer_SetVolume(uint8_t volume_percent) {
    if (volume_percent > 100) volume_percent = 100;
    s_volume = volume_percent;

    if (out) {
        float vol = (float)s_volume / 100.0f; 
        out->SetGain(vol);
        Serial.printf("AudioPlayer: Volume set to %d%% (gain: %.2f)\n", s_volume, vol);
    }
}

bool AudioPlayer_IsPlaying() {
    return is_playing_flag;
}

int AudioPlayer_ScanFiles(const char* directory, audio_playlist_t* playlist) {
    if (!playlist) return 0;
    
    playlist->file_count = 0;
    File root = SD.open(directory);
    if (!root) {
        Serial.printf("AudioPlayer: Failed to open directory: %s\n", directory);
        return 0;
    }
    if (!root.isDirectory()) {
        Serial.printf("AudioPlayer: Not a directory: %s\n", directory);
        root.close();
        return 0;
    }

    File file_item = root.openNextFile();
    while (file_item) {
        if (!file_item.isDirectory()) {
            const char* filename = file_item.name();
            const char* extension = strrchr(filename, '.');
            if (extension && strcasecmp(extension, ".wav") == 0) {
                if (playlist->file_count < MAX_AUDIO_FILES_IN_PLAYLIST) {
                    snprintf(playlist->files[playlist->file_count], MAX_AUDIO_FILENAME_LENGTH, "%s", file_item.path());
                    playlist->file_count++;
                } else {
                    break; 
                }
            }
        }
        file_item.close();
        file_item = root.openNextFile();
    }
    root.close();
    Serial.printf("AudioPlayer: Scan complete. Found %d WAV files in %s\n", playlist->file_count, directory);
    return playlist->file_count;
}
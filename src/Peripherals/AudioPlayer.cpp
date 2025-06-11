// 文件: src/Peripherals/AudioPlayer.cpp
// 版本: 支持外接 SPI SD 卡, 受 Config.h 控制

#include "AudioPlayer.h"
#include "../Config/Config.h" // 包含配置文件
#include <Arduino.h>
#include "FS.h"
#include "AudioFileSourceSD.h"
#include "AudioOutputI2S.h"
#include "AudioGeneratorWAV.h"
#include "AudioGeneratorMP3.h"

// 仅当启用外接音频SD卡时，才包含硬件库并定义对象
#if ENABLE_AUDIO_SD == 1
#include <SPI.h>
#include <SD.h>
static SPIClass spi_audio(HSPI); // 使用 HSPI 总线
#endif

// 全局对象和状态变量
static AudioOutputI2S *out = nullptr;
static AudioGenerator *g_audio_generator = nullptr;
static AudioFileSourceSD *file_task_instance = nullptr;
static TaskHandle_t audio_task_handle = NULL;
static volatile bool g_is_playing_flag = false;
static volatile bool g_stop_request_flag = false;
static uint8_t s_volume = 100;

void audio_playback_task(void *parameter) {
    char* filepath = (char*)parameter;
    Serial.printf("AudioPlayer: Task started, playing from external SD: %s\n", filepath);
    const char* extension = strrchr(filepath, '.');
    if (!extension) { delete[] filepath; vTaskDelete(NULL); return; }
    file_task_instance = new AudioFileSourceSD(filepath);
    if (strcasecmp(extension, ".wav") == 0) g_audio_generator = new AudioGeneratorWAV();
    else if (strcasecmp(extension, ".mp3") == 0) g_audio_generator = new AudioGeneratorMP3();
    else { delete file_task_instance; delete[] filepath; vTaskDelete(NULL); return; }

    if (g_audio_generator->begin(file_task_instance, out)) {
        g_is_playing_flag = true;
        while (g_audio_generator->isRunning()) {
            if (g_stop_request_flag) { g_audio_generator->stop(); break; }
            if (!g_audio_generator->loop()) { g_audio_generator->stop(); }
            vTaskDelay(pdMS_TO_TICKS(2));
        }
    }
    delete g_audio_generator; delete file_task_instance; delete[] filepath;
    g_audio_generator = nullptr; file_task_instance = nullptr;
    g_is_playing_flag = false; g_stop_request_flag = false;
    audio_task_handle = NULL; vTaskDelete(NULL);
}

void AudioPlayer_Init() {
    Serial.println("AudioPlayer: Initializing...");
    if (out == nullptr) {
        out = new AudioOutputI2S();
        out->SetPinout(I2S_BCLK_PIN, I2S_LRC_PIN, I2S_DOUT_PIN);
    }
    AudioPlayer_SetVolume(PARROT_DEFAULT_VOLUME);

#if ENABLE_AUDIO_SD == 1
    Serial.println("Initializing external SPI SD card for audio...");
    spi_audio.begin(AUDIO_SD_SCLK_PIN, AUDIO_SD_MISO_PIN, AUDIO_SD_MOSI_PIN, AUDIO_SD_CS_PIN);
    if (SD.begin(AUDIO_SD_CS_PIN, spi_audio)) {
        Serial.println("✅ External Audio SD card initialized successfully.");
    } else {
        Serial.println("❌ External Audio SD card initialization failed!");
    }
#else
    Serial.println("AudioPlayer: External SD card is disabled in Config.h.");
#endif
}

bool AudioPlayer_PlayFile(const char* filepath) {
#if ENABLE_AUDIO_SD == 0
    Serial.println("AudioPlayer: Cannot play file, external SD is disabled.");
    return false;
#else
    if (g_is_playing_flag) { AudioPlayer_Stop(); vTaskDelay(pdMS_TO_TICKS(50)); }
    if (!filepath || strlen(filepath) == 0) return false;
    char* path_for_task = new char[strlen(filepath) + 1];
    strcpy(path_for_task, filepath);
    g_stop_request_flag = false;
    if (xTaskCreate(audio_playback_task, "AudioTask", 8192, path_for_task, 5, &audio_task_handle) != pdPASS) {
        delete[] path_for_task; return false;
    }
    return true;
#endif
}

void AudioPlayer_Stop() {
    if (g_is_playing_flag && audio_task_handle != NULL) g_stop_request_flag = true;
}

void AudioPlayer_SetVolume(uint8_t volume_percent) {
    if (volume_percent > 100) volume_percent = 100;
    s_volume = volume_percent;
    if (out) out->SetGain((float)s_volume / 100.0f);
}

bool AudioPlayer_IsPlaying() {
    return g_is_playing_flag;
}

int AudioPlayer_ScanFiles(const char* directory, audio_playlist_t* playlist) {
    if (!playlist) return 0;
    playlist->file_count = 0;
#if ENABLE_AUDIO_SD == 0
    Serial.println("AudioPlayer: Cannot scan files, external SD is disabled.");
    return 0;
#else
    fs::File root = SD.open(directory);
    if (!root || !root.isDirectory()) {
        Serial.printf("AudioPlayer: Failed to open directory on external SD: %s\n", directory);
        if (root) root.close();
        return 0;
    }
    fs::File file_item;
    while ((file_item = root.openNextFile())) {
        if (!file_item.isDirectory()) {
            const char* filename = file_item.name();
            const char* ext = strrchr(filename, '.');
            if (ext && (strcasecmp(ext, ".wav") == 0 || strcasecmp(ext, ".mp3") == 0)) {
                if (playlist->file_count < MAX_AUDIO_FILES_IN_PLAYLIST) {
                    snprintf(playlist->files[playlist->file_count++], MAX_AUDIO_FILENAME_LENGTH, "%s", filename);
                } else {
                    file_item.close(); break;
                }
            }
        }
        file_item.close();
    }
    root.close();
    Serial.printf("AudioPlayer: Scan complete. Found %d audio files in %s\n", playlist->file_count, directory);
    return playlist->file_count;
#endif
}

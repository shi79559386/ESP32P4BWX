// 文件: src/Peripherals/AudioPlayer.cpp
// 版本: 支持 WAV 和 MP3 双格式

#include "AudioPlayer.h"
#include <Arduino.h>
#include "AudioFileSourceSD.h"
#include "AudioOutputI2S.h"
// 引入两种解码器的头文件
#include "AudioGeneratorWAV.h"
#include "AudioGeneratorMP3.h"

// --- 全局对象和状态变量 ---
static AudioOutputI2S *out = nullptr; // I2S输出对象，全局唯一
static AudioGenerator *g_audio_generator = nullptr; // 通用解码器指针
static AudioFileSourceSD *file_task_instance = nullptr;

static TaskHandle_t audio_task_handle = NULL;
static volatile bool g_is_playing_flag = false;
static volatile bool g_stop_request_flag = false;


static uint8_t s_volume = 100;


// --- 后台音频播放任务 ---
void audio_playback_task(void *parameter) {
    char* filepath = (char*)parameter;
    Serial.printf("AudioPlayer: 任务启动, 准备播放 %s\n", filepath);
    
    // 1. 根据文件扩展名判断文件类型
    const char* extension = strrchr(filepath, '.');
    if (!extension) {
        Serial.println("AudioPlayer: 错误 - 文件没有扩展名.");
        delete[] filepath;
        vTaskDelete(NULL);
        return;
    }

    // 2. 创建文件源和对应的解码器
    file_task_instance = new AudioFileSourceSD(filepath);
    
    if (strcasecmp(extension, ".wav") == 0) {
        Serial.println("AudioPlayer: 检测到 WAV 文件格式.");
        g_audio_generator = new AudioGeneratorWAV();
    } else if (strcasecmp(extension, ".mp3") == 0) {
        Serial.println("AudioPlayer: 检测到 MP3 文件格式.");
        g_audio_generator = new AudioGeneratorMP3();
    } else {
        Serial.printf("AudioPlayer: 错误 - 不支持的文件格式: %s\n", extension);
        delete file_task_instance;
        delete[] filepath;
        vTaskDelete(NULL);
        return;
    }

    // 3. 开始播放
    if (g_audio_generator->begin(file_task_instance, out)) {
        g_is_playing_flag = true;
        Serial.println("AudioPlayer: 开始解码循环.");
        
        while (g_audio_generator->isRunning()) {
            if (g_stop_request_flag) {
                Serial.println("AudioPlayer: 检测到停止请求，即将退出任务.");
                g_audio_generator->stop();
                break;
            }
            if (!g_audio_generator->loop()) {
                g_audio_generator->stop();
            }
            vTaskDelay(pdMS_TO_TICKS(2));
        }
    } else {
        Serial.println("AudioPlayer: 错误 - 解码器启动失败.");
    }

    // 4. 任务结束，清理资源
    Serial.printf("AudioPlayer: 任务正常结束, 清理资源...\n");
    delete g_audio_generator;
    delete file_task_instance;
    delete[] filepath;

    g_audio_generator = nullptr;
    file_task_instance = nullptr;
    
    g_is_playing_flag = false;
    g_stop_request_flag = false;
    audio_task_handle = NULL;
    vTaskDelete(NULL);
}


// --- 公共接口实现 ---

void AudioPlayer_Init() {
    Serial.println("AudioPlayer: 初始化...");
    if (out == nullptr) {
        out = new AudioOutputI2S();
        out->SetPinout(I2S_BCLK_PIN, I2S_LRC_PIN, I2S_DOUT_PIN);
    }
    Serial.println("AudioPlayer: 初始化完成.");
}

bool AudioPlayer_PlayFile(const char* filepath) {
    if (g_is_playing_flag) {
        AudioPlayer_Stop();
        while(g_is_playing_flag) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    if (!filepath || strlen(filepath) == 0) return false;

    char* path_for_task = new char[strlen(filepath) + 1];
    strcpy(path_for_task, filepath);
    
    g_stop_request_flag = false;

    BaseType_t result = xTaskCreate(
        audio_playback_task, "AudioPlaybackTask", 8192, // MP3需要更大的栈, 8KB比较安全
        (void*)path_for_task, 5, &audio_task_handle
    );

    if (result != pdPASS) {
        Serial.println("AudioPlayer: 错误 - 无法创建音频任务.");
        delete[] path_for_task;
        return false;
    }
    return true;
}

void AudioPlayer_Stop() {
    if (g_is_playing_flag && audio_task_handle != NULL) {
        Serial.println("AudioPlayer: 发出停止请求...");
        g_stop_request_flag = true;
    }
}

void AudioPlayer_SetVolume(uint8_t volume_percent) {
    if (volume_percent > 100) volume_percent = 100;
    s_volume = volume_percent;
    if (out) {
        float gain = (float)s_volume / 100.0f;
        out->SetGain(gain);
    }
}

bool AudioPlayer_IsPlaying() {
    return g_is_playing_flag;
}

int AudioPlayer_ScanFiles(const char* directory, audio_playlist_t* playlist) {
    if (!playlist) return 0;
    
    playlist->file_count = 0;
    File root = SD.open(directory);
    if (!root) {
        Serial.printf("AudioPlayer: 无法打开目录: %s\n", directory);
        return 0;
    }
    if (!root.isDirectory()) {
        Serial.printf("AudioPlayer: %s 不是一个目录.\n", directory);
        root.close();
        return 0;
    }

    File file_item = root.openNextFile();
    while (file_item) {
        if (!file_item.isDirectory()) {
            const char* filename = file_item.name();
            const char* extension = strrchr(filename, '.');
            // 同时检查 .wav 和 .mp3 扩展名
            if (extension && (strcasecmp(extension, ".wav") == 0 || strcasecmp(extension, ".mp3") == 0)) {
                if (playlist->file_count < MAX_AUDIO_FILES_IN_PLAYLIST) {
                    snprintf(playlist->files[playlist->file_count], MAX_AUDIO_FILENAME_LENGTH, "%s", file_item.path());
                    playlist->file_count++;
                } else {
                    Serial.println("AudioPlayer: 播放列表已满, 停止扫描.");
                    file_item.close();
                    break;
                }
            }
        }
        file_item.close();
        file_item = root.openNextFile();
    }
    root.close();
    Serial.printf("AudioPlayer: 扫描完成. 在 %s 中找到 %d 个音频文件\n", directory, playlist->file_count);
    return playlist->file_count;
}
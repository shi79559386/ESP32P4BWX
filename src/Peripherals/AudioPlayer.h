// 文件: src/Peripherals/AudioPlayer.h

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include "../Config/Config.h" // 确保这里有 MAX_AUDIO_FILES_IN_PLAYLIST 等宏定义

// 播放列表结构体定义
typedef struct {
    char files[MAX_AUDIO_FILES_IN_PLAYLIST][MAX_AUDIO_FILENAME_LENGTH];
    int file_count;
} audio_playlist_t;

// --- 公共接口函数 ---

// 初始化音频播放器，包括I2S引脚
void AudioPlayer_Init();

// 播放指定路径的文件
bool AudioPlayer_PlayFile(const char* filepath);

// 请求停止当前播放
void AudioPlayer_Stop();

// 设置音量 (0-100)
void AudioPlayer_SetVolume(uint8_t volume_percent);

// 检查是否正在播放
bool AudioPlayer_IsPlaying();

// 扫描指定目录的WAV文件
int AudioPlayer_ScanFiles(const char* directory, audio_playlist_t* playlist);

#endif // AUDIOPLAYER_H
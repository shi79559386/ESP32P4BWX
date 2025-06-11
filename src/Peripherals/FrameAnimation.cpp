// 文件: src/Peripherals/FrameAnimation.cpp
// 版本: 保留所有原有函数，仅修正开机动画播放逻辑，从 SPIFFS 读取

#include "FrameAnimation.h"
#include "../Config/Config.h"
#include <Arduino.h>
#include <algorithm>
#include <esp_task_wdt.h>
#include <esp_heap_caps.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPIFFS.h>           // 使用 SPIFFS 内部闪存文件系统
#include <FS.h>
#include "MjpegClass.h"

// 如果您的项目也使用 LovyanGFX, 请保留此 include
#include <LovyanGFX.hpp>


// --- 状态变量 (保留) ---
static bool s_initialized = false;
static bool s_is_playing = false;
static bool s_loop_playback = false;
static int  s_current_frame_index = 0;
static uint16_t* full_frame_buf = nullptr;
static MjpegClass mjpeg;


// --- 任务创建函数 (保留) ---
// 注意: 此函数使用了 LGFX 类型, 如果您在其他地方调用它,
// 请确保传入的 tft 对象是 LGFX 类型。
void FrameAnimation_StartTask(LGFX& tft) {
    xTaskCreatePinnedToCore(
        [](void* p) {
            auto lcd = static_cast<LGFX*>(p);
            // 假设 FrameAnimation_PlayBootSequence 有一个 LGFX 的重载版本
            // FrameAnimation_PlayBootSequence(*lcd);
            Serial.println("Note: LGFX animation task started.");
            vTaskDelete(nullptr);
        },
        "MJPEGTask",
        8192,
        &tft,
        1,
        nullptr,
        1
    );
}


// --- 其他原有函数 (全部保留) ---
bool FrameAnimation_Init(void) {
    if (s_initialized) return true;
    size_t fb_sz = size_t(ANIM_FRAME_WIDTH) * ANIM_FRAME_HEIGHT * sizeof(uint16_t);
    full_frame_buf = (uint16_t*)heap_caps_malloc(fb_sz, MALLOC_CAP_SPIRAM);
    if (full_frame_buf) {
        Serial.printf("✅ PSRAM buffer for frames: %u bytes\n", (unsigned)fb_sz);
    } else {
        Serial.println("⚠️ PSRAM allocation for frames failed.");
    }
    s_initialized = true;
    s_current_frame_index = 0;
    return true;
}

void FrameAnimation_Start(bool loop) {
    s_is_playing          = true;
    s_loop_playback       = loop;
    s_current_frame_index = 0;
}

void FrameAnimation_Stop(void) {
    s_is_playing = false;
}

bool FrameAnimation_IsPlaying(void) {
    return s_is_playing;
}

bool FrameAnimation_GetAndAdvanceToNextFramePath(char* buf, size_t len) {
    if (!s_is_playing) return false;
    if (s_current_frame_index >= ANIM_TOTAL_FRAMES) {
        if (!s_loop_playback) {
            s_is_playing = false;
            return false;
        }
        s_current_frame_index = 0;
    }
    snprintf(buf, len, ANIM_FRAME_PATH_FORMAT, s_current_frame_index + 1);
    s_current_frame_index++;
    return true;
}

void FrameAnimation_DeInit(void) {
    s_initialized = false;
    if(full_frame_buf) {
        free(full_frame_buf);
        full_frame_buf = nullptr;
    }
    Serial.println("FrameAnimation: De-initialized.");
}


// --- 核心修改：统一并修正开机动画播放函数 ---
bool FrameAnimation_PlayBootSequence(Adafruit_ST7789& tft) {
    Serial.println("Playing boot animation from SPIFFS...");

    // 1. 从 SPIFFS 打开动画文件
    //    确保文件路径与你放在 data 文件夹中的路径一致
    fs::File mjpegFile = SPIFFS.open("/boot.mjpeg", "r");
    if (!mjpegFile || mjpegFile.isDirectory()) {
        Serial.println("Error: Failed to open /boot.mjpeg from SPIFFS!");
        return false;
    }
    
    // 2. 分配用于解码的缓冲区
    constexpr size_t BUF_SZ = 32 * 1024;
    uint8_t* buf = (uint8_t*)heap_caps_malloc(BUF_SZ, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buf) {
        Serial.println("Error: Failed to allocate MJPEG buffer from PSRAM!");
        mjpegFile.close();
        return false;
    }

    // 3. 设置解码器，数据源现在是 SPIFFS 文件
    mjpeg.setup(mjpegFile, buf, &tft, false);

    // 4. 循环解码并显示每一帧
    while (mjpeg.readMjpegBuf()) {
        mjpeg.drawJpg();
    }

    // 5. 释放资源
    mjpegFile.close();
    free(buf);
    Serial.println("Boot animation finished.");
    return true;
}

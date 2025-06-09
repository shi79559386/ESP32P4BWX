#include "FrameAnimation.h"
#include "../Config/Config.h"
#include <SD.h>
#include <Arduino.h>
#include <algorithm>
#include <esp_task_wdt.h>
#include "../Config/LGFX_Config.h"  // 包含 LGFX 定义
#include "App/AppController.h"      // 用于检查 SD 卡是否就绪


// 状态变量
static bool s_initialized = false;
static bool s_is_playing = false;
static bool s_loop_playback = false;
static int  s_current_frame_index = 0;

// 临时的行缓冲区
static uint16_t frame_chunk_buffer[ANIM_FRAME_WIDTH * ANIM_FRAME_BUFFER_LINES];

bool FrameAnimation_Init(void) {
    if (s_initialized) return true;
    s_initialized = true;
    s_current_frame_index = 0;
    Serial.println("FrameAnimation: 初始化完成");
    return true;
}

void FrameAnimation_Start(bool loop) {
    s_is_playing        = true;
    s_loop_playback     = loop;
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
    // 比如帧路径格式 "frame_%03d.raw"
    snprintf(buf, len, ANIM_FRAME_PATH_FORMAT, s_current_frame_index + 1);
    s_current_frame_index++;
    return true;
}

void FrameAnimation_DeInit(void) {
    s_initialized = false;
    Serial.println("FrameAnimation: 已反初始化");
}

bool FrameAnimation_PlayBootSequence(LGFX& tft) {
    // 确保初始化且 SD 卡就绪
    if (!s_initialized || !AppController_IsMainSDReady()) return false;

    // 计算总帧平均间隔
    const float interval_ms = float(TARGET_TOTAL_DURATION_MS) / ANIM_TOTAL_FRAMES;
    FrameAnimation_Start(false);

    char frame_path[128];
    uint32_t start_ts = ::millis();
    int    frames_shown = 0;

    while (FrameAnimation_GetAndAdvanceToNextFramePath(frame_path, sizeof(frame_path))) {
        esp_task_wdt_reset();  // 喂狗

        // 打开 SD 卡上的原始像素文件
        File f = SD.open(frame_path);
        if (!f) {
            Serial.printf("❌ 无法打开动画帧 %s\n", frame_path);
            FrameAnimation_Stop();
            return false;
        }

        // 分行读取并绘制
        for (int y = 0; y < ANIM_FRAME_HEIGHT; y += ANIM_FRAME_BUFFER_LINES) {
            int h = std::min(ANIM_FRAME_BUFFER_LINES, ANIM_FRAME_HEIGHT - y);
            size_t bytes_to_read = ANIM_FRAME_WIDTH * h * sizeof(uint16_t);
            size_t br = f.read((uint8_t*)frame_chunk_buffer, bytes_to_read);
            if (br != bytes_to_read) {
                Serial.printf("⚠️ 帧 %s 读取到 %u/%u 字节\n",
                              frame_path, (unsigned)br, (unsigned)bytes_to_read);
            }

            // 使用 LovyanGFX 的开始／结束写入流程
            tft.startWrite();
            tft.setAddrWindow(0, y, ANIM_FRAME_WIDTH - 1, y + h - 1);
            tft.writePixels(frame_chunk_buffer, ANIM_FRAME_WIDTH * h);
            tft.endWrite();

            esp_task_wdt_reset();
            yield();
        }
        f.close();

        // 根据时间差做延时
        frames_shown++;
        int32_t wait_ms = int32_t(start_ts + frames_shown * interval_ms - ::millis());
        if (wait_ms > 2) {
            ::delay(wait_ms);
        }
    }

    FrameAnimation_Stop();
    return true;
}

#include "../Config/Config.h"
#include "../Config/LGFX_Config.h"
#include "FrameAnimation.h"
#include "App/AppController.h"
#include <Arduino.h>
#include <algorithm>
#include <esp_task_wdt.h>
#include <esp_heap_caps.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <SD_MMC.h>


// 状态变量
static bool s_initialized = false;
static bool s_is_playing = false;
static bool s_loop_playback = false;
static int  s_current_frame_index = 0;
static uint16_t* full_frame_buf = nullptr;
static void animationTask(void* parm);

// 临时的行缓冲区
static uint16_t frame_chunk_buffer[ANIM_FRAME_WIDTH * ANIM_FRAME_BUFFER_LINES];


void FrameAnimation_StartTask(LGFX& tft) {
    if (s_is_playing) return;
    xTaskCreatePinnedToCore(
        animationTask, "AnimTask", 4096, &tft, 1, nullptr, 1
    );
}

// 任务函数
static void animationTask(void* parm) {
    LGFX* pTft = static_cast<LGFX*>(parm);
    FrameAnimation_Init();
    FrameAnimation_PlayBootSequence(*pTft);
    FrameAnimation_DeInit();
    vTaskDelete(nullptr);
}

bool FrameAnimation_Init(void) {
    if (s_initialized) return true;
    size_t fb_sz = size_t(ANIM_FRAME_WIDTH) * ANIM_FRAME_HEIGHT * sizeof(uint16_t);
    full_frame_buf = (uint16_t*)heap_caps_malloc(fb_sz, MALLOC_CAP_SPIRAM);
    if (full_frame_buf) {
        Serial.printf("✅ PSRAM 缓冲: %u bytes\n", (unsigned)fb_sz);
    } else {
        Serial.println("⚠️ PSRAM 分配失败，回退分段");
    }
    s_initialized = true;
    s_current_frame_index = 0;
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
    Serial.println("▶️ FrameAnimation_PlayBootSequence start");
    if (!s_initialized || !AppController_IsMainSDReady()) return false;
    FrameAnimation_Start(false);

    const int w = ANIM_FRAME_WIDTH, h = ANIM_FRAME_HEIGHT, chunk_h = ANIM_FRAME_BUFFER_LINES;
    char path[128];
    uint32_t start_ts = millis();
    int frame_cnt = 0;

    while (FrameAnimation_GetAndAdvanceToNextFramePath(path, sizeof(path))) {
        Serial.printf("🔄 Frame %d → %s\n", ++frame_cnt, path);
        // ↓↓↓ 这里改成直接 open(path) ↓↓↓
        Serial.printf("    opening: %s\n", path);
        File f = SD_MMC.open(path);
        if (!f) {
            Serial.printf("❌ open failed: %s\n", path);
            break;
        }

        tft.startWrite();
        tft.setWindow(0, 0, w, h);

        for (int y = 0; y < h; y += chunk_h) {
            int lines = min(chunk_h, h - y);
            size_t bytes = size_t(w) * lines * sizeof(uint16_t);
            size_t rd = f.read((uint8_t*)frame_chunk_buffer, bytes);
            Serial.printf("      🔽 read y=%d lines=%d bytes=%u\n", y, lines, (unsigned)rd);
            for (size_t i = 0; i < size_t(w) * lines; i++) {
                frame_chunk_buffer[i] = __builtin_bswap16(frame_chunk_buffer[i]);
            }
            tft.writePixels(frame_chunk_buffer, w * lines);
            yield();
        }

        tft.endWrite();
        f.close();

        // 严格帧率
        float target = start_ts + frame_cnt * (TARGET_TOTAL_DURATION_MS / float(ANIM_TOTAL_FRAMES));
        int32_t dt = int32_t(target) - int32_t(millis());
        if (dt > 0) vTaskDelay(pdMS_TO_TICKS(dt));
    }

    Serial.println("▶️ FrameAnimation_PlayBootSequence end");
    return true;
}


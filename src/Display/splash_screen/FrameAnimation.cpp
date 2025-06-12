// src/splash_screen/FrameAnimation.cpp
#include "FrameAnimation.h"
#include <SD_MMC.h>
#include <esp_heap_caps.h>       // heap_caps_malloc/free

// 例程版的 MjpegClass 里会用到这两个静态成员
//uint8_t     MjpegClass::_read_buf[READ_BUFFER_SIZE];
//MjpegClass* MjpegClass::_instance = nullptr;

bool FrameAnimation_Play(LGFX* lcd, const char* path) {
  if (!lcd) {
        Serial.println("Error: LCD is null");
    return false;
  }
  if (SD_MMC.cardType() == CARD_NONE) {
        Serial.println("Error: No SD card detected");
    return false;
  }

    // 1) 打开文件
    File f = SD_MMC.open(path, FILE_READ);
  if (!f || f.isDirectory()) {
        Serial.printf("Error: open %s failed\n", path);
    return false;
  }
    Serial.printf("Opened %s, size=%u bytes\n", path, (unsigned)f.size());

    // 2) 分配帧缓冲（PSRAM 优先）
    uint8_t *mjpeg_buf = (uint8_t*)heap_caps_malloc(MJPEG_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (!mjpeg_buf) {
        Serial.println("PSRAM alloc failed, try heap");
        mjpeg_buf = (uint8_t*)malloc(MJPEG_BUFFER_SIZE);
        if (!mjpeg_buf) {
            Serial.println("malloc MJPEG_BUFFER_SIZE failed");
      f.close();
      return false;
    }
  }

    // 3) 创建解码器实例并 setup
    MjpegClass mjpeg;
    if (!mjpeg.setup(f, mjpeg_buf, lcd, true)) {
        Serial.println("MjpegClass.setup failed");
        heap_caps_free(mjpeg_buf);
    f.close();
    return false;
  }

    // 4) 构建索引，一次扫描，把每帧 SOI 偏移都记下来
    if (!mjpeg.buildIndex()) {
        Serial.println("MjpegClass.buildIndex failed");
        heap_caps_free(mjpeg_buf);
        f.close();
        return false;
    }

    // 5) 播放
    Serial.println("Starting indexed playback...");
    int frameCount = 0;
    unsigned long t0 = millis(), lastFps = t0;
    int fpsCounter = 0;
    while (mjpeg.readMjpegBuf()) {
        if (mjpeg.drawJpg()) {
            frameCount++;
            fpsCounter++;
        }
        // 每秒输出 FPS
        unsigned long now = millis();
        if (now - lastFps >= 1000) {
            float fps = fpsCounter * 1000.0f / (now - lastFps);
            Serial.printf("FPS: %.1f\n", fps);
            lastFps = now;
            fpsCounter = 0;
        }
        // 25fps 固定速率
        unsigned long target = t0 + frameCount * 40;
        if (millis() < target) delay(target - millis());
    }

    Serial.printf("Playback done: %d frames\n", frameCount);
  f.close();
    heap_caps_free(mjpeg_buf);
    return frameCount > 0;
}

#pragma once

#include <SD_MMC.h>
#include <TJpg_Decoder.h>
#include "../Config/LGFX_Config.h"
#include "esp_heap_caps.h"

#define READ_BUFFER_SIZE    4096
#define MJPEG_BUFFER_SIZE  (320 * 480 * 2)
#define MAX_FRAMES         500  // 支持最多500帧

class MjpegClass {
public:
    MjpegClass();
    ~MjpegClass();
    
    bool setup(File input, LGFX* lcd);
    bool buildIndex();      // 构建帧索引
    bool readMjpegBuf();    // 按索引读取帧
    bool drawJpg();

private:
    File    _input;
    LGFX*   _lcd;
    uint8_t* _mjpeg_buf = nullptr;
    int32_t _mjpeg_buf_offset = 0;
    
    // 帧索引相关
    uint32_t _frame_offsets[MAX_FRAMES + 1];  // 每帧的起始偏移
    int _frame_count = 0;                      // 总帧数
    int _current_frame = 0;                    // 当前帧索引
    
    // 读取缓冲区
    static uint8_t _read_buf[READ_BUFFER_SIZE];
    
    // 静态实例和回调
    static MjpegClass* _instance;
    static bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
    
    friend bool FrameAnimation_Play(LGFX* lcd, const char* path);
};

// 播放函数
bool FrameAnimation_Play(LGFX* lcd, const char* path);
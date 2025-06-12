#include "FrameAnimation.h"
#include <SD_MMC.h>
#include <TJpg_Decoder.h>
#include "esp_heap_caps.h"

// 静态成员定义
uint8_t MjpegClass::_read_buf[READ_BUFFER_SIZE];
MjpegClass* MjpegClass::_instance = nullptr;

MjpegClass::MjpegClass() {
    _instance = this;
}

MjpegClass::~MjpegClass() {
    if (_mjpeg_buf) {
        heap_caps_free(_mjpeg_buf);
        _mjpeg_buf = nullptr;
    }
}

bool MjpegClass::setup(File input, LGFX* lcd) {
    _input = input;
    _lcd = lcd;
    
    // 关键改动1：在PSRAM中分配大缓冲区，避免内部SRAM耗尽
    _mjpeg_buf = (uint8_t*)heap_caps_malloc(MJPEG_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (!_mjpeg_buf) {
        Serial.println("❌ PSRAM allocation failed!");
        // 如果PSRAM失败，尝试内部RAM但使用更小的缓冲区
        _mjpeg_buf = (uint8_t*)malloc(MJPEG_BUFFER_SIZE / 2);
        if (!_mjpeg_buf) {
            Serial.println("❌ Memory allocation totally failed!");
            return false;
        }
        Serial.println("⚠️ Using internal RAM with reduced buffer");
    } else {
        Serial.println("✅ PSRAM allocation successful");
    }
    
    // 配置TJpg解码器
    TJpgDec.setJpgScale(0);        // 原始尺寸，确保全屏
    TJpgDec.setSwapBytes(false);   
    TJpgDec.setCallback(tft_output);
    
    // 为解码器分配工作缓冲区（也使用PSRAM）
    size_t workBufferSize = 3200;
    void* workBuffer = heap_caps_malloc(workBufferSize, MALLOC_CAP_SPIRAM);
    if (workBuffer) {
        TJpgDec.setJpgBuffer((uint8_t*)workBuffer, workBufferSize);
    }
    
    return true;
}

bool MjpegClass::buildIndex() {
    _frame_count = 0;
    _current_frame = 0;
    
    uint8_t prev = 0, cur;
    size_t file_size = _input.size();
    
    Serial.printf("Building frame index for %d bytes file...\n", file_size);
    unsigned long indexStart = millis();
    
    // 回到文件开头
    _input.seek(0);
    
    // 批量读取以提高效率
    size_t pos = 0;
    while (pos < file_size && _frame_count < MAX_FRAMES) {
        int bytes_read = _input.read(_read_buf, READ_BUFFER_SIZE);
        if (bytes_read <= 0) break;
        
        for (int i = 0; i < bytes_read && _frame_count < MAX_FRAMES; i++) {
            cur = _read_buf[i];
            
            // 检测SOI标记 (0xFF 0xD8) - 这是真正的帧开始
            if (prev == 0xFF && cur == 0xD8) {
                _frame_offsets[_frame_count] = pos + i - 1;
                _frame_count++;
                
                if (_frame_count % 50 == 0) {
                    Serial.printf("Found %d frames...\n", _frame_count);
                }
            }
            
            prev = cur;
        }
        
        pos += bytes_read;
    }
    
    // 文件末尾作为最后一帧的结束
    _frame_offsets[_frame_count] = file_size;
    
    unsigned long indexTime = millis() - indexStart;
    Serial.printf("✅ Index built in %lu ms: %d frames found\n", indexTime, _frame_count);
    
    // 输出帧信息用于调试
    if (_frame_count > 0) {
        Serial.println("Frame info:");
        for (int i = 0; i < min(5, _frame_count); i++) {
            uint32_t frame_size = _frame_offsets[i + 1] - _frame_offsets[i];
            Serial.printf("  Frame %d: offset=%d, size=%d bytes\n", 
                         i, _frame_offsets[i], frame_size);
        }
        
        // 计算平均帧大小
        uint32_t total_size = _frame_offsets[_frame_count] - _frame_offsets[0];
        uint32_t avg_size = total_size / _frame_count;
        Serial.printf("Average frame size: %d bytes\n", avg_size);
    }
    
    // 重置文件指针
    _input.seek(0);
    
    return _frame_count > 0;
}

bool MjpegClass::readMjpegBuf() {
    // 关键改动2：基于索引的随机访问，确保读取完整帧
    if (_current_frame >= _frame_count) {
        return false;
    }
    
    // 计算当前帧的精确位置和大小
    uint32_t start = _frame_offsets[_current_frame];
    uint32_t end = _frame_offsets[_current_frame + 1];
    uint32_t frame_size = end - start;
    
    // 检查帧大小是否合理
    if (frame_size > MJPEG_BUFFER_SIZE) {
        Serial.printf("❌ Frame %d too large: %d bytes (buffer: %d)\n", 
                     _current_frame, frame_size, MJPEG_BUFFER_SIZE);
        _current_frame++;
        return readMjpegBuf(); // 跳过这帧，尝试下一帧
    }
    
    if (frame_size < 1000) { // 太小的帧可能是损坏的
        Serial.printf("⚠️ Frame %d suspiciously small: %d bytes\n", 
                     _current_frame, frame_size);
    }
    
    // 直接定位到帧开始位置
    _input.seek(start);
    
    // 一次性读取整帧数据
    _mjpeg_buf_offset = _input.read(_mjpeg_buf, frame_size);
    
    if (_mjpeg_buf_offset != frame_size) {
        Serial.printf("❌ Read error for frame %d: expected %d, got %d\n", 
                     _current_frame, frame_size, _mjpeg_buf_offset);
        _current_frame++;
        return false;
    }
    
    // 验证帧数据完整性
    if (_mjpeg_buf[0] != 0xFF || _mjpeg_buf[1] != 0xD8) {
        Serial.printf("❌ Invalid SOI for frame %d\n", _current_frame);
        _current_frame++;
        return false;
    }
    
    // 可选：验证EOI
    if (_mjpeg_buf_offset >= 2 && 
        (_mjpeg_buf[_mjpeg_buf_offset-2] != 0xFF || 
         _mjpeg_buf[_mjpeg_buf_offset-1] != 0xD9)) {
        Serial.printf("⚠️ Missing EOI for frame %d\n", _current_frame);
    }
    
    _current_frame++;
    return true;
}

bool MjpegClass::drawJpg() {
    if (_mjpeg_buf_offset == 0) {
        Serial.println("drawJpg: empty buffer!");
        return false;
    }
    
    // 获取JPEG尺寸
    uint16_t w = 0, h = 0;
    TJpgDec.getJpgSize(&w, &h, _mjpeg_buf, _mjpeg_buf_offset);
    
    if (w == 0 || h == 0) {
        Serial.printf("❌ Failed to get JPEG size for frame %d\n", _current_frame - 1);
        return false;
    }
    
    // 仅在关键帧输出信息，减少串口开销
    if ((_current_frame - 1) % 30 == 0) {
        Serial.printf("Frame %d: %dx%d, %d bytes\n", 
                     _current_frame - 1, w, h, _mjpeg_buf_offset);
    }
    
    // 开始写入LCD
    _lcd->startWrite();
    bool result = TJpgDec.drawJpg(0, 0, _mjpeg_buf, _mjpeg_buf_offset);
    _lcd->endWrite();
    
    if (!result) {
        Serial.printf("❌ Decode failed for frame %d\n", _current_frame - 1);
    }
    
    return result;
}

bool MjpegClass::tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (!_instance || !_instance->_lcd) return false;
    
    // 使用DMA如果可用，否则使用普通方法
    if (_instance->_lcd->dmaBusy()) {
        _instance->_lcd->pushImage(x, y, w, h, bitmap);
    } else {
        _instance->_lcd->pushImageDMA(x, y, w, h, bitmap);
    }
    
    return true;
}

bool FrameAnimation_Play(LGFX* lcd, const char* path) {
    if (!lcd) {
        Serial.println("Error: lcd is null");
        return false;
    }
    
    if (SD_MMC.cardType() == CARD_NONE) {
        Serial.println("Error: No SD card detected");
        return false;
    }
    
    // 打开MJPEG文件
    File mjpegFile = SD_MMC.open(path, FILE_READ);
    if (!mjpegFile || mjpegFile.isDirectory()) {
        Serial.printf("Error: Failed to open %s\n", path);
        return false;
    }
    
    Serial.printf("Opened file: %s, size: %lu bytes\n", path, mjpegFile.size());
    
    // 设置文件缓冲区以提高读取性能
    mjpegFile.setBufferSize(8192);
    
    // 创建MJPEG解码器实例
    MjpegClass mjpeg;
    if (!mjpeg.setup(mjpegFile, lcd)) {
        Serial.println("Failed to setup MJPEG decoder");
        mjpegFile.close();
        return false;
    }
    
    // 关键步骤：构建帧索引
    if (!mjpeg.buildIndex()) {
        Serial.println("Failed to build frame index");
        mjpegFile.close();
        return false;
    }
    
    Serial.println("Starting indexed MJPEG playback...");
    
    int frameCount = 0;
    unsigned long startTime = millis();
    unsigned long lastFpsTime = startTime;
    int fpsFrameCount = 0;
    
    // 主播放循环
    while (mjpeg.readMjpegBuf()) {
        if (mjpeg.drawJpg()) {
            frameCount++;
            fpsFrameCount++;
            
            // 计算并显示FPS
            unsigned long currentTime = millis();
            if (currentTime - lastFpsTime >= 1000) {
                float fps = fpsFrameCount * 1000.0 / (currentTime - lastFpsTime);
                Serial.printf("FPS: %.1f (Frame %d)\n", fps, frameCount);
                lastFpsTime = currentTime;
                fpsFrameCount = 0;
            }
        }
        
        // 精确的帧率控制 (25 FPS = 40ms per frame)
        unsigned long targetTime = startTime + frameCount * 40;
        unsigned long currentTime = millis();
        if (currentTime < targetTime) {
            delay(targetTime - currentTime);
        }
    }
    
    mjpegFile.close();
    
    unsigned long totalTime = millis() - startTime;
    float avgFps = frameCount * 1000.0 / totalTime;
    
    Serial.println("========================================");
    Serial.printf("✅ MJPEG playback finished!\n");
    Serial.printf("Total frames: %d\n", frameCount);
    Serial.printf("Total time: %lu ms\n", totalTime);
    Serial.printf("Average FPS: %.2f\n", avgFps);
    Serial.println("========================================");
    
    return frameCount > 0;
}
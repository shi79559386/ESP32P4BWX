#include "FrameAnimation.h"
#include <TJpg_Decoder.h>
#include <SD_MMC.h>

#define MJPEG_BUFFER_SIZE (32 * 1024)  // 增大缓冲区
#define READ_BUFFER_SIZE (2 * 1024)     // 读取块大小
#define TARGET_FPS 25
#define TARGET_FRAME_DELAY_MS (1000 / TARGET_FPS)

static LGFX* _gfx_ptr_for_callback = nullptr;

static bool tft_output_callback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    Serial.printf("callback called: x=%d y=%d w=%d h=%d\n", x, y, w, h);
    if (_gfx_ptr_for_callback) {
        _gfx_ptr_for_callback->startWrite();  // 非常重要！
        _gfx_ptr_for_callback->pushImage(x, y, w, h, bitmap);
        _gfx_ptr_for_callback->endWrite();    // 非常重要
    }
    return true;
}

bool FrameAnimation_Play(LGFX* lcd, const char* path) {
    if (!lcd) {
        Serial.println("FrameAnimation_Play Error: lcd is null.");
        return false;
    }

    Serial.printf("Attempting to play animation from: %s\n", path);
    
    // 检查SD卡状态
    if (SD_MMC.cardType() == CARD_NONE) {
        Serial.println("FrameAnimation_Play Error: No SD card detected.");
        return false;
    }
    
    // 尝试打开文件
    File mjpegFile = SD_MMC.open(path, FILE_READ);
    if (!mjpegFile || mjpegFile.size() == 0) {
        Serial.printf("FrameAnimation_Play Error: Cannot open file: %s\n", path);
        if (mjpegFile) mjpegFile.close();
        
        // 列出目录内容以帮助调试
        String dirPath = path;
        int lastSlash = dirPath.lastIndexOf('/');
        if (lastSlash != -1) {
            dirPath = dirPath.substring(0, lastSlash);
            if (dirPath.length() == 0) dirPath = "/";
            Serial.printf("Listing directory: %s\n", dirPath.c_str());
            File dir = SD_MMC.open(dirPath.c_str());
            if (dir && dir.isDirectory()) {
                File file = dir.openNextFile();
                while (file) {
                    Serial.printf("  - %s (%d bytes)\n", file.name(), file.size());
                    file = dir.openNextFile();
                }
                dir.close();
            }
        }
        
        return false;
    }

    Serial.printf("Opened file: %s, size: %lu bytes\n", path, mjpegFile.size());

    Serial.println("Allocating buffers...");

    // 分配缓冲区
    uint8_t* mjpeg_buf = (uint8_t*)heap_caps_malloc(MJPEG_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (!mjpeg_buf) {
        // 如果SPIRAM分配失败，尝试使用普通内存
        Serial.println("SPIRAM allocation failed, trying regular heap...");
        mjpeg_buf = (uint8_t*)malloc(MJPEG_BUFFER_SIZE);
    }

    uint8_t* read_buf = (uint8_t*)heap_caps_malloc(READ_BUFFER_SIZE, MALLOC_CAP_DMA);
    if (!read_buf) {
        Serial.println("DMA allocation failed, trying regular heap...");
        read_buf = (uint8_t*)malloc(READ_BUFFER_SIZE);
    }

    if (!mjpeg_buf || !read_buf) {
        Serial.println("FrameAnimation_Play Error: Failed to allocate buffers.");
        Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
        mjpegFile.close();
        if (mjpeg_buf) free(mjpeg_buf);
        if (read_buf) free(read_buf);
        return false;
    }

    Serial.println("Buffers allocated successfully");

    // 初始化JPEG解码器
    Serial.println("Initializing JPEG decoder...");
    _gfx_ptr_for_callback = lcd;
    TJpgDec.setJpgScale(2);
    TJpgDec.setCallback(tft_output_callback);
    Serial.println("JPEG decoder initialized");

    int buf_pos = 0;
    int bytes_read = 0;
    long last_frame_time = millis();
    int frame_count = 0;
    bool playing = true;

    Serial.println("Starting animation playback...");
    lcd->startWrite();

    // 先尝试读取一小部分数据测试
    Serial.println("Reading initial data...");
    bytes_read = mjpegFile.read(mjpeg_buf, 1024); // 先读1KB测试
    Serial.printf("Initial read: %d bytes\n", bytes_read);
    buf_pos = bytes_read;

    // 如果初始读取成功，继续主循环
    if (bytes_read > 0) {
        Serial.println("Starting main loop...");
        
        while (mjpegFile.available() && playing && frame_count < 10) { // 先只播放10帧测试
            // 读取数据到临时缓冲区
            int space_left = MJPEG_BUFFER_SIZE - buf_pos;
            if (space_left > READ_BUFFER_SIZE) {
                bytes_read = mjpegFile.read(read_buf, READ_BUFFER_SIZE);
                if (bytes_read > 0) {
                    memcpy(mjpeg_buf + buf_pos, read_buf, bytes_read);
                    buf_pos += bytes_read;
                }
                Serial.printf("Read %d bytes, buffer pos: %d\n", bytes_read, buf_pos);
            }

            // 查找JPEG帧的代码...
            Serial.println("Searching for JPEG frame...");

            // 查找并处理JPEG帧 - 修改的部分开始
            int search_end = buf_pos - 1;
            int frames_found_this_loop = 0;

            for (int i = 0; i < search_end && frames_found_this_loop < 1; i++) {  // 每次循环只处理一帧
                // 查找JPEG起始标记
                if (mjpeg_buf[i] == 0xFF && mjpeg_buf[i + 1] == 0xD8) {
                    Serial.printf("Found JPEG start at position %d\n", i);
                    
                    // 从起始位置查找结束标记
                    for (int j = i + 2; j < search_end; j++) {
                        if (mjpeg_buf[j] == 0xFF && mjpeg_buf[j + 1] == 0xD9) {
                            // 找到完整的JPEG帧
                            int frame_size = j + 2 - i;
                            Serial.printf("Found complete JPEG frame, size: %d bytes\n", frame_size);
                            
                            // 帧率控制
                            long current_time = millis();
                            long elapsed = current_time - last_frame_time;
                            if (elapsed < TARGET_FRAME_DELAY_MS) {
                                delay(TARGET_FRAME_DELAY_MS - elapsed);
                            }
                            last_frame_time = millis();

                            // 解码并显示
                            Serial.println("Decoding JPEG...");
                            TJpgDec.drawJpg(0, 0, mjpeg_buf + i, frame_size);
                            frame_count++;
                            Serial.printf("Frame %d decoded\n", frame_count);
                            frames_found_this_loop++;

                            // 移动剩余数据到缓冲区开始
                            int remaining = buf_pos - (j + 2);
                            if (remaining > 0) {
                                memmove(mjpeg_buf, mjpeg_buf + j + 2, remaining);
                            }
                            buf_pos = remaining;
                            
                            break;  // 退出内层循环
                        }
                    }
                    
                    if (frames_found_this_loop > 0) {
                        break;  // 退出外层循环
                    }
                }
            }

            if (frames_found_this_loop == 0 && buf_pos > MJPEG_BUFFER_SIZE / 2) {
                Serial.println("No frame found, clearing half buffer");
                // 如果没找到帧且缓冲区过半，清理一部分
                memmove(mjpeg_buf, mjpeg_buf + buf_pos/2, buf_pos/2);
                buf_pos = buf_pos/2;
            }
            // 修改的部分结束

            // 防止缓冲区溢出 - 这部分已经包含在上面的逻辑中
        }
    }

    lcd->endWrite();

    // 清理资源
    heap_caps_free(mjpeg_buf);
    heap_caps_free(read_buf);
    mjpegFile.close();
    _gfx_ptr_for_callback = nullptr;

    Serial.printf("Frame animation finished. Total frames: %d\n", frame_count);
    return frame_count > 0;
}
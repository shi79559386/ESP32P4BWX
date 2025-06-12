#ifndef _MJPEGCLASS_H_
#define _MJPEGCLASS_H_

#include <SD_MMC.h>
#include <esp_heap_caps.h>
#include "../Config/LGFX_Config.h"   // LovyanGFX 配置
#include "tjpgdClass.h"              // 例程版解码器

#define READ_BUFFER_SIZE   2048
#define MJPEG_BUFFER_SIZE (320 * 480 * 2)
static constexpr int MAX_FRAMES = 30;

class MjpegClass {
public:
  bool setup(File input, uint8_t *mjpeg_buf, LGFX *lcd, bool multiTask);
  bool buildIndex();
  bool readMjpegBuf();
  bool drawJpg();

private:
  // — 静态回调声明 —
  static uint32_t jpgRead(TJpgD *jdec, uint8_t *buf, uint32_t len);
  static uint32_t jpgWrite16(TJpgD *jdec, void *bitmap, TJpgD::JRECT *rect);
  static uint32_t jpgWriteRow(TJpgD *jdec, uint32_t y, uint32_t h);
  static bool     tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);

  // — 私有成员 —
  File      _input;
  LGFX*     _lcd       = nullptr;
  bool      _multiTask = false;

  uint8_t  *_read_buf           = nullptr;              // 文件流缓冲区
  uint8_t  *_mjpeg_buf          = nullptr;              // 单帧 JPEG 缓冲
  int32_t   _mjpeg_buf_offset   = 0;

  uint8_t  *_out_bufs[2]        = { nullptr, nullptr }; // 双缓冲，每次 48 行
  uint8_t  *_out_buf            = nullptr;
  TJpgD     _jdec;                                    // 例程版解码器实例

  // 解码状态
  int32_t   _inputindex = 0, _buf_read = 0, _remain = 0;
  uint32_t  _fileindex  = 0;

  // 屏幕和显示参数
  int32_t   _tft_width = 0, _tft_height = 0;
  int32_t   _jpg_x = 0, _jpg_y = 0, _off_x = 0, _off_y = 0;
  int32_t   _out_width = 0, _out_height = 0;

  // 帧索引
  uint32_t  _frame_offsets[MAX_FRAMES + 1] = {0};
  int       _frame_count   = 0;
  int       _current_frame = 0;

  // 例程里需要这两个静态成员
  static MjpegClass* _instance;
  static uint8_t     _read_buf_static[READ_BUFFER_SIZE];

  friend bool FrameAnimation_Play(LGFX* lcd, const char* path);
};

#endif // _MJPEGCLASS_H_

#include "MjpegClass.h"
#include <SD_MMC.h>
#include <esp_heap_caps.h>

// —— 静态成员定义 —— 
MjpegClass* MjpegClass::_instance = nullptr;
uint8_t     MjpegClass::_read_buf_static[READ_BUFFER_SIZE];

// 1) setup()
bool MjpegClass::setup(File input, uint8_t *mjpeg_buf, LGFX *lcd, bool multiTask) {
    _instance = this;  
  _input      = input;
  _mjpeg_buf  = mjpeg_buf;
  _lcd        = lcd;
  _multiTask  = multiTask;
  _tft_width  = lcd->width();
  _tft_height = lcd->height();
  // 分配 _read_buf、_out_bufs 在例程里（略）
  //if (!_read_buf)    _read_buf = (uint8_t*)malloc(READ_BUFFER_SIZE);
  for (int i = 0; i < 2; i++) {
    if (!_out_bufs[i]) {
      size_t sz = _tft_width * 48 * 2;
      _out_bufs[i] = (uint8_t*)heap_caps_malloc(sz, MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    }
  }
  _out_buf = _out_bufs[0];
  if (_multiTask) _jdec.multitask_begin();
  return true;
}

// 2) buildIndex()
bool MjpegClass::buildIndex() {
  _frame_count = _current_frame = 0;
  _input.seek(0);
  uint8_t prev = 0, cur;
  size_t pos = 0, sz = _input.size();
  while (pos < sz) {
    int n = _input.read(_read_buf, READ_BUFFER_SIZE);
    if (n <= 0) break;
    for (int i = 0; i < n && _frame_count < MAX_FRAMES; i++) {
      cur = _read_buf[i];
      if (prev == 0xFF && cur == 0xD8) _frame_offsets[_frame_count++] = pos + i - 1;
      prev = cur;
    }
    pos += n;
  }
  _frame_offsets[_frame_count] = sz;
  _input.seek(0);
  return _frame_count > 0;
}

// 3) readMjpegBuf()
bool MjpegClass::readMjpegBuf() {
  // 用 buildIndex 的分支
  if (_current_frame < _frame_count) {
    uint32_t s = _frame_offsets[_current_frame],
             e = _frame_offsets[_current_frame+1],
             len = e - s;
    _input.seek(s);
    int r = _input.read(_mjpeg_buf, len);
    _current_frame++;
    _mjpeg_buf_offset = r;
    return r == (int)len;
  }
  // 单流方式（和例程一模一样）
  if (_inputindex == 0) {
    _buf_read = _input.read(_read_buf, READ_BUFFER_SIZE);
    _inputindex += _buf_read;
  }
  _mjpeg_buf_offset = 0;
  bool found = false;
  while (_buf_read > 0 && !found) {
  int i = 1;
    if (_mjpeg_buf_offset>0 && _mjpeg_buf[_mjpeg_buf_offset-1]==0xFF && _read_buf[0]==0xD9) {
        found = true;
      i = 1;
    } else {
      // 查 0xFFD9
      while (i < _buf_read-1 && !found) {
        if (_read_buf[i]==0xFF && _read_buf[i+1]==0xD9) { found = true; ++i; }
        i++;
      }
    }
    memcpy(_mjpeg_buf + _mjpeg_buf_offset, _read_buf, i);
    _mjpeg_buf_offset += i;
    size_t rem = _buf_read - i;
    if (rem>0) {
      memmove(_read_buf, _read_buf + i, rem);
      int nr = _input.read(_read_buf + rem, READ_BUFFER_SIZE - rem);
      _inputindex += nr;
      _buf_read = rem + nr;
    } else {
      _buf_read = _input.read(_read_buf, READ_BUFFER_SIZE);
      _inputindex += _buf_read;
    }
  }
  return found;
}

// 4) drawJpg()
bool MjpegClass::drawJpg() {
    Serial.println(">> drawJpg(): start");
  _fileindex = 0;
  _remain    = _mjpeg_buf_offset;
  Serial.printf("   remain=%d bytes\n", _remain);
  auto res = _jdec.prepare(jpgRead, this);
  Serial.printf("   prepare returned %d\n", (int)res);
  if (res != TJpgD::JDR_OK) return false;
  // 居中、限制尺寸
  _out_width  = std::min<int>(_jdec.width,  _tft_width);
  _out_height = std::min<int>(_jdec.height, _tft_height);
  _jpg_x = ((_tft_width  - _jdec.width )/2) > 0 ? ((_tft_width  - _jdec.width )/2) : 0;
  _jpg_y = ((_tft_height - _jdec.height)/2) > 0 ? ((_tft_height - _jdec.height)/2) : 0;
  _off_x = _jpg_x>0 ? 0 : -_jpg_x;
  _off_y = _jpg_y>0 ? 0 : -_jpg_y;
  if (_multiTask) {
    res = _jdec.decomp_multitask(jpgWrite16, jpgWriteRow);
    _jdec.multitask_end();
  } else {
    res = _jdec.decomp(jpgWrite16, jpgWriteRow);
  }
  return res == TJpgD::JDR_OK;
}

// —— 回调函数实现 —— 
uint32_t MjpegClass::jpgRead(TJpgD *jdec, uint8_t *buf, uint32_t len) {
    MjpegClass *me = (MjpegClass*)jdec->device;
    if (len > me->_remain) len = me->_remain;
    if (buf) {
      memcpy(buf, me->_mjpeg_buf + me->_fileindex, len);
    }
    me->_fileindex += len;
    me->_remain    -= len;
    return len;
  }
  
  uint32_t MjpegClass::jpgWrite16(TJpgD *jdec, void *bitmap, TJpgD::JRECT *rect) {
    MjpegClass *me = (MjpegClass*)jdec->device;
    int16_t x = me->_jpg_x + rect->left;
    int16_t y = me->_jpg_y + rect->top;
    int16_t w = rect->right - rect->left + 1;
    int16_t h = rect->bottom - rect->top + 1;
    Serial.printf(">> jpgWrite16: x=%d,y=%d,w=%d,h=%d\n", x, y, w, h);
    uint16_t *src = (uint16_t*)bitmap;
    me->_lcd->pushImage(x, y, w, h, src);
    return 1;
  }
  
  uint32_t MjpegClass::jpgWriteRow(TJpgD *jdec, uint32_t y, uint32_t h) {
    //// 如果使用多任务模式，则由 tft_output 处理；否则可以忽略
 return 1;
  }
  
  bool MjpegClass::tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
    // 从单例实例取回 this 指针
    MjpegClass *me = MjpegClass::_instance;
    me->_lcd->pushImage(x, y, w, h, bitmap);
    return true;
  }

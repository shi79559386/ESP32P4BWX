#pragma once
#include <LovyanGFX.hpp>
#include "Config.h"   // 这里定义 TFT_SCLK/MOSI/MISO/DC/CS/RST/BL

// LGFX 继承自 LGFX_Device，融合了 Bus_SPI + Panel_ST7796 + Light_PWM
class LGFX : public lgfx::LGFX_Device {
  lgfx::Bus_SPI      _bus;
  lgfx::Panel_ST7796 _panel;
  lgfx::Light_PWM    _light;

public:
  LGFX() { config(); }

private:
  void config() {
    // —— 1) SPI 总线配置 ——  
    {
      auto bus_cfg = _bus.config();
      bus_cfg.spi_host   = SPI3_HOST;      // VSPI = SPI3_HOST
      bus_cfg.pin_sclk   = TFT_SCLK;
      bus_cfg.pin_mosi   = TFT_MOSI;
      bus_cfg.pin_miso   = TFT_MISO;       // 如果完全不用读可设 -1
      bus_cfg.pin_dc     = TFT_DC;
      bus_cfg.freq_write = 80 * 1000 * 1000; // 40MHz
      bus_cfg.freq_read  = 10 * 1000 * 1000; // 10MHz（若用到读回）
      _bus.config(bus_cfg);
      _panel.setBus(&_bus);
    }

    // —— 2) 面板参数 ——  
    {
      auto panel_cfg = _panel.config();
      panel_cfg.pin_cs           = TFT_CS;
      panel_cfg.pin_rst          = TFT_RST;
      panel_cfg.pin_busy         = -1;        // 如果不用 BUSY
      panel_cfg.panel_width      = 320;
      panel_cfg.panel_height     = 480;
      panel_cfg.memory_width     = 320;
      panel_cfg.memory_height    = 480;

      panel_cfg.offset_x         = 0;
      panel_cfg.offset_y         = 0;
      panel_cfg.offset_rotation  = 0;   // 关闭旋转
      panel_cfg.invert           = false;
      panel_cfg.rgb_order        = false;
      panel_cfg.dummy_read_pixel = 8;
      panel_cfg.dummy_read_bits  = 1;
      panel_cfg.readable         = false;     // 不用读回可省时间
      _panel.config(panel_cfg);
    }

    _panel.setRotation(1);

    // —— 3) 背光控制 ——  
    {
      auto light_cfg = _light.config();
      light_cfg.pin_bl     = TFT_BL;  // 若无背光，可设 -1
      light_cfg.invert     = false;
      light_cfg.freq       = 5000;    // PWM 频率
      light_cfg.pwm_channel = BACKLIGHT_PWM_CHANNEL;      // 任意不冲突通道
      _light.config(light_cfg);
      _panel.setLight(&_light);
    }

    // —— 4) 注入到 LovyanGFX ——  
    setPanel(&_panel);

    // —— 5) （可选）在 PSRAM 上申请一帧缓冲 ——  
    //{
      //size_t bufsize = size_t(_panel.width())
       //              * size_t(_panel.height())
      //               * sizeof(lgfx::rgb565_t);
      //void* fb = heap_caps_malloc(bufsize, MALLOC_CAP_SPIRAM);
      //if (fb) {
        // v1.2.7 签名： bool setFrameBuffer(void* buf, size_t size_bytes);
     //   this->setFrameBuffer(fb, bufsize);
     // }
   // }
  }
};

// **唯一的**外部实例声明
extern LGFX lcd;

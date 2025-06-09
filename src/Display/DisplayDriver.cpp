// 文件: src/Display/DisplayDriver.cpp
#include "DisplayDriver.h"
#include <Arduino.h>
#include <Wire.h>
#include <Ticker.h>
#include "touch/TouchDriver.h"
#include "../Config/Config.h"
#include "../Config/LGFX_Config.h"   // 这里已经包含了我们自己写的 LGFX

//—— 1) 用新版 LGFX 驱动
static LGFX lcd;

//—— 2) LVGL 绘制缓冲区和驱动句柄
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[ screenWidth * LVGL_INTERNAL_RAM_BUFFER_LINES ];
static lv_disp_drv_t    disp_drv;
static lv_indev_drv_t   indev_drv;
static Ticker           lvgl_ticker;

//—— 3) LVGL 屏幕刷新回调：用 PSRAM 缓冲 + DMA
static void lvgl_disp_flush_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
    int32_t x = area->x1;
    int32_t y = area->y1;
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    lcd.startWrite();
    lcd.setAddrWindow(x, y, w, h);
    // pushColors 第三个参数 true 表示用 DMA 一次推送
    lcd.pushColors((uint16_t*)color_p, (uint32_t)w * h, true);
    lcd.endWrite();

    lv_disp_flush_ready(disp);
}

//—— 4) LVGL Tick 回调（保持不变）
static void lvgl_tick_interrupt_cb(void) {
    lv_tick_inc(5);
}

//—— 5) 总初始化入口
void display_init(void) {
    Serial.println("DisplayDriver: Initializing with LovyanGFX...");

    //—— 5.1) lcd 已在构造里 init() 完成，可选再手动：
    // lcd.init(); 

    // 设置方向、清屏
    lcd.setRotation(0);           // 0=竖屏, 1=横屏 ...
    lcd.fillScreen(TFT_BLACK);

    Serial.printf("DisplayDriver: TFT initialized, resolution %dx%d\n",
                  lcd.width(), lcd.height());

    //—— 5.2) 触摸硬件复位
    pinMode(TOUCH_RST_PIN, OUTPUT);
    digitalWrite(TOUCH_RST_PIN, LOW);
    ::delay(20);
    digitalWrite(TOUCH_RST_PIN, HIGH);
    ::delay(50);

    //—— 5.3) I2C for FT6336
    Wire.begin(TOUCH_SDA_PIN, TOUCH_SCL_PIN, 100000);

    //—— 5.4) LVGL 显示驱动注册
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL,
                          screenWidth * LVGL_INTERNAL_RAM_BUFFER_LINES);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = lvgl_disp_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    //—— 5.5) LVGL 触摸驱动注册
    if (TOUCH_INT_PIN >= 0) {
        pinMode(TOUCH_INT_PIN, INPUT_PULLUP);
        touch_init(screenWidth, screenHeight, 1);
    } else {
        touch_init(screenWidth, screenHeight, 0);
    }
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_touch_read_cb;
    lv_indev_drv_register(&indev_drv);

    //—— 5.6) 启动 LVGL tick
    lvgl_ticker.attach_ms(5, lvgl_tick_interrupt_cb);

    Serial.println("DisplayDriver: All done.");
}

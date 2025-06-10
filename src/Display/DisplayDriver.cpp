// 文件: src/Display/DisplayDriver.cpp (最终版 - 仅作为调用者)
#include "DisplayDriver.h"
#include <Arduino.h>
//#include <Ticker.h>
#include "touch/TouchDriver.h" // 关键：包含了厂家的触摸驱动头文件
#include "../Config/Config.h"
#include "../Config/LGFX_Config.h"   

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[ screenWidth  * LVGL_INTERNAL_RAM_BUFFER_LINES ];
static lv_disp_drv_t    disp_drv;
static lv_indev_drv_t   indev_drv;
//static Ticker           lvgl_ticker;

// LVGL 刷屏回调
static void lvgl_disp_flush_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p) {
    uint32_t x1 = area->x1;
    uint32_t y1 = area->y1;
    uint32_t w  = lv_area_get_width(area);      
    uint32_t h  = lv_area_get_height(area);
    //uint32_t w  = lv_area_get_height(area);
    //uint32_t h  = lv_area_get_width(area);
    uint32_t total = w * h;

    // --- 同步写入全部像素（最保险的方式） ---
    lcd.startWrite();                   // 开始写入
    lcd.setAddrWindow(x1, y1, w, h);    // 定位窗口

    // 逐像素写入
    for (uint32_t i = 0; i < total; i++) {
        // lv_color_t.full 就是 16bit RGB565 原始值
        lcd.writeColor(color_p[i].full, 1);
    }

    lcd.endWrite();                     // 结束写入
    lv_disp_flush_ready(disp);          // 通知 LVGL 本次刷新完成
}

// LVGL Tick 中断
//static void lvgl_tick_interrupt_cb(void) {
//    lv_tick_inc(5);
//}

// --- 统一的显示初始化函数 ---
void display_init(void) {
    Serial.println("DisplayDriver: Unified Initialization Started...");

    lv_init();
    lcd.init();
    //lcd.setRotation(3);
    lcd.fillScreen(TFT_BLACK);
    lcd.setBrightness(255);

    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, screenWidth * LVGL_INTERNAL_RAM_BUFFER_LINES);
    lv_disp_drv_init(&disp_drv);
    //disp_drv.hor_res = screenHeight;  // 改成: 320
    //disp_drv.ver_res = screenWidth;   // 改成: 480
    disp_drv.hor_res = screenWidth;  // 改成: 320
    disp_drv.ver_res = screenHeight;   // 改成: 480
    disp_drv.flush_cb = lvgl_disp_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // --- 触摸驱动的初始化与注册 ---
    // 1. 调用厂家提供的初始化函数
    //touch_init(screenHeight, screenWidth, 1);
    touch_init(screenWidth, screenHeight, 1);

    // 2. 初始化LVGL的输入设备驱动
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_POINTER;
    
    // 3. 将回调函数指向厂家驱动中提供的那个
    //    这里不再有任何实现，只是一个函数指针的赋值
    indev_drv.read_cb = lvgl_touch_read_cb; 

    lv_indev_drv_register(&indev_drv);
    Serial.println("DisplayDriver: All initialization done.");
}
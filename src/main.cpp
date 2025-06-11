#include <Arduino.h>
#include "lvgl.h"

#include "Config/LGFX_Config.h"
#include "App/AppController.h"
#include "App/AppGlobal.h"      
#include "App/AppTasks.h"       

extern LGFX lcd; 

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 20];

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    lcd.pushImage(area->x1, area->y1, w, h, (lgfx::rgb565_t*)color_p);
    lv_disp_flush_ready(disp);
}

void my_touch_read(lv_indev_drv_t *indev, lv_indev_data_t *data) {
    uint32_t x, y;
    bool touched = lcd.getTouch(&x, &y);
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = x;
        data->point.y = y;
    }
}

void setup() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println("\n\n=== System Starting ===");
    Serial.printf("ESP32-P4 with %d KB free heap\n", ESP.getFreeHeap() / 1024);

    // LCD初始化
    Serial.println("Initializing LCD...");
    lcd.init();
    lcd.setRotation(1);  // 确保是横屏
    lcd.setBrightness(255);  // 最大亮度
    lcd.fillScreen(TFT_BLACK);
    Serial.println("LCD initialized");
    
    // LVGL初始化
    Serial.println("Initializing LVGL...");
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, nullptr, screenWidth * 20);
    
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touch_read;
    lv_indev_drv_register(&indev_drv);
    Serial.println("LVGL initialized");

    // 应用初始化（包括SD卡和开机动画）
    AppController_Init(&lcd);

    Serial.println("🎉 系统已准备就绪，可以运行任务！");
}

void loop() {
    AppGlobal_UpdateSensorData();
    lv_timer_handler();  // LVGL不需要手动调用lv_tick_inc
    AppTasks_Handler();
    delay(5);
}
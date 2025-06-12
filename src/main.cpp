#include <Arduino.h>
#include "lvgl.h"
#include "Display/DisplayDriver.h"  // ✅ 使用你封装好的显示驱动
#include "Config/LGFX_Config.h"
#include "App/AppController.h"
#include "App/AppGlobal.h"      
#include "App/AppTasks.h"       


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

    // ✅ 只调用一次封装的显示初始化
    display_init();

    // ✅ 初始化主应用（含 SD 卡 / 动画 / UI 等）
    AppController_Init(&lcd);

    Serial.println("🎉 系统已准备就绪，可以运行任务！");
}
void loop() {
    AppGlobal_UpdateSensorData();
    lv_timer_handler();  // LVGL不需要手动调用lv_tick_inc
    AppTasks_Handler();
    delay(5);
}
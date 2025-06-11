// 文件: src/main.cpp
// 版本: 修正启动流程

#include <Arduino.h>
#include "lvgl.h"
#include "App/AppController.h"
#include "App/AppGlobal.h"
#include "App/AppTasks.h"

// 您在 AppController.cpp 中已经定义了 tft 对象，这里不再需要
// #include <Adafruit_ST7789.h>
// #include "LGFX_Config.h"


void setup() {
    Serial.begin(115200);
    delay(100); // 等待串口稳定

    // AppController_Init() 现在将负责所有硬件的初始化, 包括显示屏
    AppController_Init();

    Serial.println("🎉 System is Ready to Run Tasks!");
}

void loop() {
    // 更新传感器数据
    AppGlobal_UpdateSensorData();

    // LVGL 心跳
    lv_tick_inc(5);
    lv_timer_handler();

    // 应用任务
    AppTasks_Handler();

    delay(5);
}

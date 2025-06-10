#include <Arduino.h>
#include "lvgl.h"                // lv_tick_inc/lv_timer_handler
#include "App/AppController.h"
#include "App/AppGlobal.h"
#include "App/AppTasks.h"

void setup() {
  Serial.begin(115200);
  delay(100);

  // 一切初始化（包括板载 SDMMC 挂载 + 动画后台任务 + 其它模块）
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

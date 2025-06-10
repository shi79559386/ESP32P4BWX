// 文件: src/main.cpp (最终修正版)

#include <Arduino.h>
#include "lvgl.h" // <--- 为了调用 lv_timer_handler，需要包含它
#include "App/AppController.h"
#include "App/AppGlobal.h" // <--- 添加这一行
#include "App/AppTasks.h"

void setup() {
  Serial.begin(115200);
  delay(100); 

  AppController_Init();

  Serial.println("🎉 System is Ready to Run Tasks!");
}

void loop() {
  AppGlobal_UpdateSensorData();
  // 在主循环中，我们需要同时处理 LVGL 的任务和我们自己的应用任务
  lv_tick_inc(5);     // 通知 LVGL 已过 5ms
  lv_timer_handler(); // LVGL 的心跳，负责UI刷新和动画
  AppTasks_Handler(); // 我们自己的应用任务处理器
  delay(5); // 短暂延时，让系统有机会处理其他事情
}
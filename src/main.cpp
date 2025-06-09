#include <Arduino.h>
#include "esp_task_wdt.h"
#include "esp_timer.h"

#include <LovyanGFX.hpp>    // LovyanGFX 主库
#include "LGFX_Config.h"    // 我们刚写的配置
#include "lvgl.h"

#include "Config/Config.h"
#include "App/AppController.h"
#include "App/AppTasks.h"
#include "App/AppGlobal.h"
#include "Peripherals/AHT20_Sensor.h"
#include "Control/ThermalControl.h"

#include "Display/UI_MainScreen.h"
#include "Display/UI_ControlPage.h"
#include "Display/UI_SystemSettingPage.h"

// —— 1. 全局对象 ——  
static LGFX lcd;                // 这个名字“lcd”可以随便，但类型必须是 LGFX
static esp_timer_handle_t lvgl_timer_handle;

// —— 2. LVGL 刷屏回调 ——  
//    LVGL 请求刷新区域时会调用它
static void lvgl_flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p) {
  // 用 DMA 的调用，效率最高
  lcd.pushImageDMA( area->x1, area->y1
                  , area->x2 - area->x1 + 1
                  , area->y2 - area->y1 + 1
                  , &color_p->full );
  lv_disp_flush_ready(drv);
}

// —— 3. LVGL 定时器，每 2ms 调一次 ——  
static void IRAM_ATTR lvgl_tick_cb(void*) {
  lv_timer_handler();
}

void setup() {
  Serial.begin(115200);
  delay(500);
  esp_task_wdt_delete(NULL);

  // —— 4. 初始化屏幕 ——  
  lcd.init();  
  Serial.println("✅ LovyanGFX & TFT init done");

  // —— 5. 初始化 LVGL 显示驱动 ——  
  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t buf1[screenWidth * 40];  // 40 行的缓冲，大小可调整
  lv_disp_draw_buf_init(&draw_buf, buf1, nullptr, screenWidth * 40);

  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.flush_cb = lvgl_flush_cb;
  lv_disp_drv_register(&disp_drv);

  // —— 6. 启动 LVGL tick ——  
  const esp_timer_create_args_t targs = {
    .callback = lvgl_tick_cb,
    .dispatch_method = ESP_TIMER_ISR,
    .skip_unhandled_events = true,
    .name = "lvgl_tick"
  };
  esp_timer_create(&targs, &lvgl_timer_handle);
  esp_timer_start_periodic(lvgl_timer_handle, 2 * 1000); // 2ms

  // —— 7. 其它子系统 & UI ——  
  AppController_Init();
  ThermalControl_Init();

  static lv_obj_t* scr_main    = lv_obj_create(nullptr);
  static lv_obj_t* scr_ctrl    = lv_obj_create(nullptr);
  static lv_obj_t* scr_setting = lv_obj_create(nullptr);
  create_main_ui(scr_main);
  create_control_page_ui(scr_ctrl);
  create_setting_page_ui(scr_setting);
  lv_disp_load_scr(scr_main);

  Serial.println("🎉 System READY!");
}


void loop() {
    unsigned long t0 = millis();
    Serial.printf("[T] → loop start @ %lums\n", t0);

    // 1) LVGL 刷新
    unsigned long t1 = millis();
    Serial.printf("[T] before lv_task_handler @ %lums\n", t1);
    lv_task_handler();
    unsigned long t2 = millis();
    Serial.printf("[T] after  lv_task_handler  @ %lums (Δ%lums)\n",
                  t2, t2 - t1);
    delay(1);

    // 2) 应用核心逻辑
    t1 = millis();
    Serial.printf("[T] before AppTasks_Handler @ %lums\n", t1);
    AppTasks_Handler();
    t2 = millis();
    Serial.printf("[T] after  AppTasks_Handler  @ %lums (Δ%lums)\n",
                  t2, t2 - t1);
    delay(1);

    // 3) 你其它的模块（如果有）也同理埋点…
    //    test_sensors_periodic(), processAudioBoardCommunication() 等

    unsigned long tend = millis();
    Serial.printf("[T] ← loop end   @ %lums  (total Δ%lums)\n\n",
                  tend, tend - t0);

    // 最后保留一点延时，避免刷串口太快
    delay(2000);
}

// File: src/App/AppGlobal.h
#ifndef APP_GLOBAL_H
#define APP_GLOBAL_H

#include "lvgl.h" // For lv_obj_t
#include <stdint.h> // For uint8_t
#include <stdbool.h> // For bool

typedef struct {
    float temperature;
    float humidity;
    bool  isValid; // 指示当前读数是否有效
} SensorData_t;

// ============== 全局变量声明 ==============

// 传感器数据结构体
extern SensorData_t sensor1Data;
extern SensorData_t sensor2Data;

// 页面ID枚举
typedef enum {
    SCREEN_ID_MAIN,
    SCREEN_ID_CONTROL,
    SCREEN_ID_SETTING
} screen_id_t;

// 控制页面卡片ID枚举
typedef enum {
    CARD_ID_LIGHTING = 0,
    CARD_ID_VOICE,
    CARD_ID_DRYING,
    CARD_ID_FRESH_AIR,
    CARD_ID_STERILIZE,
    CARD_ID_INCUBATION,
    CARD_ID_THERMAL,        // 新增：保温卡片
    CARD_ID_HUMIDIFY,    // 新增：加湿卡片
    CARD_ID_COUNT = 8// 用于获取卡片总数
} control_card_id_t;

// 通用的定时操作结构体
typedef struct {
    bool enabled;           // 定时功能是否启用
    uint8_t countdown_hour;   // 倒计时小时数 (0-23)
    uint8_t countdown_minute; // 倒计时分钟数 (0-59)
    uint8_t countdown_second; // 倒计时秒数 (0-59)
} timed_operation_config_t;

// 温湿度全局变量声明
extern float g_current_temperature;
extern int   g_current_humidity;
extern float g_current_temperature_2;
extern int   g_current_humidity_2;

// 时间日期字符串
extern char  g_current_time_str[20];
extern char  g_current_date_str[20];
extern char  g_work_status_str[50];

// 通用保温功能的目标值
extern float g_target_temperature_left;
extern int   g_target_humidity_left;
extern float g_target_temperature_right;
extern int   g_target_humidity_right;
extern bool  g_generic_thermostat_active; // 通用保温功能总开关
extern bool  g_incubation_control_active; // 孵化模式总开关

// LVGL对象指针
extern lv_obj_t *screen_main;
extern lv_obj_t *screen_control;
extern lv_obj_t *screen_setting;

// 卡片标题和图标数组
extern const char* card_titles[CARD_ID_COUNT];
extern const char* card_icons[CARD_ID_COUNT];
extern lv_disp_t* lvgl_disp;  // 声明变量

// ============== 函数声明 ==============
void AppGlobal_Init(); // 全局变量初始化函数
void AppGlobal_UpdateSensorData(); // 传感器数据更新函数

#endif // APP_GLOBAL_H
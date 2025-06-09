// File: src/SettingsLogic/HumidifySettings.h
#ifndef HUMIDIFY_SETTINGS_H
#define HUMIDIFY_SETTINGS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../App/AppGlobal.h"

// 加湿模式枚举
typedef enum {
    HUMIDIFY_MODE_MANUAL = 0,   // 手动模式
    HUMIDIFY_MODE_AUTO,         // 自动模式
    HUMIDIFY_MODE_TIMED         // 定时模式
} humidify_mode_t;

// 加湿配置结构体
typedef struct {
    bool enabled;               // 加湿功能是否启用
    humidify_mode_t mode;       // 加湿模式
    int target_humidity_left;   // 左区域目标湿度
    int target_humidity_right;  // 右区域目标湿度
    int humidity_tolerance;     // 湿度容差 (±%)
} humidify_config_t;

// 加湿状态更新回调函数类型
typedef void (*humidify_status_update_cb_t)(void);

// === 核心API函数 ===
void HumidifySettings_Init();
void HumidifySettings_SetEnabled(bool enabled);
bool HumidifySettings_IsEnabled();
void HumidifySettings_SetMode(humidify_mode_t mode);
humidify_mode_t HumidifySettings_GetMode();
void HumidifySettings_SetTargetHumidity(int left_humidity, int right_humidity);
void HumidifySettings_GetTargetHumidity(int* left_humidity, int* right_humidity);
void HumidifySettings_SetTolerance(int tolerance);
int HumidifySettings_GetTolerance();
const humidify_config_t* HumidifySettings_GetConfig();
void HumidifySettings_RegisterStatusUpdateCallback(humidify_status_update_cb_t cb);
const char* HumidifySettings_GetStatusString();

#endif // HUMIDIFY_SETTINGS_H
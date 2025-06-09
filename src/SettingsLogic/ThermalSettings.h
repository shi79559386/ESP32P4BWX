// File: src/SettingsLogic/ThermalSettings.h
#ifndef THERMAL_SETTINGS_H
#define THERMAL_SETTINGS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../App/AppGlobal.h"

// 简化的保温配置结构体
typedef struct {
    bool heater_enabled;            // 加热器是否启用
    bool fan_enabled;               // 风扇是否启用
    bool timed_shutdown_enabled;    // 定时关闭是否启用
    uint8_t countdown_hour;         // 倒计时小时数
    uint8_t countdown_minute;       // 倒计时分钟数
    uint8_t countdown_second;       // 倒计时秒数
} thermal_settings_config_t;

// 保温状态更新回调函数类型
typedef void (*thermal_status_update_cb_t)(void);

// === 核心API函数 ===
void ThermalSettings_Init();

// 加热器控制
void ThermalSettings_SetHeaterEnabled(bool enabled);
bool ThermalSettings_IsHeaterEnabled();
//bool ThermalSettings_CanEnableHeater();  // 新增：检查是否可以启用加热器

// 风扇控制
void ThermalSettings_SetFanEnabled(bool enabled);
bool ThermalSettings_IsFanEnabled();

// 通用状态
bool ThermalSettings_IsAnyDeviceEnabled();

// 定时关闭功能
void ThermalSettings_SetTimedShutdown(const timed_operation_config_t* config);
void ThermalSettings_UpdateTimedState();
bool ThermalSettings_IsTimedSessionActive();
void ThermalSettings_GetFormattedRemainingCountdown(char* buffer, size_t buffer_len);

// 其他功能
const thermal_settings_config_t* ThermalSettings_GetConfig();
void ThermalSettings_RegisterStatusUpdateCallback(thermal_status_update_cb_t cb);
const char* ThermalSettings_GetStatusString();

#endif // THERMAL_SETTINGS_H
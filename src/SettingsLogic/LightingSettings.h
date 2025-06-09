// File: src/SettingsLogic/LightingSettings.h
#ifndef LIGHTING_SETTINGS_H
#define LIGHTING_SETTINGS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h> 

// 枚举定义亮度档位
typedef enum {
    LIGHT_LEVEL_OFF = -1, // 特殊值，表示灯光完全关闭的状态
    LIGHT_LEVEL_LOW = 0,
    LIGHT_LEVEL_MEDIUM = 1,
    LIGHT_LEVEL_HIGH = 2
} light_level_t;

// 定义不同档位对应的PWM百分比 (0-100)
#define PWM_PERCENT_LOW 30
#define PWM_PERCENT_MEDIUM 60
#define PWM_PERCENT_HIGH 100

// 照明设置模块初始化
void LightingSettings_Init();

// 设置和获取目标亮度档位
void LightingSettings_SetLightLevel(light_level_t level);
light_level_t LightingSettings_GetLightLevel();
uint8_t LightingSettings_GetPwmForLevel(light_level_t level); // 获取指定档位对应的PWM值

// 设置和获取唤醒模式启用状态
void LightingSettings_SetWakeupEnabled(bool enabled);
bool LightingSettings_IsWakeupEnabled();

// 设置和获取渐亮时长（单位：总秒数）
void LightingSettings_SetBrighteningDurationSeconds(uint16_t seconds);
uint16_t LightingSettings_GetBrighteningDurationSeconds();

// 处理照明总开关状态变化的核心函数
void LightingSettings_HandleLightToggle(bool is_on);

// 定期更新渐亮状态的函数 (由AppTasks调用)
void LightingSettings_UpdateBrighteningState();

// 获取当前实际应用的PWM值 (0-100)
uint8_t LightingSettings_GetCurrentAppliedPwm();

// 获取照明状态字符串，用于在UI卡片上显示
// buffer: 用于存储状态字符串的缓冲区
// buffer_len: 缓冲区的长度
void LightingSettings_GetStatusString(char* buffer, size_t buffer_len);

#endif // LIGHTING_SETTINGS_H
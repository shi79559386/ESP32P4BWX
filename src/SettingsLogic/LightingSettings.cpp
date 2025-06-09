// File: src/SettingsLogic/LightingSettings.cpp
#include "LightingSettings.h"
#include "../Peripherals/OutputControls.h" // 用于实际控制灯光PWM
#include <Arduino.h> // For millis() and Serial
#include <stdio.h>   // For snprintf

// 静态变量存储当前设置和状态
static light_level_t current_light_level;
static bool wakeup_mode_enabled;
static uint16_t brightening_duration_seconds; // 总秒数

static bool is_brightening_process_active;
static unsigned long brightening_start_time_millis;
static uint8_t current_applied_pwm; // 当前实际应用的PWM值 (0-100)
static bool manual_override_active; // 用于标记手动操作是否覆盖了唤醒

// 内部辅助函数，实际应用PWM到硬件
static void apply_pwm_to_hardware(uint8_t pwm_percentage) {
    set_light_pwm(pwm_percentage); // 调用OutputControls中的函数
    current_applied_pwm = pwm_percentage;
    // Serial.printf("Lighting: Applied PWM %d%%\n", pwm_percentage);
}

void LightingSettings_Init() {
    // 设置默认值
    current_light_level = LIGHT_LEVEL_HIGH; // 默认高档亮度
    wakeup_mode_enabled = false;            // 默认关闭唤醒模式
    brightening_duration_seconds = 0;       // 默认渐亮时长0秒 (立即达到目标亮度)

    is_brightening_process_active = false;
    brightening_start_time_millis = 0;
    manual_override_active = false;

    // 根据初始状态应用灯光 (假设灯默认是关的，除非OutputControls有其他逻辑)
    // 如果希望开机时灯亮，可以在这里调用 LightingSettings_HandleLightToggle(true);
    // 但通常这个由卡片上的开关状态决定，而开关状态可能需要从持久化存储中读取（如果实现的话）
    // 为简单起见，这里先设置为关闭状态对应的PWM
    apply_pwm_to_hardware(0); 
    current_applied_pwm = 0;


    Serial.println("LightingSettings: Initialized with default values.");
}

void LightingSettings_SetLightLevel(light_level_t level) {
    if (level >= LIGHT_LEVEL_LOW && level <= LIGHT_LEVEL_HIGH) {
        current_light_level = level;
        Serial.printf("LightingSettings: Light level set to %d\n", level);

        // 如果当前灯是亮的，并且没有在进行渐亮，或者用户手动调整了亮度，则立即应用新档位的亮度
        if (is_light_on_from_pwm() && !is_brightening_process_active) {
            manual_override_active = true; // 标记手动覆盖
            apply_pwm_to_hardware(LightingSettings_GetPwmForLevel(current_light_level));
        } else if (is_brightening_process_active) {
            // 如果在渐亮过程中调整亮度档位，则停止当前渐亮，并立即应用新档位亮度
            is_brightening_process_active = false;
            manual_override_active = true;
             apply_pwm_to_hardware(LightingSettings_GetPwmForLevel(current_light_level));
            Serial.println("LightingSettings: Brightening process overridden by manual level change.");
        }
    }
}

light_level_t LightingSettings_GetLightLevel() {
    return current_light_level;
}

uint8_t LightingSettings_GetPwmForLevel(light_level_t level) {
    switch (level) {
        case LIGHT_LEVEL_LOW:    return PWM_PERCENT_LOW;
        case LIGHT_LEVEL_MEDIUM: return PWM_PERCENT_MEDIUM;
        case LIGHT_LEVEL_HIGH:   return PWM_PERCENT_HIGH;
        case LIGHT_LEVEL_OFF:    return 0;
        default:                 return PWM_PERCENT_HIGH; // 默认为高档
    }
}

void LightingSettings_SetWakeupEnabled(bool enabled) {
    wakeup_mode_enabled = enabled;
    Serial.printf("LightingSettings: Wakeup mode %s\n", enabled ? "enabled" : "disabled");
    if (!enabled && is_brightening_process_active) {
        // 如果关闭唤醒模式时正在渐亮，则停止渐亮，并应用当前档位的亮度
        is_brightening_process_active = false;
        manual_override_active = true;
        if (is_light_on_from_pwm() || current_applied_pwm > 0) { // 只有在灯之前是亮的情况下
             apply_pwm_to_hardware(LightingSettings_GetPwmForLevel(current_light_level));
        }
        Serial.println("LightingSettings: Brightening process stopped due to wakeup mode disable.");
    }
}

bool LightingSettings_IsWakeupEnabled() {
    return wakeup_mode_enabled;
}

void LightingSettings_SetBrighteningDurationSeconds(uint16_t seconds) {
    // 可以设置一个最大值，例如 59*60 + 59 = 3599 秒
    if (seconds <= 3599) {
        brightening_duration_seconds = seconds;
        Serial.printf("LightingSettings: Brightening duration set to %u seconds\n", seconds);
    }
}

uint16_t LightingSettings_GetBrighteningDurationSeconds() {
    return brightening_duration_seconds;
}

void LightingSettings_HandleLightToggle(bool is_on) {
    Serial.printf("LightingSettings: HandleLightToggle called with is_on = %s\n", is_on ? "true" : "false");
    is_brightening_process_active = false; // 任何手动开关操作都停止当前的渐亮过程
    manual_override_active = true;         // 标记为手动操作

    if (is_on) {
        if (wakeup_mode_enabled && brightening_duration_seconds > 0) {
            Serial.println("LightingSettings: Starting brightening process.");
            is_brightening_process_active = true;
            manual_override_active = false; // 开始新的渐亮，不是手动覆盖
            brightening_start_time_millis = millis();
            apply_pwm_to_hardware(0); // 从0开始渐亮
        } else {
            Serial.println("LightingSettings: Turning light on to target PWM (no brightening).");
            apply_pwm_to_hardware(LightingSettings_GetPwmForLevel(current_light_level));
        }
    } else {
        Serial.println("LightingSettings: Turning light off.");
        apply_pwm_to_hardware(0);
    }
}

void LightingSettings_UpdateBrighteningState() {
    if (!is_brightening_process_active || manual_override_active) {
        return; // 如果没在渐亮或已被手动覆盖，则不执行
    }

    unsigned long elapsed_time_ms = millis() - brightening_start_time_millis;
    unsigned long total_duration_ms = (unsigned long)brightening_duration_seconds * 1000;
    uint8_t target_pwm = LightingSettings_GetPwmForLevel(current_light_level);

    if (total_duration_ms == 0) { // 渐亮时长为0，应立即达到目标亮度 (理论上在HandleLightToggle已处理)
        apply_pwm_to_hardware(target_pwm);
        is_brightening_process_active = false;
        return;
    }

    if (elapsed_time_ms < total_duration_ms) {
        float progress_ratio = (float)elapsed_time_ms / total_duration_ms;
        uint8_t calculated_pwm = (uint8_t)(progress_ratio * target_pwm);
        apply_pwm_to_hardware(calculated_pwm);
    } else {
        // 渐亮完成
        apply_pwm_to_hardware(target_pwm);
        is_brightening_process_active = false;
        Serial.println("LightingSettings: Brightening process completed.");
    }
}

uint8_t LightingSettings_GetCurrentAppliedPwm() {
    return current_applied_pwm;
}

void LightingSettings_GetStatusString(char* buffer, size_t buffer_len) {
    if (!buffer || buffer_len == 0) return;

    const char* level_str;
    switch (current_light_level) {
        case LIGHT_LEVEL_LOW:    level_str = "低"; break;
        case LIGHT_LEVEL_MEDIUM: level_str = "中"; break;
        case LIGHT_LEVEL_HIGH:   level_str = "高"; break;
        default:                 level_str = "未知"; break;
    }

    if (wakeup_mode_enabled) {
        uint8_t minutes = brightening_duration_seconds / 60;
        uint8_t seconds = brightening_duration_seconds % 60;
        snprintf(buffer, buffer_len, "亮度：%s\n唤醒：开启\n时间：%02u:%02u", level_str, minutes, seconds);
    } else {
        snprintf(buffer, buffer_len, "亮度：%s\n唤醒：关闭", level_str);
    }
}

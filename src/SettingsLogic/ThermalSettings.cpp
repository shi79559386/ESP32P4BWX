// File: src/SettingsLogic/ThermalSettings.cpp
#include "ThermalSettings.h"
#include "../Peripherals/OutputControls.h"
#include "../App/AppGlobal.h"
#include <Arduino.h>
#include <stdio.h>

// 简化的配置结构体
static thermal_settings_config_t s_thermal_config = {
    .heater_enabled = false,
    .fan_enabled = false,
    .timed_shutdown_enabled = false,
    .countdown_hour = 0,
    .countdown_minute = 30,
    .countdown_second = 0
};

static unsigned long s_countdown_start_millis = 0;
static bool s_timed_session_active = false;
static thermal_status_update_cb_t s_status_callback = NULL;

static void trigger_ui_update() {
    if (s_status_callback) {
        s_status_callback();
    }
}

static void apply_hardware_state() {
    // 直接控制硬件
    set_heater_state(s_thermal_config.heater_enabled);
    set_fan_state(s_thermal_config.fan_enabled);
    
    Serial.printf("ThermalSettings: Heater=%s, Fan=%s\n", 
                  s_thermal_config.heater_enabled ? "ON" : "OFF",
                  s_thermal_config.fan_enabled ? "ON" : "OFF");
    
    trigger_ui_update();
}

void ThermalSettings_Init() {
    // 从硬件状态同步
    s_thermal_config.heater_enabled = get_heater_state();
    s_thermal_config.fan_enabled = get_fan_state();
    
    s_thermal_config.timed_shutdown_enabled = false;
    s_thermal_config.countdown_hour = 0;
    s_thermal_config.countdown_minute = 30;
    s_thermal_config.countdown_second = 0;
    s_timed_session_active = false;
    
    Serial.println("ThermalSettings: Initialized.");
    trigger_ui_update();
}

// 加热器控制 - 开启时自动开启风扇
void ThermalSettings_SetHeaterEnabled(bool enabled) {
    if (s_thermal_config.heater_enabled != enabled) {
        s_thermal_config.heater_enabled = enabled;
        
        // 关键逻辑：如果开启加热器，自动开启风扇
        if (enabled && !s_thermal_config.fan_enabled) {
            s_thermal_config.fan_enabled = true;
            Serial.println("ThermalSettings: Heater enabled - automatically enabling fan");
        }
        
        Serial.printf("ThermalSettings: Heater %s\n", enabled ? "enabled" : "disabled");
        apply_hardware_state();
    }
}

bool ThermalSettings_IsHeaterEnabled() {
    return s_thermal_config.heater_enabled;
}

// 风扇控制 - 关闭时必须同时关闭加热器
void ThermalSettings_SetFanEnabled(bool enabled) {
    if (s_thermal_config.fan_enabled != enabled) {
        s_thermal_config.fan_enabled = enabled;
        
        // 关键逻辑：如果关闭风扇，必须同时关闭加热器
        if (!enabled && s_thermal_config.heater_enabled) {
            s_thermal_config.heater_enabled = false;
            Serial.println("ThermalSettings: Fan disabled - automatically disabling heater for safety");
        }
        
        Serial.printf("ThermalSettings: Fan %s\n", enabled ? "enabled" : "disabled");
        apply_hardware_state();
    }
}

bool ThermalSettings_IsFanEnabled() {
    return s_thermal_config.fan_enabled;
}

// 移除或简化这个检查函数，因为现在加热器可以自动开启风扇
//bool ThermalSettings_CanEnableHeater() {
    //return true;  // 现在总是可以开启，因为会自动开启风扇
//}

// 任一设备是否开启
bool ThermalSettings_IsAnyDeviceEnabled() {
    return s_thermal_config.heater_enabled || s_thermal_config.fan_enabled;
}

// 定时关闭功能
void ThermalSettings_SetTimedShutdown(const timed_operation_config_t* config) {
    if (config) {
        bool prev_enabled = s_thermal_config.timed_shutdown_enabled;
        s_thermal_config.timed_shutdown_enabled = config->enabled;
        s_thermal_config.countdown_hour = config->countdown_hour;
        s_thermal_config.countdown_minute = config->countdown_minute;
        s_thermal_config.countdown_second = config->countdown_second;
        
        Serial.printf("ThermalSettings: Timed shutdown set. Enabled: %s, Duration: %02u:%02u:%02u\n",
                      s_thermal_config.timed_shutdown_enabled ? "Yes" : "No",
                      s_thermal_config.countdown_hour,
                      s_thermal_config.countdown_minute,
                      s_thermal_config.countdown_second);

        if (s_thermal_config.timed_shutdown_enabled) {
            s_countdown_start_millis = millis();
            s_timed_session_active = true;
            Serial.println("ThermalSettings: Timed shutdown started.");
        } else {
            s_timed_session_active = false;
            if (prev_enabled) {
                Serial.println("ThermalSettings: Timed shutdown disabled.");
            }
        }
        trigger_ui_update();
    }
}

const thermal_settings_config_t* ThermalSettings_GetConfig() {
    return &s_thermal_config;
}

// 定时更新处理
void ThermalSettings_UpdateTimedState() {
    if (!s_thermal_config.timed_shutdown_enabled || !s_timed_session_active) {
        return;
    }

    unsigned long total_countdown_ms =
        (unsigned long)s_thermal_config.countdown_hour * 3600000UL +
        (unsigned long)s_thermal_config.countdown_minute * 60000UL +
        (unsigned long)s_thermal_config.countdown_second * 1000UL;

    if (total_countdown_ms == 0 && s_thermal_config.timed_shutdown_enabled) {
        Serial.println("ThermalSettings: Timed shutdown enabled with zero duration. Stopping.");
        s_thermal_config.heater_enabled = false;
        s_thermal_config.fan_enabled = false;
        s_timed_session_active = false;
        s_thermal_config.timed_shutdown_enabled = false;
        apply_hardware_state();
        return;
    }

    unsigned long elapsed_ms = millis() - s_countdown_start_millis;

    if (elapsed_ms >= total_countdown_ms) {
        Serial.println("ThermalSettings: Timed shutdown completed. Turning off heater and fan.");
        s_thermal_config.heater_enabled = false;
        s_thermal_config.fan_enabled = false;
        s_timed_session_active = false;
        s_thermal_config.timed_shutdown_enabled = false;
        apply_hardware_state();
    } else {
        // 倒计时进行中，触发UI更新
        trigger_ui_update();
    }
}

bool ThermalSettings_IsTimedSessionActive() {
    return s_timed_session_active && s_thermal_config.timed_shutdown_enabled;
}

void ThermalSettings_GetFormattedRemainingCountdown(char* buffer, size_t buffer_len) {
    if (!buffer || buffer_len == 0) return;

    if (s_thermal_config.timed_shutdown_enabled && s_timed_session_active) {
        unsigned long total_countdown_ms =
            (unsigned long)s_thermal_config.countdown_hour * 3600000UL +
            (unsigned long)s_thermal_config.countdown_minute * 60000UL +
            (unsigned long)s_thermal_config.countdown_second * 1000UL;
        
        unsigned long elapsed_ms = millis() - s_countdown_start_millis;
        
        if (elapsed_ms < total_countdown_ms) {
            unsigned long remaining_ms = total_countdown_ms - elapsed_ms;
            uint8_t hours = remaining_ms / 3600000UL;
            remaining_ms %= 3600000UL;
            uint8_t minutes = remaining_ms / 60000UL;
            remaining_ms %= 60000UL;
            uint8_t seconds = remaining_ms / 1000UL;
            snprintf(buffer, buffer_len, "%02u:%02u:%02u", hours, minutes, seconds);
        } else {
            snprintf(buffer, buffer_len, "00:00:00"); 
        }
    } else {
        snprintf(buffer, buffer_len, "%02u:%02u:%02u", 
                 s_thermal_config.countdown_hour, 
                 s_thermal_config.countdown_minute, 
                 s_thermal_config.countdown_second);
    }
}

void ThermalSettings_RegisterStatusUpdateCallback(thermal_status_update_cb_t cb) {
    s_status_callback = cb;
}

// 获取状态字符串
const char* ThermalSettings_GetStatusString() {
    static char status_buffer[100];
    
    if (s_thermal_config.heater_enabled && s_thermal_config.fan_enabled) {
        strcpy(status_buffer, "加热+风扇运行");
    } else if (s_thermal_config.heater_enabled) {
        strcpy(status_buffer, "加热器运行");
    } else if (s_thermal_config.fan_enabled) {
        strcpy(status_buffer, "风扇运行");
    } else {
        strcpy(status_buffer, "已关闭");
    }
    
    if (s_timed_session_active) {
        char countdown_str[20];
        ThermalSettings_GetFormattedRemainingCountdown(countdown_str, sizeof(countdown_str));
        char temp_buffer[100];
        snprintf(temp_buffer, sizeof(temp_buffer), "%s (定时关: %s)", status_buffer, countdown_str);
        strcpy(status_buffer, temp_buffer);
    }
    
    return status_buffer;
}
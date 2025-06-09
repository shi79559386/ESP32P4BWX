#include "DryingSettings.h"
#include "../Peripherals/OutputControls.h"
#include "../App/AppGlobal.h"
#include <Arduino.h>
#include <stdio.h>

static bool s_drying_enabled_by_logic = false;
static timed_operation_config_t s_timed_config = {false, 0, 30, 0}; // 默认30分钟
static unsigned long s_countdown_start_millis = 0;
static bool s_timed_session_active = false;

static drying_status_update_cb_t s_status_callback = NULL;

static void trigger_ui_update() {
    if (s_status_callback) {
        s_status_callback();
    }
}

// 内部函数，根据逻辑状态更新硬件
static void apply_hardware_state() {
    if (s_drying_enabled_by_logic) {
        set_heater_state(true); // 烘干启动加热器
        // set_fan_state(true); // 可能也需要启动风扇
        Serial.println("DryingSettings: Hardware ON (Heater activated)");
    } else {
        set_heater_state(false);
        // set_fan_state(false);
        Serial.println("DryingSettings: Hardware OFF (Heater deactivated)");
    }
    trigger_ui_update();
}

void DryingSettings_Init() {
    s_drying_enabled_by_logic = get_heater_state(); // 与硬件状态同步
    
    s_timed_config.enabled = false;
    s_timed_config.countdown_hour = 0;
    s_timed_config.countdown_minute = 30; // 默认30分钟
    s_timed_config.countdown_second = 0;
    s_timed_session_active = false;
    Serial.println("DryingSettings: Initialized.");
    trigger_ui_update();
}

void DryingSettings_SetEnabled(bool enabled) {
    if (s_drying_enabled_by_logic != enabled) {
        s_drying_enabled_by_logic = enabled;
        Serial.printf("DryingSettings: Logic state set to %s\n", enabled ? "ON" : "OFF");
        
        if (enabled) {
            // 手动开启时，取消当前的定时会话（如果有的话）
            if (s_timed_config.enabled && s_timed_session_active) {
                s_timed_session_active = false;
                Serial.println("DryingSettings: Manual ON overrides active timed session.");
            }
        } else {
            // 手动关闭时，也取消定时会话
            s_timed_session_active = false;
            Serial.println("DryingSettings: Manual OFF, timed session cancelled.");
        }
        apply_hardware_state();
    }
}

bool DryingSettings_IsEnabled() {
    return s_drying_enabled_by_logic;
}

void DryingSettings_SetTimedOperation(const timed_operation_config_t* config) {
    if (config) {
        bool prev_timed_enabled = s_timed_config.enabled;
        s_timed_config = *config;
        Serial.printf("DryingSettings: Timed operation set. Enabled: %s, Duration: %02u:%02u:%02u\n",
                      s_timed_config.enabled ? "Yes" : "No",
                      s_timed_config.countdown_hour,
                      s_timed_config.countdown_minute,
                      s_timed_config.countdown_second);

        if (s_timed_config.enabled) {
            s_countdown_start_millis = millis();
            s_timed_session_active = true;
            s_drying_enabled_by_logic = true; // 定时启动时，自动开启烘干
            apply_hardware_state();
            Serial.println("DryingSettings: Timed drying started.");
        } else {
            s_timed_session_active = false;
            if (prev_timed_enabled) {
                Serial.println("DryingSettings: Timed operation disabled.");
            }
        }
        trigger_ui_update();
    }
}

const timed_operation_config_t* DryingSettings_GetTimedOperationSettings() {
    return &s_timed_config;
}

void DryingSettings_UpdateTimedState() {
    if (!s_timed_config.enabled || !s_timed_session_active || !s_drying_enabled_by_logic) {
        return;
    }

    unsigned long total_countdown_ms =
        (unsigned long)s_timed_config.countdown_hour * 3600000UL +
        (unsigned long)s_timed_config.countdown_minute * 60000UL +
        (unsigned long)s_timed_config.countdown_second * 1000UL;

    if (total_countdown_ms == 0 && s_timed_config.enabled) {
        Serial.println("DryingSettings: Timed operation enabled with zero duration. Stopping.");
        s_drying_enabled_by_logic = false;
        s_timed_session_active = false;
        s_timed_config.enabled = false;
        apply_hardware_state();
        return;
    }

    unsigned long elapsed_ms = millis() - s_countdown_start_millis;

    if (elapsed_ms >= total_countdown_ms) {
        Serial.println("DryingSettings: Timed operation completed. Stopping drying.");
        s_drying_enabled_by_logic = false;
        s_timed_session_active = false;
        s_timed_config.enabled = false;
        apply_hardware_state();
    } else {
        // 倒计时进行中，触发UI更新以显示剩余时间
        trigger_ui_update();
    }
}

bool DryingSettings_IsTimedSessionActive() {
    return s_timed_session_active && s_timed_config.enabled;
}

void DryingSettings_GetFormattedRemainingCountdown(char* buffer, size_t buffer_len) {
    if (!buffer || buffer_len == 0) return;

    if (s_timed_config.enabled && s_timed_session_active && s_drying_enabled_by_logic) {
        unsigned long total_countdown_ms =
            (unsigned long)s_timed_config.countdown_hour * 3600000UL +
            (unsigned long)s_timed_config.countdown_minute * 60000UL +
            (unsigned long)s_timed_config.countdown_second * 1000UL;
        
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
                 s_timed_config.countdown_hour, 
                 s_timed_config.countdown_minute, 
                 s_timed_config.countdown_second);
    }
}

void DryingSettings_RegisterStatusUpdateCallback(drying_status_update_cb_t cb) {
    s_status_callback = cb;
}
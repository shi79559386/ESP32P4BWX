#include "SterilizationSettings.h"
#include "../Peripherals/OutputControls.h"
#include "../App/AppGlobal.h"
#include <Arduino.h>
#include <stdio.h> // For snprintf

static bool s_sterilization_enabled_by_logic = false;
static timed_operation_config_t s_timed_config = {false, 0, 15, 0}; // 默认禁用，15分钟倒计时
static unsigned long s_countdown_start_millis = 0;
static bool s_timed_session_active = false;

static sterilization_status_update_cb_t s_status_callback = NULL;

static void trigger_ui_update() {
    if (s_status_callback) {
        s_status_callback();
    }
}

static void apply_hardware_state() {
    if (s_sterilization_enabled_by_logic) {
        set_sterilize_state(true);
        Serial.println("SterilizationSettings: Hardware ON");
    } else {
        set_sterilize_state(false);
        Serial.println("SterilizationSettings: Hardware OFF");
    }
    trigger_ui_update();
}

void SterilizationSettings_Init() {
    s_sterilization_enabled_by_logic = get_sterilize_state(); // 同步硬件状态
    // s_sterilization_enabled_by_logic = false; // 或者默认关闭
    // apply_hardware_state();

    s_timed_config.enabled = false;
    s_timed_config.countdown_hour = 0;
    s_timed_config.countdown_minute = 15; // 默认15分钟
    s_timed_config.countdown_second = 0;
    s_timed_session_active = false;
    Serial.println("SterilizationSettings: Initialized.");
    trigger_ui_update();
}

void SterilizationSettings_SetEnabled(bool enabled) {
    if (s_sterilization_enabled_by_logic != enabled) {
        s_sterilization_enabled_by_logic = enabled;
        Serial.printf("SterilizationSettings: Logic state set to %s\n", enabled ? "ON" : "OFF");
        if (enabled) {
            if (s_timed_config.enabled && s_timed_session_active) {
                s_timed_session_active = false;
                Serial.println("SterilizationSettings: Manual ON overrides active timed session.");
            }
        } else {
            s_timed_session_active = false;
            Serial.println("SterilizationSettings: Manual OFF, timed session cancelled.");
        }
        apply_hardware_state();
    }
}

bool SterilizationSettings_IsEnabled() {
    return s_sterilization_enabled_by_logic;
}

void SterilizationSettings_SetTimedOperation(const timed_operation_config_t* config) {
    if (config) {
        bool prev_timed_enabled = s_timed_config.enabled;
        s_timed_config = *config;
        Serial.printf("SterilizationSettings: Timed operation set. Enabled: %s, Duration: %02u:%02u:%02u\n",
                      s_timed_config.enabled ? "Yes" : "No",
                      s_timed_config.countdown_hour,
                      s_timed_config.countdown_minute,
                      s_timed_config.countdown_second);

        if (s_timed_config.enabled) {
            s_countdown_start_millis = millis();
            s_timed_session_active = true;
            s_sterilization_enabled_by_logic = true; 
            apply_hardware_state(); 
            Serial.println("SterilizationSettings: Timed sterilization started.");
        } else {
            s_timed_session_active = false;
            if (prev_timed_enabled) {
                 Serial.println("SterilizationSettings: Timed operation disabled.");
                 // Optionally stop hardware if disabling timer means stopping the process
                 // s_sterilization_enabled_by_logic = false;
                 // apply_hardware_state();
            }
        }
        trigger_ui_update();
    }
}

const timed_operation_config_t* SterilizationSettings_GetTimedOperationSettings() {
    return &s_timed_config;
}

void SterilizationSettings_UpdateTimedState() {
    if (!s_timed_config.enabled || !s_timed_session_active || !s_sterilization_enabled_by_logic) {
        return; 
    }

    unsigned long total_countdown_ms =
        (unsigned long)s_timed_config.countdown_hour * 3600000UL +
        (unsigned long)s_timed_config.countdown_minute * 60000UL +
        (unsigned long)s_timed_config.countdown_second * 1000UL;

    if (total_countdown_ms == 0 && s_timed_config.enabled) {
        Serial.println("SterilizationSettings: Timed operation enabled with zero duration. Stopping.");
        s_sterilization_enabled_by_logic = false;
        s_timed_session_active = false;
        s_timed_config.enabled = false; 
        apply_hardware_state(); 
        return;
    }

    unsigned long elapsed_ms = millis() - s_countdown_start_millis;

    if (elapsed_ms >= total_countdown_ms) {
        Serial.println("SterilizationSettings: Timed operation completed. Stopping sterilization.");
        s_sterilization_enabled_by_logic = false;
        s_timed_session_active = false;
        s_timed_config.enabled = false; 
        apply_hardware_state(); 
    } else {
        trigger_ui_update(); // Update remaining time on UI
    }
}

bool SterilizationSettings_IsTimedSessionActive() {
    return s_timed_session_active && s_timed_config.enabled;
}

void SterilizationSettings_GetFormattedRemainingCountdown(char* buffer, size_t buffer_len) {
    if (!buffer || buffer_len == 0) return;

    if (s_timed_config.enabled && s_timed_session_active && s_sterilization_enabled_by_logic) {
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

void SterilizationSettings_RegisterStatusUpdateCallback(sterilization_status_update_cb_t cb) {
    s_status_callback = cb;
}
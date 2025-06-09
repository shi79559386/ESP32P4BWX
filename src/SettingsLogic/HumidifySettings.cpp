// File: src/SettingsLogic/HumidifySettings.cpp
#include "HumidifySettings.h"
#include "../Peripherals/OutputControls.h"
#include <Arduino.h>
#include <stdio.h>

static humidify_config_t s_humidify_config = {
    .enabled = false,
    .mode = HUMIDIFY_MODE_MANUAL,
    .target_humidity_left = 60,
    .target_humidity_right = 60,
    .humidity_tolerance = 5
};

static humidify_status_update_cb_t s_status_callback = NULL;
static char s_status_string[100];

static void trigger_ui_update() {
    if (s_status_callback) {
        s_status_callback();
    }
}

static void apply_humidify_control() {
    if (s_humidify_config.enabled) {
        // 这里应该调用实际的加湿硬件接口
        // 例如：set_humidifier_state(true);
        
        const char* mode_str[] = {"手动", "自动", "定时"};
        snprintf(s_status_string, sizeof(s_status_string), 
                "状态: 加湿中\n模式: %s\n目标: %d%% | %d%%", 
                mode_str[s_humidify_config.mode],
                s_humidify_config.target_humidity_left,
                s_humidify_config.target_humidity_right);
    } else {
        // set_humidifier_state(false);
        snprintf(s_status_string, sizeof(s_status_string), "状态: 关闭");
    }
    
    // 同步全局变量
    g_target_humidity_left = s_humidify_config.target_humidity_left;
    g_target_humidity_right = s_humidify_config.target_humidity_right;
    
    trigger_ui_update();
}

void HumidifySettings_Init() {
    // 从全局变量同步初始值
    s_humidify_config.target_humidity_left = g_target_humidity_left;
    s_humidify_config.target_humidity_right = g_target_humidity_right;
    
    apply_humidify_control();
    Serial.println("HumidifySettings: Initialized");
}

void HumidifySettings_SetEnabled(bool enabled) {
    if (s_humidify_config.enabled != enabled) {
        s_humidify_config.enabled = enabled;
        apply_humidify_control();
        Serial.printf("HumidifySettings: Enabled set to %s\n", enabled ? "true" : "false");
    }
}

bool HumidifySettings_IsEnabled() {
    return s_humidify_config.enabled;
}

void HumidifySettings_SetMode(humidify_mode_t mode) {
    if (s_humidify_config.mode != mode) {
        s_humidify_config.mode = mode;
        apply_humidify_control();
        Serial.printf("HumidifySettings: Mode set to %d\n", mode);
    }
}

humidify_mode_t HumidifySettings_GetMode() {
    return s_humidify_config.mode;
}

void HumidifySettings_SetTargetHumidity(int left_humidity, int right_humidity) {
    if (s_humidify_config.target_humidity_left != left_humidity || 
        s_humidify_config.target_humidity_right != right_humidity) {
        s_humidify_config.target_humidity_left = left_humidity;
        s_humidify_config.target_humidity_right = right_humidity;
        apply_humidify_control();
        Serial.printf("HumidifySettings: Target humidity set to %d%% | %d%%\n", left_humidity, right_humidity);
    }
}

void HumidifySettings_GetTargetHumidity(int* left_humidity, int* right_humidity) {
    if (left_humidity) *left_humidity = s_humidify_config.target_humidity_left;
    if (right_humidity) *right_humidity = s_humidify_config.target_humidity_right;
}

void HumidifySettings_SetTolerance(int tolerance) {
    s_humidify_config.humidity_tolerance = tolerance;
    Serial.printf("HumidifySettings: Tolerance set to %d%%\n", tolerance);
}

int HumidifySettings_GetTolerance() {
    return s_humidify_config.humidity_tolerance;
}

const humidify_config_t* HumidifySettings_GetConfig() {
    return &s_humidify_config;
}

void HumidifySettings_RegisterStatusUpdateCallback(humidify_status_update_cb_t cb) {
    s_status_callback = cb;
}

const char* HumidifySettings_GetStatusString() {
    return s_status_string;
}
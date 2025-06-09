// FreshAirSettings.cpp - 简化版
#include "FreshAirSettings.h"
#include "../Peripherals/OutputControls.h"
#include <Arduino.h>
#include <stdio.h>

static bool s_fresh_air_enabled = false;
static fresh_air_mode_t s_current_mode = FRESH_AIR_MODE_MANUAL;
static fresh_air_schedule_t s_schedule = {false, 8, 0, 20, 0};
static fresh_air_status_update_cb_t s_status_callback = NULL;
static char s_status_string[100];

static void trigger_ui_update() {
    if (s_status_callback) {
        s_status_callback();
    }
}

static void apply_fresh_air_control() {
    set_fresh_air_state(s_fresh_air_enabled);
    
    const char* mode_str[] = {"手动", "自动", "定时"};
    if (s_fresh_air_enabled) {
        snprintf(s_status_string, sizeof(s_status_string), 
                "状态: 运行中\n模式: %s", mode_str[s_current_mode]);
    } else {
        snprintf(s_status_string, sizeof(s_status_string), "状态: 关闭");
    }
    
    trigger_ui_update();
}

void FreshAirSettings_Init() {
    s_fresh_air_enabled = get_fresh_air_state();
    apply_fresh_air_control();
    Serial.println("FreshAirSettings: Initialized");
}

void FreshAirSettings_SetEnabled(bool enabled) {
    if (s_fresh_air_enabled != enabled) {
        s_fresh_air_enabled = enabled;
        apply_fresh_air_control();
    }
}

bool FreshAirSettings_IsEnabled() {
    return s_fresh_air_enabled;
}

void FreshAirSettings_SetMode(fresh_air_mode_t mode) {
    if (s_current_mode != mode) {
        s_current_mode = mode;
        apply_fresh_air_control();
    }
}

fresh_air_mode_t FreshAirSettings_GetMode() {
    return s_current_mode;
}

void FreshAirSettings_SetSchedule(const fresh_air_schedule_t* schedule) {
    if (schedule) {
        s_schedule = *schedule;
        apply_fresh_air_control();
    }
}

const fresh_air_schedule_t* FreshAirSettings_GetSchedule() {
    return &s_schedule;
}

void FreshAirSettings_UpdateAutoControl() {
    // 简化版：暂时空实现
}

void FreshAirSettings_RegisterStatusUpdateCallback(fresh_air_status_update_cb_t cb) {
    s_status_callback = cb;
}

const char* FreshAirSettings_GetStatusString() {
    return s_status_string;
}
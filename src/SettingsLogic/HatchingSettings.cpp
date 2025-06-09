// File: src/SettingsLogic/HatchingSettings.cpp - 修正版本
#include "HatchingSettings.h"
#include "../App/AppGlobal.h"
#include "../Control/ThermalControl.h" 
#include <Arduino.h>
#include <stdio.h>
#include <string.h>

// 品种参数表 - 8个元素对应PARROT_SPECIES_COUNT = 8
static const struct {
    const char* name;
    float temp_default;
    int humidity_default;
    uint8_t days;
} species_data[PARROT_SPECIES_COUNT] = {
    {"牡丹", 37.5f, 55, 23},         // PARROT_SPECIES_BUDGIE
    {"玄凤", 37.6f, 60, 18},         // PARROT_SPECIES_COCKATIEL
    {"太阳锥尾", 37.7f, 62, 24},     // PARROT_SPECIES_SUN_CONURE
    {"金太阳", 37.8f, 67, 26},       // PARROT_SPECIES_GOLDEN_SUN
    {"非洲灰", 37.9f, 65, 28},       // PARROT_SPECIES_AFRICAN_GREY
    {"蓝黄金刚", 38.0f, 72, 26},     // PARROT_SPECIES_BLUE_GOLD_MACAW
    {"和尚", 38.0f, 70, 29},         // PARROT_SPECIES_MONK
    {"自定义", 37.5f, 60, 21}        // PARROT_SPECIES_CUSTOM
};

// 孵化阶段名称
static const char* stage_names[] = {
    "准备中", "早期孵化", "中期孵化", "后期孵化", "即将出雏"
};

// 全局状态
static hatching_status_t s_status = {0};
static hatching_status_update_cb_t s_callback = NULL;
static unsigned long s_last_day_update = 0;
static bool s_was_thermal_enabled = false;

static void trigger_ui_update() {
    if (s_callback) {
        s_callback();
    }
}

static void apply_hatching_temperature_control() {
    if (!s_status.active) {
        return;
    }
    
    Serial.printf("HatchingSettings: 应用孵化温控设置 - 目标温度%.1f°C\n", s_status.target_temp);
    
    // 先更新全局变量（确保UI同步）
    g_target_temperature_left = s_status.target_temp;
    g_target_temperature_right = s_status.target_temp;
    
    // 设置温控系统的目标温度
    bool left_ok = ThermalControl_SetTargetTemp(THERMAL_ZONE_LEFT, s_status.target_temp);
    bool right_ok = ThermalControl_SetTargetTemp(THERMAL_ZONE_RIGHT, s_status.target_temp);
    
    if (!left_ok || !right_ok) {
        Serial.printf("HatchingSettings: 温度设置失败 - 左区%s, 右区%s\n", 
                     left_ok ? "成功" : "失败", right_ok ? "成功" : "失败");
        return;
    }
    
    // 启用温控系统
    if (!ThermalControl_IsEnabled()) {
        ThermalControl_SetEnabled(true);
        Serial.println("HatchingSettings: 已启用温控系统");
        
        // 等待一小段时间让系统启动
        delay(100);
    }
    
    // 验证设置是否生效
    float actual_left = ThermalControl_GetTargetTemp(THERMAL_ZONE_LEFT);
    float actual_right = ThermalControl_GetTargetTemp(THERMAL_ZONE_RIGHT);
    
    if (abs(actual_left - s_status.target_temp) > 0.1f || 
        abs(actual_right - s_status.target_temp) > 0.1f) {
        Serial.printf("HatchingSettings: 温度设置验证失败 - 期望%.1f°C, 实际左区%.1f°C, 右区%.1f°C\n", 
                     s_status.target_temp, actual_left, actual_right);
    } else {
        Serial.printf("HatchingSettings: 温控设置验证成功 - 目标温度%.1f°C\n", s_status.target_temp);
    }
}

static void update_stage() {
    uint8_t prev_stage = s_status.current_stage;
    
    if (s_status.days_elapsed >= s_status.total_days) {
        s_status.current_stage = HATCH_STAGE_HATCHING;
    } else if (s_status.days_elapsed >= (s_status.total_days - 3)) {
        s_status.current_stage = HATCH_STAGE_LATE_INCUBATION;
    } else if (s_status.days_elapsed >= 1) {
        s_status.current_stage = HATCH_STAGE_EARLY_INCUBATION;
    } else {
        s_status.current_stage = HATCH_STAGE_SETUP;
    }
    
    if (prev_stage != s_status.current_stage) {
        Serial.printf("HatchingSettings: 孵化阶段变更为 %s (第%d天/%d天)\n", 
                     stage_names[s_status.current_stage], 
                     s_status.days_elapsed, s_status.total_days);
    }
}

// === 基础API函数 ===

void HatchingSettings_Init() {
    memset(&s_status, 0, sizeof(s_status));
    s_last_day_update = 0;
    s_was_thermal_enabled = false;
    Serial.println("HatchingSettings: 初始化完成");
}

void HatchingSettings_StartIncubation(parrot_species_t species) {
    if (species >= PARROT_SPECIES_COUNT) {
        Serial.println("HatchingSettings: 无效的鸟类品种");
        return;
    }
    
    // 记录启动前的温控状态
    s_was_thermal_enabled = ThermalControl_IsEnabled();
    
    // 重置状态
    memset(&s_status, 0, sizeof(s_status));
    
    // 基本设置
    s_status.active = true;
    s_status.species = species;
    s_status.current_stage = HATCH_STAGE_SETUP;
    s_status.process_start_time = millis();
    s_status.days_elapsed = 0;
    
    // 使用推荐参数
    const auto& data = species_data[species];
    s_status.target_temp = data.temp_default;
    s_status.target_humidity = data.humidity_default;
    s_status.total_days = data.days;
    
    // 重置时间戳
    s_last_day_update = millis();
    
    // 应用温控设置
    apply_hatching_temperature_control();
    trigger_ui_update();
    
    if (species == PARROT_SPECIES_CUSTOM) {
        Serial.printf("HatchingSettings: 开始自定义孵化 - %.1f°C, %d%%, %d天\n", 
                      s_status.target_temp, s_status.target_humidity, s_status.total_days);
    } else {
        Serial.printf("HatchingSettings: 开始孵化 %s - %.1f°C, %d%%, %d天\n", 
                      data.name, s_status.target_temp, s_status.target_humidity, s_status.total_days);
    }
}

void HatchingSettings_StopIncubation() {
    if (!s_status.active) {
        return;
    }
    
    s_status.active = false;
    
    // 恢复之前的温控状态
    if (!s_was_thermal_enabled) {
        ThermalControl_SetEnabled(false);
    }
    
    trigger_ui_update();
    Serial.println("HatchingSettings: 孵化停止");
}

bool HatchingSettings_IsActive() {
    return s_status.active;
}

void HatchingSettings_SetCustomParameters(float temp, int humidity) {
    if (!s_status.active) {
        Serial.println("HatchingSettings: 孵化未激活，无法设置参数");
        return;
    }
    
    // 参数范围检查
    if (temp < 20.0f || temp > 40.0f) {
        Serial.printf("HatchingSettings: 温度 %.1f°C 超出范围 (20-40°C)\n", temp);
        return;
    }
    
    if (humidity < 0 || humidity > 100) {
        Serial.printf("HatchingSettings: 湿度 %d%% 超出范围 (0-100%%)\n", humidity);
        return;
    }
    
    s_status.target_temp = temp;
    s_status.target_humidity = humidity;
    
    apply_hatching_temperature_control();
    trigger_ui_update();
    
    Serial.printf("HatchingSettings: 参数已更新 - %.1f°C, %d%%\n", temp, humidity);
}

const hatching_status_t* HatchingSettings_GetStatus() {
    return &s_status;
}

const char* HatchingSettings_GetStageName(hatch_stage_t stage) {
    if (stage < (sizeof(stage_names) / sizeof(stage_names[0]))) {
        return stage_names[stage];
    }
    return "未知阶段";
}

void HatchingSettings_GetFormattedTimeRemaining(char* buffer, size_t buffer_len) {
    if (!buffer || buffer_len == 0) return;
    
    if (!s_status.active) {
        snprintf(buffer, buffer_len, "未启动");
        return;
    }
    
    int remaining_days = s_status.total_days - s_status.days_elapsed;
    if (remaining_days <= 0) {
        snprintf(buffer, buffer_len, "完成");
    } else if (remaining_days == 1) {
        snprintf(buffer, buffer_len, "1天");
    } else {
        snprintf(buffer, buffer_len, "%d天", remaining_days);
    }
}

void HatchingSettings_UpdateProcess() {
    if (!s_status.active) {
        return;
    }
    
    unsigned long current_time = millis();
    
    // 更新孵化天数 (测试：每分钟=1天)
    static const unsigned long DAY_INTERVAL_MS = 60000UL;
    
    if (current_time - s_last_day_update >= DAY_INTERVAL_MS) {
        s_last_day_update = current_time;
        
        if (s_status.days_elapsed < s_status.total_days) {
            s_status.days_elapsed++;
            update_stage();
            trigger_ui_update();
            
            Serial.printf("HatchingSettings: 第%d天/%d天, 阶段: %s\n", 
                         s_status.days_elapsed, s_status.total_days, 
                         stage_names[s_status.current_stage]);
        }
        
        if (s_status.days_elapsed >= s_status.total_days) {
            Serial.println("HatchingSettings: 🐣 孵化完成！");
        }
    }
}

void HatchingSettings_RegisterStatusUpdateCallback(hatching_status_update_cb_t cb) {
    s_callback = cb;
    Serial.println("HatchingSettings: 状态更新回调已注册");
}

// === 新增函数实现 ===

bool HatchingSettings_SetCustomDays(int days) {
    if (!s_status.active) {
        return false;
    }
    
    if (days < 1 || days > 40) {
        return false;
    }
    
    if (s_status.species == PARROT_SPECIES_CUSTOM) {
        s_status.total_days = days;
        trigger_ui_update();
        return true;
    }
    
    return false;
}

bool HatchingSettings_IsCustomMode() {
    return s_status.active && (s_status.species == PARROT_SPECIES_CUSTOM);
}

const char* HatchingSettings_GetSpeciesName(parrot_species_t species) {
    if (species < PARROT_SPECIES_COUNT) {
        return species_data[species].name;
    }
    return "未知品种";
}

bool HatchingSettings_GetSpeciesDefaults(parrot_species_t species, float* temp, int* humidity, int* days) {
    if (species >= PARROT_SPECIES_COUNT || !temp || !humidity || !days) {
        return false;
    }
    
    const auto& data = species_data[species];
    *temp = data.temp_default;
    *humidity = data.humidity_default;
    *days = data.days;
    
    return true;
}

// === 简化的手动操作函数 ===

void HatchingSettings_SetAutoTurn(bool enabled) {
    Serial.printf("HatchingSettings: 自动翻蛋设置 %s\n", enabled ? "启用" : "禁用");
}

void HatchingSettings_SetAutoVentilation(bool enabled) {
    Serial.printf("HatchingSettings: 自动通风设置 %s\n", enabled ? "启用" : "禁用");
}

void HatchingSettings_ManualTurn() {
    if (s_status.active) {
        s_status.total_turns++;
        Serial.printf("HatchingSettings: 手动翻蛋 (总计: %u次)\n", s_status.total_turns);
        trigger_ui_update();
    }
}

void HatchingSettings_ManualVentilation() {
    if (s_status.active) {
        s_status.total_ventilations++;
        Serial.printf("HatchingSettings: 手动通风 (总计: %u次)\n", s_status.total_ventilations);
        trigger_ui_update();
    }
}

void HatchingSettings_GetNextActionTime(char* turn_buffer, char* vent_buffer, size_t buffer_len) {
    if (turn_buffer && buffer_len > 0) {
        snprintf(turn_buffer, buffer_len, "--:--");
    }
    if (vent_buffer && buffer_len > 0) {
        snprintf(vent_buffer, buffer_len, "--:--");
    }
}
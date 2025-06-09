// File: Display/UI_MainScreen.cpp
#include "UI_MainScreen.h"
#include "UI_Styles.h"
#include "../Config/Config.h"
#include "font/fengyahei_s16_bpp4.h"
#include "font/fengyahei_s24_bpp4.h"
#include "font/lv_font_digital_28.h"
#include "font/lv_font_digital_24.h"
#include "../Control/ThermalControl.h" 

#include <cstdio>
#include <Arduino.h>
#include <algorithm> // For std::min, std::max
#include "lvgl.h"

#include "UI_Common.h"
#include "../App/AppGlobal.h"

// --- 颜色和阈值定义 (保持不变) ---
static const lv_color_t C_COOL_ICE_BLUE      = lv_color_make(0x4D, 0xB6, 0xAC);
static const lv_color_t C_NORMAL_SKY_BLUE    = lv_color_make(0x4D, 0xB6, 0xAC);
static const lv_color_t C_WARM_PEACH         = lv_color_make(0xFF, 0xC0, 0x90);
static const lv_color_t C_TARGET_ORANGE_RED  = lv_color_make(0xF0, 0x5A, 0x20);
static const lv_color_t C_HIGH_ALERT_RED     = lv_color_make(0xD8, 0x30, 0x30);

const float TEMP_NORMAL_SKY_BLUE_PEAK   = 27.5f;
const float TEMP_WARM_PEACH_PEAK        = 29.8f;
const float TEMP_TARGET_ORANGE_RED_MID  = 32.5f;
const float TEMP_HIGH_ALERT_RED_TRANS_START = 35.0f;


// --- 静态变量 ---
static lv_obj_t *target_adjust_screen = NULL; 
static lv_obj_t *current_target_slider = NULL;
static lv_timer_t *slider_timeout_timer = NULL; 
static target_setting_type_t current_slider_type = TARGET_SETTING_NONE;
static lv_obj_t *slider_info_label = NULL;
static lv_obj_t *slider_btn_plus = NULL;
static lv_obj_t *slider_btn_minus = NULL;

static lv_timer_t *adj_button_long_press_timer = NULL;
static bool adj_button_is_plus = false;
static volatile bool adj_button_long_press_active = false;

static const uint32_t SLIDER_AUTO_CLOSE_TIMEOUT_MS = 10000; // 重命名以反映新行为
static unsigned long last_slider_interaction_time = 0;

#define MAX_GLOW_LAYERS 3 // 采纳您最新的修改
static lv_obj_t *meter_temp_left = NULL;
static lv_meter_indicator_t *indic_arc_temp_left = NULL;
static lv_meter_indicator_t *glow_inds_temp_left[MAX_GLOW_LAYERS];
static lv_obj_t *label_value_temp_left = NULL;

static lv_obj_t *meter_humid_left = NULL;
static lv_meter_indicator_t *indic_arc_humid_left = NULL;
static lv_meter_indicator_t *glow_inds_humid_left[MAX_GLOW_LAYERS];
static lv_obj_t *label_value_humid_left = NULL;

static lv_obj_t *meter_temp_right = NULL;
static lv_meter_indicator_t *indic_arc_temp_right = NULL;
static lv_meter_indicator_t *glow_inds_temp_right[MAX_GLOW_LAYERS];
static lv_obj_t *label_value_temp_right = NULL;

static lv_obj_t *meter_humid_right = NULL;
static lv_meter_indicator_t *indic_arc_humid_right = NULL;
static lv_meter_indicator_t *glow_inds_humid_right[MAX_GLOW_LAYERS];
static lv_obj_t *label_value_humid_right = NULL;

static lv_obj_t *label_top_scrolling_info = NULL;

static const lv_coord_t MAIN_ARC_BASE_WIDTH = 3;
static const int     GLOW_LAYERS_COUNT = MAX_GLOW_LAYERS;
static const lv_coord_t GLOW_WIDTH_INCREASE[MAX_GLOW_LAYERS] = { 10, 13, 16 }; // 采纳您最新的修改
static const lv_opa_t   GLOW_OPA_VALUES[MAX_GLOW_LAYERS]   = { LV_OPA_50, LV_OPA_40, LV_OPA_30 }; // 采纳您最新的修改
static const lv_coord_t GLOW_RADIUS_OFFSET[MAX_GLOW_LAYERS] = { 0, 1 ,2}; // 采纳您最新的修改

// --- 前向声明 ---
static void update_slider_and_info_label(float value, target_setting_type_t type);
static void adj_button_long_press_timer_cb(lv_timer_t *timer);
static void plus_minus_btn_event_cb(lv_event_t *e);
static void target_slider_event_cb(lv_event_t *e);

static void open_target_adjust_screen_event_cb(lv_event_t *e);
static void create_target_adjust_screen_ui(lv_obj_t* screen, target_setting_type_t type); // Renamed from _content
static void target_adjust_screen_bg_click_event_cb(lv_event_t *e); // 新增：用于屏幕背景点击
static void close_target_adjust_screen();
static void cleanup_target_adjust_screen_resources();
static void slider_timeout_close_screen_cb(lv_timer_t *timer); // 重命名: slider_auto_hide_cb -> slider_timeout_close_screen_cb


static lv_color_t calculate_dynamic_temp_color(float current_temp) {
    float t_norm;
    current_temp = LV_CLAMP(current_temp, ACTUAL_TEMP_MIN, ACTUAL_TEMP_MAX);

    if (current_temp <= ACTUAL_TEMP_MIN) {
        return C_COOL_ICE_BLUE;
    } else if (current_temp < TEMP_NORMAL_SKY_BLUE_PEAK) {
        if (TEMP_NORMAL_SKY_BLUE_PEAK <= ACTUAL_TEMP_MIN) return C_NORMAL_SKY_BLUE;
        t_norm = (current_temp - ACTUAL_TEMP_MIN) / (TEMP_NORMAL_SKY_BLUE_PEAK - ACTUAL_TEMP_MIN);
        t_norm = LV_CLAMP(t_norm, 0.0f, 1.0f);
        return lv_color_mix(C_COOL_ICE_BLUE, C_NORMAL_SKY_BLUE, (uint8_t)(t_norm * 255.0f));
    } else if (current_temp < TEMP_WARM_PEACH_PEAK) {
        if (TEMP_WARM_PEACH_PEAK <= TEMP_NORMAL_SKY_BLUE_PEAK) return C_WARM_PEACH;
        t_norm = (current_temp - TEMP_NORMAL_SKY_BLUE_PEAK) / (TEMP_WARM_PEACH_PEAK - TEMP_NORMAL_SKY_BLUE_PEAK);
        t_norm = LV_CLAMP(t_norm, 0.0f, 1.0f);
        return lv_color_mix(C_NORMAL_SKY_BLUE, C_WARM_PEACH, (uint8_t)(t_norm * 255.0f));
    } else if (current_temp < TEMP_TARGET_ORANGE_RED_MID) {
        if (TEMP_TARGET_ORANGE_RED_MID <= TEMP_WARM_PEACH_PEAK) return C_TARGET_ORANGE_RED;
        t_norm = (current_temp - TEMP_WARM_PEACH_PEAK) / (TEMP_TARGET_ORANGE_RED_MID - TEMP_WARM_PEACH_PEAK);
        t_norm = LV_CLAMP(t_norm, 0.0f, 1.0f);
        return lv_color_mix(C_WARM_PEACH, C_TARGET_ORANGE_RED, (uint8_t)(t_norm * 255.0f));
    } else if (current_temp < TEMP_HIGH_ALERT_RED_TRANS_START) {
        if (TEMP_HIGH_ALERT_RED_TRANS_START <= TEMP_TARGET_ORANGE_RED_MID) return lv_color_mix(C_TARGET_ORANGE_RED, C_HIGH_ALERT_RED, (uint8_t)(0.3 * 255.0f));
        t_norm = (current_temp - TEMP_TARGET_ORANGE_RED_MID) / (TEMP_HIGH_ALERT_RED_TRANS_START - TEMP_TARGET_ORANGE_RED_MID);
        t_norm = LV_CLAMP(t_norm, 0.0f, 1.0f);
        lv_color_t color_at_alert_start_boundary = lv_color_mix(C_TARGET_ORANGE_RED, C_HIGH_ALERT_RED, (uint8_t)(0.3 * 255.0f));
        return lv_color_mix(C_TARGET_ORANGE_RED, color_at_alert_start_boundary, (uint8_t)(t_norm * 255.0f));
    } else {
        lv_color_t start_color_for_red_zone = lv_color_mix(C_TARGET_ORANGE_RED, C_HIGH_ALERT_RED, (uint8_t)(0.3 * 255.0f));
        if (ACTUAL_TEMP_MAX <= TEMP_HIGH_ALERT_RED_TRANS_START) return C_HIGH_ALERT_RED;
        t_norm = (current_temp - TEMP_HIGH_ALERT_RED_TRANS_START) / (ACTUAL_TEMP_MAX - TEMP_HIGH_ALERT_RED_TRANS_START);
        t_norm = LV_CLAMP(t_norm, 0.0f, 1.0f);
        return lv_color_mix(start_color_for_red_zone, C_HIGH_ALERT_RED, (uint8_t)(t_norm * 255.0f));
    }
}


static void ext_draw_size_cb(lv_event_t * e) { lv_event_set_ext_draw_size(e, 40);}
static void meter_humidity_label_draw_event_cb(lv_event_t * e) {
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->part == LV_PART_TICKS && dsc->label_dsc != NULL) {
        int32_t v = dsc->value;
        if(v == 20 || v == 40 || v == 60 || v == 80) {
            dsc->label_dsc->opa = LV_OPA_TRANSP;
        }
    }
}

// --- 清理设置屏幕相关资源 ---
static void cleanup_target_adjust_screen_resources() {
    if (slider_timeout_timer) {
        lv_timer_del(slider_timeout_timer);
        slider_timeout_timer = NULL;
    }
    if (adj_button_long_press_timer) {
        lv_timer_del(adj_button_long_press_timer);
        adj_button_long_press_timer = NULL;
    }
    adj_button_long_press_active = false;

    current_target_slider = NULL;
    slider_info_label = NULL;
    slider_btn_plus = NULL;
    slider_btn_minus = NULL;
    current_slider_type = TARGET_SETTING_NONE;
}

// --- 关闭目标设置屏幕并返回主屏幕 ---
static void close_target_adjust_screen() {
    if (target_adjust_screen && lv_obj_is_valid(target_adjust_screen)) {
        if (lv_scr_act() == target_adjust_screen) { 
            lv_scr_load(screen_main);
        }
        lv_obj_del_async(target_adjust_screen);
        target_adjust_screen = NULL;
    }
    cleanup_target_adjust_screen_resources();
}

// --- 目标设置屏幕背景点击事件回调 ---
static void target_adjust_screen_bg_click_event_cb(lv_event_t *e) {
    lv_obj_t * target_obj = lv_event_get_target(e);
    if (target_obj == target_adjust_screen) { // 确保是点击屏幕背景，而不是子控件
        Serial.println("UI_MainScreen: Background of target adjust screen clicked. Closing.");
        close_target_adjust_screen();
    }
}

// --- Slider超时关闭屏幕的定时器回调 ---
static void slider_timeout_close_screen_cb(lv_timer_t *timer) {
    if (target_adjust_screen && lv_obj_is_valid(target_adjust_screen) && lv_scr_act() == target_adjust_screen) {
        if (millis() - last_slider_interaction_time >= SLIDER_AUTO_CLOSE_TIMEOUT_MS) {
            Serial.println("UI_MainScreen: Slider timeout on target adjust screen, closing.");
            close_target_adjust_screen();
        }
    } else { 
        if (slider_timeout_timer) { 
            lv_timer_del(slider_timeout_timer);
            slider_timeout_timer = NULL;
        }
    }
}

// --- +/- 按钮快速调节定时器回调 (逻辑不变) ---
static void adj_button_long_press_timer_cb(lv_timer_t *timer) {
    if (!adj_button_long_press_active) {
        if (adj_button_long_press_timer) { lv_timer_del(adj_button_long_press_timer); adj_button_long_press_timer = NULL; }
        return;
    }
    intptr_t change_multiplier = adj_button_is_plus ? 1 : -1;
    if (current_target_slider && lv_obj_is_valid(current_target_slider) && current_slider_type != TARGET_SETTING_NONE) {
        float current_val_float; int current_val_int;
        switch (current_slider_type) {
            case TARGET_SETTING_TEMP_LEFT: current_val_float = g_target_temperature_left + (change_multiplier * 0.1f); current_val_float = LV_CLAMP(current_val_float, 20.0f, 40.0f); update_slider_and_info_label(current_val_float, current_slider_type); break;
            case TARGET_SETTING_HUMID_LEFT: current_val_int = g_target_humidity_left + (change_multiplier * 1); current_val_int = LV_CLAMP(current_val_int, 0, 100); update_slider_and_info_label((float)current_val_int, current_slider_type); break;
            case TARGET_SETTING_TEMP_RIGHT: current_val_float = g_target_temperature_right + (change_multiplier * 0.1f); current_val_float = LV_CLAMP(current_val_float, 20.0f, 40.0f); update_slider_and_info_label(current_val_float, current_slider_type); break;
            case TARGET_SETTING_HUMID_RIGHT: current_val_int = g_target_humidity_right + (change_multiplier * 1); current_val_int = LV_CLAMP(current_val_int, 0, 100); update_slider_and_info_label((float)current_val_int, current_slider_type); break;
            default: break;
        }
        last_slider_interaction_time = millis();
        if (slider_timeout_timer) { lv_timer_reset(slider_timeout_timer); }
    }
}


// --- +/- 按钮事件回调 ---
static void plus_minus_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_CLICKED && code != LV_EVENT_LONG_PRESSED && code != LV_EVENT_RELEASED && code != LV_EVENT_PRESS_LOST) { 
        return; 
    }
    
    last_slider_interaction_time = millis();
    if (slider_timeout_timer) { 
        lv_timer_reset(slider_timeout_timer); 
    }
    
    intptr_t change_multiplier = (intptr_t)lv_event_get_user_data(e);
    
    if (code == LV_EVENT_CLICKED) {
        if (adj_button_long_press_timer) { 
            lv_timer_del(adj_button_long_press_timer); 
            adj_button_long_press_timer = NULL; 
        }
        adj_button_long_press_active = false;
        
        if (current_target_slider && lv_obj_is_valid(current_target_slider) && current_slider_type != TARGET_SETTING_NONE) {
            float current_val_float; 
            int current_val_int;
            
            switch (current_slider_type) {
                case TARGET_SETTING_TEMP_LEFT: 
                    current_val_float = g_target_temperature_left + (change_multiplier * 0.1f); 
                    current_val_float = LV_CLAMP(current_val_float, 20.0f, 40.0f); 
                    update_slider_and_info_label(current_val_float, current_slider_type); 
                    break;
                case TARGET_SETTING_HUMID_LEFT: 
                    current_val_int = g_target_humidity_left + (change_multiplier * 1); 
                    current_val_int = LV_CLAMP(current_val_int, 0, 100); 
                    update_slider_and_info_label((float)current_val_int, current_slider_type); 
                    break;
                case TARGET_SETTING_TEMP_RIGHT: 
                    current_val_float = g_target_temperature_right + (change_multiplier * 0.1f); 
                    current_val_float = LV_CLAMP(current_val_float, 20.0f, 40.0f); 
                    update_slider_and_info_label(current_val_float, current_slider_type); 
                    break;
                case TARGET_SETTING_HUMID_RIGHT: 
                    current_val_int = g_target_humidity_right + (change_multiplier * 1); 
                    current_val_int = LV_CLAMP(current_val_int, 0, 100); 
                    update_slider_and_info_label((float)current_val_int, current_slider_type); 
                    break;
                default: 
                    break;
            }
        }
    } else if (code == LV_EVENT_LONG_PRESSED) {
        adj_button_long_press_active = true; 
        adj_button_is_plus = (change_multiplier == 1);
        if (adj_button_long_press_timer) { 
            lv_timer_del(adj_button_long_press_timer); 
            adj_button_long_press_timer = NULL; 
        }
        adj_button_long_press_timer = lv_timer_create(adj_button_long_press_timer_cb, 100, NULL);
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        adj_button_long_press_active = false;
        if (adj_button_long_press_timer) { 
            lv_timer_del(adj_button_long_press_timer); 
            adj_button_long_press_timer = NULL; 
        }
    }
}


static void target_slider_event_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e); 
    int32_t raw_value = lv_slider_get_value(slider);
    target_setting_type_t type = (target_setting_type_t)(uintptr_t)lv_event_get_user_data(e);
    
    float float_value;
    if (type == TARGET_SETTING_TEMP_LEFT || type == TARGET_SETTING_TEMP_RIGHT) { 
        float_value = (float)raw_value / 10.0f; 
        update_slider_and_info_label(float_value, type); 
    } else { 
        update_slider_and_info_label((float)raw_value, type); 
    }
    
    last_slider_interaction_time = millis();
    if (slider_timeout_timer) { 
        lv_timer_reset(slider_timeout_timer); 
    }
}



// --- 更新Slider和提示标签的函数 (逻辑不变) ---
// 在 UI_MainScreen.cpp 中修改 update_slider_and_info_label 函数
static void update_slider_and_info_label(float value, target_setting_type_t type) {
    if (!current_target_slider || !lv_obj_is_valid(current_target_slider) ||
        !slider_info_label || !lv_obj_is_valid(slider_info_label)) {
        return;
    }
    
    char slider_info_text[50]; 
    const char* zone_prefix = ""; 
    const char* unit = "";
    float current_target_val_float = value; 
    int current_target_val_int = (int)value;
    
    lv_obj_set_style_text_font(slider_info_label, &fengyahei_s24_bpp4, 0);
    
    switch (type) {
        case TARGET_SETTING_TEMP_LEFT: 
            g_target_temperature_left = value; 
            zone_prefix = "左区"; 
            unit = "°C"; 
            current_target_val_float = g_target_temperature_left; 
            lv_slider_set_value(current_target_slider, (int32_t)(g_target_temperature_left * 10), LV_ANIM_OFF); 
            
            // 🔥 新增：同步到温控系统
            ThermalControl_SetTargetTemp(THERMAL_ZONE_LEFT, value);
            // 如果温控系统未启用，自动启用
            if (!ThermalControl_IsEnabled()) {
                ThermalControl_SetEnabled(true);
                Serial.println("UI_MainScreen: 自动启用温控系统（左区温度设置）");
            }
            break;
            
        case TARGET_SETTING_HUMID_LEFT: 
            g_target_humidity_left = (int)value; 
            zone_prefix = "左区"; 
            unit = "%"; 
            current_target_val_int = g_target_humidity_left; 
            lv_slider_set_value(current_target_slider, g_target_humidity_left, LV_ANIM_OFF); 
            break;
            
        case TARGET_SETTING_TEMP_RIGHT: 
            g_target_temperature_right = value; 
            zone_prefix = "右区"; 
            unit = "°C"; 
            current_target_val_float = g_target_temperature_right; 
            lv_slider_set_value(current_target_slider, (int32_t)(g_target_temperature_right * 10), LV_ANIM_OFF); 
            
            // 🔥 新增：同步到温控系统
            ThermalControl_SetTargetTemp(THERMAL_ZONE_RIGHT, value);
            // 如果温控系统未启用，自动启用
            if (!ThermalControl_IsEnabled()) {
                ThermalControl_SetEnabled(true);
                Serial.println("UI_MainScreen: 自动启用温控系统（右区温度设置）");
            }
            break;
            
        case TARGET_SETTING_HUMID_RIGHT: 
            g_target_humidity_right = (int)value; 
            zone_prefix = "右区"; 
            unit = "%"; 
            current_target_val_int = g_target_humidity_right; 
            lv_slider_set_value(current_target_slider, g_target_humidity_right, LV_ANIM_OFF); 
            break;
            
        default: 
            strcpy(slider_info_text, "未知"); 
            lv_label_set_text(slider_info_label, slider_info_text); 
            return;
    }
    
    if (type == TARGET_SETTING_TEMP_LEFT || type == TARGET_SETTING_TEMP_RIGHT) { 
        sprintf(slider_info_text, "%s: %.1f%s", zone_prefix, current_target_val_float, unit); 
    } else { 
        sprintf(slider_info_text, "%s: %d%s", zone_prefix, current_target_val_int, unit); 
    }
    
    lv_label_set_text(slider_info_label, slider_info_text);
    lv_obj_align_to(slider_info_label, current_target_slider, LV_ALIGN_OUT_TOP_MID, 0, -15);
}

// --- 创建目标调节屏幕UI内容的核心函数 ---
static void create_target_adjust_screen_ui(lv_obj_t* screen, target_setting_type_t type_to_set) {
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0); 
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    // 使屏幕背景可点击以关闭
    lv_obj_add_flag(screen, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(screen, target_adjust_screen_bg_click_event_cb, LV_EVENT_CLICKED, NULL);

    current_target_slider = lv_slider_create(screen);
    // MODIFIED: 滑块尺寸调小
    lv_obj_set_size(current_target_slider, 20, lv_disp_get_ver_res(NULL) * 0.60); // 宽度25, 高度屏幕60%
    lv_obj_align_to(current_target_slider, screen, LV_ALIGN_CENTER, 0, 20);                     // y 偏移
    lv_obj_add_event_cb(current_target_slider, target_slider_event_cb, LV_EVENT_VALUE_CHANGED, (void*)type_to_set);
    lv_obj_set_style_bg_color(current_target_slider, PANEL_BG_COLOR, LV_PART_MAIN);
    // MODIFIED: 滑块两端圆弧
    lv_obj_set_style_radius(current_target_slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_radius(current_target_slider, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    
    lv_obj_set_style_opa(current_target_slider, LV_OPA_COVER, LV_PART_KNOB); 
    lv_obj_set_ext_click_area(current_target_slider, 10); 

    current_slider_type = type_to_set;

    slider_info_label = lv_label_create(screen);
    lv_obj_add_style(slider_info_label, &style_text_white_custom_font, 0);
    lv_obj_align_to(slider_info_label, current_target_slider, LV_ALIGN_OUT_TOP_MID, 0, -20); // Y偏移调整

    slider_btn_minus = lv_btn_create(screen);
    lv_obj_remove_style_all(slider_btn_minus);
    lv_obj_set_size(slider_btn_minus, 60, 60); 
    lv_obj_align_to(slider_btn_minus, current_target_slider, LV_ALIGN_LEFT_MID, -70, 0); 
    lv_obj_add_event_cb(slider_btn_minus, plus_minus_btn_event_cb, LV_EVENT_ALL, (void*)-1);
    lv_obj_t *label_minus = lv_label_create(slider_btn_minus);
    lv_label_set_text(label_minus, LV_SYMBOL_MINUS);
    lv_obj_set_style_text_font(label_minus, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label_minus, lv_color_white(), 0);
    lv_obj_center(label_minus);

    slider_btn_plus = lv_btn_create(screen);
    lv_obj_remove_style_all(slider_btn_plus);
    lv_obj_set_size(slider_btn_plus, 60, 60); 
    lv_obj_align_to(slider_btn_plus, current_target_slider, LV_ALIGN_RIGHT_MID, 70, 0); 
    lv_obj_add_event_cb(slider_btn_plus, plus_minus_btn_event_cb, LV_EVENT_ALL, (void*)1);
    lv_obj_t *label_plus = lv_label_create(slider_btn_plus);
    lv_label_set_text(label_plus, LV_SYMBOL_PLUS);
    lv_obj_set_style_text_font(label_plus, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(label_plus, lv_color_white(), 0);
    lv_obj_center(label_plus);

    float initial_float_val = 0; int initial_int_val = 0; lv_color_t indicator_color;
    switch (type_to_set) {
        case TARGET_SETTING_TEMP_LEFT: lv_slider_set_range(current_target_slider, 200, 400); initial_float_val = g_target_temperature_left; indicator_color = calculate_dynamic_temp_color(g_current_temperature); break;
        case TARGET_SETTING_HUMID_LEFT: lv_slider_set_range(current_target_slider, 0, 100); initial_int_val = g_target_humidity_left; indicator_color = HUMID_VALUE_COLOR; break;
        case TARGET_SETTING_TEMP_RIGHT: lv_slider_set_range(current_target_slider, 200, 400); initial_float_val = g_target_temperature_right; indicator_color = calculate_dynamic_temp_color(g_current_temperature_2); break;
        case TARGET_SETTING_HUMID_RIGHT: lv_slider_set_range(current_target_slider, 0, 100); initial_int_val = g_target_humidity_right; indicator_color = HUMID_VALUE_COLOR; break;
        default: close_target_adjust_screen(); return;
    }
    lv_obj_set_style_bg_color(current_target_slider, indicator_color, LV_PART_INDICATOR);
    if (type_to_set == TARGET_SETTING_TEMP_LEFT || type_to_set == TARGET_SETTING_TEMP_RIGHT) { update_slider_and_info_label(initial_float_val, type_to_set); } 
    else { update_slider_and_info_label((float)initial_int_val, type_to_set); }
    lv_obj_set_style_bg_color(current_target_slider, indicator_color, LV_PART_KNOB);
    last_slider_interaction_time = millis();
    if (!slider_timeout_timer) { slider_timeout_timer = lv_timer_create(slider_timeout_close_screen_cb, 500, NULL); } 
    else { lv_timer_reset(slider_timeout_timer); }
}

// --- 打开目标调节屏幕的事件回调 ---
static void open_target_adjust_screen_event_cb(lv_event_t *e) {
    target_setting_type_t type_to_set = (target_setting_type_t)(uintptr_t)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_SHORT_CLICKED) {
        Serial.printf("UI_MainScreen: Meter short clicked to open target adjust screen for type: %d\n", type_to_set);
        if (target_adjust_screen && lv_obj_is_valid(target_adjust_screen)) {
            lv_obj_del(target_adjust_screen); 
            target_adjust_screen = NULL;
        }
        cleanup_target_adjust_screen_resources(); 

        target_adjust_screen = lv_obj_create(NULL); 
        create_target_adjust_screen_ui(target_adjust_screen, type_to_set); 
        lv_scr_load(target_adjust_screen); 
    }
}


// --- 创建仪表盘的函数 (事件绑定已更新) ---
static lv_obj_t* create_styled_meter(
    lv_obj_t* parent_obj, lv_coord_t meter_logic_size,
    const char* unit_text, lv_obj_t** out_value_label,
    int32_t min_val, int32_t max_val, lv_color_t initial_indicator_color,
    const lv_font_t* value_font, const lv_font_t* unit_font,
    lv_meter_indicator_t** out_indic_arc,
    lv_meter_indicator_t* out_glow_inds[],
    target_setting_type_t label_target_type
    ) {
    lv_obj_t* meter = lv_meter_create(parent_obj);
    lv_obj_remove_style_all(meter);
    lv_obj_set_style_bg_opa(meter, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_add_flag(meter, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_add_event_cb(meter, ext_draw_size_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);
    lv_obj_set_size(meter, meter_logic_size, meter_logic_size);
    lv_meter_scale_t* scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_range(meter, scale, min_val, max_val, 270, 135);

    lv_obj_add_flag(meter, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_ext_click_area(meter, 20); 
    lv_obj_add_event_cb(meter, open_target_adjust_screen_event_cb, LV_EVENT_SHORT_CLICKED, (void*)label_target_type);

    lv_color_t tick_color_val = lv_color_white();
    uint16_t fine_tick_total_count; uint16_t current_major_tick_nth;
    lv_coord_t current_label_gap_val;
    lv_coord_t actual_fine_tick_len, actual_fine_tick_width, actual_major_tick_len, actual_major_tick_width;

    if (strcmp(unit_text, "°C") == 0) { fine_tick_total_count = (uint16_t)(ACTUAL_TEMP_MAX - ACTUAL_TEMP_MIN) + 1; current_major_tick_nth = 5; actual_fine_tick_len = 9; actual_fine_tick_width = 4; actual_major_tick_len = 15; actual_major_tick_width = 5; current_label_gap_val = 10; } 
    else if (strcmp(unit_text, "%") == 0) { fine_tick_total_count = (uint16_t)(ACTUAL_HUMID_MAX - ACTUAL_HUMID_MIN) / 5 + 1; current_major_tick_nth = 4; actual_fine_tick_len = 6; actual_fine_tick_width = 2; actual_major_tick_len = 10; actual_major_tick_width = 4; current_label_gap_val = -20; lv_obj_add_event_cb(meter, meter_humidity_label_draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL); } 
    else { fine_tick_total_count = 21; current_major_tick_nth = 5; actual_fine_tick_len = 9; actual_fine_tick_width = 4; actual_major_tick_len = 15; actual_major_tick_width = 5; current_label_gap_val = 10; }
    lv_meter_set_scale_ticks(meter, scale, fine_tick_total_count, actual_fine_tick_width, actual_fine_tick_len, tick_color_val);
    lv_meter_set_scale_major_ticks(meter, scale, current_major_tick_nth, actual_major_tick_width, actual_major_tick_len, tick_color_val, current_label_gap_val);
    lv_obj_set_style_text_color(meter, METER_SCALE_TEXT_COLOR, (uint32_t)LV_PART_TICKS | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(meter, &lv_font_montserrat_14, (uint32_t)LV_PART_TICKS | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(meter, LV_OPA_COVER, (uint32_t)LV_PART_TICKS | (uint32_t)LV_STATE_DEFAULT);
    *out_value_label = lv_label_create(meter);
    lv_obj_add_style(*out_value_label, &style_text_white_custom_font, 0); 
    lv_obj_set_style_text_font(*out_value_label, value_font, 0);
    lv_label_set_text(*out_value_label, "N/A");
    if (strcmp(unit_text, "°C") == 0) { lv_obj_align(*out_value_label, LV_ALIGN_CENTER, 0, -10); lv_obj_add_style(*out_value_label, &style_meter_temp_value_text, 0); } 
    else if (strcmp(unit_text, "%") == 0) { lv_obj_align(*out_value_label, LV_ALIGN_CENTER, 0, -5); lv_label_set_long_mode(*out_value_label, LV_LABEL_LONG_CLIP); lv_obj_add_style(*out_value_label, &style_meter_humid_value_text, 0); } 
    else { lv_obj_center(*out_value_label); }
    lv_obj_add_flag(*out_value_label, LV_OBJ_FLAG_CLICKABLE); 
    lv_obj_add_flag(*out_value_label, LV_OBJ_FLAG_EVENT_BUBBLE); 
    if (unit_text && unit_font) {
        lv_obj_t* unit_label = lv_label_create(meter);
        lv_obj_add_style(unit_label, &style_text_white_custom_font, 0); 
        lv_obj_set_style_text_font(unit_label, unit_font, 0);
        lv_label_set_text(unit_label, unit_text);
        int unit_y_offset = 5; 
        if (strcmp(unit_text, "°C") == 0) unit_y_offset = 2; 
        else if (strcmp(unit_text, "%") == 0) unit_y_offset = 2; 
        lv_obj_align_to(unit_label, *out_value_label, LV_ALIGN_OUT_BOTTOM_MID, 0, unit_y_offset);
    }
    *out_indic_arc = lv_meter_add_arc(meter, scale, MAIN_ARC_BASE_WIDTH, initial_indicator_color, 0);
    if (*out_indic_arc) { lv_meter_set_indicator_start_value(meter, *out_indic_arc, min_val); lv_meter_set_indicator_end_value(meter,   *out_indic_arc, min_val); (*out_indic_arc)->opa = LV_OPA_COVER; }
    if (out_glow_inds != NULL) {
        for (int i = 0; i < GLOW_LAYERS_COUNT; i++) { // GLOW_LAYERS_COUNT is now 2
            lv_coord_t current_glow_arc_width = MAIN_ARC_BASE_WIDTH + GLOW_WIDTH_INCREASE[i];
            lv_coord_t r_offset = GLOW_RADIUS_OFFSET[i];
            out_glow_inds[i] = lv_meter_add_arc(meter, scale, current_glow_arc_width, initial_indicator_color, r_offset);
            if (out_glow_inds[i]) { lv_meter_set_indicator_start_value(meter, out_glow_inds[i], min_val); lv_meter_set_indicator_end_value(meter, out_glow_inds[i], min_val); out_glow_inds[i]->opa = GLOW_OPA_VALUES[i]; }
        }
    }
    return meter;
}

// --- create_meter_group, create_status_scroll_bar, create_main_ui 函数与您提供的版本一致 ---
static void create_meter_group(lv_obj_t* group_parent, bool left_side) {
    lv_coord_t W_group = lv_obj_get_content_width(group_parent);
    lv_coord_t H_group = lv_obj_get_content_height(group_parent);
    lv_coord_t meter_logic_size_temp = W_group;
    meter_logic_size_temp = std::min(meter_logic_size_temp, (lv_coord_t)(H_group * 0.85f));
    meter_logic_size_temp = std::max(meter_logic_size_temp, (lv_coord_t)180);
    lv_coord_t meter_logic_size_humid = meter_logic_size_temp * 0.50f;
    meter_logic_size_humid = std::max(meter_logic_size_humid, (lv_coord_t)70);
    lv_coord_t temp_meter_total_h = meter_logic_size_temp + 20;
    lv_coord_t humid_meter_total_h = meter_logic_size_humid + 20;
    lv_coord_t vertical_spacing = -62;
    lv_coord_t total_needed_height = temp_meter_total_h + humid_meter_total_h + vertical_spacing;
    lv_coord_t start_y = (H_group - total_needed_height) / 2;
    if (start_y < 5) start_y = 5;
    lv_coord_t temp_meter_y = start_y;
    lv_coord_t humid_meter_y = temp_meter_y + temp_meter_total_h + vertical_spacing;
    lv_coord_t temp_meter_x = (W_group - meter_logic_size_temp) / 2;
    lv_coord_t humid_meter_x = (W_group - meter_logic_size_humid) / 2;
    lv_color_t initial_temp_indic_color = calculate_dynamic_temp_color(ACTUAL_TEMP_MIN);
    lv_color_t humid_indic_color = HUMID_VALUE_COLOR; 
    const lv_font_t* temp_val_font = &lv_font_digital_28;
    const lv_font_t* humid_val_font = &lv_font_digital_24;
    const lv_font_t* temp_unit_font = &lv_font_montserrat_26; 
    const lv_font_t* humid_unit_font = &lv_font_montserrat_20;
    if(left_side) {
        meter_temp_left  = create_styled_meter(group_parent, meter_logic_size_temp,  "°C", &label_value_temp_left, (int32_t)ACTUAL_TEMP_MIN, (int32_t)ACTUAL_TEMP_MAX, initial_temp_indic_color, temp_val_font, temp_unit_font, &indic_arc_temp_left, glow_inds_temp_left, TARGET_SETTING_TEMP_LEFT);
        lv_obj_set_pos(meter_temp_left, temp_meter_x, temp_meter_y);
        meter_humid_left = create_styled_meter(group_parent, meter_logic_size_humid, "%", &label_value_humid_left, (int32_t)ACTUAL_HUMID_MIN, (int32_t)ACTUAL_HUMID_MAX, humid_indic_color, humid_val_font, humid_unit_font, &indic_arc_humid_left, glow_inds_humid_left, TARGET_SETTING_HUMID_LEFT);
        lv_obj_set_pos(meter_humid_left, humid_meter_x, humid_meter_y);
    } else {
        meter_temp_right = create_styled_meter(group_parent,  meter_logic_size_temp,  "°C", &label_value_temp_right, (int32_t)ACTUAL_TEMP_MIN, (int32_t)ACTUAL_TEMP_MAX, initial_temp_indic_color, temp_val_font, temp_unit_font, &indic_arc_temp_right, glow_inds_temp_right, TARGET_SETTING_TEMP_RIGHT); 
        lv_obj_set_pos(meter_temp_right, temp_meter_x, temp_meter_y);
        meter_humid_right = create_styled_meter(group_parent, meter_logic_size_humid, "%", &label_value_humid_right, (int32_t)ACTUAL_HUMID_MIN, (int32_t)ACTUAL_HUMID_MAX, humid_indic_color, humid_val_font, humid_unit_font, &indic_arc_humid_right, glow_inds_humid_right, TARGET_SETTING_HUMID_RIGHT); 
        lv_obj_set_pos(meter_humid_right, humid_meter_x, humid_meter_y);
    }
}

void create_status_scroll_bar(lv_obj_t* parent) {
    if (!parent) { Serial.println("UI_MainScreen: Parent object is NULL in create_status_scroll_bar."); return; }
    label_top_scrolling_info = lv_label_create(parent);
    if (!label_top_scrolling_info) { Serial.println("UI_MainScreen: FAILED to create label_top_scrolling_info!"); return; }
    lv_obj_set_width(label_top_scrolling_info, lv_disp_get_hor_res(NULL));
    lv_obj_set_height(label_top_scrolling_info, STATUS_BAR_HEIGHT); 
    lv_label_set_long_mode(label_top_scrolling_info, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_bg_color(label_top_scrolling_info, PANEL_BG_COLOR, 0); 
    lv_obj_set_style_bg_opa(label_top_scrolling_info, LV_OPA_COVER, 0);   
    if (style_text_white_custom_font.prop_cnt > 0) { lv_obj_add_style(label_top_scrolling_info, &style_text_white_custom_font, 0); } 
    else { Serial.println("UI_MainScreen: WARN - style_text_white_custom_font not initialized, using fallback for scrollbar."); lv_obj_set_style_text_font(label_top_scrolling_info, &fengyahei_s16_bpp4, 0); lv_obj_set_style_text_color(label_top_scrolling_info, TEXT_COLOR_WHITE, 0); }
    lv_obj_set_style_text_align(label_top_scrolling_info, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label_top_scrolling_info, LV_ALIGN_TOP_MID, 0, 0);
    const lv_font_t* scroll_font = lv_obj_get_style_text_font(label_top_scrolling_info, LV_PART_MAIN);
    lv_coord_t font_line_height = lv_font_get_line_height(scroll_font);
    lv_coord_t pad_ver = (STATUS_BAR_HEIGHT - font_line_height) / 2;
    if (pad_ver < 0) pad_ver = 0;
    lv_obj_set_style_pad_top(label_top_scrolling_info, pad_ver, 0);
    lv_obj_set_style_pad_bottom(label_top_scrolling_info, pad_ver, 0);
    refresh_top_bar_info();
    Serial.println("UI_MainScreen: Status scroll bar created.");
}

void create_main_ui(lv_obj_t *scr) {
    Serial.println("UI_MainScreen: create_main_ui START");
    lv_obj_remove_style_all(scr);
    if(style_scr_bg.prop_cnt > 0) { lv_obj_add_style(scr, &style_scr_bg, 0); } 
    else { lv_obj_set_style_bg_color(scr, SCREEN_BG_COLOR, 0); lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0); }
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    create_status_scroll_bar(scr); 
    const lv_coord_t W = lv_disp_get_hor_res(NULL);
    const lv_coord_t H_meter_area = lv_disp_get_ver_res(NULL) - STATUS_BAR_HEIGHT - NAV_BAR_HEIGHT; 
    lv_obj_t* meter_area = lv_obj_create(scr);
    lv_obj_remove_style_all(meter_area);
    lv_obj_set_style_bg_color(meter_area, lv_color_black(), (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(meter_area, LV_OPA_COVER, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(meter_area, 240, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(meter_area, 120, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_clear_flag(meter_area, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(meter_area, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_add_event_cb(meter_area, ext_draw_size_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);
    lv_obj_set_size(meter_area, W, H_meter_area);
    lv_obj_align(meter_area, LV_ALIGN_TOP_LEFT, 0, STATUS_BAR_HEIGHT);
    lv_obj_set_layout(meter_area, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(meter_area, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(meter_area, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); 
    lv_obj_set_style_pad_column(meter_area, 120, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT); 
    lv_obj_t* left_grp = lv_obj_create(meter_area);
    lv_obj_remove_style_all(left_grp); lv_obj_set_style_bg_opa(left_grp, LV_OPA_TRANSP, 0); lv_obj_set_style_pad_all(left_grp, 0, 0); lv_obj_add_flag(left_grp, LV_OBJ_FLAG_OVERFLOW_VISIBLE); lv_obj_clear_flag(left_grp, LV_OBJ_FLAG_SCROLLABLE); lv_obj_add_event_cb(left_grp, ext_draw_size_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL); lv_obj_set_width(left_grp, LV_PCT(50)); lv_obj_set_height(left_grp, LV_PCT(100)); create_meter_group(left_grp, true);
    lv_obj_t* right_grp = lv_obj_create(meter_area);
    lv_obj_remove_style_all(right_grp); lv_obj_set_style_bg_opa(right_grp, LV_OPA_TRANSP, 0); lv_obj_set_style_pad_all(right_grp, 0, 0); lv_obj_add_flag(right_grp, LV_OBJ_FLAG_OVERFLOW_VISIBLE); lv_obj_clear_flag(right_grp, LV_OBJ_FLAG_SCROLLABLE); lv_obj_add_event_cb(right_grp, ext_draw_size_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL); lv_obj_set_width(right_grp, LV_PCT(50)); lv_obj_set_height(right_grp, LV_PCT(100)); create_meter_group(right_grp, false);
    create_common_bottom_nav(scr, SCREEN_ID_MAIN); 
    if (label_top_scrolling_info && lv_obj_is_valid(label_top_scrolling_info)) { lv_obj_move_foreground(label_top_scrolling_info); }
    Serial.println("UI_MainScreen: create_main_ui END (Full UI with black meter area).");
}

// update_temperature_humidity_displays 函数逻辑保持不变
void update_temperature_humidity_displays(float temp1, int humid1, float temp2, int humid2) {
    char buf[32]; lv_color_t new_temp_color_left, new_temp_color_right; lv_color_t humid_color_fixed = HUMID_VALUE_COLOR; 
    new_temp_color_left = calculate_dynamic_temp_color(temp1); new_temp_color_right = calculate_dynamic_temp_color(temp2);
    if (meter_temp_left && lv_obj_is_valid(meter_temp_left) && label_value_temp_left && lv_obj_is_valid(label_value_temp_left)) {
        if (indic_arc_temp_left) { lv_meter_set_indicator_end_value(meter_temp_left, indic_arc_temp_left, (int32_t)temp1); if (indic_arc_temp_left->type == LV_METER_INDICATOR_TYPE_ARC) { indic_arc_temp_left->type_data.arc.color = new_temp_color_left; } }
        for (int i = 0; i < GLOW_LAYERS_COUNT; i++) { if (glow_inds_temp_left[i]) { lv_meter_set_indicator_end_value(meter_temp_left, glow_inds_temp_left[i], (int32_t)temp1); if (glow_inds_temp_left[i]->type == LV_METER_INDICATOR_TYPE_ARC) { glow_inds_temp_left[i]->type_data.arc.color = new_temp_color_left; } } }
        lv_obj_invalidate(meter_temp_left); snprintf(buf, sizeof(buf), "%.1f", temp1); lv_label_set_text(label_value_temp_left, buf); lv_obj_set_style_text_color(label_value_temp_left, new_temp_color_left, 0);
    }
    if (meter_humid_left && lv_obj_is_valid(meter_humid_left) && label_value_humid_left && lv_obj_is_valid(label_value_humid_left)) {
        if (indic_arc_humid_left) { lv_meter_set_indicator_end_value(meter_humid_left, indic_arc_humid_left, (int32_t)humid1); if (indic_arc_humid_left->type == LV_METER_INDICATOR_TYPE_ARC) { indic_arc_humid_left->type_data.arc.color = humid_color_fixed; } }
        for (int i = 0; i < GLOW_LAYERS_COUNT; i++) { if (glow_inds_humid_left[i]) { lv_meter_set_indicator_end_value(meter_humid_left, glow_inds_humid_left[i], (int32_t)humid1); if (glow_inds_humid_left[i]->type == LV_METER_INDICATOR_TYPE_ARC) { glow_inds_humid_left[i]->type_data.arc.color = humid_color_fixed; } } }
        lv_obj_invalidate(meter_humid_left); snprintf(buf, sizeof(buf), "%d", humid1); lv_label_set_text(label_value_humid_left, buf);
    }
    if (meter_temp_right && lv_obj_is_valid(meter_temp_right) && label_value_temp_right && lv_obj_is_valid(label_value_temp_right)) {
        if (indic_arc_temp_right) { lv_meter_set_indicator_end_value(meter_temp_right, indic_arc_temp_right, (int32_t)temp2); if (indic_arc_temp_right->type == LV_METER_INDICATOR_TYPE_ARC) { indic_arc_temp_right->type_data.arc.color = new_temp_color_right; } }
        for (int i = 0; i < GLOW_LAYERS_COUNT; i++) { if (glow_inds_temp_right[i]) { lv_meter_set_indicator_end_value(meter_temp_right, glow_inds_temp_right[i], (int32_t)temp2); if (glow_inds_temp_right[i]->type == LV_METER_INDICATOR_TYPE_ARC) { glow_inds_temp_right[i]->type_data.arc.color = new_temp_color_right; } } }
        lv_obj_invalidate(meter_temp_right); snprintf(buf, sizeof(buf), "%.1f", temp2); lv_label_set_text(label_value_temp_right, buf); lv_obj_set_style_text_color(label_value_temp_right, new_temp_color_right, 0);
    }
    if (meter_humid_right && lv_obj_is_valid(meter_humid_right) && label_value_humid_right && lv_obj_is_valid(label_value_humid_right)) {
        if (indic_arc_humid_right) { lv_meter_set_indicator_end_value(meter_humid_right, indic_arc_humid_right, (int32_t)humid2); if (indic_arc_humid_right->type == LV_METER_INDICATOR_TYPE_ARC) { indic_arc_humid_right->type_data.arc.color = humid_color_fixed; } }
        for (int i = 0; i < GLOW_LAYERS_COUNT; i++) { if (glow_inds_humid_right[i]) { lv_meter_set_indicator_end_value(meter_humid_right, glow_inds_humid_right[i], (int32_t)humid2); if (glow_inds_humid_right[i]->type == LV_METER_INDICATOR_TYPE_ARC) { glow_inds_humid_right[i]->type_data.arc.color = humid_color_fixed; } } }
        lv_obj_invalidate(meter_humid_right); snprintf(buf, sizeof(buf), "%d", humid2); lv_label_set_text(label_value_humid_right, buf);
    }
}

void update_top_scrolling_info_label(const char *info_str) {
    if (label_top_scrolling_info && lv_obj_is_valid(label_top_scrolling_info)) {
       lv_label_set_text(label_top_scrolling_info, info_str);
    }
}

void refresh_top_bar_info() {
    if (!label_top_scrolling_info || !lv_obj_is_valid(label_top_scrolling_info)) return;
    char combined_info_buffer[256]; char temp_status_part[100];
    const char* date_str = (strlen(g_current_date_str) > 3 && strcmp(g_current_date_str, "---- -- --") != 0) ? g_current_date_str : g_current_date_str;
    const char* time_str = (strlen(g_current_time_str) > 3 && strcmp(g_current_time_str, "--:--:--") != 0) ? g_current_time_str : g_current_time_str;
    const char* work_status = (strlen(g_work_status_str) > 0) ? g_work_status_str : "工作状态：保温中...";
    snprintf(temp_status_part, sizeof(temp_status_part), "%s  %s  %s", date_str, time_str, work_status); 
    snprintf(combined_info_buffer, sizeof(combined_info_buffer), "%s            %s            %s", temp_status_part, temp_status_part, temp_status_part);
    update_top_scrolling_info_label(combined_info_buffer);
}
// File: src/Display/Popups/UI_ThermalSettingsPopup.cpp
#include "UI_ThermalSettingsPopup.h"
#include "UI_PopupBase.h"
#include "../UI_Styles.h"
#include "../font/fengyahei_s16_bpp4.h"
#include "../../Config/Config.h"
#include "../../App/AppGlobal.h"
#include <Arduino.h>
#include <stdio.h>

static lv_obj_t *thermal_timed_enable_switch = NULL;
static lv_obj_t *thermal_countdown_hour_roller = NULL;
static lv_obj_t *thermal_countdown_minute_roller = NULL;
static lv_obj_t *thermal_countdown_second_roller = NULL;
static lv_obj_t *thermal_countdown_rollers_container = NULL;
static lv_obj_t *warning_label_thermal = NULL;

static char thermal_hour_roller_options[24 * 4];
static char thermal_minute_roller_options[60 * 4];
static char thermal_second_roller_options[60 * 4];

// 简单的定时配置（参考杀菌模块）
static timed_operation_config_t thermal_timed_config = {false, 0, 30, 0}; // 默认30分钟

static void build_thermal_roller_options() {
    // 构建小时选项 (00-23)
    thermal_hour_roller_options[0] = '\0';
    char *p_hour = thermal_hour_roller_options;
    for (int i = 0; i < 24; i++) {
        p_hour += sprintf(p_hour, "%02u", i);
        if (i < 23) *(p_hour++) = '\n';
    }
    *p_hour = '\0';
    
    // 构建分钟选项 (00-59)
    thermal_minute_roller_options[0] = '\0';
    char *p_min = thermal_minute_roller_options;
    for (int i = 0; i < 60; i++) {
        p_min += sprintf(p_min, "%02u", i);
        if (i < 59) *(p_min++) = '\n';
    }
    *p_min = '\0';
    
    // 构建秒钟选项 (00-59)
    thermal_second_roller_options[0] = '\0';
    char *p_sec = thermal_second_roller_options;
    for (int i = 0; i < 60; i++) {
        p_sec += sprintf(p_sec, "%02u", i);
        if (i < 59) *(p_sec++) = '\n';
    }
    *p_sec = '\0';
}

static void thermal_popup_delete_cleanup_handler(lv_event_t *e) { 
    thermal_timed_enable_switch = NULL; 
    thermal_countdown_hour_roller = NULL; 
    thermal_countdown_minute_roller = NULL; 
    thermal_countdown_second_roller = NULL; 
    thermal_countdown_rollers_container = NULL; 
    warning_label_thermal = NULL;
}

static void thermal_timed_enable_switch_event_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool timed_enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    thermal_timed_config.enabled = timed_enabled;
    
    if (thermal_countdown_rollers_container) {
        if (timed_enabled) {
            lv_obj_clear_flag(thermal_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
            if(warning_label_thermal) lv_obj_clear_flag(warning_label_thermal, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(thermal_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
            if(warning_label_thermal) lv_obj_add_flag(warning_label_thermal, LV_OBJ_FLAG_HIDDEN);
        }
    }
    
    Serial.printf("Thermal timed operation: %s\n", timed_enabled ? "Enabled" : "Disabled");
}

static void thermal_countdown_roller_event_cb(lv_event_t *e) {
    if (thermal_timed_config.enabled) {
        if (lv_obj_is_valid(thermal_countdown_hour_roller))
            thermal_timed_config.countdown_hour = lv_roller_get_selected(thermal_countdown_hour_roller);
        if (lv_obj_is_valid(thermal_countdown_minute_roller))
            thermal_timed_config.countdown_minute = lv_roller_get_selected(thermal_countdown_minute_roller);
        if (lv_obj_is_valid(thermal_countdown_second_roller))
            thermal_timed_config.countdown_second = lv_roller_get_selected(thermal_countdown_second_roller);
        
        Serial.printf("Thermal timer set to: %02u:%02u:%02u\n", 
                      thermal_timed_config.countdown_hour,
                      thermal_timed_config.countdown_minute,
                      thermal_timed_config.countdown_second);
    }
}

void create_thermal_settings_popup(lv_obj_t *parent_scr) {
    build_thermal_roller_options();
    lv_obj_t *content_area = create_popup_frame(parent_scr, "保温定时");
    if (!content_area) return;

    lv_obj_t *popup_box = lv_obj_get_parent(content_area);
    lv_obj_t *popup_mask = lv_obj_get_parent(popup_box);
    if (popup_mask) {
        lv_obj_add_flag(popup_mask, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(popup_mask, thermal_popup_delete_cleanup_handler, LV_EVENT_DELETE, NULL);
    }

    // --- 定时保温设置区域 ---
    lv_obj_t *timed_section_cont = lv_obj_create(content_area);
    lv_obj_remove_style_all(timed_section_cont);
    lv_obj_set_width(timed_section_cont, LV_PCT(100));
    lv_obj_set_height(timed_section_cont, LV_SIZE_CONTENT);
    lv_obj_set_layout(timed_section_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(timed_section_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(timed_section_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(timed_section_cont, 8, 0);

    // 定时开关行
    lv_obj_t *timed_enable_row = lv_obj_create(timed_section_cont);
    lv_obj_remove_style_all(timed_enable_row);
    lv_obj_set_width(timed_enable_row, LV_PCT(100));
    lv_obj_set_height(timed_enable_row, LV_SIZE_CONTENT);
    lv_obj_set_layout(timed_enable_row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(timed_enable_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(timed_enable_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *timed_enable_label = lv_label_create(timed_enable_row);
    lv_label_set_text(timed_enable_label, "启用定时关闭:");
    lv_obj_add_style(timed_enable_label, &style_text_white_custom_font, 0);

    thermal_timed_enable_switch = lv_switch_create(timed_enable_row);
    if (thermal_timed_config.enabled) 
        lv_obj_add_state(thermal_timed_enable_switch, LV_STATE_CHECKED);
    lv_obj_add_event_cb(thermal_timed_enable_switch, thermal_timed_enable_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_style(thermal_timed_enable_switch, &style_switch_on_cyan, LV_PART_INDICATOR | LV_STATE_CHECKED);

    // 时间选择滚轮容器
    thermal_countdown_rollers_container = lv_obj_create(timed_section_cont);
    lv_obj_remove_style_all(thermal_countdown_rollers_container);
    lv_obj_set_width(thermal_countdown_rollers_container, LV_PCT(100));
    lv_obj_set_height(thermal_countdown_rollers_container, LV_SIZE_CONTENT);
    lv_obj_set_layout(thermal_countdown_rollers_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(thermal_countdown_rollers_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(thermal_countdown_rollers_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(thermal_countdown_rollers_container, 8, 0);
    lv_obj_set_style_pad_top(thermal_countdown_rollers_container, 5, 0);

    // 小时滚轮
    thermal_countdown_hour_roller = lv_roller_create(thermal_countdown_rollers_container);
    lv_obj_set_width(thermal_countdown_hour_roller, LV_PCT(25));
    lv_roller_set_options(thermal_countdown_hour_roller, thermal_hour_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(thermal_countdown_hour_roller, 3);
    lv_obj_add_style(thermal_countdown_hour_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(thermal_countdown_hour_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(thermal_countdown_hour_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(thermal_countdown_hour_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(thermal_countdown_hour_roller, 1, 0);
    lv_obj_set_style_radius(thermal_countdown_hour_roller, 4, 0);
    lv_obj_set_style_text_color(thermal_countdown_hour_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(thermal_countdown_hour_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_set_style_text_font(thermal_countdown_hour_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);
    lv_obj_add_event_cb(thermal_countdown_hour_roller, thermal_countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 冒号1
    lv_obj_t* colon1_t = lv_label_create(thermal_countdown_rollers_container);
    lv_label_set_text(colon1_t, ":");
    lv_obj_add_style(colon1_t, &style_text_white_custom_font, 0);

    // 分钟滚轮
    thermal_countdown_minute_roller = lv_roller_create(thermal_countdown_rollers_container);
    lv_obj_set_width(thermal_countdown_minute_roller, LV_PCT(25));
    lv_roller_set_options(thermal_countdown_minute_roller, thermal_minute_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(thermal_countdown_minute_roller, 3);
    lv_obj_add_style(thermal_countdown_minute_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(thermal_countdown_minute_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(thermal_countdown_minute_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(thermal_countdown_minute_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(thermal_countdown_minute_roller, 1, 0);
    lv_obj_set_style_radius(thermal_countdown_minute_roller, 4, 0);
    lv_obj_set_style_text_color(thermal_countdown_minute_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(thermal_countdown_minute_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_set_style_text_font(thermal_countdown_minute_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);
    lv_obj_add_event_cb(thermal_countdown_minute_roller, thermal_countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 冒号2
    lv_obj_t* colon2_t = lv_label_create(thermal_countdown_rollers_container);
    lv_label_set_text(colon2_t, ":");
    lv_obj_add_style(colon2_t, &style_text_white_custom_font, 0);

    // 秒钟滚轮
    thermal_countdown_second_roller = lv_roller_create(thermal_countdown_rollers_container);
    lv_obj_set_width(thermal_countdown_second_roller, LV_PCT(25));
    lv_roller_set_options(thermal_countdown_second_roller, thermal_second_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(thermal_countdown_second_roller, 3);
    lv_obj_add_style(thermal_countdown_second_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(thermal_countdown_second_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(thermal_countdown_second_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(thermal_countdown_second_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(thermal_countdown_second_roller, 1, 0);
    lv_obj_set_style_radius(thermal_countdown_second_roller, 4, 0);
    lv_obj_set_style_text_color(thermal_countdown_second_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(thermal_countdown_second_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_set_style_text_font(thermal_countdown_second_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);
    lv_obj_add_event_cb(thermal_countdown_second_roller, thermal_countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 安全提示标签
    warning_label_thermal = lv_label_create(content_area);
    lv_label_set_text(warning_label_thermal, "⚠️ 提示：定时关闭功能将在设定时间后自动关闭加热器和风扇。");
    lv_obj_add_style(warning_label_thermal, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_color(warning_label_thermal, lv_palette_main(LV_PALETTE_ORANGE), 0); 
    lv_obj_set_width(warning_label_thermal, LV_PCT(100));
    lv_obj_set_style_pad_top(warning_label_thermal, 15, 0);
    lv_label_set_long_mode(warning_label_thermal, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(warning_label_thermal, LV_TEXT_ALIGN_CENTER, 0);

    // 初始化滚轮值和可见性
    lv_roller_set_selected(thermal_countdown_hour_roller, thermal_timed_config.countdown_hour, LV_ANIM_OFF);
    lv_roller_set_selected(thermal_countdown_minute_roller, thermal_timed_config.countdown_minute, LV_ANIM_OFF);
    lv_roller_set_selected(thermal_countdown_second_roller, thermal_timed_config.countdown_second, LV_ANIM_OFF);
    
    if (!thermal_timed_config.enabled) {
        lv_obj_add_flag(thermal_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
        if(warning_label_thermal) lv_obj_add_flag(warning_label_thermal, LV_OBJ_FLAG_HIDDEN);
    }
}
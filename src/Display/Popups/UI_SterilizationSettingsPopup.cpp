// src/Display/Popups/UI_SterilizationSettingsPopup.cpp
#include "UI_SterilizationSettingsPopup.h"
#include "UI_PopupBase.h"
#include "../UI_Styles.h"
#include "../font/fengyahei_s16_bpp4.h"
#include "../../Config/Config.h"
#include "../../SettingsLogic/SterilizationSettings.h"
#include "../../App/AppGlobal.h" 
#include <Arduino.h>
#include <stdio.h>

static lv_obj_t *sterilization_timed_enable_switch = NULL;
static lv_obj_t *sterilization_countdown_hour_roller = NULL;
static lv_obj_t *sterilization_countdown_minute_roller = NULL;
static lv_obj_t *sterilization_countdown_second_roller = NULL;
static lv_obj_t *sterilization_countdown_rollers_container = NULL;
static lv_obj_t *warning_label_sterilization = NULL;

static char sterilization_hour_roller_options[24 * 4];
static char sterilization_minute_roller_options[60 * 4];
static char sterilization_second_roller_options[60 * 4];

static void build_sterilization_roller_options() {
    // 构建小时选项 (00-23)
    sterilization_hour_roller_options[0] = '\0';
    char *p_hour = sterilization_hour_roller_options;
    for (int i = 0; i < 24; i++) {
        p_hour += sprintf(p_hour, "%02u", i);
        if (i < 23) *(p_hour++) = '\n';
    }
    *p_hour = '\0';
    
    // 构建分钟选项 (00-59)
    sterilization_minute_roller_options[0] = '\0';
    char *p_min = sterilization_minute_roller_options;
    for (int i = 0; i < 60; i++) {
        p_min += sprintf(p_min, "%02u", i);
        if (i < 59) *(p_min++) = '\n';
    }
    *p_min = '\0';
    
    // 构建秒钟选项 (00-59)
    sterilization_second_roller_options[0] = '\0';
    char *p_sec = sterilization_second_roller_options;
    for (int i = 0; i < 60; i++) {
        p_sec += sprintf(p_sec, "%02u", i);
        if (i < 59) *(p_sec++) = '\n';
    }
    *p_sec = '\0';
}

static void sterilization_popup_delete_cleanup_handler(lv_event_t *e) { 
    sterilization_timed_enable_switch = NULL; 
    sterilization_countdown_hour_roller = NULL; 
    sterilization_countdown_minute_roller = NULL; 
    sterilization_countdown_second_roller = NULL; 
    sterilization_countdown_rollers_container = NULL; 
    warning_label_sterilization = NULL;
}

static void sterilization_timed_enable_switch_event_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool timed_enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    timed_operation_config_t current_cfg = *SterilizationSettings_GetTimedOperationSettings();
    current_cfg.enabled = timed_enabled;
    SterilizationSettings_SetTimedOperation(&current_cfg);
    
    if (sterilization_countdown_rollers_container) {
        if (timed_enabled) {
            lv_obj_clear_flag(sterilization_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
            if(warning_label_sterilization) lv_obj_clear_flag(warning_label_sterilization, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(sterilization_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
            if(warning_label_sterilization) lv_obj_add_flag(warning_label_sterilization, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void sterilization_countdown_roller_event_cb(lv_event_t *e) {
    const timed_operation_config_t* p_current_cfg = SterilizationSettings_GetTimedOperationSettings();
    if(!p_current_cfg) return;
    
    timed_operation_config_t current_cfg = *p_current_cfg;
    if (current_cfg.enabled) {
        if (lv_obj_is_valid(sterilization_countdown_hour_roller))
            current_cfg.countdown_hour = lv_roller_get_selected(sterilization_countdown_hour_roller);
        if (lv_obj_is_valid(sterilization_countdown_minute_roller))
            current_cfg.countdown_minute = lv_roller_get_selected(sterilization_countdown_minute_roller);
        if (lv_obj_is_valid(sterilization_countdown_second_roller))
            current_cfg.countdown_second = lv_roller_get_selected(sterilization_countdown_second_roller);
        
        SterilizationSettings_SetTimedOperation(&current_cfg);
    }
}

void create_sterilization_settings_popup(lv_obj_t *parent_scr) {
    build_sterilization_roller_options();
    lv_obj_t *content_area = create_popup_frame(parent_scr, "杀菌设置");
    if (!content_area) return;

    lv_obj_t *popup_box = lv_obj_get_parent(content_area);
    lv_obj_t *popup_mask = lv_obj_get_parent(popup_box);
    if (popup_mask) {
        lv_obj_add_flag(popup_mask, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(popup_mask, sterilization_popup_delete_cleanup_handler, LV_EVENT_DELETE, NULL);
    }

    // --- 定时杀菌设置区域 ---
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
    lv_label_set_text(timed_enable_label, "启用定时杀菌:");
    lv_obj_add_style(timed_enable_label, &style_text_white_custom_font, 0);

    sterilization_timed_enable_switch = lv_switch_create(timed_enable_row);
    const timed_operation_config_t* current_timed_cfg = SterilizationSettings_GetTimedOperationSettings();
    if (current_timed_cfg && current_timed_cfg->enabled) 
        lv_obj_add_state(sterilization_timed_enable_switch, LV_STATE_CHECKED);
    lv_obj_add_event_cb(sterilization_timed_enable_switch, sterilization_timed_enable_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_style(sterilization_timed_enable_switch, &style_switch_on_cyan, (uint32_t)LV_PART_INDICATOR | (uint32_t)LV_STATE_CHECKED);

    // 时间选择滚轮容器
    sterilization_countdown_rollers_container = lv_obj_create(timed_section_cont);
    lv_obj_remove_style_all(sterilization_countdown_rollers_container);
    lv_obj_set_width(sterilization_countdown_rollers_container, LV_PCT(100));
    lv_obj_set_height(sterilization_countdown_rollers_container, LV_SIZE_CONTENT);
    lv_obj_set_layout(sterilization_countdown_rollers_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(sterilization_countdown_rollers_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(sterilization_countdown_rollers_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(sterilization_countdown_rollers_container, 8, 0);
    lv_obj_set_style_pad_top(sterilization_countdown_rollers_container, 5, 0);

    // 小时滚轮
    sterilization_countdown_hour_roller = lv_roller_create(sterilization_countdown_rollers_container);
    lv_obj_set_width(sterilization_countdown_hour_roller, LV_PCT(25));
    lv_roller_set_options(sterilization_countdown_hour_roller, sterilization_hour_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(sterilization_countdown_hour_roller, 3);
    lv_obj_add_style(sterilization_countdown_hour_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(sterilization_countdown_hour_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(sterilization_countdown_hour_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(sterilization_countdown_hour_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(sterilization_countdown_hour_roller, 1, 0);
    lv_obj_set_style_radius(sterilization_countdown_hour_roller, 4, 0);
    lv_obj_set_style_text_color(sterilization_countdown_hour_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(sterilization_countdown_hour_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_set_style_text_font(sterilization_countdown_hour_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);
    lv_obj_add_event_cb(sterilization_countdown_hour_roller, sterilization_countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 冒号1
    lv_obj_t* colon1_s = lv_label_create(sterilization_countdown_rollers_container);
    lv_label_set_text(colon1_s, ":");
    lv_obj_add_style(colon1_s, &style_text_white_custom_font, 0);

    // 分钟滚轮
    sterilization_countdown_minute_roller = lv_roller_create(sterilization_countdown_rollers_container);
    lv_obj_set_width(sterilization_countdown_minute_roller, LV_PCT(25));
    lv_roller_set_options(sterilization_countdown_minute_roller, sterilization_minute_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(sterilization_countdown_minute_roller, 3);
    lv_obj_add_style(sterilization_countdown_minute_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(sterilization_countdown_minute_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(sterilization_countdown_minute_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(sterilization_countdown_minute_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(sterilization_countdown_minute_roller, 1, 0);
    lv_obj_set_style_radius(sterilization_countdown_minute_roller, 4, 0);
    lv_obj_set_style_text_color(sterilization_countdown_minute_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(sterilization_countdown_minute_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_set_style_text_font(sterilization_countdown_minute_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);
    lv_obj_add_event_cb(sterilization_countdown_minute_roller, sterilization_countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 冒号2
    lv_obj_t* colon2_s = lv_label_create(sterilization_countdown_rollers_container);
    lv_label_set_text(colon2_s, ":");
    lv_obj_add_style(colon2_s, &style_text_white_custom_font, 0);

    // 秒钟滚轮
    sterilization_countdown_second_roller = lv_roller_create(sterilization_countdown_rollers_container);
    lv_obj_set_width(sterilization_countdown_second_roller, LV_PCT(25));
    lv_roller_set_options(sterilization_countdown_second_roller, sterilization_second_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(sterilization_countdown_second_roller, 3);
    lv_obj_add_style(sterilization_countdown_second_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(sterilization_countdown_second_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(sterilization_countdown_second_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(sterilization_countdown_second_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(sterilization_countdown_second_roller, 1, 0);
    lv_obj_set_style_radius(sterilization_countdown_second_roller, 4, 0);
    lv_obj_set_style_text_color(sterilization_countdown_second_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(sterilization_countdown_second_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_set_style_text_font(sterilization_countdown_second_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);
    lv_obj_add_event_cb(sterilization_countdown_second_roller, sterilization_countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 安全警告标签
    warning_label_sterilization = lv_label_create(content_area);
    lv_label_set_text(warning_label_sterilization, "安全提示：杀菌过程中请确保设备内无活体生物，避免直接接触杀菌光源。");
    lv_obj_add_style(warning_label_sterilization, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_color(warning_label_sterilization, lv_palette_main(LV_PALETTE_ORANGE), 0); 
    lv_obj_set_width(warning_label_sterilization, LV_PCT(100));
    lv_obj_set_style_pad_top(warning_label_sterilization, 15, 0);
    lv_label_set_long_mode(warning_label_sterilization, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(warning_label_sterilization, LV_TEXT_ALIGN_CENTER, 0);

    // 初始化滚轮值和可见性
    if (current_timed_cfg) {
        lv_roller_set_selected(sterilization_countdown_hour_roller, current_timed_cfg->countdown_hour, LV_ANIM_OFF);
        lv_roller_set_selected(sterilization_countdown_minute_roller, current_timed_cfg->countdown_minute, LV_ANIM_OFF);
        lv_roller_set_selected(sterilization_countdown_second_roller, current_timed_cfg->countdown_second, LV_ANIM_OFF);
        
        if (!current_timed_cfg->enabled) {
            lv_obj_add_flag(sterilization_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
            if(warning_label_sterilization) lv_obj_add_flag(warning_label_sterilization, LV_OBJ_FLAG_HIDDEN);
        }
    } else {
        lv_obj_add_flag(sterilization_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
        if(warning_label_sterilization) lv_obj_add_flag(warning_label_sterilization, LV_OBJ_FLAG_HIDDEN);
    }
}
// src/Display/Popups/UI_DryingSettingsPopup.cpp
#include "UI_DryingSettingsPopup.h"
#include "UI_PopupBase.h"
#include "../UI_Styles.h" 
#include "../font/fengyahei_s16_bpp4.h" 
#include "../../Config/Config.h" 
#include "../../SettingsLogic/DryingSettings.h"
#include "../../App/AppGlobal.h" 
#include <Arduino.h>
#include <stdio.h> 

static lv_obj_t *drying_timed_enable_switch = NULL;
static lv_obj_t *drying_countdown_hour_roller = NULL;
static lv_obj_t *drying_countdown_minute_roller = NULL;
static lv_obj_t *drying_countdown_second_roller = NULL;
static lv_obj_t *drying_countdown_rollers_container = NULL;

static char drying_hour_roller_options[24 * 4];
static char drying_minute_roller_options[60 * 4];
static char drying_second_roller_options[60 * 4];

static void build_drying_roller_options() {
    // 构建小时选项 (00-23)
    drying_hour_roller_options[0] = '\0';
    char *p_hour = drying_hour_roller_options;
    for (int i = 0; i < 24; i++) {
        p_hour += sprintf(p_hour, "%02u", i);
        if (i < 23) *(p_hour++) = '\n';
    }
    *p_hour = '\0';
    
    // 构建分钟选项 (00-59)
    drying_minute_roller_options[0] = '\0';
    char *p_min = drying_minute_roller_options;
    for (int i = 0; i < 60; i++) {
        p_min += sprintf(p_min, "%02u", i);
        if (i < 59) *(p_min++) = '\n';
    }
    *p_min = '\0';
    
    // 构建秒钟选项 (00-59)
    drying_second_roller_options[0] = '\0';
    char *p_sec = drying_second_roller_options;
    for (int i = 0; i < 60; i++) {
        p_sec += sprintf(p_sec, "%02u", i);
        if (i < 59) *(p_sec++) = '\n';
    }
    *p_sec = '\0';
}

static void drying_popup_delete_cleanup_handler(lv_event_t *e) { 
    drying_timed_enable_switch = NULL; 
    drying_countdown_hour_roller = NULL; 
    drying_countdown_minute_roller = NULL; 
    drying_countdown_second_roller = NULL; 
    drying_countdown_rollers_container = NULL; 
}

static void drying_timed_enable_switch_event_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool timed_enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    timed_operation_config_t current_cfg = *DryingSettings_GetTimedOperationSettings();
    current_cfg.enabled = timed_enabled;
    DryingSettings_SetTimedOperation(&current_cfg);
    
    if (drying_countdown_rollers_container) {
        if (timed_enabled) {
            lv_obj_clear_flag(drying_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(drying_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void drying_countdown_roller_event_cb(lv_event_t *e) {
    const timed_operation_config_t* p_current_cfg = DryingSettings_GetTimedOperationSettings();
    if (!p_current_cfg) return;
    
    timed_operation_config_t current_cfg = *p_current_cfg;
    if (current_cfg.enabled) {
        if (lv_obj_is_valid(drying_countdown_hour_roller))
            current_cfg.countdown_hour = lv_roller_get_selected(drying_countdown_hour_roller);
        if (lv_obj_is_valid(drying_countdown_minute_roller))
            current_cfg.countdown_minute = lv_roller_get_selected(drying_countdown_minute_roller);
        if (lv_obj_is_valid(drying_countdown_second_roller))
            current_cfg.countdown_second = lv_roller_get_selected(drying_countdown_second_roller);
        
        DryingSettings_SetTimedOperation(&current_cfg);
    }
}

void create_drying_settings_popup(lv_obj_t *parent_scr) {
    build_drying_roller_options();
    lv_obj_t *content_area = create_popup_frame(parent_scr, "设置");
    if (!content_area) return;

    lv_obj_t *popup_box = lv_obj_get_parent(content_area);
    lv_obj_t *popup_mask = lv_obj_get_parent(popup_box);
    if (popup_mask) {
        lv_obj_add_flag(popup_mask, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(popup_mask, drying_popup_delete_cleanup_handler, LV_EVENT_DELETE, NULL);
    }

    // --- 定时烘干设置 ---
    lv_obj_t *timed_enable_cont = lv_obj_create(content_area);
    lv_obj_remove_style_all(timed_enable_cont);
    lv_obj_set_width(timed_enable_cont, LV_PCT(100));
    lv_obj_set_height(timed_enable_cont, LV_SIZE_CONTENT);
    lv_obj_set_layout(timed_enable_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(timed_enable_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(timed_enable_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *timed_enable_label = lv_label_create(timed_enable_cont);
    lv_label_set_text(timed_enable_label, "启用定时烘干:");
    lv_obj_add_style(timed_enable_label, &style_text_white_custom_font, 0);

    drying_timed_enable_switch = lv_switch_create(timed_enable_cont);
    const timed_operation_config_t* current_timed_cfg = DryingSettings_GetTimedOperationSettings();
    if (current_timed_cfg && current_timed_cfg->enabled) 
        lv_obj_add_state(drying_timed_enable_switch, LV_STATE_CHECKED);
    lv_obj_add_event_cb(drying_timed_enable_switch, drying_timed_enable_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_style(drying_timed_enable_switch, &style_switch_on_cyan, (uint32_t)LV_PART_INDICATOR | (uint32_t)LV_STATE_CHECKED);

    // --- 倒计时滚轮容器 ---
    drying_countdown_rollers_container = lv_obj_create(content_area);
    lv_obj_remove_style_all(drying_countdown_rollers_container);
    lv_obj_set_width(drying_countdown_rollers_container, LV_PCT(100));
    lv_obj_set_height(drying_countdown_rollers_container, LV_SIZE_CONTENT);
    lv_obj_set_layout(drying_countdown_rollers_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(drying_countdown_rollers_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(drying_countdown_rollers_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(drying_countdown_rollers_container, 5, 0);
    lv_obj_set_style_pad_top(drying_countdown_rollers_container, 5, 0);

    // 小时滚轮
    drying_countdown_hour_roller = lv_roller_create(drying_countdown_rollers_container);
    lv_obj_set_width(drying_countdown_hour_roller, LV_PCT(25));
    lv_roller_set_options(drying_countdown_hour_roller, drying_hour_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(drying_countdown_hour_roller, 3);
    lv_obj_add_style(drying_countdown_hour_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(drying_countdown_hour_roller, ROLLER_BORDER_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(drying_countdown_hour_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(drying_countdown_hour_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(drying_countdown_hour_roller, 1, 0);
    lv_obj_set_style_radius(drying_countdown_hour_roller, 4, 0);
    lv_obj_set_style_text_color(drying_countdown_hour_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(drying_countdown_hour_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_add_event_cb(drying_countdown_hour_roller, drying_countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 冒号1
    lv_obj_t* colon1_d = lv_label_create(drying_countdown_rollers_container);
    lv_label_set_text(colon1_d, ":");
    lv_obj_add_style(colon1_d, &style_text_white_custom_font, 0);

    // 分钟滚轮
    drying_countdown_minute_roller = lv_roller_create(drying_countdown_rollers_container);
    lv_obj_set_width(drying_countdown_minute_roller, LV_PCT(25));
    lv_roller_set_options(drying_countdown_minute_roller, drying_minute_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(drying_countdown_minute_roller, 3);
    lv_obj_add_style(drying_countdown_minute_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(drying_countdown_minute_roller, ROLLER_BORDER_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(drying_countdown_minute_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(drying_countdown_minute_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(drying_countdown_minute_roller, 1, 0);
    lv_obj_set_style_radius(drying_countdown_minute_roller, 4, 0);
    lv_obj_set_style_text_color(drying_countdown_minute_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(drying_countdown_minute_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_add_event_cb(drying_countdown_minute_roller, drying_countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 冒号2
    lv_obj_t* colon2_d = lv_label_create(drying_countdown_rollers_container);
    lv_label_set_text(colon2_d, ":");
    lv_obj_add_style(colon2_d, &style_text_white_custom_font, 0);

    // 秒钟滚轮
    drying_countdown_second_roller = lv_roller_create(drying_countdown_rollers_container);
    lv_obj_set_width(drying_countdown_second_roller, LV_PCT(25));
    lv_roller_set_options(drying_countdown_second_roller, drying_second_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(drying_countdown_second_roller, 3);
    lv_obj_add_style(drying_countdown_second_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(drying_countdown_second_roller, ROLLER_BORDER_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(drying_countdown_second_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(drying_countdown_second_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(drying_countdown_second_roller, 1, 0);
    lv_obj_set_style_radius(drying_countdown_second_roller, 4, 0);
    lv_obj_set_style_text_color(drying_countdown_second_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(drying_countdown_second_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_add_event_cb(drying_countdown_second_roller, drying_countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 初始化滚轮值
    if (current_timed_cfg) {
        lv_roller_set_selected(drying_countdown_hour_roller, current_timed_cfg->countdown_hour, LV_ANIM_OFF);
        lv_roller_set_selected(drying_countdown_minute_roller, current_timed_cfg->countdown_minute, LV_ANIM_OFF);
        lv_roller_set_selected(drying_countdown_second_roller, current_timed_cfg->countdown_second, LV_ANIM_OFF);
        
        if (!current_timed_cfg->enabled) {
            lv_obj_add_flag(drying_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
        }
    } else {
        lv_obj_add_flag(drying_countdown_rollers_container, LV_OBJ_FLAG_HIDDEN);
    }

    // 添加使用说明
    lv_obj_t *info_label = lv_label_create(content_area);
    lv_label_set_text(info_label, "提示：启用定时烘干后，设备将自动开始工作并在指定时间后停止。");
    lv_obj_add_style(info_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_color(info_label, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_width(info_label, LV_PCT(100));
    lv_obj_set_style_pad_top(info_label, 10, 0);
    lv_label_set_long_mode(info_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(info_label, LV_TEXT_ALIGN_CENTER, 0);
}
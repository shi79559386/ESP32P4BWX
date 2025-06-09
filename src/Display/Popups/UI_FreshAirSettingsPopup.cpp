// UI_FreshAirSettingsPopup.cpp
#include "UI_FreshAirSettingsPopup.h"
#include "UI_PopupBase.h"
#include "../UI_Styles.h"
#include "../font/fengyahei_s16_bpp4.h"
#include "../font/fengyahei_s14_bpp4.h"
#include "../../Config/Config.h"
#include "../../SettingsLogic/FreshAirSettings.h"
#include "../../App/AppGlobal.h"
#include <Arduino.h>
#include <cstdio>

// UI元素指针
static lv_obj_t *fresh_air_mode_dropdown = NULL;
static lv_obj_t *fresh_air_schedule_switch = NULL;
static lv_obj_t *fresh_air_start_hour_roller = NULL;
static lv_obj_t *fresh_air_start_minute_roller = NULL;
static lv_obj_t *fresh_air_end_hour_roller = NULL;
static lv_obj_t *fresh_air_end_minute_roller = NULL;
static lv_obj_t *fresh_air_schedule_container = NULL;
static lv_obj_t *fresh_air_auto_info_label = NULL;
static lv_obj_t *fresh_air_status_label = NULL;

// 滚轮选项字符串
static char hour_options[24 * 4];    // "00\n01\n02\n..."
static char minute_options[60 * 4];  // "00\n01\n02\n..."

static void build_time_roller_options() {
    // 构建小时选项 (00-23)
    hour_options[0] = '\0';
    char *p_hour = hour_options;
    for (int i = 0; i < 24; i++) {
        p_hour += sprintf(p_hour, "%02d", i);
        if (i < 23) *(p_hour++) = '\n';
    }
    *p_hour = '\0';
    
    // 构建分钟选项 (00-59)
    minute_options[0] = '\0';
    char *p_min = minute_options;
    for (int i = 0; i < 60; i++) {
        p_min += sprintf(p_min, "%02d", i);
        if (i < 59) *(p_min++) = '\n';
    }
    *p_min = '\0';
}

static void fresh_air_popup_delete_cleanup_handler(lv_event_t *e) {
    fresh_air_mode_dropdown = NULL;
    fresh_air_schedule_switch = NULL;
    fresh_air_start_hour_roller = NULL;
    fresh_air_start_minute_roller = NULL;
    fresh_air_end_hour_roller = NULL;
    fresh_air_end_minute_roller = NULL;
    fresh_air_schedule_container = NULL;
    fresh_air_auto_info_label = NULL;
    fresh_air_status_label = NULL;
}

static void update_ui_visibility() {
    if (!fresh_air_mode_dropdown || !lv_obj_is_valid(fresh_air_mode_dropdown)) return;
    
    fresh_air_mode_t current_mode = FreshAirSettings_GetMode();
    
    // 根据模式显示/隐藏相关控件
    if (fresh_air_schedule_container && lv_obj_is_valid(fresh_air_schedule_container)) {
        if (current_mode == FRESH_AIR_MODE_SCHEDULED) {
            lv_obj_clear_flag(fresh_air_schedule_container, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(fresh_air_schedule_container, LV_OBJ_FLAG_HIDDEN);
        }
    }
    
    // 更新自动模式信息显示
    if (fresh_air_auto_info_label && lv_obj_is_valid(fresh_air_auto_info_label)) {
        if (current_mode == FRESH_AIR_MODE_AUTO) {
            lv_label_set_text(fresh_air_auto_info_label, 
                "自动模式触发条件:\n"
                "• 左右温差 > 2.0°C\n"
                "• 左右湿度差 > 10%\n" 
                "• 任一区域湿度 > 80%");
            lv_obj_clear_flag(fresh_air_auto_info_label, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(fresh_air_auto_info_label, LV_OBJ_FLAG_HIDDEN);
        }
    }
    
    // 更新状态显示
    if (fresh_air_status_label && lv_obj_is_valid(fresh_air_status_label)) {
        const char* status_str = FreshAirSettings_GetStatusString();
        lv_label_set_text(fresh_air_status_label, status_str ? status_str : "状态获取失败");
    }
}

static void fresh_air_mode_dropdown_event_cb(lv_event_t *e) {
    lv_obj_t *dropdown = lv_event_get_target(e);
    if (!dropdown || !lv_obj_is_valid(dropdown)) return;
    
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    fresh_air_mode_t new_mode = (fresh_air_mode_t)selected;
    
    FreshAirSettings_SetMode(new_mode);
    update_ui_visibility();
    
    const char* mode_names[] = {"手动", "自动", "定时"};
    Serial.printf("FreshAir: Mode changed to %s\n", mode_names[selected]);
}

static void fresh_air_schedule_switch_event_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    if (!sw || !lv_obj_is_valid(sw)) return;
    
    bool enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    // 获取当前定时设置
    const fresh_air_schedule_t* current_schedule = FreshAirSettings_GetSchedule();
    if (current_schedule) {
        fresh_air_schedule_t new_schedule = *current_schedule;
        new_schedule.enabled = enabled;
        FreshAirSettings_SetSchedule(&new_schedule);
    }
    
    Serial.printf("FreshAir: Schedule %s\n", enabled ? "enabled" : "disabled");
}

static void fresh_air_time_roller_event_cb(lv_event_t *e) {
    // 获取当前定时设置
    const fresh_air_schedule_t* current_schedule = FreshAirSettings_GetSchedule();
    if (!current_schedule) return;
    
    fresh_air_schedule_t new_schedule = *current_schedule;
    
    // 更新时间设置
    if (fresh_air_start_hour_roller && lv_obj_is_valid(fresh_air_start_hour_roller)) {
        new_schedule.start_hour = lv_roller_get_selected(fresh_air_start_hour_roller);
    }
    if (fresh_air_start_minute_roller && lv_obj_is_valid(fresh_air_start_minute_roller)) {
        new_schedule.start_minute = lv_roller_get_selected(fresh_air_start_minute_roller);
    }
    if (fresh_air_end_hour_roller && lv_obj_is_valid(fresh_air_end_hour_roller)) {
        new_schedule.end_hour = lv_roller_get_selected(fresh_air_end_hour_roller);
    }
    if (fresh_air_end_minute_roller && lv_obj_is_valid(fresh_air_end_minute_roller)) {
        new_schedule.end_minute = lv_roller_get_selected(fresh_air_end_minute_roller);
    }
    
    FreshAirSettings_SetSchedule(&new_schedule);
    
    Serial.printf("FreshAir: Schedule time updated %02d:%02d - %02d:%02d\n",
                  new_schedule.start_hour, new_schedule.start_minute,
                  new_schedule.end_hour, new_schedule.end_minute);
}

void create_fresh_air_settings_popup(lv_obj_t *parent_scr) {
    build_time_roller_options();
    
    lv_obj_t *content_area = create_popup_frame(parent_scr, "设置");
    if (!content_area) return;
    
    // 注册清理回调
    lv_obj_t *popup_box = lv_obj_get_parent(content_area);
    lv_obj_t *popup_mask = lv_obj_get_parent(popup_box);
    if (popup_mask) {
        lv_obj_add_flag(popup_mask, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(popup_mask, fresh_air_popup_delete_cleanup_handler, LV_EVENT_DELETE, NULL);
    }
    
    // --- 1. 运行模式选择 ---
    lv_obj_t *mode_section = lv_obj_create(content_area);
    lv_obj_remove_style_all(mode_section);
    lv_obj_set_width(mode_section, LV_PCT(100));
    lv_obj_set_height(mode_section, LV_SIZE_CONTENT);
    lv_obj_set_layout(mode_section, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(mode_section, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(mode_section, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(mode_section, 8, 0);
    
    lv_obj_t *mode_label = lv_label_create(mode_section);
    lv_label_set_text(mode_label, "运行模式:");
    lv_obj_add_style(mode_label, &style_text_white_custom_font, 0);
    
    fresh_air_mode_dropdown = lv_dropdown_create(mode_section);
    lv_obj_set_width(fresh_air_mode_dropdown, LV_PCT(80));
    lv_dropdown_set_options(fresh_air_mode_dropdown, "手动\n自动\n定时");
    lv_dropdown_set_selected(fresh_air_mode_dropdown, (uint16_t)FreshAirSettings_GetMode());
    lv_obj_add_style(fresh_air_mode_dropdown, &style_text_white_custom_font, 0);
    lv_obj_set_style_bg_color(fresh_air_mode_dropdown, PANEL_BG_COLOR, 0);
    lv_obj_set_style_border_color(fresh_air_mode_dropdown, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(fresh_air_mode_dropdown, 1, 0);
    lv_dropdown_set_symbol(fresh_air_mode_dropdown, LV_SYMBOL_DOWN);
    lv_obj_add_event_cb(fresh_air_mode_dropdown, fresh_air_mode_dropdown_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // --- 2. 当前状态显示 ---
    fresh_air_status_label = lv_label_create(content_area);
    lv_obj_add_style(fresh_air_status_label, &style_text_white_custom_font, 0);
    lv_obj_set_width(fresh_air_status_label, LV_PCT(100));
    lv_obj_set_style_text_align(fresh_air_status_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_bg_color(fresh_air_status_label, PANEL_BG_COLOR, 0);
    lv_obj_set_style_bg_opa(fresh_air_status_label, LV_OPA_50, 0);
    lv_obj_set_style_pad_all(fresh_air_status_label, 8, 0);
    lv_obj_set_style_radius(fresh_air_status_label, 4, 0);
    
    // --- 3. 自动模式信息显示 ---
    fresh_air_auto_info_label = lv_label_create(content_area);
    lv_obj_add_style(fresh_air_auto_info_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(fresh_air_auto_info_label, &fengyahei_s14_bpp4, 0);
    lv_obj_set_width(fresh_air_auto_info_label, LV_PCT(100));
    lv_obj_set_style_text_align(fresh_air_auto_info_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_bg_color(fresh_air_auto_info_label, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_opa(fresh_air_auto_info_label, LV_OPA_20, 0);
    lv_obj_set_style_pad_all(fresh_air_auto_info_label, 8, 0);
    lv_obj_set_style_radius(fresh_air_auto_info_label, 4, 0);
    lv_label_set_long_mode(fresh_air_auto_info_label, LV_LABEL_LONG_WRAP);
    
    // --- 4. 定时设置容器 ---
    fresh_air_schedule_container = lv_obj_create(content_area);
    lv_obj_remove_style_all(fresh_air_schedule_container);
    lv_obj_set_width(fresh_air_schedule_container, LV_PCT(100));
    lv_obj_set_height(fresh_air_schedule_container, LV_SIZE_CONTENT);
    lv_obj_set_layout(fresh_air_schedule_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(fresh_air_schedule_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(fresh_air_schedule_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(fresh_air_schedule_container, 10, 0);
    lv_obj_set_style_bg_color(fresh_air_schedule_container, PANEL_BG_COLOR, 0);
    lv_obj_set_style_bg_opa(fresh_air_schedule_container, LV_OPA_30, 0);
    lv_obj_set_style_pad_all(fresh_air_schedule_container, 10, 0);
    lv_obj_set_style_radius(fresh_air_schedule_container, 6, 0);
    
    // 4.1 定时开关
    lv_obj_t *schedule_enable_row = lv_obj_create(fresh_air_schedule_container);
    lv_obj_remove_style_all(schedule_enable_row);
    lv_obj_set_width(schedule_enable_row, LV_PCT(100));
    lv_obj_set_height(schedule_enable_row, LV_SIZE_CONTENT);
    lv_obj_set_layout(schedule_enable_row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(schedule_enable_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(schedule_enable_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    lv_obj_t *schedule_enable_label = lv_label_create(schedule_enable_row);
    lv_label_set_text(schedule_enable_label, "启用定时:");
    lv_obj_add_style(schedule_enable_label, &style_text_white_custom_font, 0);
    
    fresh_air_schedule_switch = lv_switch_create(schedule_enable_row);
    lv_obj_add_style(fresh_air_schedule_switch, &style_switch_on_cyan, (uint32_t)LV_PART_INDICATOR | (uint32_t)LV_STATE_CHECKED);
    lv_obj_add_event_cb(fresh_air_schedule_switch, fresh_air_schedule_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // 4.2 开始时间设置
    lv_obj_t *start_time_section = lv_obj_create(fresh_air_schedule_container);
    lv_obj_remove_style_all(start_time_section);
    lv_obj_set_width(start_time_section, LV_PCT(100));
    lv_obj_set_height(start_time_section, LV_SIZE_CONTENT);
    lv_obj_set_layout(start_time_section, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(start_time_section, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(start_time_section, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(start_time_section, 5, 0);
    
    lv_obj_t *start_time_label = lv_label_create(start_time_section);
    lv_label_set_text(start_time_label, "开启时间:");
    lv_obj_add_style(start_time_label, &style_text_white_custom_font, 0);
    
    lv_obj_t *start_time_rollers = lv_obj_create(start_time_section);
    lv_obj_remove_style_all(start_time_rollers);
    lv_obj_set_width(start_time_rollers, LV_PCT(80));
    lv_obj_set_height(start_time_rollers, LV_SIZE_CONTENT);
    lv_obj_set_layout(start_time_rollers, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(start_time_rollers, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(start_time_rollers, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(start_time_rollers, 8, 0);
    
    // 开始小时滚轮
    fresh_air_start_hour_roller = lv_roller_create(start_time_rollers);
    lv_obj_set_width(fresh_air_start_hour_roller, LV_PCT(40));
    lv_roller_set_options(fresh_air_start_hour_roller, hour_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(fresh_air_start_hour_roller, 3);
    lv_obj_add_style(fresh_air_start_hour_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(fresh_air_start_hour_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(fresh_air_start_hour_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(fresh_air_start_hour_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(fresh_air_start_hour_roller, 1, 0);
    lv_obj_set_style_radius(fresh_air_start_hour_roller, 4, 0);
    lv_obj_set_style_text_color(fresh_air_start_hour_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(fresh_air_start_hour_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_add_event_cb(fresh_air_start_hour_roller, fresh_air_time_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    lv_obj_t *colon1 = lv_label_create(start_time_rollers);
    lv_label_set_text(colon1, ":");
    lv_obj_add_style(colon1, &style_text_white_custom_font, 0);
    
    // 开始分钟滚轮
    fresh_air_start_minute_roller = lv_roller_create(start_time_rollers);
    lv_obj_set_width(fresh_air_start_minute_roller, LV_PCT(40));
    lv_roller_set_options(fresh_air_start_minute_roller, minute_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(fresh_air_start_minute_roller, 3);
    lv_obj_add_style(fresh_air_start_minute_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(fresh_air_start_minute_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(fresh_air_start_minute_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(fresh_air_start_minute_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(fresh_air_start_minute_roller, 1, 0);
    lv_obj_set_style_radius(fresh_air_start_minute_roller, 4, 0);
    lv_obj_set_style_text_color(fresh_air_start_minute_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(fresh_air_start_minute_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_add_event_cb(fresh_air_start_minute_roller, fresh_air_time_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // 4.3 结束时间设置 (类似开始时间)
    lv_obj_t *end_time_section = lv_obj_create(fresh_air_schedule_container);
    lv_obj_remove_style_all(end_time_section);
    lv_obj_set_width(end_time_section, LV_PCT(100));
    lv_obj_set_height(end_time_section, LV_SIZE_CONTENT);
    lv_obj_set_layout(end_time_section, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(end_time_section, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(end_time_section, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(end_time_section, 5, 0);
    
    lv_obj_t *end_time_label = lv_label_create(end_time_section);
    lv_label_set_text(end_time_label, "关闭时间:");
    lv_obj_add_style(end_time_label, &style_text_white_custom_font, 0);
    
    lv_obj_t *end_time_rollers = lv_obj_create(end_time_section);
    lv_obj_remove_style_all(end_time_rollers);
    lv_obj_set_width(end_time_rollers, LV_PCT(80));
    lv_obj_set_height(end_time_rollers, LV_SIZE_CONTENT);
    lv_obj_set_layout(end_time_rollers, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(end_time_rollers, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(end_time_rollers, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(end_time_rollers, 8, 0);
    
    // 结束小时滚轮
    fresh_air_end_hour_roller = lv_roller_create(end_time_rollers);
    lv_obj_set_width(fresh_air_end_hour_roller, LV_PCT(40));
    lv_roller_set_options(fresh_air_end_hour_roller, hour_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(fresh_air_end_hour_roller, 3);
    lv_obj_add_style(fresh_air_end_hour_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(fresh_air_end_hour_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(fresh_air_end_hour_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(fresh_air_end_hour_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(fresh_air_end_hour_roller, 1, 0);
    lv_obj_set_style_radius(fresh_air_end_hour_roller, 4, 0);
    lv_obj_set_style_text_color(fresh_air_end_hour_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(fresh_air_end_hour_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_add_event_cb(fresh_air_end_hour_roller, fresh_air_time_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    lv_obj_t *colon2 = lv_label_create(end_time_rollers);
    lv_label_set_text(colon2, ":");
    lv_obj_add_style(colon2, &style_text_white_custom_font, 0);
    
    // 结束分钟滚轮
    fresh_air_end_minute_roller = lv_roller_create(end_time_rollers);
    lv_obj_set_width(fresh_air_end_minute_roller, LV_PCT(40));
    lv_roller_set_options(fresh_air_end_minute_roller, minute_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(fresh_air_end_minute_roller, 3);
    lv_obj_add_style(fresh_air_end_minute_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(fresh_air_end_minute_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(fresh_air_end_minute_roller, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(fresh_air_end_minute_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(fresh_air_end_minute_roller, 1, 0);
    lv_obj_set_style_radius(fresh_air_end_minute_roller, 4, 0);
    lv_obj_set_style_text_color(fresh_air_end_minute_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(fresh_air_end_minute_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_add_event_cb(fresh_air_end_minute_roller, fresh_air_time_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // 初始化UI状态
    const fresh_air_schedule_t* current_schedule = FreshAirSettings_GetSchedule();
    if (current_schedule) {
        if (current_schedule->enabled) {
            lv_obj_add_state(fresh_air_schedule_switch, LV_STATE_CHECKED);
        }
        lv_roller_set_selected(fresh_air_start_hour_roller, current_schedule->start_hour, LV_ANIM_OFF);
        lv_roller_set_selected(fresh_air_start_minute_roller, current_schedule->start_minute, LV_ANIM_OFF);
        lv_roller_set_selected(fresh_air_end_hour_roller, current_schedule->end_hour, LV_ANIM_OFF);
        lv_roller_set_selected(fresh_air_end_minute_roller, current_schedule->end_minute, LV_ANIM_OFF);
    }
    
    // 更新UI可见性
    update_ui_visibility();
    
    Serial.println("FreshAir: Settings popup created");
}
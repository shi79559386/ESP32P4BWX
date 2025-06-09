// ui_fresh_air_card.cpp - 优化版本
#include "ui_fresh_air_card.h"
#include "../UI_Styles.h"
#include "../../Config/Config.h"
#include "../../Peripherals/OutputControls.h"
#include "../../SettingsLogic/FreshAirSettings.h"
#include "../font/fengyahei_s16_bpp4.h"
#include "../../App/AppGlobal.h"    
#include "../UI_Common.h"     
#include "../UI_ControlPage.h"
#include <Arduino.h>

static lv_obj_t *card_fresh_air_status_label = NULL;
static lv_obj_t *card_fresh_air_schedule_label = NULL; // 合并模式和定时显示
static lv_obj_t *card_fresh_air_main_switch = NULL;

static void fresh_air_card_main_toggle_event_handler(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool is_checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    // 在手动模式下才允许直接开关
    if (FreshAirSettings_GetMode() == FRESH_AIR_MODE_MANUAL) {
        FreshAirSettings_SetEnabled(is_checked);
        Serial.printf("FreshAirCard: Manual toggle to %s\n", is_checked ? "ON" : "OFF");
    } else {
        // 非手动模式下恢复开关状态
        if (FreshAirSettings_IsEnabled()) {
            lv_obj_add_state(sw, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(sw, LV_STATE_CHECKED);
        }
        Serial.println("FreshAirCard: Auto/Scheduled mode - use settings to control");
    }
}

void create_fresh_air_card_ui(lv_obj_t* parent_dashboard) {
    lv_obj_t* card = lv_obj_create(parent_dashboard);
    lv_obj_add_style(card, &style_card_main, 0);
    lv_obj_set_width(card, LV_PCT(48));
    lv_obj_set_height(card, CARD_FIXED_HEIGHT);
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(card, CARD_CONTENT_PADDING, 0);

    extern const char* card_titles[];
    extern const char* card_icons[];
    create_common_card_header(card, card_titles[CARD_ID_FRESH_AIR], card_icons[CARD_ID_FRESH_AIR], 
                             CARD_ID_FRESH_AIR, card_settings_event_handler_global);

    // 主内容容器 - 设置为相对定位，作为绝对定位的参考
    lv_obj_t* content_area_card = lv_obj_create(card);
    lv_obj_remove_style_all(content_area_card);
    lv_obj_set_width(content_area_card, LV_PCT(100));
    lv_obj_set_flex_grow(content_area_card, 1);  // 占据剩余所有空间
    lv_obj_set_style_pad_all(content_area_card, CARD_CONTENT_INNER_PADDING, 0);
    

    // 文字信息区域 - 占据除开关外的所有空间
    lv_obj_t* info_container = lv_obj_create(content_area_card);
    lv_obj_remove_style_all(info_container);
    lv_obj_set_pos(info_container, 0, 0);  // 定位在左上角
    lv_obj_set_size(info_container, lv_pct(100), lv_pct(100));  // 占据全部空间
    lv_obj_set_style_pad_right(info_container, CARD_TEXT_RIGHT_MARGIN, 0);  // 右侧为开关留空间
    lv_obj_set_layout(info_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(info_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(info_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(info_container, CARD_ROW_SPACING, 0);

    // 第一行：状态标签
    card_fresh_air_status_label = lv_label_create(info_container);
    lv_obj_add_style(card_fresh_air_status_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_fresh_air_status_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_fresh_air_status_label, LV_PCT(100));
    lv_label_set_text(card_fresh_air_status_label, "已关闭: 手动");

    // 第二行：定时状态标签
    card_fresh_air_schedule_label = lv_label_create(info_container);
    lv_obj_add_style(card_fresh_air_schedule_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_fresh_air_schedule_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_fresh_air_schedule_label, LV_PCT(100));
    lv_label_set_text(card_fresh_air_schedule_label, "定时: 未启用");

    // 开关 - 绝对定位在右下角，完全独立于文字布局
    card_fresh_air_main_switch = lv_switch_create(content_area_card);
    lv_obj_add_style(card_fresh_air_main_switch, &style_switch_on_cyan, (lv_style_selector_t)(LV_PART_INDICATOR | LV_STATE_CHECKED));
    lv_obj_add_event_cb(card_fresh_air_main_switch, fresh_air_card_main_toggle_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    
    // 关键：绝对定位开关在右下角
    lv_obj_align(card_fresh_air_main_switch,LV_ALIGN_BOTTOM_RIGHT,-CARD_SWITCH_RIGHT_MARGIN,-CARD_SWITCH_BOTTOM_MARGIN);

    FreshAirSettings_RegisterStatusUpdateCallback(update_fresh_air_card_status);
    update_fresh_air_card_status();
    Serial.println("Fresh air card UI created with absolute positioned switch.");
}

void update_fresh_air_card_status() {
    if (!card_fresh_air_status_label || !lv_obj_is_valid(card_fresh_air_status_label) ||
        !card_fresh_air_schedule_label || !lv_obj_is_valid(card_fresh_air_schedule_label) ||
        !card_fresh_air_main_switch || !lv_obj_is_valid(card_fresh_air_main_switch)) {
        return;
    }

    // 合并状态和模式显示
    bool is_on = FreshAirSettings_IsEnabled();
    const char* mode_names[] = {"手动", "自动", "定时"};
    fresh_air_mode_t current_mode = FreshAirSettings_GetMode();
    
    lv_label_set_text_fmt(card_fresh_air_status_label, "%s: %s", 
                          is_on ? "运行中" : "已关闭", 
                          mode_names[current_mode]);
    
    // 更新定时状态
    const fresh_air_schedule_t* schedule = FreshAirSettings_GetSchedule();
    if (schedule && schedule->enabled) {
        lv_label_set_text_fmt(card_fresh_air_schedule_label, 
                              "定时: %02d:%02d-%02d:%02d",
                              schedule->start_hour, schedule->start_minute,
                              schedule->end_hour, schedule->end_minute);
    } else {
        lv_label_set_text(card_fresh_air_schedule_label, "定时: 未启用");
    }
    
    // 更新开关状态
    if (is_on) {
        lv_obj_add_state(card_fresh_air_main_switch, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(card_fresh_air_main_switch, LV_STATE_CHECKED);
    }
}
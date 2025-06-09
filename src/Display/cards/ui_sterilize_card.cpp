// File: src/Display/cards/ui_sterilize_card.cpp
#include "ui_sterilize_card.h"
#include "../UI_Styles.h"
#include "../../Config/Config.h"
#include "../../SettingsLogic/SterilizationSettings.h"
#include "../../App/AppGlobal.h"   
#include "../font/fengyahei_s16_bpp4.h" 
#include "../font/fengyahei_s16_bpp4.h" 
#include "../UI_Common.h"     
#include "../UI_ControlPage.h"
#include <Arduino.h>
#include <stdio.h> 

static lv_obj_t *card_sterilize_status_label = NULL;
static lv_obj_t *card_sterilize_timed_status_label = NULL; 
static lv_obj_t *card_sterilize_main_switch = NULL;

static void sterilize_card_main_toggle_event_handler(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool is_checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    Serial.printf("SterilizeCard: Main toggle visually changed to: %s. Applying to logic.\n", is_checked ? "ON" : "OFF");
    SterilizationSettings_SetEnabled(is_checked); 
}

void create_sterilize_card_ui(lv_obj_t* parent_dashboard) {
    lv_obj_t* card = lv_obj_create(parent_dashboard);
    lv_obj_add_style(card, &style_card_main, 0);
    lv_obj_set_width(card, LV_PCT(48));
    lv_obj_set_height(card, CARD_FIXED_HEIGHT);
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(card, CARD_CONTENT_PADDING, 0);

    extern const char* card_titles[];
    extern const char* card_icons[];
    create_common_card_header(card, card_titles[CARD_ID_STERILIZE], card_icons[CARD_ID_STERILIZE], 
                             CARD_ID_STERILIZE, card_settings_event_handler_global);

    // 主内容容器
    lv_obj_t* content_area_card = lv_obj_create(card);
    lv_obj_remove_style_all(content_area_card);
    lv_obj_set_width(content_area_card, LV_PCT(100));
    lv_obj_set_flex_grow(content_area_card, 1);
    lv_obj_set_style_pad_all(content_area_card, CARD_CONTENT_INNER_PADDING, 0);
  

    // 文字信息区域
    lv_obj_t* info_container = lv_obj_create(content_area_card);
    lv_obj_remove_style_all(info_container);
    lv_obj_set_pos(info_container, 0, 0);
    lv_obj_set_size(info_container, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_right(info_container, CARD_TEXT_RIGHT_MARGIN, 0);
    lv_obj_set_layout(info_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(info_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(info_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(info_container, CARD_ROW_SPACING, 0);

    // 第一行：杀菌状态标签
    card_sterilize_status_label = lv_label_create(info_container);
    lv_obj_add_style(card_sterilize_status_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_sterilize_status_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_sterilize_status_label, LV_PCT(100));
    lv_label_set_text(card_sterilize_status_label, "状态: 已关闭");
    
    // 第二行：定时状态标签
    card_sterilize_timed_status_label = lv_label_create(info_container);
    lv_obj_add_style(card_sterilize_timed_status_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_sterilize_timed_status_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_sterilize_timed_status_label, LV_PCT(100));
    lv_label_set_text(card_sterilize_timed_status_label, "定时: 未启用");

    // 开关 - 绝对定位
    card_sterilize_main_switch = lv_switch_create(content_area_card);
    lv_obj_add_style(card_sterilize_main_switch, &style_switch_on_cyan, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_event_cb(card_sterilize_main_switch, sterilize_card_main_toggle_event_handler, LV_EVENT_CLICKED, NULL);
    

    lv_obj_align(card_sterilize_main_switch,LV_ALIGN_BOTTOM_RIGHT,-CARD_SWITCH_RIGHT_MARGIN,-CARD_SWITCH_BOTTOM_MARGIN);
    
    SterilizationSettings_RegisterStatusUpdateCallback(update_sterilize_card_status);
    update_sterilize_card_status();
    Serial.println("Sterilize card UI created with absolute positioned switch.");
}

void update_sterilize_card_status() {
    if (card_sterilize_status_label && lv_obj_is_valid(card_sterilize_status_label)) {
        if (SterilizationSettings_IsEnabled()) {
            lv_label_set_text(card_sterilize_status_label, "状态: 杀菌中");
        } else {
            lv_label_set_text(card_sterilize_status_label, "状态: 已关闭");
        }
    }

    if (card_sterilize_timed_status_label && lv_obj_is_valid(card_sterilize_timed_status_label)) {
        const timed_operation_config_t* timed_cfg = SterilizationSettings_GetTimedOperationSettings();
        if (timed_cfg && timed_cfg->enabled) {
            if (SterilizationSettings_IsEnabled() && SterilizationSettings_IsTimedSessionActive()) {
                char countdown_str[30];
                SterilizationSettings_GetFormattedRemainingCountdown(countdown_str, sizeof(countdown_str));
                lv_label_set_text_fmt(card_sterilize_timed_status_label, "定时关: %s", countdown_str);
            } else {
                 lv_label_set_text_fmt(card_sterilize_timed_status_label, "定时: %02u:%02u:%02u",
                                      timed_cfg->countdown_hour,
                                      timed_cfg->countdown_minute,
                                      timed_cfg->countdown_second);
            }
        } else {
            lv_label_set_text(card_sterilize_timed_status_label, "定时: 未启用");
        }
    }

    if (card_sterilize_main_switch && lv_obj_is_valid(card_sterilize_main_switch)) {
        if (SterilizationSettings_IsEnabled()) {
            lv_obj_add_state(card_sterilize_main_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(card_sterilize_main_switch, LV_STATE_CHECKED);
        }
    }
}
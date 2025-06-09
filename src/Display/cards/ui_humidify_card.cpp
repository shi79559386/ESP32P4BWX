// File: src/Display/cards/ui_humidify_card.cpp
#include "ui_humidify_card.h"
#include "../UI_Styles.h"
#include "../../Config/Config.h"
#include "../font/fengyahei_s16_bpp4.h"
#include "../../App/AppGlobal.h"
#include "../UI_Common.h"
#include "../UI_ControlPage.h"
#include <Arduino.h>
#include <stdio.h>

static lv_obj_t *card_humidify_status_label = NULL;
static lv_obj_t *card_humidify_target_label = NULL;
static lv_obj_t *card_humidify_current_label = NULL;
static lv_obj_t *card_humidify_main_switch = NULL;

// 简单的加湿控制状态变量
static bool g_humidify_enabled = false;

static void humidify_card_main_toggle_event_handler(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool is_checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    Serial.printf("HumidifyCard: Main toggle changed to: %s\n", is_checked ? "ON" : "OFF");
    g_humidify_enabled = is_checked;
    update_humidify_card_status();
}

void create_humidify_card_ui(lv_obj_t* parent_dashboard) {
    lv_obj_t* card = lv_obj_create(parent_dashboard);
    lv_obj_add_style(card, &style_card_main, 0);
    lv_obj_set_width(card, LV_PCT(48));
    lv_obj_set_height(card, CARD_FIXED_HEIGHT);
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(card, CARD_CONTENT_PADDING, 0);

    extern const char* card_titles[];
    extern const char* card_icons[];
    create_common_card_header(card, 
                             card_titles[CARD_ID_HUMIDIFY], 
                             card_icons[CARD_ID_HUMIDIFY], 
                             CARD_ID_HUMIDIFY, 
                             card_settings_event_handler_global);

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

    // 第一行：加湿状态标签
    card_humidify_status_label = lv_label_create(info_container);
    lv_obj_add_style(card_humidify_status_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_humidify_status_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_humidify_status_label, LV_PCT(100));
    lv_label_set_text(card_humidify_status_label, "状态: 已关闭");

    // 第二行：目标湿度标签
    card_humidify_target_label = lv_label_create(info_container);
    lv_obj_add_style(card_humidify_target_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_humidify_target_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_humidify_target_label, LV_PCT(100));
    lv_label_set_text(card_humidify_target_label, "目标: 60% | 60%");

    // 第三行：当前湿度标签
    card_humidify_current_label = lv_label_create(info_container);
    lv_obj_add_style(card_humidify_current_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_humidify_current_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_humidify_current_label, LV_PCT(100));
    lv_label_set_text(card_humidify_current_label, "当前: --% | --%");

    // 开关 - 绝对定位
    card_humidify_main_switch = lv_switch_create(content_area_card);
    lv_obj_add_style(card_humidify_main_switch, &style_switch_on_cyan, (lv_style_selector_t)(LV_PART_INDICATOR | LV_STATE_CHECKED));
    lv_obj_add_event_cb(card_humidify_main_switch, humidify_card_main_toggle_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    
    lv_obj_align(card_humidify_main_switch, LV_ALIGN_BOTTOM_RIGHT, -CARD_SWITCH_RIGHT_MARGIN, -CARD_SWITCH_BOTTOM_MARGIN);
    
    update_humidify_card_status();
    Serial.println("Humidify card UI created with absolute positioned switch.");
}

void update_humidify_card_status() {
    if (card_humidify_status_label && lv_obj_is_valid(card_humidify_status_label)) {
        if (g_humidify_enabled) {
            lv_label_set_text(card_humidify_status_label, "状态: 加湿中");
        } else {
            lv_label_set_text(card_humidify_status_label, "状态: 已关闭");
        }
    }

    if (card_humidify_target_label && lv_obj_is_valid(card_humidify_target_label)) {
        lv_label_set_text_fmt(card_humidify_target_label, "目标: %d%% | %d%%", 
                              g_target_humidity_left, g_target_humidity_right);
    }

    if (card_humidify_current_label && lv_obj_is_valid(card_humidify_current_label)) {
        lv_label_set_text_fmt(card_humidify_current_label, "当前: %d%% | %d%%", 
                              g_current_humidity, g_current_humidity_2);
    }

    if (card_humidify_main_switch && lv_obj_is_valid(card_humidify_main_switch)) {
        if (g_humidify_enabled) {
            lv_obj_add_state(card_humidify_main_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(card_humidify_main_switch, LV_STATE_CHECKED);
        }
    }
}
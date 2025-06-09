// File: src/Display/cards/ui_thermal_card.cpp
#include "ui_thermal_card.h"
#include "../UI_Styles.h"
#include "../../Config/Config.h"
#include "../../SettingsLogic/ThermalSettings.h"
#include "../font/fengyahei_s16_bpp4.h"
#include "../../App/AppGlobal.h"
#include "../UI_Common.h"
#include "../UI_ControlPage.h"
#include <Arduino.h>
#include <stdio.h>


static lv_obj_t *card_thermal_timer_label = NULL;
static lv_obj_t *card_thermal_heater_label = NULL;
static lv_obj_t *card_thermal_fan_label = NULL;
static lv_obj_t *card_thermal_heater_switch = NULL;
static lv_obj_t *card_thermal_fan_switch = NULL;

static void thermal_card_heater_toggle_event_handler(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool is_checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    // 简化处理：直接设置加热器状态，逻辑在 ThermalSettings 中处理
    ThermalSettings_SetHeaterEnabled(is_checked);
    
    if (is_checked) {
        Serial.println("ThermalCard: Heater enabled (fan auto-enabled)");
    } else {
        Serial.println("ThermalCard: Heater disabled");
    }
}

static void thermal_card_fan_toggle_event_handler(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool is_checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    // 风扇开关：逻辑在 ThermalSettings 中处理
    ThermalSettings_SetFanEnabled(is_checked);
    
    if (is_checked) {
        Serial.println("ThermalCard: Fan enabled");
    } else {
        Serial.println("ThermalCard: Fan disabled (heater auto-disabled)");
    }
}

void create_thermal_card_ui(lv_obj_t* parent_dashboard) {
    lv_obj_t* card = lv_obj_create(parent_dashboard);
    lv_obj_add_style(card, &style_card_main, 0);
    lv_obj_set_width(card, LV_PCT(48));
    lv_obj_set_height(card, CARD_FIXED_HEIGHT);
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(card, CARD_CONTENT_PADDING, 0);

    extern const char* card_titles[];
    extern const char* card_icons[];
    create_common_card_header(card, card_titles[CARD_ID_THERMAL], card_icons[CARD_ID_THERMAL], 
                             CARD_ID_THERMAL, card_settings_event_handler_global);

    // 主内容容器
    lv_obj_t* content_area_card = lv_obj_create(card);
    lv_obj_remove_style_all(content_area_card);
    lv_obj_set_width(content_area_card, LV_PCT(100));
    lv_obj_set_flex_grow(content_area_card, 1);
    lv_obj_set_style_pad_all(content_area_card, CARD_CONTENT_INNER_PADDING, 0);

    // 文字信息区域 - 左侧，与其他卡片一致
    lv_obj_t* info_container = lv_obj_create(content_area_card);
    lv_obj_remove_style_all(info_container);
    lv_obj_set_pos(info_container, 0, 0);
    lv_obj_set_size(info_container, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_right(info_container, CARD_TEXT_RIGHT_MARGIN, 0);
    lv_obj_set_layout(info_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(info_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(info_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(info_container, CARD_ROW_SPACING, 0);

    // 删除状态标签，直接从定时开始

    // 第一行：定时状态标签
    card_thermal_timer_label = lv_label_create(info_container);
    lv_obj_add_style(card_thermal_timer_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_thermal_timer_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_thermal_timer_label, LV_PCT(100));
    lv_label_set_text(card_thermal_timer_label, "定时: 未启用");

    // 第二行：加热器状态标签
    card_thermal_heater_label = lv_label_create(info_container);
    lv_obj_add_style(card_thermal_heater_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_thermal_heater_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_thermal_heater_label, LV_PCT(100));
    lv_label_set_text(card_thermal_heater_label, "保温: 关闭");

    // 第三行：风机状态标签
    card_thermal_fan_label = lv_label_create(info_container);
    lv_obj_add_style(card_thermal_fan_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_thermal_fan_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_thermal_fan_label, LV_PCT(100));
    lv_label_set_text(card_thermal_fan_label, "风机: 关闭");

    // 开关容器 - 右侧垂直排列，绝对定位
    lv_obj_t* switches_container = lv_obj_create(content_area_card);
    lv_obj_remove_style_all(switches_container);
    lv_obj_set_size(switches_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_layout(switches_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(switches_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(switches_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(switches_container, 8, 0);  // 两个开关之间的间距
    
    // 绝对定位在右下角
    lv_obj_align(switches_container, LV_ALIGN_BOTTOM_RIGHT, -CARD_SWITCH_RIGHT_MARGIN, -CARD_SWITCH_BOTTOM_MARGIN);

    // 风机开关 - 上方
    card_thermal_fan_switch = lv_switch_create(switches_container);
    lv_obj_add_style(card_thermal_fan_switch, &style_switch_on_cyan, (lv_style_selector_t)(LV_PART_INDICATOR | LV_STATE_CHECKED));
    lv_obj_add_event_cb(card_thermal_fan_switch, thermal_card_fan_toggle_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    // 加热器开关 - 下方
    card_thermal_heater_switch = lv_switch_create(switches_container);
    lv_obj_add_style(card_thermal_heater_switch, &style_switch_on_cyan, (lv_style_selector_t)(LV_PART_INDICATOR | LV_STATE_CHECKED));
    lv_obj_add_event_cb(card_thermal_heater_switch, thermal_card_heater_toggle_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    ThermalSettings_RegisterStatusUpdateCallback(update_thermal_card_status);
    update_thermal_card_status();
    Serial.println("Thermal card UI created with vertical switches layout.");
}

void update_thermal_card_status() {
    // 更新定时状态标签
    if (card_thermal_timer_label && lv_obj_is_valid(card_thermal_timer_label)) {
        if (ThermalSettings_IsTimedSessionActive()) {
            char countdown_str[30];
            ThermalSettings_GetFormattedRemainingCountdown(countdown_str, sizeof(countdown_str));
            lv_label_set_text_fmt(card_thermal_timer_label, "定时关: %s", countdown_str);
        } else {
            lv_label_set_text(card_thermal_timer_label, "定时: 未启用");
        }
    }

    // 更新加热器状态标签
    if (card_thermal_heater_label && lv_obj_is_valid(card_thermal_heater_label)) {
        if (ThermalSettings_IsHeaterEnabled()) {
            lv_label_set_text(card_thermal_heater_label, "加热器: 开启");
        } else {
            lv_label_set_text(card_thermal_heater_label, "加热器: 关闭");
        }
    }

    // 更新风扇状态标签
    if (card_thermal_fan_label && lv_obj_is_valid(card_thermal_fan_label)) {
        if (ThermalSettings_IsFanEnabled()) {
            lv_label_set_text(card_thermal_fan_label, "风扇: 开启");
        } else {
            lv_label_set_text(card_thermal_fan_label, "风扇: 关闭");
        }
    }

    // 更新开关状态 - 重要：确保UI与逻辑状态同步
    if (card_thermal_heater_switch && lv_obj_is_valid(card_thermal_heater_switch)) {
        if (ThermalSettings_IsHeaterEnabled()) {
            lv_obj_add_state(card_thermal_heater_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(card_thermal_heater_switch, LV_STATE_CHECKED);
        }
    }

    if (card_thermal_fan_switch && lv_obj_is_valid(card_thermal_fan_switch)) {
        if (ThermalSettings_IsFanEnabled()) {
            lv_obj_add_state(card_thermal_fan_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(card_thermal_fan_switch, LV_STATE_CHECKED);
        }
    }
}
// File: src/Display/cards/ui_lighting_card.cpp
#include "ui_lighting_card.h"
#include "../UI_Styles.h"
#include "../../Config/Config.h"
#include "../../SettingsLogic/LightingSettings.h"
#include "../font/fengyahei_s16_bpp4.h" 
#include "../font/fengyahei_s16_bpp4.h" 
#include "../../App/AppGlobal.h"        // For CARD_ID_LIGHTING and card_titles, card_icons
#include "../UI_Common.h"         // For create_common_card_header
#include "../UI_ControlPage.h"    // For card_settings_event_handler_global
#include <Arduino.h>

// 卡片内部UI元素指针
static lv_obj_t *card_lighting_brightness_label = NULL;
static lv_obj_t *card_lighting_wakeup_status_label = NULL;
static lv_obj_t *card_lighting_wakeup_time_label = NULL;
static lv_obj_t *card_lighting_main_switch = NULL;

// 照明卡片总开关的事件回调
static void lighting_card_main_toggle_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *sw = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        bool is_checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
        Serial.printf("LightingCard: Main toggle changed to: %s\n", is_checked ? "ON" : "OFF");
        LightingSettings_HandleLightToggle(is_checked);
        update_lighting_card_status();
    }
}

// 注意：照明卡片右上角“设置”按钮的专属事件回调 lighting_card_settings_button_event_cb
// 现在不再需要了，因为我们使用了 UI_ControlPage.cpp 中的 card_settings_event_handler_global

void create_lighting_card_ui(lv_obj_t* parent_dashboard) {
    lv_obj_t* card = lv_obj_create(parent_dashboard);
    lv_obj_add_style(card, &style_card_main, 0);
    lv_obj_set_width(card, LV_PCT(48));
    lv_obj_set_height(card, CARD_FIXED_HEIGHT);
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(card, CARD_CONTENT_PADDING, 0);

    extern const char* card_titles[];
    extern const char* card_icons[];
    create_common_card_header(card, card_titles[CARD_ID_LIGHTING], card_icons[CARD_ID_LIGHTING], 
                             CARD_ID_LIGHTING, card_settings_event_handler_global);

    // 主内容容器
    lv_obj_t* content_area_card = lv_obj_create(card);
    lv_obj_remove_style_all(content_area_card);
    lv_obj_set_width(content_area_card, LV_PCT(100));
    lv_obj_set_flex_grow(content_area_card, 1);
    lv_obj_set_style_pad_all(content_area_card, CARD_CONTENT_INNER_PADDING, 0);


    // 文字信息区域
    lv_obj_t* info_display_container = lv_obj_create(content_area_card);
    lv_obj_remove_style_all(info_display_container);
    lv_obj_set_pos(info_display_container, 0, 0);
    lv_obj_set_size(info_display_container, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_right(info_display_container, CARD_TEXT_RIGHT_MARGIN, 0);
    lv_obj_set_layout(info_display_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(info_display_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(info_display_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(info_display_container, CARD_ROW_SPACING, 0);

    // 第一行：亮度信息标签
    card_lighting_brightness_label = lv_label_create(info_display_container);
    lv_obj_add_style(card_lighting_brightness_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_lighting_brightness_label, &fengyahei_s16_bpp4, 0);
    lv_label_set_long_mode(card_lighting_brightness_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(card_lighting_brightness_label, LV_PCT(100));
    lv_obj_set_style_text_align(card_lighting_brightness_label, LV_TEXT_ALIGN_LEFT, 0);

    // 第二行：唤醒状态标签
    card_lighting_wakeup_status_label = lv_label_create(info_display_container);
    lv_obj_add_style(card_lighting_wakeup_status_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_lighting_wakeup_status_label, &fengyahei_s16_bpp4, 0);
    lv_label_set_long_mode(card_lighting_wakeup_status_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(card_lighting_wakeup_status_label, LV_PCT(100));
    lv_obj_set_style_text_align(card_lighting_wakeup_status_label, LV_TEXT_ALIGN_LEFT, 0);

    // 第三行：唤醒时间标签
    card_lighting_wakeup_time_label = lv_label_create(info_display_container);
    lv_obj_add_style(card_lighting_wakeup_time_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_lighting_wakeup_time_label, &fengyahei_s16_bpp4, 0);
    lv_label_set_long_mode(card_lighting_wakeup_time_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(card_lighting_wakeup_time_label, LV_PCT(100));
    lv_obj_set_style_text_align(card_lighting_wakeup_time_label, LV_TEXT_ALIGN_LEFT, 0);

    // 开关 - 绝对定位
    card_lighting_main_switch = lv_switch_create(content_area_card);
    lv_obj_add_style(card_lighting_main_switch, &style_switch_on_cyan, static_cast<lv_style_selector_t>(LV_PART_INDICATOR) | static_cast<lv_style_selector_t>(LV_STATE_CHECKED));

    lv_obj_add_event_cb(card_lighting_main_switch, lighting_card_main_toggle_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    
    lv_obj_align(card_lighting_main_switch,LV_ALIGN_BOTTOM_RIGHT,-CARD_SWITCH_RIGHT_MARGIN,-CARD_SWITCH_BOTTOM_MARGIN);
    update_lighting_card_status();
    Serial.println("Lighting card UI created with absolute positioned switch.");
}

// update_lighting_card_status() 函数保持不变
void update_lighting_card_status() {
    if (!lv_obj_is_valid(card_lighting_brightness_label) ||
        !lv_obj_is_valid(card_lighting_wakeup_status_label) ||
        !lv_obj_is_valid(card_lighting_wakeup_time_label) ||
        !lv_obj_is_valid(card_lighting_main_switch)) {
        return;
    }

    char status_buffer[100];
    LightingSettings_GetStatusString(status_buffer, sizeof(status_buffer));

    char* line1 = strtok(status_buffer, "\n");
    char* line2 = strtok(NULL, "\n");
    char* line3 = strtok(NULL, "\n");

    if (line1) lv_label_set_text(card_lighting_brightness_label, line1);
    
    if (line2) {
        lv_label_set_text(card_lighting_wakeup_status_label, line2);
        lv_obj_clear_flag(card_lighting_wakeup_status_label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_label_set_text(card_lighting_wakeup_status_label, "");
        lv_obj_add_flag(card_lighting_wakeup_status_label, LV_OBJ_FLAG_HIDDEN);
    }

    if (line3 && LightingSettings_IsWakeupEnabled()) {
        lv_label_set_text(card_lighting_wakeup_time_label, line3);
        lv_obj_clear_flag(card_lighting_wakeup_time_label, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_label_set_text(card_lighting_wakeup_time_label, "");
        lv_obj_add_flag(card_lighting_wakeup_time_label, LV_OBJ_FLAG_HIDDEN);
    }

    if (LightingSettings_GetCurrentAppliedPwm() > 0) {
        lv_obj_add_state(card_lighting_main_switch, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(card_lighting_main_switch, LV_STATE_CHECKED);
    }
}

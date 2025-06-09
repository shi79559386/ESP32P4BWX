// src/Display/Popups/UI_LightingSettingsPopup.cpp

#include "UI_LightingSettingsPopup.h"
#include "UI_PopupBase.h"
#include "../UI_Styles.h"
#include "../../Config/Config.h"
#include "../../SettingsLogic/LightingSettings.h"
#include "../font/fengyahei_s16_bpp4.h"
#include "../cards/ui_lighting_card.h"

static lv_obj_t *lighting_popup_brightness_roller = NULL;
static lv_obj_t *lighting_popup_duration_min_roller = NULL;
static lv_obj_t *lighting_popup_duration_sec_roller = NULL;

static void lighting_popup_delete_cleanup_handler(lv_event_t *e) {
    lighting_popup_brightness_roller = NULL;
    lighting_popup_duration_min_roller = NULL;
    lighting_popup_duration_sec_roller = NULL;
}

static void brightness_roller_event_cb(lv_event_t *e) {
    lv_obj_t *roller = lv_event_get_target(e);
    if (!lv_obj_is_valid(roller)) return;
    uint16_t selected_idx = lv_roller_get_selected(roller);
    LightingSettings_SetLightLevel((light_level_t)selected_idx);
    update_lighting_card_status();
}

static void duration_roller_event_cb(lv_event_t *e) {
    if (lighting_popup_duration_min_roller && lv_obj_is_valid(lighting_popup_duration_min_roller) &&
        lighting_popup_duration_sec_roller && lv_obj_is_valid(lighting_popup_duration_sec_roller)) {
        uint16_t minutes = lv_roller_get_selected(lighting_popup_duration_min_roller);
        uint16_t seconds = lv_roller_get_selected(lighting_popup_duration_sec_roller);
        uint16_t total_seconds = minutes * 60 + seconds;
        LightingSettings_SetBrighteningDurationSeconds(total_seconds);
        LightingSettings_SetWakeupEnabled(total_seconds > 0);
        update_lighting_card_status();
    }
}

void create_lighting_settings_popup(lv_obj_t *parent_scr) {
    lv_obj_t *content_area = create_popup_frame(parent_scr, "设置"); // 标题保持 "设置"
    if (!content_area) return;

    lv_obj_t *popup_box = lv_obj_get_parent(content_area);
    lv_obj_t *popup_mask = lv_obj_get_parent(popup_box);
    if (popup_mask) {
        lv_obj_add_flag(popup_mask, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(popup_mask, lighting_popup_delete_cleanup_handler, LV_EVENT_DELETE, NULL);
    }

    // 主行容器，用于并列亮度区和唤醒时间区
    lv_obj_t *settings_row_cont = lv_obj_create(content_area);
    lv_obj_remove_style_all(settings_row_cont);
    lv_obj_set_width(settings_row_cont, LV_PCT(100));
    lv_obj_set_height(settings_row_cont, LV_SIZE_CONTENT);
    lv_obj_set_layout(settings_row_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(settings_row_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(settings_row_cont, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START); // 主轴均匀分布，交叉轴顶部对齐
    lv_obj_set_style_pad_column(settings_row_cont, 10, 0); // 亮度区和时间区之间的间隔

    // 定义协调的滚轮背景色 (与上次一致)
    lv_color_t roller_coordinated_bg_color = lv_color_hex(0x1A2B3C);

    // --- 1. 亮度设置区块 (垂直布局：标题 + 滚轮) ---
    lv_obj_t *brightness_section_cont = lv_obj_create(settings_row_cont);
    lv_obj_remove_style_all(brightness_section_cont);
    lv_obj_set_width(brightness_section_cont, LV_PCT(40)); // 调整宽度占比
    lv_obj_set_height(brightness_section_cont, LV_SIZE_CONTENT);
    lv_obj_set_layout(brightness_section_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(brightness_section_cont, LV_FLEX_FLOW_COLUMN); // 垂直排列标题和滚轮
    lv_obj_set_flex_align(brightness_section_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); // 子项水平居中
    lv_obj_set_style_pad_row(brightness_section_cont, 8, 0); // 标题和滚轮之间的垂直间隔

    lv_obj_t *brightness_title_label = lv_label_create(brightness_section_cont);
    lv_label_set_text(brightness_title_label, "亮度");
    lv_obj_add_style(brightness_title_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_align(brightness_title_label, LV_TEXT_ALIGN_CENTER, 0);

    lighting_popup_brightness_roller = lv_roller_create(brightness_section_cont);
    lv_obj_set_width(lighting_popup_brightness_roller, LV_PCT(80)); // 滚轮宽度相对于其父容器 brightness_section_cont
    lv_roller_set_options(lighting_popup_brightness_roller, "低\n中\n高", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(lighting_popup_brightness_roller, 3);
    lv_obj_add_style(lighting_popup_brightness_roller, &style_roller_options_custom, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(lighting_popup_brightness_roller, roller_coordinated_bg_color, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lighting_popup_brightness_roller, LV_OPA_COVER, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(lighting_popup_brightness_roller, 1, 0);
    lv_obj_set_style_border_color(lighting_popup_brightness_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_radius(lighting_popup_brightness_roller, 4, 0);
    lv_obj_set_style_text_color(lighting_popup_brightness_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED); // 最亮白色
    lv_obj_set_style_bg_color(lighting_popup_brightness_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED); // 淡蓝色选中框
    lv_obj_set_style_bg_opa(lighting_popup_brightness_roller, LV_OPA_COVER, LV_PART_SELECTED);
    lv_obj_set_style_text_font(lighting_popup_brightness_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);
    lv_roller_set_selected(lighting_popup_brightness_roller, (uint16_t)LightingSettings_GetLightLevel(), LV_ANIM_OFF);
    lv_obj_add_event_cb(lighting_popup_brightness_roller, brightness_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // --- 2. 唤醒时间设置区块 (垂直布局：标题 + （分钟滚轮+冒号+秒钟滚轮）行) ---
    lv_obj_t *duration_section_cont = lv_obj_create(settings_row_cont);
    lv_obj_remove_style_all(duration_section_cont);
    lv_obj_set_width(duration_section_cont, LV_PCT(55)); // 调整宽度占比
    lv_obj_set_height(duration_section_cont, LV_SIZE_CONTENT);
    lv_obj_set_layout(duration_section_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(duration_section_cont, LV_FLEX_FLOW_COLUMN); // 垂直排列标题和滚轮行
    lv_obj_set_flex_align(duration_section_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); // 子项水平居中
    lv_obj_set_style_pad_row(duration_section_cont, 8, 0); // 标题和滚轮行之间的垂直间隔

    lv_obj_t *duration_title_label = lv_label_create(duration_section_cont);
    lv_label_set_text(duration_title_label, "唤醒时间");
    lv_obj_add_style(duration_title_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_align(duration_title_label, LV_TEXT_ALIGN_CENTER, 0);

    // 新容器，用于水平排列分钟、冒号、秒钟滚轮
    lv_obj_t *actual_rollers_row_cont = lv_obj_create(duration_section_cont);
    lv_obj_remove_style_all(actual_rollers_row_cont);
    lv_obj_set_width(actual_rollers_row_cont, LV_PCT(100)); // 占满其父容器 duration_section_cont
    lv_obj_set_height(actual_rollers_row_cont, LV_SIZE_CONTENT);
    lv_obj_set_layout(actual_rollers_row_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(actual_rollers_row_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(actual_rollers_row_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(actual_rollers_row_cont, 5, 0); // 滚轮和冒号之间的间隙

    char roller_num_options[180];
    char *p = roller_num_options;
    for (int i = 0; i < 60; i++) {
        p += sprintf(p, "%02d", i);
        if (i < 59) *(p++) = '\n';
    }
    *p = '\0';

    lighting_popup_duration_min_roller = lv_roller_create(actual_rollers_row_cont);
    lv_obj_set_width(lighting_popup_duration_min_roller, LV_PCT(40));
    lv_roller_set_options(lighting_popup_duration_min_roller, roller_num_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(lighting_popup_duration_min_roller, 3);
    lv_obj_add_style(lighting_popup_duration_min_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(lighting_popup_duration_min_roller, roller_coordinated_bg_color, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lighting_popup_duration_min_roller, LV_OPA_COVER, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(lighting_popup_duration_min_roller, 1, 0);
    lv_obj_set_style_border_color(lighting_popup_duration_min_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_radius(lighting_popup_duration_min_roller, 4, 0);
    lv_obj_set_style_text_color(lighting_popup_duration_min_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED); // 最亮白色
    lv_obj_set_style_bg_color(lighting_popup_duration_min_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED); // 淡蓝色选中框
    lv_obj_set_style_bg_opa(lighting_popup_duration_min_roller, LV_OPA_COVER, LV_PART_SELECTED);
    lv_obj_set_style_text_font(lighting_popup_duration_min_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);

    lv_obj_t *colon_label = lv_label_create(actual_rollers_row_cont);
    lv_label_set_text(colon_label, ":");
    lv_obj_add_style(colon_label, &style_text_white_custom_font, 0);

    lighting_popup_duration_sec_roller = lv_roller_create(actual_rollers_row_cont);
    lv_obj_set_width(lighting_popup_duration_sec_roller, LV_PCT(40));
    lv_roller_set_options(lighting_popup_duration_sec_roller, roller_num_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(lighting_popup_duration_sec_roller, 3);
    lv_obj_add_style(lighting_popup_duration_sec_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(lighting_popup_duration_sec_roller, roller_coordinated_bg_color, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(lighting_popup_duration_sec_roller, LV_OPA_COVER, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lighting_popup_duration_sec_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(lighting_popup_duration_sec_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_set_style_bg_opa(lighting_popup_duration_sec_roller, LV_OPA_COVER, LV_PART_SELECTED);
    lv_obj_set_style_text_font(lighting_popup_duration_sec_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);

    uint16_t current_total_s = LightingSettings_GetBrighteningDurationSeconds();
    lv_roller_set_selected(lighting_popup_duration_min_roller, current_total_s / 60, LV_ANIM_OFF);
    lv_roller_set_selected(lighting_popup_duration_sec_roller, current_total_s % 60, LV_ANIM_OFF);

    lv_obj_add_event_cb(lighting_popup_duration_min_roller, duration_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(lighting_popup_duration_sec_roller, duration_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    duration_roller_event_cb(NULL);
}
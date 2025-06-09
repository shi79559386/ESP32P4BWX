// File: src/Display/Popups/UI_HumidifySettingsPopup.cpp
#include "UI_HumidifySettingsPopup.h"
#include "UI_PopupBase.h"
#include "../UI_Styles.h"
#include "../font/fengyahei_s16_bpp4.h"
#include "../../Config/Config.h"
#include "../../App/AppGlobal.h"
#include <Arduino.h>
#include <stdio.h>

static lv_obj_t *humidify_enable_switch = NULL;
static lv_obj_t *humidify_left_roller = NULL;
static lv_obj_t *humidify_right_roller = NULL;
static lv_obj_t *humidify_tolerance_roller = NULL;

static char humidify_options[300];
static char humidify_tolerance_options[50];

static void build_humidify_roller_options() {
    // 构建湿度选项 (10% - 90%，步长1%)  // 修改：从30%改为10%
    humidify_options[0] = '\0';
    char *p_hum = humidify_options;
    for (int i = 10; i <= 90; i++) {  // 修改：从10开始
        p_hum += sprintf(p_hum, "%d%%", i);
        if (i < 90) *(p_hum++) = '\n';
    }
    *p_hum = '\0';
    
    // 构建容差选项 (1% - 10%，步长1%)
    humidify_tolerance_options[0] = '\0';
    char *p_tol = humidify_tolerance_options;
    for (int i = 1; i <= 10; i++) {
        p_tol += sprintf(p_tol, "%d%%", i);
        if (i < 10) *(p_tol++) = '\n';
    }
    *p_tol = '\0';
}

static void humidify_popup_delete_cleanup_handler(lv_event_t *e) {
    humidify_left_roller = NULL;
    humidify_right_roller = NULL;
    humidify_tolerance_roller = NULL;
}

static void humidify_enable_switch_event_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    Serial.printf("Humidify enabled: %s\n", enabled ? "true" : "false");
}

static void humidify_roller_event_cb(lv_event_t *e) {
    if (lv_obj_is_valid(humidify_left_roller) && lv_obj_is_valid(humidify_right_roller)) {
        int left_idx = lv_roller_get_selected(humidify_left_roller);
        int right_idx = lv_roller_get_selected(humidify_right_roller);
        
        int left_humidity = 10 + left_idx;   // 修改：从10%开始
        int right_humidity = 10 + right_idx; // 修改：从10%开始
        
        g_target_humidity_left = left_humidity;
        g_target_humidity_right = right_humidity;
        
        Serial.printf("Target humidity: Left=%d%%, Right=%d%%\n", left_humidity, right_humidity);
    }
}

void create_humidify_settings_popup(lv_obj_t *parent_scr) {
    build_humidify_roller_options();
    lv_obj_t *content_area = create_popup_frame(parent_scr, "设置");
    if (!content_area) return;

    lv_obj_t *popup_box = lv_obj_get_parent(content_area);
    lv_obj_t *popup_mask = lv_obj_get_parent(popup_box);
    if (popup_mask) {
        lv_obj_add_flag(popup_mask, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(popup_mask, humidify_popup_delete_cleanup_handler, LV_EVENT_DELETE, NULL);
    }

    // === 湿度设置区域（左右并排布局）===
    lv_obj_t *humidity_section = lv_obj_create(content_area);
    lv_obj_remove_style_all(humidity_section);
    lv_obj_set_width(humidity_section, LV_PCT(100));
    lv_obj_set_height(humidity_section, LV_SIZE_CONTENT);
    lv_obj_set_layout(humidity_section, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(humidity_section, LV_FLEX_FLOW_ROW);  // 水平排列
    // 修改：从 SPACE_EVENLY 改为 CENTER，让左右区聚集在中间
    lv_obj_set_flex_align(humidity_section, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(humidity_section, 0, 0);
    // 添加：设置左右区之间的间距
    lv_obj_set_style_pad_column(humidity_section, 20, 0);  // 调整这个值来控制距离，越小越近

    // === 左区域容器 ===
    lv_obj_t *left_humidity_container = lv_obj_create(humidity_section);
    lv_obj_remove_style_all(left_humidity_container);
    lv_obj_set_width(left_humidity_container, LV_SIZE_CONTENT);
    lv_obj_set_height(left_humidity_container, LV_SIZE_CONTENT);
    lv_obj_set_layout(left_humidity_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(left_humidity_container, LV_FLEX_FLOW_COLUMN);  // 垂直排列
    lv_obj_set_flex_align(left_humidity_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(left_humidity_container, 10, 0);  // 文字和滚轮间距

    // 左区文字标签
    lv_obj_t *left_humidity_label = lv_label_create(left_humidity_container);
    lv_label_set_text(left_humidity_label, "左区");
    lv_obj_add_style(left_humidity_label, &style_text_white_custom_font, 0);

    // 左区滚轮
    humidify_left_roller = lv_roller_create(left_humidity_container);
    lv_obj_set_width(humidify_left_roller, 80);
    lv_roller_set_options(humidify_left_roller, humidify_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(humidify_left_roller, 3);
    lv_obj_add_style(humidify_left_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(humidify_left_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_border_color(humidify_left_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(humidify_left_roller, 1, 0);
    lv_obj_set_style_radius(humidify_left_roller, 4, 0);
    lv_obj_set_style_text_color(humidify_left_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(humidify_left_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_set_style_text_font(humidify_left_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);
    lv_obj_add_event_cb(humidify_left_roller, humidify_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // === 右区域容器 ===
    lv_obj_t *right_humidity_container = lv_obj_create(humidity_section);
    lv_obj_remove_style_all(right_humidity_container);
    lv_obj_set_width(right_humidity_container, LV_SIZE_CONTENT);
    lv_obj_set_height(right_humidity_container, LV_SIZE_CONTENT);
    lv_obj_set_layout(right_humidity_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(right_humidity_container, LV_FLEX_FLOW_COLUMN);  // 垂直排列
    lv_obj_set_flex_align(right_humidity_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(right_humidity_container, 10, 0);  // 文字和滚轮间距

    // 右区文字标签
    lv_obj_t *right_humidity_label = lv_label_create(right_humidity_container);
    lv_label_set_text(right_humidity_label, "右区");
    lv_obj_add_style(right_humidity_label, &style_text_white_custom_font, 0);

    // 右区滚轮
    humidify_right_roller = lv_roller_create(right_humidity_container);
    lv_obj_set_width(humidify_right_roller, 80);
    lv_roller_set_options(humidify_right_roller, humidify_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(humidify_right_roller, 3);
    lv_obj_add_style(humidify_right_roller, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(humidify_right_roller, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_border_color(humidify_right_roller, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(humidify_right_roller, 1, 0);
    lv_obj_set_style_radius(humidify_right_roller, 4, 0);
    lv_obj_set_style_text_color(humidify_right_roller, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(humidify_right_roller, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_set_style_text_font(humidify_right_roller, &fengyahei_s16_bpp4, LV_PART_SELECTED);
    lv_obj_add_event_cb(humidify_right_roller, humidify_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // === 初始化滚轮值 ===
    int left_idx = g_target_humidity_left - 10;   // 修改：从30改为10
    int right_idx = g_target_humidity_right - 10; // 修改：从30改为10
    if (left_idx < 0) left_idx = 50; // 默认60% (索引50 = 60%)
    if (right_idx < 0) right_idx = 50; // 默认60% (索引50 = 60%)
    
    lv_roller_set_selected(humidify_left_roller, left_idx, LV_ANIM_OFF);
    lv_roller_set_selected(humidify_right_roller, right_idx, LV_ANIM_OFF);
}
// File: src/Display/UI_SystemSettingsPage.cpp
#include "lvgl.h"
#include <stdio.h>
#include <string.h>
#include "UI_SystemSettingPage.h"      // Renamed
#include "font/fengyahei_s16_bpp4.h"    // Included from original

#include "UI_Styles.h"
#include "UI_Common.h"
#include "../App/AppGlobal.h"           // Adjusted path
#include "../Config/Config.h"
#include "../SettingsLogic/SystemSettings.h" // Include new settings logic header

// Page-specific styles (definitions remain the same as in your original UI_SettingPage.cpp)
static lv_style_t style_setting_page_card;
// ... (all other static style definitions from original file)
static lv_style_t style_setting_section_title;
static lv_style_t style_setting_btn_default;
static lv_style_t style_setting_btn_pressed;
static lv_style_t style_setting_btn_checked;
static lv_style_t style_setting_status_text;
static lv_style_t style_setting_page_title_text;
static lv_style_t style_setting_page_title_bar;


// --- Screen Brightness Control Related (UI part) ---
// static int current_brightness_level_new = 2; // This will now be managed by SettingsLogic or internal UI state
static lv_obj_t *brightness_buttons_new[3];
const char *brightness_texts_new[] = { "低", "中", "高" };

// platform_set_screen_brightness_hw_new is now in SettingsLogic/SystemSettings.cpp

// --- WiFi Related (UI part) ---
static lv_obj_t *label_wifi_status_new = NULL;
// app_update_wifi_connection_status_ui_new and its related static char arrays
// should be moved to SettingsLogic/SystemSettings.cpp if their logic is retained.
// For now, assuming WiFi section is "待开发中" and UI only displays this.

static void init_setting_page_styles() {
    // (Style initializations remain the same as in your original UI_SettingPage.cpp)
    // --- 页面标题栏样式 ---
    lv_style_init(&style_setting_page_title_bar);
    lv_style_set_bg_color(&style_setting_page_title_bar, SCREEN_BG_COLOR);
    lv_style_set_bg_opa(&style_setting_page_title_bar, LV_OPA_COVER);
    lv_style_set_radius(&style_setting_page_title_bar, 0);
    lv_style_set_pad_ver(&style_setting_page_title_bar, 8);
    lv_style_set_pad_hor(&style_setting_page_title_bar, 10);
    lv_style_set_border_width(&style_setting_page_title_bar, 0);

    // --- 页面大标题文本样式 ---
    lv_style_init(&style_setting_page_title_text);
    lv_style_set_text_font(&style_setting_page_title_text, &fengyahei_s16_bpp4);
    lv_style_set_text_color(&style_setting_page_title_text, TEXT_COLOR_WHITE);

    // --- 设置项卡片样式 ---
    lv_style_init(&style_setting_page_card);
    lv_style_set_bg_color(&style_setting_page_card, PANEL_BG_COLOR);
    lv_style_set_bg_opa(&style_setting_page_card, LV_OPA_COVER);
    lv_style_set_radius(&style_setting_page_card, 8);
    lv_style_set_border_color(&style_setting_page_card, lv_color_hex(0x354B68));
    lv_style_set_border_width(&style_setting_page_card, 1);
    lv_style_set_shadow_width(&style_setting_page_card, 10);
    lv_style_set_shadow_opa(&style_setting_page_card, LV_OPA_10); 
    lv_style_set_shadow_ofs_y(&style_setting_page_card, 5);
    lv_style_set_shadow_color(&style_setting_page_card, lv_color_black());
    lv_style_set_pad_all(&style_setting_page_card, 12); 

    // --- 卡片内区域标题样式 ---
    lv_style_init(&style_setting_section_title);
    lv_style_set_text_font(&style_setting_section_title, &fengyahei_s16_bpp4);
    lv_style_set_text_color(&style_setting_section_title, TEXT_COLOR_WHITE);
    lv_style_set_pad_bottom(&style_setting_section_title, 8); 

    // --- 按钮默认状态样式 ---
    lv_style_init(&style_setting_btn_default);
    lv_style_set_radius(&style_setting_btn_default, 5);
    lv_style_set_bg_opa(&style_setting_btn_default, LV_OPA_COVER);
    lv_style_set_bg_color(&style_setting_btn_default, BUTTON_BG_COLOR);
    lv_style_set_bg_grad_color(&style_setting_btn_default, lv_color_darken(BUTTON_BG_COLOR, LV_OPA_30));
    lv_style_set_bg_grad_dir(&style_setting_btn_default, LV_GRAD_DIR_VER);
    lv_style_set_border_opa(&style_setting_btn_default, LV_OPA_40);
    lv_style_set_border_width(&style_setting_btn_default, 1);
    lv_style_set_border_color(&style_setting_btn_default, lv_color_darken(BUTTON_BG_COLOR, LV_OPA_50));
    lv_style_set_shadow_width(&style_setting_btn_default, 6);
    lv_style_set_shadow_color(&style_setting_btn_default, lv_color_hex(0x333333));
    lv_style_set_shadow_ofs_y(&style_setting_btn_default, 4);
    lv_style_set_text_color(&style_setting_btn_default, TEXT_COLOR_WHITE);
    lv_style_set_text_font(&style_setting_btn_default, &fengyahei_s16_bpp4);
    lv_style_set_pad_ver(&style_setting_btn_default, 6); 
    lv_style_set_pad_hor(&style_setting_btn_default, 10); 

    // --- 按钮按下状态样式 ---
    lv_style_init(&style_setting_btn_pressed);
    // (Copy relevant properties from original style_btn_animated_pressed or UI_SettingPage's version)
    lv_style_set_radius(&style_setting_btn_pressed, 5);
    lv_style_set_text_color(&style_setting_btn_pressed, TEXT_COLOR_WHITE);
    lv_style_set_text_font(&style_setting_btn_pressed, &fengyahei_s16_bpp4);
    lv_style_set_pad_ver(&style_setting_btn_pressed, 6);
    lv_style_set_pad_hor(&style_setting_btn_pressed, 10);
    lv_style_set_bg_color(&style_setting_btn_pressed, lv_color_darken(BUTTON_BG_COLOR, LV_OPA_30));
    lv_style_set_bg_grad_color(&style_setting_btn_pressed, lv_color_darken(BUTTON_BG_COLOR, LV_OPA_50));
    lv_style_set_shadow_ofs_y(&style_setting_btn_pressed, 1);


    // --- 亮度按钮选中状态样式 ---
    lv_style_init(&style_setting_btn_checked);
    lv_style_set_radius(&style_setting_btn_checked, 5);
    lv_style_set_bg_opa(&style_setting_btn_checked, LV_OPA_COVER);
    lv_style_set_text_color(&style_setting_btn_checked, TEXT_COLOR_WHITE);
    lv_style_set_text_font(&style_setting_btn_checked, &fengyahei_s16_bpp4);
    lv_style_set_pad_ver(&style_setting_btn_checked, 6);
    lv_style_set_pad_hor(&style_setting_btn_checked, 10);
    lv_style_set_bg_color(&style_setting_btn_checked, lv_color_hex(0x27AE60)); // Green for checked
    lv_style_set_border_width(&style_setting_btn_checked, 2);
    lv_style_set_border_color(&style_setting_btn_checked, lv_color_hex(0x2ECC71));

    // --- WiFi状态文本样式 / "项目待开发中" 提示 ---
    lv_style_init(&style_setting_status_text);
    lv_style_set_text_font(&style_setting_status_text, &fengyahei_s16_bpp4);
    lv_style_set_text_color(&style_setting_status_text, lv_color_mix(TEXT_COLOR_WHITE, SCREEN_BG_COLOR, LV_OPA_80));
}


static void update_brightness_buttons_ui_new() {
    int current_level_from_logic = SystemSettings_GetBrightnessLevel();
    for (int i = 0; i < 3; ++i) {
        if (brightness_buttons_new[i] && lv_obj_is_valid(brightness_buttons_new[i])) {
            if (i == current_level_from_logic) {
                lv_obj_add_state(brightness_buttons_new[i], LV_STATE_CHECKED);
            } else {
                lv_obj_clear_state(brightness_buttons_new[i], LV_STATE_CHECKED);
            }
        }
    }
}

static void brightness_btn_event_handler_new(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        int level_idx = (int)(intptr_t)lv_obj_get_user_data(btn); // This is the button index (0, 1, or 2)
        SystemSettings_SetBrightnessLevel(level_idx); // Call logic layer to set brightness
        update_brightness_buttons_ui_new(); // Update UI based on new state from logic layer
    }
}


static lv_obj_t* create_page_title_bar_setting(lv_obj_t *parent, const char* title_text) {
    // (Function remains the same as in your original UI_SettingPage.cpp)
    lv_obj_t *title_bar = lv_obj_create(parent);
    lv_obj_remove_style_all(title_bar);
    lv_obj_add_style(title_bar, &style_setting_page_title_bar, 0);
    lv_obj_set_size(title_bar, LV_PCT(100), LV_SIZE_CONTENT);

    lv_obj_t *title_label = lv_label_create(title_bar);
    lv_obj_add_style(title_label, &style_setting_page_title_text, 0);
    lv_label_set_text(title_label, title_text);
    lv_obj_center(title_label);

    return title_bar;
}

static lv_obj_t* create_brightness_section_ui_new(lv_obj_t *parent) {
    // (Card creation and title label remain similar)
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_remove_style_all(card);
    lv_obj_add_style(card, &style_setting_page_card, 0);
    lv_obj_set_width(card, LV_PCT(100));
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(card, 8, LV_PART_MAIN);

    lv_obj_t *title = lv_label_create(card);
    lv_obj_add_style(title, &style_setting_section_title, 0);
    lv_label_set_text(title, "屏幕亮度");

    lv_obj_t *btn_container = lv_obj_create(card);
    // ... (btn_container setup as before)
    lv_obj_remove_style_all(btn_container);
    lv_obj_set_width(btn_container, LV_PCT(100));
    lv_obj_set_height(btn_container, LV_SIZE_CONTENT);
    lv_obj_set_layout(btn_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(btn_container, 5, LV_PART_MAIN);


    for (int i = 0; i < 3; ++i) {
        brightness_buttons_new[i] = lv_btn_create(btn_container);
        lv_obj_add_style(brightness_buttons_new[i], &style_setting_btn_default, LV_STATE_DEFAULT);
        lv_obj_add_style(brightness_buttons_new[i], &style_setting_btn_pressed, LV_STATE_PRESSED);
        lv_obj_add_style(brightness_buttons_new[i], &style_setting_btn_checked, LV_STATE_CHECKED);
        // For combined states like CHECKED | PRESSED, ensure pressed style overrides CHECKED bg_color if needed, or define a specific style
        lv_obj_add_style(brightness_buttons_new[i], &style_setting_btn_pressed, LV_STATE_CHECKED | LV_STATE_PRESSED);


        lv_obj_add_flag(brightness_buttons_new[i], LV_OBJ_FLAG_CHECKABLE);
        lv_obj_set_user_data(brightness_buttons_new[i], (void*)(intptr_t)i);
        lv_obj_add_event_cb(brightness_buttons_new[i], brightness_btn_event_handler_new, LV_EVENT_CLICKED, NULL);

        lv_obj_t *label = lv_label_create(brightness_buttons_new[i]);
        lv_label_set_text(label, brightness_texts_new[i]);
        lv_obj_center(label);
        lv_obj_set_style_min_width(brightness_buttons_new[i], 70, 0);
    }
    // Initial UI update for brightness buttons based on logic layer's current setting
    update_brightness_buttons_ui_new();
    return card;
}

static lv_obj_t* create_wifi_section_ui_new(lv_obj_t *parent) {
    // (This section remains largely the same, displaying "项目待开发中")
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_remove_style_all(card);
    lv_obj_add_style(card, &style_setting_page_card, 0);
    lv_obj_set_width(card, LV_PCT(100));
    lv_obj_set_height(card, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(card, 15, LV_PART_MAIN);

    lv_obj_t *title = lv_label_create(card);
    lv_obj_add_style(title, &style_setting_section_title, 0);
    lv_label_set_text(title, "WIFI");

    label_wifi_status_new = lv_label_create(card); 
    lv_obj_add_style(label_wifi_status_new, &style_setting_status_text, 0); 
    lv_label_set_text(label_wifi_status_new, "项目待开发中");
    lv_obj_set_width(label_wifi_status_new, LV_PCT(100)); 
    lv_obj_set_style_text_align(label_wifi_status_new, LV_TEXT_ALIGN_CENTER, 0); 
    return card;
}

void create_setting_page_ui(lv_obj_t *screen) { // Or create_system_settings_page_ui
    static bool styles_initialized_local = false;
    if (!styles_initialized_local) {
        init_setting_page_styles();
        styles_initialized_local = true;
    }

    // (Rest of the function remains the same as in your original UI_SettingPage.cpp,
    // creating the screen, main_content_container, content_area, and adding sections)
    // Just ensure it calls the correct create_brightness_section_ui_new and create_wifi_section_ui_new.
     if (style_scr_bg.prop_cnt > 0) { 
        lv_obj_add_style(screen, &style_scr_bg, 0); 
    } else {
        printf("警告: 全局 style_scr_bg 未初始化, 系统设置页面将使用备用背景!\n");
        lv_obj_set_style_bg_color(screen, SCREEN_BG_COLOR, 0);
        lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    }
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);

    lv_obj_t *main_content_container = lv_obj_create(screen);
    lv_obj_remove_style_all(main_content_container);
    lv_obj_set_width(main_content_container, LV_PCT(100));
    lv_obj_set_flex_grow(main_content_container, 1);
    lv_obj_set_layout(main_content_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(main_content_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_content_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    create_page_title_bar_setting(main_content_container, "系统设置");

    lv_obj_t *content_area = lv_obj_create(main_content_container);
    lv_obj_remove_style_all(content_area);
    lv_obj_set_width(content_area, LV_PCT(100));
    lv_obj_set_flex_grow(content_area, 1);
    lv_obj_set_style_pad_all(content_area, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(content_area, 15, LV_PART_MAIN);
    lv_obj_set_layout(content_area, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(content_area, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content_area, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);  
    lv_obj_add_flag(content_area, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(content_area, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_clear_flag(content_area, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(content_area, LV_OBJ_FLAG_SCROLL_ELASTIC);

    create_brightness_section_ui_new(content_area);
    create_wifi_section_ui_new(content_area);
    
    create_common_bottom_nav(screen, SCREEN_ID_SETTING);

    printf("UI_SystemSettingsPage: 系统设置页面UI已创建。\n");
}
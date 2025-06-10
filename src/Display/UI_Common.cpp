// File: src/Display/UI_Common.cpp
#include "UI_Common.h"
#include "../Config/Config.h"
#include "UI_Styles.h"
#include "font/fengyahei_s22_bpp4.h"
#include <Arduino.h>
#include "../App/AppGlobal.h" // For screen_id_t and control_card_id_t
#include "UI_ControlPage.h" // For card_settings_event_handler_global declaration

// 底部导航按钮的文本
static const char *bottom_nav_btn_map[] = {"主页", "控制", "设置", ""}; // LV_BTNMATRIX_CTRL_HIDDEN for the last empty string

// 导航栏按钮点击事件处理回调
static void bottom_nav_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        lv_obj_t *current_active_screen = lv_scr_act();

        switch (id) {
            case 0:
                if (screen_main && current_active_screen != screen_main) {
                    lv_disp_load_scr(screen_main);
                }
                break;
            case 1:
                if (screen_control && current_active_screen != screen_control) {
                    lv_disp_load_scr(screen_control);
                }
                break;
            case 2:
                if (screen_setting) {
                    if (current_active_screen != screen_setting) lv_disp_load_scr(screen_setting);
                } else {
                    Serial.println("UI_Common: screen_setting is NULL, cannot navigate.");
                    lv_obj_t * cur_scr = lv_scr_act();
                    if (cur_scr) {
                        static lv_obj_t *popup_msg_obj = NULL;
                        if (popup_msg_obj && lv_obj_is_valid(popup_msg_obj)) {
                           lv_obj_del_async(popup_msg_obj);
                           popup_msg_obj = NULL;
                        }
                        popup_msg_obj = lv_label_create(cur_scr);
                        lv_label_set_text(popup_msg_obj, "设置页面正在开发中...");
                        // Ensure style_text_white_custom_font is initialized before use
                        if (style_text_white_custom_font.prop_cnt > 0 ) {
                            lv_obj_add_style(popup_msg_obj, &style_text_white_custom_font, 0);
                        } else { // Fallback style
                            lv_obj_set_style_text_font(popup_msg_obj, &fengyahei_s22_bpp4, 0);
                            lv_obj_set_style_text_color(popup_msg_obj, lv_color_white(), 0);
                        }
                        lv_obj_set_style_bg_color(popup_msg_obj, lv_color_black(), 0);
                        lv_obj_set_style_bg_opa(popup_msg_obj, LV_OPA_70, 0);
                        lv_obj_set_style_pad_all(popup_msg_obj, 10, 0);
                        lv_obj_set_style_radius(popup_msg_obj, 5, 0);
                        lv_obj_center(popup_msg_obj);
                        lv_obj_add_flag(popup_msg_obj, LV_OBJ_FLAG_CLICKABLE);
                        lv_obj_add_event_cb(popup_msg_obj, [](lv_event_t* evt_popup){
                            lv_obj_t* target = lv_event_get_target(evt_popup);
                            if (target) lv_obj_del_async(target);
                            // popup_msg_obj = NULL; // Be careful with static vars in lambdas if re-entering
                        }, LV_EVENT_CLICKED, NULL);
                    }
                }
                break;
        }
    }
}

static bool common_nav_styles_initialized = false;
static lv_style_t style_btnm_bg;
static lv_style_t style_btnm_items_default;
static lv_style_t style_btnm_items_checked;

static void init_common_nav_styles_if_needed() {
    if (common_nav_styles_initialized) return;

    const lv_coord_t font_height_approx = 18; 
    const lv_coord_t indicator_height = 3;   

    lv_style_init(&style_btnm_bg);
    lv_style_set_radius(&style_btnm_bg, 0);
    lv_style_set_bg_color(&style_btnm_bg, lv_color_hex(0x0A1931)); // 深蓝色背景
    lv_style_set_bg_opa(&style_btnm_bg, LV_OPA_COVER);
    lv_style_set_pad_all(&style_btnm_bg, 0); // 无内边距
    lv_style_set_border_width(&style_btnm_bg, 0); // 无边框

    lv_style_init(&style_btnm_items_default);
    lv_style_set_bg_opa(&style_btnm_items_default, LV_OPA_TRANSP); // 按钮项背景透明
    lv_style_set_text_font(&style_btnm_items_default, &fengyahei_s22_bpp4);
    lv_style_set_text_color(&style_btnm_items_default, lv_color_hex(0xCCCCCC)); // 默认文字颜色
    lv_style_set_radius(&style_btnm_items_default, 0);
    lv_style_set_border_width(&style_btnm_items_default, 0);
    lv_style_set_outline_width(&style_btnm_items_default, 0);
    lv_style_set_shadow_width(&style_btnm_items_default, 0);

    // 确保文本垂直居中
    lv_coord_t default_total_vertical_space_for_text = NAV_BAR_HEIGHT - font_height_approx;
    if (default_total_vertical_space_for_text < 0) default_total_vertical_space_for_text = 0;
    lv_coord_t default_pad_top = default_total_vertical_space_for_text / 2;
    // Pad bottom might need adjustment if indicator is present and on bottom
    lv_coord_t default_pad_bottom = default_total_vertical_space_for_text - default_pad_top;
    lv_style_set_pad_top(&style_btnm_items_default, default_pad_top);
    lv_style_set_pad_bottom(&style_btnm_items_default, default_pad_bottom);
    lv_style_set_pad_left(&style_btnm_items_default, 5); // 左右留一些边距
    lv_style_set_pad_right(&style_btnm_items_default, 5);

lv_style_init(&style_btnm_items_checked);
lv_style_set_text_font(&style_btnm_items_checked, &fengyahei_s22_bpp4);
lv_style_set_text_color(&style_btnm_items_checked, lv_color_white());
lv_style_set_bg_opa(&style_btnm_items_checked, LV_OPA_TRANSP);
lv_style_set_radius(&style_btnm_items_checked, 0);
lv_style_set_outline_width(&style_btnm_items_checked, 0);
lv_style_set_shadow_width(&style_btnm_items_checked, 0);

// ★★★ 关键修复 (2/3) ★★★
// 确保选中状态和默认状态的内边距一致，这样文字才不会上下跳动
// (直接复用上面为 style_btnm_items_default 计算好的值)
lv_style_set_pad_top(&style_btnm_items_checked, default_pad_top);
lv_style_set_pad_bottom(&style_btnm_items_checked, default_pad_bottom);
lv_style_set_pad_left(&style_btnm_items_checked, 5);
lv_style_set_pad_right(&style_btnm_items_checked, 5);

// ★★★ 关键修复 (3/3) ★★★
// 底部指示器 (这部分保持不变，但现在它的位置会是正确的)
lv_style_set_border_color(&style_btnm_items_checked, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
lv_style_set_border_width(&style_btnm_items_checked, indicator_height);
lv_style_set_border_side(&style_btnm_items_checked, LV_BORDER_SIDE_BOTTOM);
lv_style_set_border_opa(&style_btnm_items_checked, LV_OPA_COVER);


    common_nav_styles_initialized = true;
}

void create_common_bottom_nav(lv_obj_t *parent_screen, screen_id_t active_screen_id) {
    // 1. 初始化样式
    init_common_nav_styles_if_needed();

    // ★ 稳定性增强：使用 lv_disp_get_hor_res 获取可靠的屏幕宽度 ★
    const lv_coord_t SCREEN_W = lv_disp_get_hor_res(NULL);
    const lv_coord_t LOGO_AREA_WIDTH = 80;

    // 2. 创建 Flexbox 容器 (恢复)
    lv_obj_t *nav_flex_container = lv_obj_create(parent_screen);
    lv_obj_remove_style_all(nav_flex_container);
    lv_obj_set_size(nav_flex_container, SCREEN_W, NAV_BAR_HEIGHT);
    lv_obj_align(nav_flex_container, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_flex_flow(nav_flex_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(nav_flex_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_START); // 垂直居中对齐
    lv_obj_add_style(nav_flex_container, &style_btnm_bg, 0);


    // 3. 创建左侧的 Logo 占位区域 (恢复)
    lv_obj_t *logo_placeholder = lv_obj_create(nav_flex_container);
    lv_obj_remove_style_all(logo_placeholder);
    lv_obj_set_size(logo_placeholder, LOGO_AREA_WIDTH, LV_PCT(100));


    // 4. 创建按钮矩阵，并让它自动填充剩余空间 (恢复)
    lv_obj_t *btnm = lv_btnmatrix_create(nav_flex_container);
    lv_btnmatrix_set_map(btnm, bottom_nav_btn_map);
    lv_obj_set_height(btnm, LV_PCT(100));
    lv_obj_set_flex_grow(btnm, 1); // 关键：让按钮矩阵占据所有剩余宽度


    // 5. 应用按钮的样式 (恢复)
    lv_obj_set_style_bg_opa(btnm, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btnm, 0, 0);
    lv_obj_add_style(btnm, &style_btnm_items_default, (lv_style_selector_t)(LV_PART_ITEMS | LV_STATE_DEFAULT));
    lv_obj_add_style(btnm, &style_btnm_items_checked, (lv_style_selector_t)(LV_PART_ITEMS | LV_STATE_CHECKED));


    // 6. 设置初始状态和事件 (恢复)
    lv_btnmatrix_set_one_checked(btnm, true);
    uint32_t initial_selected_id = (uint32_t)active_screen_id;
    const uint32_t button_count = sizeof(bottom_nav_btn_map)/sizeof(bottom_nav_btn_map[0]) -1;

    if (initial_selected_id < button_count) {
        lv_btnmatrix_set_btn_ctrl(btnm, initial_selected_id, LV_BTNMATRIX_CTRL_CHECKED);
    } else {
        Serial.printf("Warning: Invalid initial_selected_id (%u) for bottom_nav btnmatrix. Defaulting to 0.\n", initial_selected_id);
        if (button_count > 0) lv_btnmatrix_set_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKED);
    }

    lv_obj_add_event_cb(btnm, bottom_nav_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
}

// 创建通用卡片头部的函数实现
lv_obj_t* create_common_card_header(lv_obj_t* parent_card, const char* title_text, const char* icon_symbol, int card_id_val, lv_event_cb_t settings_event_cb) {
    lv_obj_t* title_row = lv_obj_create(parent_card);
    lv_obj_remove_style_all(title_row);
    lv_obj_set_width(title_row, LV_PCT(100));
    lv_obj_set_height(title_row, LV_SIZE_CONTENT);
    lv_obj_set_layout(title_row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(title_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(title_row, 5, 0);
    lv_obj_set_style_pad_bottom(title_row, 5, 0); // 标题行和下面内容区域的间距

    if (icon_symbol && strlen(icon_symbol) > 0) {
        lv_obj_t* icon = lv_label_create(title_row);
        lv_label_set_text(icon, icon_symbol);
        // 确保 style_card_icon 已初始化
        if (style_card_icon.prop_cnt > 0 ) {
             lv_obj_add_style(icon, &style_card_icon, 0);
        }
        lv_obj_set_style_text_font(icon, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(icon, lv_color_white(), 0);
    }

    lv_obj_t* title_label_obj = lv_label_create(title_row);
    lv_label_set_text(title_label_obj, title_text);
    // 确保 style_card_title_label 已初始化
    if (style_card_title_label.prop_cnt > 0 ) {
        lv_obj_add_style(title_label_obj, &style_card_title_label, 0);
    } else { // Fallback
        lv_obj_set_style_text_font(title_label_obj, &fengyahei_s16_bpp4, 0);
        lv_obj_set_style_text_color(title_label_obj, lv_color_white(), 0);
    }
    lv_obj_set_flex_grow(title_label_obj, 1);

    lv_obj_t* settings_btn = lv_btn_create(title_row);
    lv_obj_remove_style_all(settings_btn);
    lv_obj_set_size(settings_btn, 35, 35);
    // 确保 style_btn_animated_default 和 style_btn_animated_pressed 已初始化
    if((style_btn_animated_default.prop_cnt > 0 ) &&
       (style_btn_animated_pressed.prop_cnt > 0 ) ) {
        lv_obj_add_style(settings_btn, &style_btn_animated_default, LV_STATE_DEFAULT);
        lv_obj_add_style(settings_btn, &style_btn_animated_pressed, LV_STATE_PRESSED);
    }
    lv_obj_set_style_bg_opa(settings_btn, LV_OPA_TRANSP, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(settings_btn, LV_OPA_TRANSP, LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(settings_btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(settings_btn, 2, LV_STATE_DEFAULT);

    lv_obj_t* settings_icon_label = lv_label_create(settings_btn);
    lv_label_set_text(settings_icon_label, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_font(settings_icon_label, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(settings_icon_label, lv_color_white(), 0);
    lv_obj_center(settings_icon_label);

    if (settings_event_cb) {
        lv_obj_add_event_cb(settings_btn, settings_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)card_id_val);
    }
    return title_row;
}

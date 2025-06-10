#include "UI_Common.h"
#include "../Config/Config.h"
#include "UI_Styles.h"
#include "font/fengyahei_s22_bpp4.h"
#include <Arduino.h>
#include "../App/AppGlobal.h" // For screen_id_t and control_card_id_t
#include "UI_ControlPage.h" // For card_settings_event_handler_global declaration

// 底部导航按钮的文本
static const char *bottom_nav_btn_map[] = {"主页", "控制", "设置", ""};

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
    lv_style_set_pad_all(&style_btnm_bg, 0);
    lv_style_set_border_width(&style_btnm_bg, 0);

    lv_style_init(&style_btnm_items_default);
    lv_style_set_bg_opa(&style_btnm_items_default, LV_OPA_TRANSP);
    lv_style_set_text_font(&style_btnm_items_default, &fengyahei_s22_bpp4);
    lv_style_set_text_color(&style_btnm_items_default, lv_color_hex(0xCCCCCC));
    lv_style_set_radius(&style_btnm_items_default, 0);
    lv_style_set_border_width(&style_btnm_items_default, 0);
    lv_style_set_outline_width(&style_btnm_items_default, 0);
    lv_style_set_shadow_width(&style_btnm_items_default, 0);

    // 确保文本垂直居中
    lv_coord_t default_total_vertical_space_for_text = NAV_BAR_HEIGHT - font_height_approx;
    if (default_total_vertical_space_for_text < 0) default_total_vertical_space_for_text = 0;
    lv_coord_t default_pad_top = default_total_vertical_space_for_text / 2;
    lv_coord_t default_pad_bottom = default_total_vertical_space_for_text - default_pad_top;
    lv_style_set_pad_top(&style_btnm_items_default, default_pad_top);
    lv_style_set_pad_bottom(&style_btnm_items_default, default_pad_bottom);
    lv_style_set_pad_left(&style_btnm_items_default, 5);
    lv_style_set_pad_right(&style_btnm_items_default, 5);

    lv_style_init(&style_btnm_items_checked);
    lv_style_set_text_font(&style_btnm_items_checked, &fengyahei_s22_bpp4);
    lv_style_set_text_color(&style_btnm_items_checked, lv_color_white());
    lv_style_set_bg_opa(&style_btnm_items_checked, LV_OPA_TRANSP);
    lv_style_set_radius(&style_btnm_items_checked, 0);
    lv_style_set_outline_width(&style_btnm_items_checked, 0);
    lv_style_set_shadow_width(&style_btnm_items_checked, 0);

    // 确保选中状态和默认状态的内边距一致
    lv_style_set_pad_top(&style_btnm_items_checked, default_pad_top);
    lv_style_set_pad_bottom(&style_btnm_items_checked, default_pad_bottom);
    lv_style_set_pad_left(&style_btnm_items_checked, 5);
    lv_style_set_pad_right(&style_btnm_items_checked, 5);

    // 底部指示器
    lv_style_set_border_color(&style_btnm_items_checked, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
    lv_style_set_border_width(&style_btnm_items_checked, indicator_height);
    lv_style_set_border_side(&style_btnm_items_checked, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_opa(&style_btnm_items_checked, LV_OPA_COVER);

    common_nav_styles_initialized = true;
}

// ======================================================================
// ============ ★★★ 最终修复：使用手动对齐，不再使用Flexbox ★★★ ============
// ======================================================================
void create_common_bottom_nav(lv_obj_t *parent_screen, screen_id_t active_screen_id) {
    // 1. 初始化样式 (此函数不变)
    init_common_nav_styles_if_needed();

    // 2. 获取尺寸
    const lv_coord_t SCREEN_W = lv_disp_get_hor_res(NULL);
    const lv_coord_t LOGO_AREA_WIDTH = 80;
    const lv_coord_t BTNM_WIDTH = SCREEN_W - LOGO_AREA_WIDTH;

    // 3. 创建一个底部背景板 (代替 Flex 容器)
    lv_obj_t *nav_bg = lv_obj_create(parent_screen);
    lv_obj_remove_style_all(nav_bg); // 移除所有默认样式
    lv_obj_add_style(nav_bg, &style_btnm_bg, 0); // 应用我们的深蓝色背景样式
    lv_obj_set_size(nav_bg, SCREEN_W, NAV_BAR_HEIGHT);
    lv_obj_align(nav_bg, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    // 从这个对象上移除所有滚动条和点击事件，让它纯粹作为背景
    lv_obj_clear_flag(nav_bg, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    // 4. 创建 Logo 占位区域 (作为 nav_bg 的子对象)
    lv_obj_t *logo_placeholder = lv_obj_create(nav_bg);
    lv_obj_remove_style_all(logo_placeholder);
    lv_obj_set_size(logo_placeholder, LOGO_AREA_WIDTH, NAV_BAR_HEIGHT);
    lv_obj_align(logo_placeholder, LV_ALIGN_LEFT_MID, 0, 0);

    // 5. 创建按钮矩阵 (也作为 nav_bg 的子对象)
    lv_obj_t *btnm = lv_btnmatrix_create(nav_bg);
    lv_btnmatrix_set_map(btnm, bottom_nav_btn_map);
    // ★ 明确设置尺寸，这是成功的关键 ★
    lv_obj_set_size(btnm, BTNM_WIDTH, NAV_BAR_HEIGHT); 
    // ★ 使用手动对齐，将其放在右侧 ★
    lv_obj_align(btnm, LV_ALIGN_RIGHT_MID, 0, 0); 

    // 6. 应用按钮的样式 (和以前一样)
    // 按钮矩阵本身应该是透明的，没有边框
    lv_obj_set_style_bg_opa(btnm, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btnm, 0, 0);
    lv_obj_set_style_pad_all(btnm, 0, 0);
    
    // ★★★ 警告修复：使用 static_cast 消除编译器警告 ★★★
    lv_obj_add_style(btnm, &style_btnm_items_default, static_cast<lv_style_selector_t>(LV_PART_ITEMS) | static_cast<lv_style_selector_t>(LV_STATE_DEFAULT));
    lv_obj_add_style(btnm, &style_btnm_items_checked, static_cast<lv_style_selector_t>(LV_PART_ITEMS) | static_cast<lv_style_selector_t>(LV_STATE_CHECKED));

    // 7. 设置初始状态和事件 (和以前一样)
    lv_btnmatrix_set_one_checked(btnm, true);
    uint32_t initial_selected_id = (uint32_t)active_screen_id;
    const uint32_t button_count = sizeof(bottom_nav_btn_map) / sizeof(bottom_nav_btn_map[0]) - 1;

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
    lv_obj_set_layout(title_row, LV_LAYOUT_FLEX); // 这里使用Flexbox没问题，因为只是简单横向排列
    lv_obj_set_flex_flow(title_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(title_row, 5, 0);
    lv_obj_set_style_pad_bottom(title_row, 5, 0);

    if (icon_symbol && strlen(icon_symbol) > 0) {
        lv_obj_t* icon = lv_label_create(title_row);
        lv_label_set_text(icon, icon_symbol);
        if (style_card_icon.prop_cnt > 0 ) {
             lv_obj_add_style(icon, &style_card_icon, 0);
        }
        lv_obj_set_style_text_font(icon, &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(icon, lv_color_white(), 0);
    }

    lv_obj_t* title_label_obj = lv_label_create(title_row);
    lv_label_set_text(title_label_obj, title_text);
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

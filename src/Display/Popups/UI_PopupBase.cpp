#include "UI_PopupBase.h"              // 假设 UI_PopupBase.h 与此文件在同一目录 (src/Display/)
#include "Display/UI_Styles.h"                 // 假设 UI_Styles.h 与此文件在同一目录 (src/Display/)
#include "font/fengyahei_s16_bpp4.h"   // 假设 font 是 src/Display/ 的子目录
#include "font/fengyahei_s24_bpp4.h"
#include "../Config/Config.h"          // 假设 Config.h 在 src/Config/

// 事件回调：点击 'X' 按钮关闭弹窗
static void popup_close_button_event_handler(lv_event_t *e) {
    lv_obj_t *popup_mask_to_delete = (lv_obj_t *)lv_event_get_user_data(e);
    if (popup_mask_to_delete && lv_obj_is_valid(popup_mask_to_delete)) {
        lv_obj_del_async(popup_mask_to_delete);
    }
}

lv_obj_t* create_popup_frame(lv_obj_t *parent_scr, const char *title_text) {
    // --- 1. 创建根遮罩层 (popup_mask) ---
    lv_obj_t *popup_mask = lv_obj_create(parent_scr);
    lv_obj_remove_style_all(popup_mask);
    lv_obj_set_size(popup_mask, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(popup_mask, lv_color_hex(0x061A2E), 0);
    lv_obj_set_style_bg_opa(popup_mask, LV_OPA_90, 0);
    lv_obj_clear_flag(popup_mask, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(popup_mask, LV_OBJ_FLAG_CLICKABLE); // 确保 mask 不拦截点击

    // --- 2. 创建弹窗主体容器 (popup_box) ---
    lv_obj_t *popup_box = lv_obj_create(popup_mask);
    lv_obj_remove_style_all(popup_box);
    lv_obj_add_style(popup_box, &style_card_main, LV_STATE_DEFAULT);
    lv_obj_clear_flag(popup_box, LV_OBJ_FLAG_CLICKABLE); // 确保 popup_box 本身不拦截点击

    lv_obj_set_size(popup_box, LV_PCT(70), LV_PCT(80)); // 您设置的尺寸
    lv_obj_center(popup_box);
    lv_obj_set_layout(popup_box, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(popup_box, LV_FLEX_FLOW_COLUMN); // 内部元素垂直排列

    // --- 3. 弹窗标题 ---
    lv_obj_t *title_label = lv_label_create(popup_box);
    lv_label_set_text(title_label, title_text);
    lv_obj_set_style_text_font(title_label, &fengyahei_s24_bpp4, 0);
    lv_obj_set_style_text_color(title_label, TEXT_COLOR_WHITE, 0);
    lv_obj_set_width(title_label, LV_PCT(100));
    lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);

    // --- 4. 可滚动的内容区域 (scrollable_content_area) ---
    // *** 注意：将内容区域的创建移到关闭按钮之前 ***
    lv_obj_t *scrollable_content_area = lv_obj_create(popup_box);
    lv_obj_remove_style_all(scrollable_content_area);
    lv_obj_set_style_bg_opa(scrollable_content_area, LV_OPA_TRANSP, 0);
    lv_obj_set_width(scrollable_content_area, LV_PCT(100));
    lv_obj_set_flex_grow(scrollable_content_area, 1); // 占据剩余垂直空间

    lv_obj_add_flag(scrollable_content_area, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(scrollable_content_area, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(scrollable_content_area, LV_SCROLLBAR_MODE_AUTO);

   lv_obj_set_layout(scrollable_content_area, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(scrollable_content_area, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scrollable_content_area, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(scrollable_content_area, 10, 0);

    // --- 5. 'X' 关闭按钮 (移到最后创建并置顶) ---
    lv_obj_t *close_btn = lv_btn_create(popup_box); // 作为 popup_box 的子对象
    lv_obj_remove_style_all(close_btn);
    lv_obj_set_size(close_btn, 40, 40); // 您设置的尺寸 (40x40)

    lv_obj_set_style_bg_color(close_btn, lv_color_hex(0x25374F), 0);
    lv_obj_set_style_bg_opa(close_btn, LV_OPA_50, 0); // 您设置的50%不透明度
    lv_obj_set_style_border_color(close_btn, ROLLER_BORDER_COLOR, 0); // 确保 ROLLER_BORDER_COLOR 在 Config.h 中定义
    lv_obj_set_style_border_width(close_btn, 1, 0);
    lv_obj_set_style_radius(close_btn, 6, 0);

    lv_obj_t *close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, LV_SYMBOL_CLOSE);
    lv_obj_set_style_text_font(close_label, &lv_font_montserrat_18, 0); // 您设置的字体 (Montserrat 18)
    lv_obj_set_style_text_color(close_label, TEXT_COLOR_WHITE, 0);    // 确保 TEXT_COLOR_WHITE 在 Config.h 中定义
    lv_obj_center(close_label);

    lv_obj_add_flag(close_btn, LV_OBJ_FLAG_IGNORE_LAYOUT); // 忽略父容器的Flex布局
    lv_obj_add_flag(close_btn, LV_OBJ_FLAG_CLICKABLE);    // 明确保证按钮可点击
    // 在所有其他主要子元素（如 scrollable_content_area）创建之后再对齐和置顶
    lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, 0, 0);    // 对齐到父容器内边距的右上角
                                                          // 您提供的代码中 lv_obj_align 的x,y偏移为空，我补上了0,0
                                                          // 如果需要偏移，例如向左上角内缩一点，可以用负值和正值：lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_move_foreground(close_btn);                    // 提升按钮层级，确保在最上

    // 使用 LV_EVENT_CLICKED
    lv_obj_add_event_cb(close_btn, popup_close_button_event_handler, LV_EVENT_CLICKED, popup_mask);

    return scrollable_content_area;
}
#ifndef UI_POPUP_BASE_H
#define UI_POPUP_BASE_H

#include "lvgl.h"

/**
 * @brief 创建一个通用的弹窗框架。
 *
 * 该框架包含一个半透明的遮罩层、一个居中的弹窗主体（应用 style_card_main 样式）、
 * 弹窗顶部的标题文本，以及弹窗右上角的 'X' 关闭按钮。
 * 'X' 按钮点击后会异步删除整个弹窗（通过删除其根遮罩层实现）。
 *
 * @param parent_scr 父屏幕对象，弹窗将在此对象上创建。对于覆盖全屏的弹窗，
 * 通常应传递 lv_layer_top()。
 * @param title_text 弹窗的标题字符串。
 * @return lv_obj_t* 指向弹窗内部为具体内容准备的可滚动区域对象。
 * 调用者应在此对象上构建其实际的UI元素。
 */
lv_obj_t* create_popup_frame(lv_obj_t *parent_scr, const char *title_text);

#endif // UI_POPUP_BASE_H
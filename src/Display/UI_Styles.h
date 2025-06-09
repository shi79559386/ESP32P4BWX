// File: src/Display/UI_Styles.h
#ifndef UI_STYLES_H
#define UI_STYLES_H

#include "lvgl.h"

// 在UI_Styles.h文件中添加以下常量定义
#define CARD_FIXED_HEIGHT           150    // 统一卡片高度
#define CARD_SWITCH_SIZE           40     // 开关区域大小
#define CARD_SWITCH_RIGHT_MARGIN   10     // 开关距离右边的距离
#define CARD_SWITCH_BOTTOM_MARGIN  10     // 开关距离底边的距离
#define CARD_CONTENT_PADDING       7      // 卡片外边距
#define CARD_CONTENT_INNER_PADDING 5      // 卡片内容区域内边距
#define CARD_ROW_SPACING          3      // 卡片行间距
#define CARD_TEXT_RIGHT_MARGIN    60     // 文字区域右边距（为开关留空间）
#define CAROUSEL_SWITCH_INTERVAL_MS    3000    // 轮播切换间隔（毫秒）
#define CAROUSEL_FADE_DURATION_MS      300     // 淡入淡出动画时长
#define CAROUSEL_PAGE_COUNT           2        // 轮播页面数量


// --- 外部样式声明 ---
extern lv_style_t style_scr_bg;
extern lv_style_t style_panel_independent;
// extern lv_style_t style_panel_text_button_subset; // 将被新的动画按钮样式替代或合并
// extern lv_style_t style_button_custom; // 将被新的动画按钮样式替代
extern lv_style_t style_text_white_custom_font;
extern lv_style_t style_meter_temp_value_text;
extern lv_style_t style_meter_humid_value_text;

// 新增: 动画按钮样式声明
extern lv_style_t style_btn_animated_default;
extern lv_style_t style_btn_animated_pressed;
extern lv_style_t style_meter_indicator_arc_shadow; // 仪表盘指示弧的阴影样式

// 【新增】卡片样式
extern lv_style_t style_card_main;
extern lv_style_t style_card_title_label; // Style for card titles
extern lv_style_t style_card_icon;      // Style fo

// 【新增】开关控件的青色样式
extern lv_style_t style_switch_on_cyan;


// 公共样式变量声明
extern lv_style_t style_text_white_custom_font;
extern lv_style_t style_roller_options_custom;
extern bool       style_roller_options_inited;

extern lv_style_t style_progress_bar_glow;

// --- 函数声明 ---
void ui_styles_init(void);

#endif // UI_STYLES_H
// File: Display/UI_MainScreen.h
#ifndef UI_MAIN_SCREEN_H
#define UI_MAIN_SCREEN_H

#include "lvgl.h"

// --- 枚举定义哪个仪表盘的设置被触发 ---
typedef enum {
    TARGET_SETTING_NONE,
    TARGET_SETTING_TEMP_LEFT,
    TARGET_SETTING_HUMID_LEFT,
    TARGET_SETTING_TEMP_RIGHT,
    TARGET_SETTING_HUMID_RIGHT
} target_setting_type_t;


// --- 函数声明 ---
void create_main_ui(lv_obj_t *parent_scr);
void update_top_scrolling_info_label(const char *info_str);
void update_temperature_humidity_displays(float temp1, int humid1, float temp2, int humid2);
void refresh_top_bar_info();

// 新增：函数用于显示/隐藏目标设置slider (如果选择将逻辑封装)
 //void show_target_value_slider(target_setting_type_t type, lv_obj_t* meter_parent);


#endif // UI_MAIN_SCREEN_H
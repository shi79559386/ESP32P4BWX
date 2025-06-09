#ifndef UI_FEATURE_SETTINGS_H
#define UI_FEATURE_SETTINGS_H

#include "lvgl.h"

// 定义特性 ID (如果尚未在全局头文件中定义)
typedef enum {
    FEATURE_ID_LIGHTING = 0,
    FEATURE_ID_PARROT,
    FEATURE_ID_DRYING,
    FEATURE_ID_FRESH_AIR,
    FEATURE_ID_HATCHING,
    FEATURE_ID_STERILIZATION, // 新增杀菌特性
    FEATURE_ID_THERMAL,        // 新增：保温功能
    FEATURE_ID_HUMIDIFY,        // 新增：加湿功能
    FEATURE_ID_COUNT // 用于计数或标记结束
} feature_id_t;

/**
 * @brief 根据提供的特性ID，创建并显示对应的设置弹窗。
 *
 * @param feature_id 要显示的弹窗的特性ID。
 * @param parent_scr 弹窗将创建于其上的父对象 (通常是 lv_layer_top() 或当前活动屏幕)。
 */
void show_feature_settings_popup(feature_id_t feature_id, lv_obj_t *parent_scr);

#endif // UI_FEATURE_SETTINGS_H
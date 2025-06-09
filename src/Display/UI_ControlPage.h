// File: src/Display/UI_ControlPage.h
#ifndef UI_CONTROL_PAGE_H
#define UI_CONTROL_PAGE_H

#include "lvgl.h"
// control_card_id_t 现在在 AppGlobal.h 中

void create_control_page_ui(lv_obj_t *parent_scr);

// 全局可访问的设置按钮事件回调函数声明
extern void card_settings_event_handler_global(lv_event_t *e);


#endif // UI_CONTROL_PAGE_H

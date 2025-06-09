// File: src/Display/UI_Common.h
#ifndef UI_COMMON_H
#define UI_COMMON_H

#include "lvgl.h"
#include "../App/AppGlobal.h" // For screen_id_t and global screen pointers

void create_common_bottom_nav(lv_obj_t *parent_screen, screen_id_t active_screen_id);
lv_obj_t* create_common_card_header(lv_obj_t* parent_card, const char* title_text, const char* icon_symbol, int card_id_val, lv_event_cb_t settings_event_cb);



#endif // UI_COMMON_H

// File: src/Display/cards/ui_lighting_card.h
#ifndef UI_LIGHTING_CARD_H
#define UI_LIGHTING_CARD_H

#include "lvgl.h"

// 创建照明卡片的UI
void create_lighting_card_ui(lv_obj_t *parent);

// 更新照明卡片上的状态信息显示
void update_lighting_card_status();

#endif // UI_LIGHTING_CARD_H

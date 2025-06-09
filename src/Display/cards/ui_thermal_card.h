// File: src/Display/cards/ui_thermal_card.h
#ifndef UI_THERMAL_CARD_H
#define UI_THERMAL_CARD_H

#include "lvgl.h"

void create_thermal_card_ui(lv_obj_t* parent_dashboard);
void update_thermal_card_status(void);

#endif // UI_THERMAL_CARD_H
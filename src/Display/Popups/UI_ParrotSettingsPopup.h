// File: src/Display/Popups/UI_ParrotSettingsPopup.h
#ifndef UI_PARROT_SETTINGS_POPUP_H
#define UI_PARROT_SETTINGS_POPUP_H

#include "lvgl.h"

void create_parrot_settings_popup(lv_obj_t *parent_scr);
// update_parrot_info_label 不再需要单独公开，由内部回调处理
// void update_parrot_info_label(const char* new_info);

#endif // UI_PARROT_SETTINGS_POPUP_H
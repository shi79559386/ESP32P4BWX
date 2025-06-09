#include "UI_ParrotSettingsPopup.h"
#include "UI_PopupBase.h"
#include "../UI_Styles.h"
#include "../Config/Config.h"
#include "../../SettingsLogic/ParrotSettings.h" 
#include "../font/fengyahei_s16_bpp4.h"
#include <Arduino.h>
#include <stdio.h>

static lv_obj_t *popup_parrot_settings_view = NULL;
static lv_obj_t *roller_file_list;
static lv_obj_t *slider_volume;
static lv_obj_t *label_volume_value;
static lv_obj_t *play_mode_btn = NULL;
static lv_obj_t *play_mode_label = NULL;
static lv_obj_t *play_stop_btn = NULL;
static lv_obj_t *play_stop_label = NULL;
static lv_group_t *parrot_settings_group = NULL;
static lv_group_t *previous_indev_group = NULL;
static lv_obj_t *switch_timed_play_enable;
static lv_obj_t *roller_countdown_hour;
static lv_obj_t *roller_countdown_minute;
static lv_obj_t *roller_countdown_second;
static lv_obj_t *countdown_rollers_row_container = NULL;
static char file_list_roller_options[MAX_AUDIO_FILES_IN_PLAYLIST * MAX_AUDIO_FILENAME_LENGTH];
static char hour_roller_options[24 * 4];
static char minute_roller_options[60 * 4];
static char second_roller_options[60 * 4];

static const parrot_play_mode_t parrot_ui_play_modes[] = {
    PARROT_PLAY_MODE_FOLDER_LOOP,
    PARROT_PLAY_MODE_SINGLE_LOOP,
    PARROT_PLAY_MODE_ALL_RANDOM,
    PARROT_PLAY_MODE_SINGLE_ONCE,
    PARROT_PLAY_MODE_ALL_LOOP
};
static const char* parrot_ui_play_mode_icons[] = {
    LV_SYMBOL_LOOP,
    LV_SYMBOL_REFRESH,
    LV_SYMBOL_SHUFFLE,
    "1",
    LV_SYMBOL_NEW_LINE,
};
static int current_ui_play_mode_index = 0;

static void roller_block_scroll_cb(lv_event_t *e) {
    lv_event_stop_bubbling(e);
}

static void build_roller_options() {
    file_list_roller_options[0] = '\0';
    int count = ParrotSettings_GetFileCount();
    if (count == 0) {
        strcpy(file_list_roller_options, "无音频文件");
    } else {
        for (int i = 0; i < count; ++i) {
            const char *name = ParrotSettings_GetFileName(i);
            if (name) {
                if (strlen(file_list_roller_options) + strlen(name) + 2 < sizeof(file_list_roller_options)) {
                    strcat(file_list_roller_options, name);
                    if (i < count - 1) {
                        strcat(file_list_roller_options, "\n");
                    }
                } else {
                    break;
                }
            }
        }
    }

    hour_roller_options[0] = '\0'; char *p_hour = hour_roller_options; for (int i = 0; i < 24; i++) { p_hour += sprintf(p_hour, "%02d", i); if (i < 23) *(p_hour++) = '\n'; } *p_hour = '\0';
    minute_roller_options[0] = '\0'; char *p_min = minute_roller_options; for (int i = 0; i < 60; i++) { p_min += sprintf(p_min, "%02d", i); if (i < 59) *(p_min++) = '\n'; } *p_min = '\0';
    second_roller_options[0] = '\0'; char *p_sec = second_roller_options; for (int i = 0; i < 60; i++) { p_sec += sprintf(p_sec, "%02d", i); if (i < 59) *(p_sec++) = '\n'; } *p_sec = '\0';
}

static void update_play_mode_button_icon() {
    if (play_mode_label && lv_obj_is_valid(play_mode_label)) {
        lv_label_set_text(play_mode_label, parrot_ui_play_mode_icons[current_ui_play_mode_index]);
    }
}

static void update_play_stop_button_icon() {
    if (play_stop_label && lv_obj_is_valid(play_stop_label)) {
        lv_label_set_text(play_stop_label, ParrotSettings_IsCurrentlyPlaying() ? LV_SYMBOL_STOP : LV_SYMBOL_PLAY);
    }
}

static void update_parrot_popup_ui() {
    if (!popup_parrot_settings_view || !lv_obj_is_valid(popup_parrot_settings_view)) {
        return;
    }
    
    if(roller_file_list && lv_obj_is_valid(roller_file_list)){
        int current_selection = ParrotSettings_GetCurrentFileIndex();
        if(current_selection >=0 && current_selection < ParrotSettings_GetFileCount()){
            if(lv_roller_get_option_cnt(roller_file_list) > 0) {
                lv_roller_set_selected(roller_file_list, current_selection, LV_ANIM_OFF);
            }
        }
    }
    if(slider_volume && lv_obj_is_valid(slider_volume)){
        lv_slider_set_value(slider_volume, ParrotSettings_GetVolume(), LV_ANIM_OFF);
    }
    if(label_volume_value && lv_obj_is_valid(label_volume_value)){
        lv_label_set_text_fmt(label_volume_value, "%d%%", ParrotSettings_GetVolume());
    }

    update_play_mode_button_icon();
    update_play_stop_button_icon(); 

    const parrot_timed_play_t *timed_cfg = ParrotSettings_GetTimedPlaySettings(); 
    if (timed_cfg) {
        if (switch_timed_play_enable && lv_obj_is_valid(switch_timed_play_enable)) {
            if (timed_cfg->enabled) lv_obj_add_state(switch_timed_play_enable, LV_STATE_CHECKED);
            else lv_obj_clear_state(switch_timed_play_enable, LV_STATE_CHECKED);
        }

        bool show_countdown_controls = timed_cfg->enabled;
        if (countdown_rollers_row_container && lv_obj_is_valid(countdown_rollers_row_container)) {
            if (show_countdown_controls) {
                lv_obj_clear_flag(countdown_rollers_row_container, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(countdown_rollers_row_container, LV_OBJ_FLAG_HIDDEN);
            }
        }
        
        if (show_countdown_controls) {
            if (roller_countdown_hour && lv_obj_is_valid(roller_countdown_hour)) lv_roller_set_selected(roller_countdown_hour, timed_cfg->countdown_hour, LV_ANIM_OFF);
            if (roller_countdown_minute && lv_obj_is_valid(roller_countdown_minute)) lv_roller_set_selected(roller_countdown_minute, timed_cfg->countdown_minute, LV_ANIM_OFF);
            if (roller_countdown_second && lv_obj_is_valid(roller_countdown_second)) lv_roller_set_selected(roller_countdown_second, timed_cfg->countdown_second, LV_ANIM_OFF);
        }
    }
}

// ... (此处省略其他UI事件回调函数，与你之前的版本相同) ...
static void parrot_popup_delete_event_handler(lv_event_t *e) { ParrotSettings_RegisterStatusUpdateCallback(NULL); lv_indev_t *current_indev = lv_indev_get_act(); if (current_indev && current_indev->group == parrot_settings_group) { lv_indev_set_group(current_indev, previous_indev_group); } if (parrot_settings_group) { lv_group_del(parrot_settings_group); parrot_settings_group = NULL; } previous_indev_group = NULL; popup_parrot_settings_view = NULL; roller_file_list = NULL; slider_volume = NULL; label_volume_value = NULL; play_mode_btn = NULL; play_mode_label = NULL; play_stop_btn = NULL; play_stop_label = NULL; switch_timed_play_enable = NULL; roller_countdown_hour = NULL; roller_countdown_minute = NULL; roller_countdown_second = NULL; countdown_rollers_row_container = NULL; }
static void file_roller_event_cb(lv_event_t *e) { lv_obj_t *roller = lv_event_get_target(e); if (!roller) return; uint16_t selected_idx = lv_roller_get_selected(roller); ParrotSettings_SetCurrentFileIndex(selected_idx); }
static void play_mode_btn_event_cb(lv_event_t *e) { current_ui_play_mode_index = (current_ui_play_mode_index + 1) % (sizeof(parrot_ui_play_modes) / sizeof(parrot_ui_play_modes[0])); ParrotSettings_SetPlayMode(parrot_ui_play_modes[current_ui_play_mode_index]); }
static void play_stop_btn_event_cb(lv_event_t *e) { ParrotSettings_TogglePlayPause(); }
static void volume_slider_event_cb(lv_event_t *e) { lv_obj_t *slider = lv_event_get_target(e); if (!slider) return; ParrotSettings_SetVolume((uint8_t)lv_slider_get_value(slider)); }
static void prev_btn_event_cb(lv_event_t *e) { ParrotSettings_PreviousTrack(); }
static void next_btn_event_cb(lv_event_t *e) { ParrotSettings_NextTrack(); }
static void timed_close_switch_event_cb(lv_event_t *e) { lv_obj_t *sw = lv_event_get_target(e); if(!sw) return; parrot_timed_play_t cfg = *ParrotSettings_GetTimedPlaySettings(); cfg.enabled = lv_obj_has_state(sw, LV_STATE_CHECKED); ParrotSettings_SetTimedPlaySettings(&cfg); }
static void countdown_roller_event_cb(lv_event_t *e) { parrot_timed_play_t cfg = *ParrotSettings_GetTimedPlaySettings(); lv_obj_t* target = lv_event_get_target(e); if(!target) return; uint16_t sel = lv_roller_get_selected(target); if (target == roller_countdown_hour) cfg.countdown_hour = sel; else if (target == roller_countdown_minute) cfg.countdown_minute = sel; else if (target == roller_countdown_second) cfg.countdown_second = sel; ParrotSettings_SetTimedPlaySettings(&cfg); }


void create_parrot_settings_popup(lv_obj_t *parent_scr) {
    // ... (This function's structure remains the same)
    
    // The implementation details are simplified as most of the UI creation logic is sound.
    // The key is calling the correct ParrotSettings functions.
    popup_parrot_settings_view = create_popup_frame(parent_scr, "鹦语设置");
    if (!popup_parrot_settings_view) return;
    
    // ... all lv_... create calls for rollers, sliders, buttons ...

    ParrotSettings_ScanAudioFiles();
    build_roller_options();
    
    roller_file_list = lv_roller_create(popup_parrot_settings_view);
    lv_roller_set_options(roller_file_list, file_list_roller_options, LV_ROLLER_MODE_NORMAL);
    // ... and so on for the rest of the UI elements
    
    ParrotSettings_RegisterStatusUpdateCallback(update_parrot_popup_ui);
    update_parrot_popup_ui();
}
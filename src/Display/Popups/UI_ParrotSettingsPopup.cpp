// File: src/Display/Popups/UI_ParrotSettingsPopup.cpp
#include "UI_ParrotSettingsPopup.h"
#include "UI_PopupBase.h"
#include "../UI_Styles.h"
#include "../Config/Config.h"
#include "../../SettingsLogic/ParrotSettings.h" // parrot_timed_play_t comes from here
#include "../font/fengyahei_s16_bpp4.h"
#include <Arduino.h>
#include <stdio.h>

// 静态UI元素指针声明
static lv_obj_t *popup_parrot_settings_view = NULL;
static lv_obj_t *label_current_playing_info;
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

// 字符串资源
static char file_list_roller_options[MAX_AUDIO_FILES_IN_PLAYLIST * MAX_AUDIO_FILENAME_LENGTH];
static char hour_roller_options[24 * 4];
static char minute_roller_options[60 * 4];
static char second_roller_options[60 * 4]; 

// UI播放模式定义
static const parrot_play_mode_t parrot_ui_play_modes[] = {
    PARROT_PLAY_MODE_ALL_LOOP,
    PARROT_PLAY_MODE_SINGLE_LOOP,
    PARROT_PLAY_MODE_FOLDER_LOOP,
    PARROT_PLAY_MODE_ALL_RANDOM
};
static const char* parrot_ui_play_mode_icons[] = {
    LV_SYMBOL_NEW_LINE,
    LV_SYMBOL_REFRESH,
    LV_SYMBOL_LOOP,
    LV_SYMBOL_SHUFFLE
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
                const char* display_name = strrchr(name, '/');
                display_name = display_name ? display_name + 1 : name;
                if (strlen(file_list_roller_options) + strlen(display_name) + 2 < sizeof(file_list_roller_options)) {
                    strcat(file_list_roller_options, display_name);
                    if (i < count - 1) {
                        strcat(file_list_roller_options, "\n");
                    }
                } else {
                    break;
                }
            }
        }
    }

    hour_roller_options[0] = '\0'; 
    char *p_hour = hour_roller_options;
    for (int i = 0; i < 24; i++) { 
        p_hour += sprintf(p_hour, "%02d", i);
        if (i < 23) {
            *(p_hour++) = '\n';
        }
    }
    *p_hour = '\0';

    minute_roller_options[0] = '\0'; 
    char *p_min = minute_roller_options;
    for (int i = 0; i < 60; i++) { 
        p_min += sprintf(p_min, "%02d", i);
        if (i < 59) {
            *(p_min++) = '\n';
        }
    }
    *p_min = '\0';

    second_roller_options[0] = '\0'; 
    char *p_sec = second_roller_options;
    for (int i = 0; i < 60; i++) { 
        p_sec += sprintf(p_sec, "%02d", i);
        if (i < 59) {
            *(p_sec++) = '\n';
        }
    }
    *p_sec = '\0';
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
    lv_obj_t *content_area = popup_parrot_settings_view;

    //if (label_current_playing_info && lv_obj_is_valid(label_current_playing_info)){
        //const char* playing_file_info_str = ParrotSettings_GetCurrentPlayingInfoString();
        //lv_label_set_text(label_current_playing_info, playing_file_info_str ? playing_file_info_str : "信息加载中...");
    //}

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
        lv_label_set_text_fmt(label_volume_value, "%d", ParrotSettings_GetVolume());
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
                if(roller_countdown_hour) lv_obj_clear_state(roller_countdown_hour, LV_STATE_DISABLED);
                if(roller_countdown_minute) lv_obj_clear_state(roller_countdown_minute, LV_STATE_DISABLED);
                if(roller_countdown_second) lv_obj_clear_state(roller_countdown_second, LV_STATE_DISABLED);
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
    
    if (content_area && lv_obj_is_valid(content_area)) {
        lv_obj_set_height(content_area, LV_SIZE_CONTENT);
    }
}

static void parrot_popup_delete_event_handler(lv_event_t *e) {
    ParrotSettings_RegisterStatusUpdateCallback(NULL);
    lv_indev_t *current_indev = lv_indev_get_act();
    if (current_indev && current_indev->group == parrot_settings_group) {
        lv_indev_set_group(current_indev, previous_indev_group);
    }

    if (parrot_settings_group) {
        lv_group_del(parrot_settings_group);
        parrot_settings_group = NULL;
    }
    previous_indev_group = NULL;

    popup_parrot_settings_view = NULL;
    label_current_playing_info = NULL;
    roller_file_list = NULL;
    slider_volume = NULL;
    label_volume_value = NULL;
    play_mode_btn = NULL;
    play_mode_label = NULL;
    play_stop_btn = NULL;
    play_stop_label = NULL;
    switch_timed_play_enable = NULL;
    
    roller_countdown_hour = NULL;
    roller_countdown_minute = NULL;
    roller_countdown_second = NULL;
    countdown_rollers_row_container = NULL;
}

static void file_roller_event_cb(lv_event_t *e) {
    lv_obj_t *roller = lv_event_get_target(e);
    if (!roller) return;
    uint16_t selected_idx = lv_roller_get_selected(roller);
    ParrotSettings_SetCurrentFileIndex(selected_idx);
}

static void play_mode_btn_event_cb(lv_event_t *e) {
    current_ui_play_mode_index = (current_ui_play_mode_index + 1) % (sizeof(parrot_ui_play_modes) / sizeof(parrot_ui_play_modes[0]));
    ParrotSettings_SetPlayMode(parrot_ui_play_modes[current_ui_play_mode_index]);
}

static void play_stop_btn_event_cb(lv_event_t *e) {
    if (ParrotSettings_IsCurrentlyPlaying()) {
        ParrotSettings_Stop();
    } else {
        if (ParrotSettings_GetFileCount() > 0) {
            if (ParrotSettings_GetCurrentFileIndex() < 0) { 
                ParrotSettings_SetCurrentFileIndex(0);
            }
            ParrotSettings_PlaySelectedFile();
        } else {
            Serial.println("ParrotPopup: No audio files to play for play_stop_btn_event_cb."); // 添加日志
        }
    }
    // 移除此处的 update_play_stop_button_icon(); 依赖全局回调
    // ParrotSettings_Stop() 或 ParrotSettings_PlaySelectedFile() 内部应更新状态并触发回调
}

static void volume_slider_event_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    if (!slider) return;
    uint8_t volume = (uint8_t)lv_slider_get_value(slider);
    ParrotSettings_SetVolume(volume);
}

static void prev_btn_event_cb(lv_event_t *e) { ParrotSettings_PreviousTrack(); }
static void next_btn_event_cb(lv_event_t *e) { ParrotSettings_NextTrack(); }

static void timed_close_switch_event_cb(lv_event_t *e) { 
    lv_obj_t *sw = lv_event_get_target(e);
    if(!sw) return;
    parrot_timed_play_t cfg = *ParrotSettings_GetTimedPlaySettings();
    cfg.enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
    ParrotSettings_SetTimedPlaySettings(&cfg); 
    // update_parrot_popup_ui(); // ParrotSettings_SetTimedPlaySettings 应该会触发全局更新
}

static void countdown_roller_event_cb(lv_event_t *e) { 
    parrot_timed_play_t cfg = *ParrotSettings_GetTimedPlaySettings();
    lv_obj_t* target = lv_event_get_target(e);
    if(!target) return;
    uint16_t sel = lv_roller_get_selected(target);

    if (target == roller_countdown_hour) cfg.countdown_hour = sel;
    else if (target == roller_countdown_minute) cfg.countdown_minute = sel;
    else if (target == roller_countdown_second) cfg.countdown_second = sel;
    ParrotSettings_SetTimedPlaySettings(&cfg);
}

void create_parrot_settings_popup(lv_obj_t *parent_scr) {
    parrot_play_mode_t initial_mode_from_logic = ParrotSettings_GetPlayMode();
    current_ui_play_mode_index = 0;
    for(int i=0; i < sizeof(parrot_ui_play_modes)/sizeof(parrot_ui_play_modes[0]); ++i){
        if(parrot_ui_play_modes[i] == initial_mode_from_logic){
            current_ui_play_mode_index = i;
            break;
        }
    }
    if (parrot_ui_play_modes[current_ui_play_mode_index] != initial_mode_from_logic) {
        current_ui_play_mode_index = 0;
        ParrotSettings_SetPlayMode(parrot_ui_play_modes[current_ui_play_mode_index]);
    }

    ParrotSettings_RegisterStatusUpdateCallback(NULL); 
    ParrotSettings_ScanAudioFiles();
    build_roller_options(); 

    popup_parrot_settings_view = create_popup_frame(parent_scr, "设置"); 
    if (!popup_parrot_settings_view) {
        return;
    }
    lv_obj_t *content_area = popup_parrot_settings_view;
    lv_obj_set_style_pad_top(content_area, 15, 0);

    lv_obj_add_flag(content_area, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(content_area, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_clear_flag(content_area, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(content_area, LV_OBJ_FLAG_SCROLL_CHAIN);

    lv_obj_t *popup_box = lv_obj_get_parent(content_area);
    lv_obj_t *popup_mask = lv_obj_get_parent(popup_box);
    if (popup_mask && lv_obj_is_valid(popup_mask)) {
        lv_obj_add_flag(popup_mask, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(popup_mask, parrot_popup_delete_event_handler, LV_EVENT_DELETE, NULL);
    }

    if (parrot_settings_group == NULL) {
        parrot_settings_group = lv_group_create();
    } else {
        lv_group_remove_all_objs(parrot_settings_group);
    }

    lv_indev_t *current_indev = lv_indev_get_act();
    if (current_indev) {
        previous_indev_group = current_indev->group;
        lv_indev_set_group(current_indev, parrot_settings_group);
    }

    lv_obj_set_flex_flow(content_area, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content_area, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START); 
    lv_obj_set_style_pad_row(content_area, 8, 0);

    //label_current_playing_info = lv_label_create(content_area);
    //lv_obj_add_style(label_current_playing_info, &style_text_white_custom_font, 0);
    //lv_obj_set_width(label_current_playing_info, LV_PCT(100));
    //lv_label_set_long_mode(label_current_playing_info, LV_LABEL_LONG_WRAP);
    //lv_obj_set_style_max_height(label_current_playing_info, 50 ,0);

    /*lv_obj_t *file_section_cont = lv_obj_create(content_area);
    lv_obj_remove_style_all(file_section_cont);
    lv_obj_set_width(file_section_cont, LV_PCT(100));
    lv_obj_set_height(file_section_cont, LV_SIZE_CONTENT);
    lv_obj_set_layout(file_section_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(file_section_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(file_section_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); 
    lv_obj_set_style_pad_row(file_section_cont, 3, 0);
    
    lv_obj_t* file_label = lv_label_create(file_section_cont);
    lv_label_set_text(file_label, "选择文件:");
    lv_obj_add_style(file_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_align(file_label, LV_TEXT_ALIGN_CENTER, 0); 
*/

    roller_file_list = lv_roller_create(content_area);
    lv_obj_set_width(roller_file_list, LV_PCT(90)); 
    lv_roller_set_options(roller_file_list, file_list_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_file_list, 3); 
    lv_obj_add_style(roller_file_list, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(roller_file_list, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(roller_file_list, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(roller_file_list, ROLLER_COORDINATED_BG_COLOR, 0);
    lv_obj_set_style_border_width(roller_file_list, 1, 0);
    lv_obj_set_style_radius(roller_file_list, 4, 0);
    lv_obj_set_style_text_color(roller_file_list, TEXT_COLOR_WHITE, LV_PART_SELECTED);
    lv_obj_set_style_bg_color(roller_file_list, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, roller_file_list);
    lv_obj_clear_flag(roller_file_list, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_event_cb(roller_file_list, file_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(roller_file_list, roller_block_scroll_cb, LV_EVENT_SCROLL_BEGIN, NULL);


    lv_obj_t *controls_row = lv_obj_create(content_area);
    lv_obj_remove_style_all(controls_row); lv_obj_set_width(controls_row, LV_PCT(100)); lv_obj_set_height(controls_row, LV_SIZE_CONTENT); lv_obj_set_layout(controls_row, LV_LAYOUT_FLEX); lv_obj_set_flex_flow(controls_row, LV_FLEX_FLOW_ROW); lv_obj_set_flex_align(controls_row, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); lv_obj_set_style_pad_top(controls_row, 5, 0);
    const lv_font_t* icon_font = &lv_font_montserrat_20;
    if(!icon_font) { icon_font = LV_FONT_DEFAULT; }
    play_mode_btn = lv_btn_create(controls_row);
    lv_obj_remove_style_all(play_mode_btn); lv_obj_set_size(play_mode_btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT); lv_obj_set_style_pad_all(play_mode_btn, 5, 0); lv_obj_add_event_cb(play_mode_btn, play_mode_btn_event_cb, LV_EVENT_CLICKED, NULL); play_mode_label = lv_label_create(play_mode_btn); if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, play_mode_btn); lv_obj_set_style_text_font(play_mode_label, icon_font, 0); lv_obj_set_style_text_color(play_mode_label, TEXT_COLOR_WHITE, 0); lv_obj_center(play_mode_label);
    lv_obj_t *tmp_prev_btn = lv_btn_create(controls_row);
    lv_obj_remove_style_all(tmp_prev_btn); lv_obj_set_size(tmp_prev_btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT); lv_obj_set_style_pad_all(tmp_prev_btn, 5, 0); lv_obj_add_event_cb(tmp_prev_btn, prev_btn_event_cb, LV_EVENT_CLICKED, NULL); lv_obj_t *prev_lbl = lv_label_create(tmp_prev_btn); lv_label_set_text(prev_lbl, LV_SYMBOL_PREV); lv_obj_set_style_text_font(prev_lbl, icon_font, 0); lv_obj_set_style_text_color(prev_lbl, TEXT_COLOR_WHITE, 0); lv_obj_center(prev_lbl); if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, tmp_prev_btn);
    play_stop_btn = lv_btn_create(controls_row);
    lv_obj_remove_style_all(play_stop_btn); lv_obj_set_size(play_stop_btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT); lv_obj_set_style_pad_all(play_stop_btn, 5, 0); lv_obj_add_event_cb(play_stop_btn, play_stop_btn_event_cb, LV_EVENT_CLICKED, NULL); play_stop_label = lv_label_create(play_stop_btn); lv_obj_set_style_text_font(play_stop_label, icon_font, 0); lv_obj_set_style_text_color(play_stop_label, TEXT_COLOR_WHITE, 0); lv_obj_center(play_stop_label); if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, play_stop_btn);
    lv_obj_t *tmp_next_btn = lv_btn_create(controls_row);
    lv_obj_remove_style_all(tmp_next_btn); lv_obj_set_size(tmp_next_btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT); lv_obj_set_style_pad_all(tmp_next_btn, 5, 0); lv_obj_add_event_cb(tmp_next_btn, next_btn_event_cb, LV_EVENT_CLICKED, NULL); lv_obj_t *next_lbl = lv_label_create(tmp_next_btn); lv_label_set_text(next_lbl, LV_SYMBOL_NEXT); lv_obj_set_style_text_font(next_lbl, icon_font, 0); lv_obj_set_style_text_color(next_lbl, TEXT_COLOR_WHITE, 0); lv_obj_center(next_lbl); if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, tmp_next_btn);

    lv_obj_t *volume_only_row = lv_obj_create(content_area);
    lv_obj_remove_style_all(volume_only_row); lv_obj_set_width(volume_only_row, LV_PCT(100)); lv_obj_set_height(volume_only_row, LV_SIZE_CONTENT); lv_obj_set_layout(volume_only_row, LV_LAYOUT_FLEX); lv_obj_set_flex_flow(volume_only_row, LV_FLEX_FLOW_COLUMN); lv_obj_set_flex_align(volume_only_row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); lv_obj_set_style_pad_top(volume_only_row, 5, 0);
    lv_obj_t *volume_cont = lv_obj_create(volume_only_row);
    lv_obj_remove_style_all(volume_cont); lv_obj_set_width(volume_cont, LV_PCT(85)); lv_obj_set_height(volume_cont, LV_SIZE_CONTENT); lv_obj_set_layout(volume_cont, LV_LAYOUT_FLEX); lv_obj_set_flex_flow(volume_cont, LV_FLEX_FLOW_COLUMN); lv_obj_set_flex_align(volume_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START); lv_obj_set_style_pad_ver(volume_cont, 5, 0);
    lv_obj_t* slider_value_row = lv_obj_create(volume_cont);
    lv_obj_remove_style_all(slider_value_row); lv_obj_set_width(slider_value_row, LV_PCT(100)); lv_obj_set_height(slider_value_row, LV_SIZE_CONTENT); lv_obj_set_layout(slider_value_row, LV_LAYOUT_FLEX); lv_obj_set_flex_flow(slider_value_row, LV_FLEX_FLOW_ROW); lv_obj_set_flex_align(slider_value_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); lv_obj_set_style_pad_column(slider_value_row, 8, 0); lv_obj_set_style_pad_ver(slider_value_row, 5, 0);
    slider_volume = lv_slider_create(slider_value_row);
    lv_obj_set_flex_grow(slider_volume, 1); lv_slider_set_range(slider_volume, 0, 30); lv_obj_add_event_cb(slider_volume, volume_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL); lv_obj_set_style_bg_color(slider_volume, lv_color_hex(0x1A2B3C), LV_PART_MAIN); lv_obj_set_style_bg_color(slider_volume, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_INDICATOR); lv_obj_set_height(slider_volume, 25); if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, slider_volume);
    label_volume_value = lv_label_create(slider_value_row);
    lv_obj_add_style(label_volume_value, &style_text_white_custom_font,0);

    // --- 定时关闭设置区 ---
    lv_obj_t *timed_close_section_cont = lv_obj_create(content_area);
    lv_obj_remove_style_all(timed_close_section_cont);
    lv_obj_set_width(timed_close_section_cont, LV_PCT(100));
    lv_obj_set_height(timed_close_section_cont, LV_SIZE_CONTENT);
    lv_obj_set_layout(timed_close_section_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(timed_close_section_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(timed_close_section_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(timed_close_section_cont, 5, 0);

    // "定时关闭设置:" 标题已被移除

    lv_obj_t* timed_enable_row = lv_obj_create(timed_close_section_cont);
    lv_obj_remove_style_all(timed_enable_row); lv_obj_set_width(timed_enable_row, LV_PCT(100)); lv_obj_set_height(timed_enable_row, LV_SIZE_CONTENT); lv_obj_set_layout(timed_enable_row, LV_LAYOUT_FLEX); lv_obj_set_flex_flow(timed_enable_row, LV_FLEX_FLOW_ROW); lv_obj_set_flex_align(timed_enable_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t* timed_enable_label = lv_label_create(timed_enable_row);
    lv_label_set_text(timed_enable_label, "定时:"); 
    lv_obj_add_style(timed_enable_label, &style_text_white_custom_font, 0);
    switch_timed_play_enable = lv_switch_create(timed_enable_row);
    lv_obj_add_style(switch_timed_play_enable, &style_switch_on_cyan, static_cast<lv_style_selector_t>(LV_PART_INDICATOR) | static_cast<lv_style_selector_t>(LV_STATE_CHECKED));
    lv_obj_add_event_cb(switch_timed_play_enable, timed_close_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, switch_timed_play_enable);

    countdown_rollers_row_container = lv_obj_create(timed_close_section_cont);
    lv_obj_remove_style_all(countdown_rollers_row_container);
    lv_obj_set_width(countdown_rollers_row_container, LV_PCT(100));
    lv_obj_set_height(countdown_rollers_row_container, LV_SIZE_CONTENT);
    lv_obj_set_layout(countdown_rollers_row_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(countdown_rollers_row_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(countdown_rollers_row_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); 
    lv_obj_set_style_pad_gap(countdown_rollers_row_container, 5, 0);
    if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, countdown_rollers_row_container);

    // "关闭倒计时:" 标签已被移除

    roller_countdown_hour = lv_roller_create(countdown_rollers_row_container);
    lv_obj_set_width(roller_countdown_hour, LV_PCT(25)); 
    lv_roller_set_options(roller_countdown_hour, hour_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_countdown_hour, 3);
    lv_obj_add_style(roller_countdown_hour, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(roller_countdown_hour, ROLLER_BORDER_COLOR, LV_PART_MAIN); lv_obj_set_style_bg_opa(roller_countdown_hour, LV_OPA_COVER, LV_PART_MAIN); lv_obj_set_style_border_color(roller_countdown_hour, ROLLER_BORDER_COLOR, 0); lv_obj_set_style_border_width(roller_countdown_hour, 1, 0); lv_obj_set_style_radius(roller_countdown_hour, 4, 0); lv_obj_set_style_text_color(roller_countdown_hour, TEXT_COLOR_WHITE, LV_PART_SELECTED); lv_obj_set_style_bg_color(roller_countdown_hour, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_add_event_cb(roller_countdown_hour, countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL); 
    if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, roller_countdown_hour);
    lv_obj_clear_flag(roller_countdown_hour, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_event_cb(roller_countdown_hour, roller_block_scroll_cb, LV_EVENT_SCROLL_BEGIN, NULL);

    lv_obj_t* colon1 = lv_label_create(countdown_rollers_row_container);
    lv_label_set_text(colon1, ":");
    lv_obj_add_style(colon1, &style_text_white_custom_font, 0);

    roller_countdown_minute = lv_roller_create(countdown_rollers_row_container);
    lv_obj_set_width(roller_countdown_minute, LV_PCT(25)); 
    lv_roller_set_options(roller_countdown_minute, minute_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_countdown_minute, 3);
    lv_obj_add_style(roller_countdown_minute, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(roller_countdown_minute, ROLLER_BORDER_COLOR, LV_PART_MAIN); lv_obj_set_style_bg_opa(roller_countdown_minute, LV_OPA_COVER, LV_PART_MAIN); lv_obj_set_style_border_color(roller_countdown_minute, ROLLER_BORDER_COLOR, 0); lv_obj_set_style_border_width(roller_countdown_minute, 1, 0); lv_obj_set_style_radius(roller_countdown_minute, 4, 0); lv_obj_set_style_text_color(roller_countdown_minute, TEXT_COLOR_WHITE, LV_PART_SELECTED); lv_obj_set_style_bg_color(roller_countdown_minute, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_add_event_cb(roller_countdown_minute, countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL); 
    if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, roller_countdown_minute);
    lv_obj_clear_flag(roller_countdown_minute, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_event_cb(roller_countdown_minute, roller_block_scroll_cb, LV_EVENT_SCROLL_BEGIN, NULL);

    lv_obj_t* colon2 = lv_label_create(countdown_rollers_row_container);
    lv_label_set_text(colon2, ":");
    lv_obj_add_style(colon2, &style_text_white_custom_font, 0);

    roller_countdown_second = lv_roller_create(countdown_rollers_row_container);
    lv_obj_set_width(roller_countdown_second, LV_PCT(25)); 
    lv_roller_set_options(roller_countdown_second, second_roller_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_countdown_second, 3);
    lv_obj_add_style(roller_countdown_second, &style_roller_options_custom, 0);
    lv_obj_set_style_bg_color(roller_countdown_second, ROLLER_BORDER_COLOR, LV_PART_MAIN); lv_obj_set_style_bg_opa(roller_countdown_second, LV_OPA_COVER, LV_PART_MAIN); lv_obj_set_style_border_color(roller_countdown_second, ROLLER_BORDER_COLOR, 0); lv_obj_set_style_border_width(roller_countdown_second, 1, 0); lv_obj_set_style_radius(roller_countdown_second, 4, 0); lv_obj_set_style_text_color(roller_countdown_second, TEXT_COLOR_WHITE, LV_PART_SELECTED); lv_obj_set_style_bg_color(roller_countdown_second, ROLLER_SELECTED_BG_LIGHT_BLUE, LV_PART_SELECTED);
    lv_obj_add_event_cb(roller_countdown_second, countdown_roller_event_cb, LV_EVENT_VALUE_CHANGED, NULL); 
    if (parrot_settings_group) lv_group_add_obj(parrot_settings_group, roller_countdown_second);
    lv_obj_clear_flag(roller_countdown_second, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_add_event_cb(roller_countdown_second, roller_block_scroll_cb, LV_EVENT_SCROLL_BEGIN, NULL);
    
    ParrotSettings_RegisterStatusUpdateCallback(update_parrot_popup_ui);
    update_parrot_popup_ui();
}


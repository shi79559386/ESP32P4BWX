#include "ui_voice_card.h"
#include "../UI_Styles.h"
#include "../../Config/Config.h"
#include "../../App/AppGlobal.h"
#include "../UI_Common.h"
#include "../UI_ControlPage.h"
#include "../../SettingsLogic/ParrotSettings.h" 
#include "../font/fengyahei_s16_bpp4.h" 
#include <Arduino.h>
#include <stdio.h> 

static lv_obj_t *card_voice_play_status_label = NULL;
static lv_obj_t *card_voice_mode_label = NULL;
static lv_obj_t *card_voice_timed_status_label = NULL;
static lv_obj_t *card_voice_main_switch = NULL;

static void voice_card_main_toggle_event_handler(lv_event_t *e) {
    ParrotSettings_TogglePlayPause(); 
}

void create_voice_card_ui(lv_obj_t* parent_dashboard) {
    lv_obj_t* card = lv_obj_create(parent_dashboard);
    lv_obj_add_style(card, &style_card_main, 0);
    lv_obj_set_width(card, LV_PCT(48));
    lv_obj_set_height(card, CARD_FIXED_HEIGHT);
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(card, CARD_CONTENT_PADDING, 0);

    extern const char* card_titles[];
    extern const char* card_icons[];
    create_common_card_header(card, card_titles[CARD_ID_VOICE], card_icons[CARD_ID_VOICE], 
                                CARD_ID_VOICE, card_settings_event_handler_global);

    lv_obj_t* content_area_card = lv_obj_create(card);
    lv_obj_remove_style_all(content_area_card);
    lv_obj_set_width(content_area_card, LV_PCT(100));
    lv_obj_set_flex_grow(content_area_card, 1);
    lv_obj_set_style_pad_all(content_area_card, CARD_CONTENT_INNER_PADDING, 0);

    lv_obj_t* info_container = lv_obj_create(content_area_card);
    lv_obj_remove_style_all(info_container);
    lv_obj_set_pos(info_container, 0, 0);
    lv_obj_set_size(info_container, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_right(info_container, CARD_TEXT_RIGHT_MARGIN, 0);
    lv_obj_set_layout(info_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(info_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(info_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(info_container, CARD_ROW_SPACING, 0);

    card_voice_play_status_label = lv_label_create(info_container);
    lv_obj_add_style(card_voice_play_status_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_voice_play_status_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_voice_play_status_label, LV_PCT(100));
    lv_label_set_text(card_voice_play_status_label, "状态: 已停止");

    card_voice_mode_label = lv_label_create(info_container);
    lv_obj_add_style(card_voice_mode_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_voice_mode_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_voice_mode_label, LV_PCT(100));
    lv_label_set_text(card_voice_mode_label, "模式: 列表循环");

    card_voice_timed_status_label = lv_label_create(info_container);
    lv_obj_add_style(card_voice_timed_status_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(card_voice_timed_status_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(card_voice_timed_status_label, LV_PCT(100));
    lv_label_set_text(card_voice_timed_status_label, "定时: 未启用");

    card_voice_main_switch = lv_switch_create(content_area_card);
    lv_obj_add_style(card_voice_main_switch, &style_switch_on_cyan, static_cast<lv_style_selector_t>(LV_PART_INDICATOR) | static_cast<lv_style_selector_t>(LV_STATE_CHECKED));
    lv_obj_add_event_cb(card_voice_main_switch, voice_card_main_toggle_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_align(card_voice_main_switch,LV_ALIGN_BOTTOM_RIGHT,-CARD_SWITCH_RIGHT_MARGIN,-CARD_SWITCH_BOTTOM_MARGIN);
    
    ParrotSettings_RegisterStatusUpdateCallback(update_voice_card_status);
    update_voice_card_status();
}

void update_voice_card_status() {
    if (card_voice_play_status_label && lv_obj_is_valid(card_voice_play_status_label)) {
        lv_label_set_text(card_voice_play_status_label, ParrotSettings_GetCurrentPlayingInfoString());
    }

    if (card_voice_mode_label && lv_obj_is_valid(card_voice_mode_label)) {
        const char* mode_str = ParrotSettings_GetPlayModeString(ParrotSettings_GetPlayMode());
        lv_label_set_text_fmt(card_voice_mode_label, "模式: %s", mode_str ? mode_str : "未知");
    }

    if (card_voice_timed_status_label && lv_obj_is_valid(card_voice_timed_status_label)) {
        const parrot_timed_play_t* timed_cfg = ParrotSettings_GetTimedPlaySettings();
        if (timed_cfg && timed_cfg->enabled) {
            if (ParrotSettings_IsTimedPlaySessionActive()) {
                char countdown_str[30];
                ParrotSettings_GetFormattedRemainingCountdown(countdown_str, sizeof(countdown_str));
                lv_label_set_text_fmt(card_voice_timed_status_label, "定时关: %s", countdown_str);
            } else {
                 lv_label_set_text_fmt(card_voice_timed_status_label, "定时: %02u:%02u:%02u",
                                       timed_cfg->countdown_hour,
                                       timed_cfg->countdown_minute,
                                       timed_cfg->countdown_second);
            }
        } else {
            lv_label_set_text(card_voice_timed_status_label, "定时: 未启用");
        }
    }

    if (card_voice_main_switch && lv_obj_is_valid(card_voice_main_switch)) {
        bool is_playing = ParrotSettings_IsCurrentlyPlaying();
        if (is_playing) {
            lv_obj_add_state(card_voice_main_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(card_voice_main_switch, LV_STATE_CHECKED);
        }
    }
}
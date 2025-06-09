// File: Display/UI_ControlPage.cpp
#include "UI_ControlPage.h"
#include "UI_Styles.h"
#include "../Config/Config.h"
#include "UI_FeatureSettings.h"
#include <Arduino.h>
#include "../App/AppGlobal.h" // 包含 AppGlobal.h 以访问 control_card_id_t, card_titles, card_icons
#include "UI_Common.h"

// 包含新的卡片头文件
#include "cards/ui_lighting_card.h"
#include "cards/ui_voice_card.h"
#include "cards/ui_drying_card.h"
#include "cards/ui_fresh_air_card.h"
#include "cards/ui_sterilize_card.h"
#include "cards/ui_hatching_card.h"
#include "cards/ui_thermal_card.h"
#include "cards/ui_humidify_card.h"

void card_settings_event_handler_global(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        control_card_id_t card_id = (control_card_id_t)(uintptr_t)lv_event_get_user_data(e);
        lv_obj_t *parent_screen_for_popup = lv_layer_top();
        if (card_id < CARD_ID_COUNT) { 
            Serial.printf("UI_ControlPage: Settings button clicked for card_id: %d (%s)\n",
                          card_id, card_titles[card_id]);
        } else {
            Serial.printf("UI_ControlPage: Settings button clicked for card_id: %d (Unknown Title)\n", card_id);
        }
        feature_id_t feature_to_show; // 用于确定要显示哪个特性弹窗
        bool known_card = true;

        switch (card_id) {
            case CARD_ID_LIGHTING:
                feature_to_show = FEATURE_ID_LIGHTING;
                break;
            case CARD_ID_VOICE: // 假设 CARD_ID_VOICE 对应学舌功能
                feature_to_show = FEATURE_ID_PARROT;
                break;
            case CARD_ID_DRYING:
                feature_to_show = FEATURE_ID_DRYING;
                break;
            case CARD_ID_FRESH_AIR:
                feature_to_show = FEATURE_ID_FRESH_AIR;
                break;
            case CARD_ID_STERILIZE:
                feature_to_show = FEATURE_ID_STERILIZATION;
                break;
            case CARD_ID_INCUBATION:
                feature_to_show = FEATURE_ID_HATCHING;
                break;
            case CARD_ID_THERMAL:
                feature_to_show = FEATURE_ID_THERMAL;
                break;
            case CARD_ID_HUMIDIFY:
                feature_to_show = FEATURE_ID_HUMIDIFY;
                break;
            default: // 正确的 default 用法
                known_card = false;
                Serial.printf("UI_ControlPage: Unknown card_id %d for settings button.\n", card_id);
                break; // default 后也需要 break
        }

        if (known_card) {
            show_feature_settings_popup(feature_to_show, parent_screen_for_popup);
        }
    }
}

void create_control_page_ui(lv_obj_t *scr) {
    lv_obj_add_style(scr, &style_scr_bg, 0);
    lv_obj_set_style_pad_all(scr, 0, 0);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t *page_content_container = lv_obj_create(scr);
    lv_obj_remove_style_all(page_content_container);
    lv_obj_set_width(page_content_container, LV_PCT(100));
    lv_obj_set_flex_grow(page_content_container, 1);
    lv_obj_add_flag(page_content_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(page_content_container, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(page_content_container, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_clear_flag(page_content_container, LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_ELASTIC);

    lv_obj_set_layout(page_content_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(page_content_container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(page_content_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_set_style_pad_all(page_content_container, 10, 0);
    lv_obj_set_style_pad_column(page_content_container, 10, 0);
    lv_obj_set_style_pad_row(page_content_container, 10, 0);

    create_lighting_card_ui(page_content_container);
    create_voice_card_ui(page_content_container);
    create_drying_card_ui(page_content_container);
    create_fresh_air_card_ui(page_content_container);
    create_sterilize_card_ui(page_content_container);
    create_hatching_card_ui(page_content_container);
    create_thermal_card_ui(page_content_container);      // 新增
    create_humidify_card_ui(page_content_container);     // 新增
    create_common_bottom_nav(scr, SCREEN_ID_CONTROL);
    Serial.println("UI_ControlPage: UI created with new card structure.");
}

#include "UI_Styles.h"
#include "../Config/Config.h"
#include "fengyahei_s20_bpp4.h"
#include "fengyahei_s16_bpp4.h"

lv_style_t style_scr_bg;
lv_style_t style_panel_independent;
lv_style_t style_text_white_custom_font;
lv_style_t style_meter_temp_value_text;
lv_style_t style_meter_humid_value_text;
lv_style_t style_btn_animated_default;
lv_style_t style_btn_animated_pressed;
lv_style_t style_meter_indicator_arc_shadow;
static lv_style_transition_dsc_t trans_btn_animated;
lv_style_t style_card_main;
lv_style_t style_card_title_label;
lv_style_t style_card_icon;
lv_style_t style_roller_options_custom;
bool style_roller_options_inited = false;
lv_style_t style_switch_on_cyan;
lv_style_t style_progress_bar_glow;

void ui_styles_init(void) {
    lv_style_init(&style_scr_bg);
    lv_style_set_bg_color(&style_scr_bg, SCREEN_BG_COLOR);
    lv_style_set_bg_opa(&style_scr_bg, LV_OPA_COVER);

    lv_style_init(&style_panel_independent);
    lv_style_set_bg_color(&style_panel_independent, PANEL_BG_COLOR);
    lv_style_set_bg_opa(&style_panel_independent, LV_OPA_COVER);
    lv_style_set_border_color(&style_panel_independent, lv_color_hex(0x4682C8));
    lv_style_set_border_width(&style_panel_independent, 1);
    lv_style_set_radius(&style_panel_independent, 6);
    lv_style_set_pad_all(&style_panel_independent, 5);

    lv_style_init(&style_text_white_custom_font);
    lv_style_set_text_font(&style_text_white_custom_font, &fengyahei_s16_bpp4);
    lv_style_set_text_color(&style_text_white_custom_font, TEXT_COLOR_WHITE);

    lv_style_init(&style_meter_temp_value_text);
    lv_style_set_text_font(&style_meter_temp_value_text, &lv_font_montserrat_24);
    lv_style_set_text_color(&style_meter_temp_value_text, TEMP_VALUE_COLOR);

    lv_style_init(&style_meter_humid_value_text);
    lv_style_set_text_font(&style_meter_humid_value_text, &lv_font_montserrat_24);
    lv_style_set_text_color(&style_meter_humid_value_text, HUMID_VALUE_COLOR);

    lv_style_init(&style_btn_animated_default);
    lv_style_set_radius(&style_btn_animated_default, 5);
    lv_style_set_bg_opa(&style_btn_animated_default, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn_animated_default, BUTTON_BG_COLOR);
    lv_style_set_bg_grad_color(&style_btn_animated_default, lv_color_darken(BUTTON_BG_COLOR, LV_OPA_30));
    lv_style_set_bg_grad_dir(&style_btn_animated_default, LV_GRAD_DIR_VER);
    lv_style_set_border_opa(&style_btn_animated_default, LV_OPA_40);
    lv_style_set_border_width(&style_btn_animated_default, 1);
    lv_style_set_border_color(&style_btn_animated_default, lv_color_darken(BUTTON_BG_COLOR, LV_OPA_50));
    lv_style_set_shadow_width(&style_btn_animated_default, 6);
    lv_style_set_shadow_color(&style_btn_animated_default, lv_color_hex(0x333333));
    lv_style_set_shadow_ofs_y(&style_btn_animated_default, 4);
    lv_style_set_text_color(&style_btn_animated_default, TEXT_COLOR_WHITE);
    lv_style_set_text_font(&style_btn_animated_default, &fengyahei_s16_bpp4);
    lv_style_set_pad_all(&style_btn_animated_default, 10);

    lv_style_init(&style_btn_animated_pressed);
    lv_style_set_outline_width(&style_btn_animated_pressed, 15);
    lv_style_set_outline_opa(&style_btn_animated_pressed, LV_OPA_TRANSP);
    lv_style_set_shadow_ofs_y(&style_btn_animated_pressed, 1);
    lv_style_set_bg_color(&style_btn_animated_pressed, lv_color_darken(BUTTON_BG_COLOR, LV_OPA_30));
    lv_style_set_bg_grad_color(&style_btn_animated_pressed, lv_color_darken(BUTTON_BG_COLOR, LV_OPA_50));

    static lv_style_prop_t props_btn_animated[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, LV_STYLE_PROP_INV};
    lv_style_transition_dsc_init(&trans_btn_animated, props_btn_animated, lv_anim_path_ease_out, 200, 0, NULL);
    lv_style_set_transition(&style_btn_animated_pressed, &trans_btn_animated);

    lv_style_init(&style_switch_on_cyan);
    lv_style_set_bg_color(&style_switch_on_cyan, SWITCH_COLOR_ON_CYAN);
    lv_style_set_bg_opa(&style_switch_on_cyan, LV_OPA_COVER);

    lv_style_init(&style_meter_indicator_arc_shadow);
    lv_style_set_shadow_width(&style_meter_indicator_arc_shadow, 8);
    lv_style_set_shadow_opa(&style_meter_indicator_arc_shadow, LV_OPA_40);
    lv_style_set_shadow_ofs_x(&style_meter_indicator_arc_shadow, 3);
    lv_style_set_shadow_ofs_y(&style_meter_indicator_arc_shadow, 3);
    lv_style_set_shadow_color(&style_meter_indicator_arc_shadow, lv_color_black());

    lv_style_init(&style_card_main);
    lv_style_set_bg_color(&style_card_main, PANEL_BG_COLOR);
    lv_style_set_bg_opa(&style_card_main, LV_OPA_COVER);
    lv_style_set_radius(&style_card_main, 8);
    lv_style_set_border_color(&style_card_main, lv_color_hex(0x354B68));
    lv_style_set_border_width(&style_card_main, 1);
    lv_style_set_shadow_width(&style_card_main, 10);
    lv_style_set_shadow_opa(&style_card_main, LV_OPA_10);
    lv_style_set_shadow_ofs_y(&style_card_main, 5);
    lv_style_set_shadow_color(&style_card_main, lv_color_black());
    lv_style_set_pad_all(&style_card_main, 10);

    lv_style_init(&style_card_title_label);
    lv_style_set_text_font(&style_card_title_label, &fengyahei_s20_bpp4);
    lv_style_set_text_color(&style_card_title_label, TEXT_COLOR_WHITE);

    lv_style_init(&style_card_icon);
    lv_style_set_text_color(&style_card_icon, lv_palette_main(LV_PALETTE_GREY));

    lv_style_init(&style_roller_options_custom);
    lv_style_set_text_color(&style_roller_options_custom, ROLLER_TEXT_MUTED_COLOR_BRIGHTER);
    lv_style_set_text_font(&style_roller_options_custom, &fengyahei_s16_bpp4);
    lv_style_set_bg_color(&style_roller_options_custom, PANEL_BG_COLOR);
    lv_style_set_bg_opa(&style_roller_options_custom, LV_OPA_COVER);

    lv_style_init(&style_progress_bar_glow);
    lv_style_set_bg_color(&style_progress_bar_glow, lv_color_hex(0x00AEEF));
    lv_style_set_radius(&style_progress_bar_glow, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_width(&style_progress_bar_glow, 15);
    lv_style_set_shadow_color(&style_progress_bar_glow, lv_color_hex(0x00AEEF));
    lv_style_set_shadow_spread(&style_progress_bar_glow, 5);
}
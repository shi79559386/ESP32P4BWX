// File: UI_HatchingSettingsPopup.cpp - 修改版本：增加和尚品种，参数框可点击选择
#include "UI_HatchingSettingsPopup.h"
#include "UI_PopupBase.h"
#include "../UI_Styles.h"
#include "../font/fengyahei_s16_bpp4.h"
#include "../font/fengyahei_s20_bpp4.h"
#include "../font/fengyahei_s14_bpp4.h"
#include "../font/fengyahei_s24_bpp4.h"
#include "../../Config/Config.h"
#include "../../SettingsLogic/HatchingSettings.h"
#include "../../App/AppGlobal.h"
#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// UI元素指针
static lv_obj_t *hatching_species_dropdown = NULL;
static lv_obj_t *hatching_status_container = NULL;
static lv_obj_t *hatching_status_label = NULL;
static lv_obj_t *hatching_progress_bar = NULL;
static lv_obj_t *hatching_temp_slider = NULL;
static lv_obj_t *hatching_humid_slider = NULL;
static lv_obj_t *hatching_temp_value_label = NULL;
static lv_obj_t *hatching_humid_value_label = NULL;
static lv_obj_t *hatching_info_label = NULL;
static lv_obj_t *hatching_days_container = NULL;
static lv_obj_t *hatching_days_slider = NULL;
static lv_obj_t *hatching_days_value_label = NULL;
static lv_obj_t *hatching_remaining_days_label = NULL;
static lv_obj_t *temp_row = NULL;
static lv_obj_t *humid_row = NULL;

// 品种参数表 - 增加和尚品种
static const struct {
    const char* name;
    float temp_default;
    int humidity_default;
    uint8_t days;
} species_params[PARROT_SPECIES_COUNT + 2] = {
    {"牡丹", 37.5f, 55, 23}, 
    {"玄凤", 37.6f, 60, 18}, 
    {"太阳锥尾", 37.7f, 62, 24},
    {"金太阳", 37.8f, 67, 26}, 
    {"非洲灰", 37.9f, 65, 28}, 
    {"蓝黄金刚", 38.0f, 72, 26},
    {"和尚", 38.0f, 70, 29}, // 新增的和尚品种
    {"自定义", 37.5f, 60, 21} // 自定义现在是最后一个
};

static parrot_species_t selected_species = PARROT_SPECIES_BUDGIE;
static float custom_temp = 37.5f;
static int custom_humidity = 60;
static int custom_days = 21;
static int elapsed_days = 0;

// Helper function to set temperature label using dtostrf
static void set_temperature_label(lv_obj_t* label, float temp_value) {
    if (label && lv_obj_is_valid(label)) {
        char buf[10];
        dtostrf(temp_value, 4, 1, buf);
        strcat(buf, "°C");
        lv_label_set_text(label, buf);
    }
}

// 点击推荐参数框的事件处理
static void hatching_info_click_event_cb(lv_event_t *e) {
    if (hatching_species_dropdown && lv_obj_is_valid(hatching_species_dropdown)) {
        lv_dropdown_open(hatching_species_dropdown);
        lv_obj_t* list = lv_dropdown_get_list(hatching_species_dropdown);
        if (list && lv_obj_is_valid(list)) {
            // 获取父容器（整个弹窗）
            lv_obj_t* parent = lv_obj_get_parent(hatching_species_dropdown);
            // 让列表相对于父容器居中显示
            lv_obj_align_to(list, parent, LV_ALIGN_CENTER, 0, 0);
        }
        Serial.println("点击参数框，打开品种选择");
    }
}

static void global_click_event_cb(lv_event_t *e) {
    // 既然点击到了遮罩，说明是点击弹窗外，只需关闭下拉框
    if (hatching_species_dropdown && lv_obj_is_valid(hatching_species_dropdown)) {
        lv_dropdown_close(hatching_species_dropdown);
    }
    // 事件在遮罩层就被消化了，不会传递到下层控件
}

static void hatching_popup_delete_cleanup_handler(lv_event_t *e) {

    hatching_species_dropdown = NULL; 
    hatching_status_container = NULL; 
    hatching_status_label = NULL;
    hatching_progress_bar = NULL; 
    hatching_temp_slider = NULL; 
    hatching_humid_slider = NULL;
    hatching_temp_value_label = NULL; 
    hatching_humid_value_label = NULL; 
    hatching_info_label = NULL;
    hatching_days_container = NULL; 
    hatching_days_slider = NULL; 
    hatching_days_value_label = NULL;
    hatching_remaining_days_label = NULL;
    temp_row = NULL;
    humid_row = NULL;
}

static void update_species_info() {
    // 更新信息标签 (hatching_info_label) - 新格式
    if (hatching_info_label && lv_obj_is_valid(hatching_info_label)) {
        char info_text[200];
        char temp_buf[10];
        if (selected_species == PARROT_SPECIES_COUNT + 1) { // 自定义（现在是最后一个）
            dtostrf(custom_temp, 4, 1, temp_buf);
            snprintf(info_text, sizeof(info_text), 
                     "自定义\n温度: %s°C 湿度: %d%%\n孵化期: %d天",
                     temp_buf, custom_humidity, custom_days);
        } else {
            const auto& params = species_params[selected_species];
            dtostrf(params.temp_default, 4, 1, temp_buf);
            snprintf(info_text, sizeof(info_text),
                     "%s\n温度: %s°C 湿度: %d%%\n孵化期: %d天",
                     params.name, temp_buf, params.humidity_default, params.days);
        }
        lv_label_set_text(hatching_info_label, info_text);
    }

    bool is_custom = (selected_species == PARROT_SPECIES_COUNT + 1); // 更新自定义判断

    // 控制温度行可见性
    if (::temp_row && lv_obj_is_valid(::temp_row)) {
        if (is_custom) {
            lv_obj_clear_flag(::temp_row, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(::temp_row, LV_OBJ_FLAG_HIDDEN);
        }
    }

    // 控制湿度行可见性
    if (::humid_row && lv_obj_is_valid(::humid_row)) {
        if (is_custom) {
            lv_obj_clear_flag(::humid_row, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(::humid_row, LV_OBJ_FLAG_HIDDEN);
        }
    }

    // 控制孵化天数容器可见性
    if (hatching_days_container && lv_obj_is_valid(hatching_days_container)) {
        if (is_custom) {
            lv_obj_clear_flag(hatching_days_container, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(hatching_days_container, LV_OBJ_FLAG_HIDDEN);
        }
    }

    // 更新参数值
    if (!is_custom) {
        const auto& params = species_params[selected_species];
        custom_temp = params.temp_default;
        custom_humidity = params.humidity_default;
        custom_days = params.days;
        if (hatching_temp_slider && lv_obj_is_valid(hatching_temp_slider)) {
            lv_slider_set_value(hatching_temp_slider, (int32_t)(custom_temp * 10), LV_ANIM_OFF);
        }
        if (hatching_humid_slider && lv_obj_is_valid(hatching_humid_slider)) {
            lv_slider_set_value(hatching_humid_slider, custom_humidity, LV_ANIM_OFF);
        }
        if (hatching_days_slider && lv_obj_is_valid(hatching_days_slider)) {
             lv_slider_set_value(hatching_days_slider, custom_days, LV_ANIM_OFF);
        }
    }

    // 更新数值标签
    set_temperature_label(hatching_temp_value_label, custom_temp);
    if (hatching_humid_value_label && lv_obj_is_valid(hatching_humid_value_label)) {
        lv_label_set_text_fmt(hatching_humid_value_label, "%d%%", custom_humidity);
    }
    if (hatching_days_value_label && lv_obj_is_valid(hatching_days_value_label)) {
        lv_label_set_text_fmt(hatching_days_value_label, "%d天", custom_days);
    }

    // 更新剩余天数标签
    if (hatching_remaining_days_label && lv_obj_is_valid(hatching_remaining_days_label)) {
        if (HatchingSettings_IsActive()) {
            int total_hatching_days = custom_days;
            const hatching_status_t* current_status = HatchingSettings_GetStatus();
            if (current_status) {
                total_hatching_days = current_status->total_days;
                elapsed_days = current_status->days_elapsed;
            }
            int remaining = total_hatching_days - elapsed_days;
            if (remaining < 0) remaining = 0;
            lv_label_set_text_fmt(hatching_remaining_days_label, "剩余天数: %d天", remaining);
            lv_obj_clear_flag(hatching_remaining_days_label, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(hatching_remaining_days_label, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void update_hatching_status() {
    bool is_active = HatchingSettings_IsActive();
    if (hatching_status_container && lv_obj_is_valid(hatching_status_container)) {
        if (is_active) {
            lv_obj_clear_flag(hatching_status_container, LV_OBJ_FLAG_HIDDEN);
            const hatching_status_t* status = HatchingSettings_GetStatus();
            if (status && hatching_status_label && lv_obj_is_valid(hatching_status_label)) {
                elapsed_days = status->days_elapsed;
                selected_species = status->species;
                custom_temp = status->target_temp;
                custom_humidity = status->target_humidity;
                custom_days = status->total_days;

                if(hatching_species_dropdown && lv_obj_is_valid(hatching_species_dropdown)) {
                    lv_dropdown_set_selected(hatching_species_dropdown, status->species);
                }
                if (hatching_temp_slider && lv_obj_is_valid(hatching_temp_slider)) {
                    lv_slider_set_value(hatching_temp_slider, (int32_t)(status->target_temp * 10), LV_ANIM_OFF);
                }
                if (hatching_humid_slider && lv_obj_is_valid(hatching_humid_slider)) {
                    lv_slider_set_value(hatching_humid_slider, status->target_humidity, LV_ANIM_OFF);
                }
                if (hatching_days_slider && lv_obj_is_valid(hatching_days_slider)) {
                    lv_slider_set_value(hatching_days_slider, status->total_days, LV_ANIM_OFF);
                }

                char status_text[120];
                char time_remaining[30];
                HatchingSettings_GetFormattedTimeRemaining(time_remaining, sizeof(time_remaining));
                snprintf(status_text, sizeof(status_text), "品种: %s 第%d/%d天\n阶段: %s 剩余: %s",
                         species_params[status->species].name, status->days_elapsed, status->total_days,
                         HatchingSettings_GetStageName(status->current_stage), time_remaining);
                lv_label_set_text(hatching_status_label, status_text);

                if (hatching_progress_bar && lv_obj_is_valid(hatching_progress_bar)) {
                    int progress = (status->total_days > 0) ? (status->days_elapsed * 100) / status->total_days : 0;
                    lv_bar_set_value(hatching_progress_bar, progress, LV_ANIM_ON);
                }
            }
        } else {
            lv_obj_add_flag(hatching_status_container, LV_OBJ_FLAG_HIDDEN);
            elapsed_days = 0;
        }
    }
    update_species_info();
}

static void hatching_species_dropdown_event_cb(lv_event_t *e) {
    lv_obj_t *dropdown = lv_event_get_target(e);
    if (!dropdown || !lv_obj_is_valid(dropdown)) return;
    selected_species = (parrot_species_t)lv_dropdown_get_selected(dropdown);
    update_species_info();
    Serial.printf("孵化：已选择物种 - %s\n", species_params[selected_species].name);
}

static void hatching_temp_slider_event_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    if (!slider || !lv_obj_is_valid(slider)) return;
    int32_t value = lv_slider_get_value(slider);
    custom_temp = value / 10.0f;
    set_temperature_label(hatching_temp_value_label, custom_temp);
    if (selected_species == PARROT_SPECIES_COUNT + 1) {
        update_species_info();
    }
    if (HatchingSettings_IsActive()) {
        HatchingSettings_SetCustomParameters(custom_temp, custom_humidity);
    }
}

static void hatching_humid_slider_event_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    if (!slider || !lv_obj_is_valid(slider)) return;
    custom_humidity = lv_slider_get_value(slider);
    if (hatching_humid_value_label && lv_obj_is_valid(hatching_humid_value_label)) {
        lv_label_set_text_fmt(hatching_humid_value_label, "%d%%", custom_humidity);
    }
    if (selected_species == PARROT_SPECIES_COUNT + 1) {
        update_species_info();
    }
    if (HatchingSettings_IsActive()) {
        HatchingSettings_SetCustomParameters(custom_temp, custom_humidity);
    }
}

static void hatching_days_slider_event_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    if (!slider || !lv_obj_is_valid(slider)) return;
    custom_days = lv_slider_get_value(slider);
    if (hatching_days_value_label && lv_obj_is_valid(hatching_days_value_label)) {
        lv_label_set_text_fmt(hatching_days_value_label, "%d天", custom_days);
    }
    if (selected_species == PARROT_SPECIES_COUNT + 1) {
        update_species_info();
    }
}

// 温度按钮事件回调
static void temp_minus_btn_event_cb(lv_event_t *e) {
    if (hatching_temp_slider && lv_obj_is_valid(hatching_temp_slider)) {
        int32_t current_value = lv_slider_get_value(hatching_temp_slider);
        int32_t new_value = current_value - 1;
        if (new_value >= 200) {
            lv_slider_set_value(hatching_temp_slider, new_value, LV_ANIM_OFF);
            custom_temp = new_value / 10.0f;
            set_temperature_label(hatching_temp_value_label, custom_temp);
            if (selected_species == PARROT_SPECIES_COUNT + 1) {
                update_species_info();
            }
            if (HatchingSettings_IsActive()) {
                HatchingSettings_SetCustomParameters(custom_temp, custom_humidity);
            }
        }
    }
}

static void temp_plus_btn_event_cb(lv_event_t *e) {
    if (hatching_temp_slider && lv_obj_is_valid(hatching_temp_slider)) {
        int32_t current_value = lv_slider_get_value(hatching_temp_slider);
        int32_t new_value = current_value + 1;
        if (new_value <= 400) {
            lv_slider_set_value(hatching_temp_slider, new_value, LV_ANIM_OFF);
            custom_temp = new_value / 10.0f;
            set_temperature_label(hatching_temp_value_label, custom_temp);
            if (selected_species == PARROT_SPECIES_COUNT + 1) {
                update_species_info();
            }
            if (HatchingSettings_IsActive()) {
                HatchingSettings_SetCustomParameters(custom_temp, custom_humidity);
            }
        }
    }
}

// 湿度按钮事件回调
static void humid_minus_btn_event_cb(lv_event_t *e) {
    if (hatching_humid_slider && lv_obj_is_valid(hatching_humid_slider)) {
        int32_t current_value = lv_slider_get_value(hatching_humid_slider);
        int32_t new_value = current_value - 1;
        if (new_value >= 0) {
            lv_slider_set_value(hatching_humid_slider, new_value, LV_ANIM_OFF);
            custom_humidity = new_value;
            if (hatching_humid_value_label && lv_obj_is_valid(hatching_humid_value_label)) {
                lv_label_set_text_fmt(hatching_humid_value_label, "%d%%", custom_humidity);
            }
            if (selected_species == PARROT_SPECIES_COUNT + 1) {
                update_species_info();
            }
            if (HatchingSettings_IsActive()) {
                HatchingSettings_SetCustomParameters(custom_temp, custom_humidity);
            }
        }
    }
}

static void humid_plus_btn_event_cb(lv_event_t *e) {
    if (hatching_humid_slider && lv_obj_is_valid(hatching_humid_slider)) {
        int32_t current_value = lv_slider_get_value(hatching_humid_slider);
        int32_t new_value = current_value + 1;
        if (new_value <= 100) {
            lv_slider_set_value(hatching_humid_slider, new_value, LV_ANIM_OFF);
            custom_humidity = new_value;
            if (hatching_humid_value_label && lv_obj_is_valid(hatching_humid_value_label)) {
                lv_label_set_text_fmt(hatching_humid_value_label, "%d%%", custom_humidity);
            }
            if (selected_species == PARROT_SPECIES_COUNT + 1) {
                update_species_info();
            }
            if (HatchingSettings_IsActive()) {
                HatchingSettings_SetCustomParameters(custom_temp, custom_humidity);
            }
        }
    }
}

// 孵化天数按钮事件回调
static void days_minus_btn_event_cb(lv_event_t *e) {
    if (hatching_days_slider && lv_obj_is_valid(hatching_days_slider)) {
        int32_t current_value = lv_slider_get_value(hatching_days_slider);
        int32_t new_value = current_value - 1;
        if (new_value >= 1) {
            lv_slider_set_value(hatching_days_slider, new_value, LV_ANIM_OFF);
            custom_days = new_value;
            if (hatching_days_value_label && lv_obj_is_valid(hatching_days_value_label)) {
                lv_label_set_text_fmt(hatching_days_value_label, "%d天", custom_days);
            }
            if (selected_species == PARROT_SPECIES_COUNT + 1) {
                update_species_info();
            }
        }
    }
}

static void days_plus_btn_event_cb(lv_event_t *e) {
    if (hatching_days_slider && lv_obj_is_valid(hatching_days_slider)) {
        int32_t current_value = lv_slider_get_value(hatching_days_slider);
        int32_t new_value = current_value + 1;
        if (new_value <= 40) {
            lv_slider_set_value(hatching_days_slider, new_value, LV_ANIM_OFF);
            custom_days = new_value;
            if (hatching_days_value_label && lv_obj_is_valid(hatching_days_value_label)) {
                lv_label_set_text_fmt(hatching_days_value_label, "%d天", custom_days);
            }
            if (selected_species == PARROT_SPECIES_COUNT + 1) {
                update_species_info();
            }
        }
    }
}

void create_hatching_settings_popup(lv_obj_t *parent_scr) {
    lv_obj_t *content_area = create_popup_frame(parent_scr, "设置");
    if (!content_area) return;
    //lv_obj_t *current_screen = lv_scr_act();
    //lv_obj_add_event_cb(current_screen, global_click_event_cb, LV_EVENT_CLICKED, content_area);
    lv_obj_set_style_pad_bottom(content_area, 20, 0);
    lv_obj_set_scrollbar_mode(content_area, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_snap_y(content_area, LV_SCROLL_SNAP_START);
    lv_obj_t *popup_box = lv_obj_get_parent(content_area);
    lv_obj_t *popup_mask = lv_obj_get_parent(popup_box);
    if (popup_mask) {
        // 让遮罩可点击，拦截所有弹窗外的点击
    lv_obj_add_flag(popup_mask, LV_OBJ_FLAG_CLICKABLE);
    // 点击遮罩时的处理逻辑
    lv_obj_add_event_cb(popup_mask, global_click_event_cb, LV_EVENT_CLICKED, content_area);
        lv_obj_add_event_cb(popup_mask, hatching_popup_delete_cleanup_handler, LV_EVENT_DELETE, NULL);
    }

    // --- 可点击的参数信息框 ---
    hatching_info_label = lv_label_create(content_area);
    lv_obj_add_style(hatching_info_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(hatching_info_label, &fengyahei_s14_bpp4, 0);
    lv_obj_set_flex_grow(hatching_info_label, 0);
    lv_obj_set_width(hatching_info_label, LV_PCT(85));
    lv_obj_align(hatching_info_label, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_align(hatching_info_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_bg_color(hatching_info_label, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_bg_opa(hatching_info_label, LV_OPA_20, 0);
    lv_obj_set_style_pad_all(hatching_info_label, 12, 0);
    lv_obj_set_style_radius(hatching_info_label, 8, 0);
    lv_label_set_long_mode(hatching_info_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_pad_top(hatching_info_label, 10, 0);
    
    // 让标签可点击
    lv_obj_add_flag(hatching_info_label, LV_OBJ_FLAG_CLICKABLE);
    // 添加点击事件
    lv_obj_add_event_cb(hatching_info_label, hatching_info_click_event_cb, LV_EVENT_CLICKED, NULL);

    // --- 隐藏的下拉框用于选择功能 ---
    hatching_species_dropdown = lv_dropdown_create(content_area);
    lv_obj_set_width(hatching_species_dropdown, 250);
    lv_obj_set_height(hatching_species_dropdown, LV_SIZE_CONTENT);
    lv_obj_add_flag(hatching_species_dropdown, LV_OBJ_FLAG_HIDDEN); // 隐藏下拉框
    lv_obj_align(hatching_species_dropdown, LV_ALIGN_CENTER, 0, 0); // 居中对齐

    lv_dropdown_set_options(hatching_species_dropdown,
        "牡丹\n玄凤\n太阳锥尾\n金太阳\n非洲灰\n蓝黄金刚\n和尚\n自定义"); // 增加和尚选项
    lv_dropdown_set_symbol(hatching_species_dropdown, "");
    lv_obj_add_style(hatching_species_dropdown, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(hatching_species_dropdown, &fengyahei_s20_bpp4, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hatching_species_dropdown, PANEL_BG_COLOR, 0);
    lv_obj_set_style_border_color(hatching_species_dropdown, ROLLER_BORDER_COLOR, 0);
    lv_obj_set_style_border_width(hatching_species_dropdown, 2, 0);
    lv_obj_set_style_radius(hatching_species_dropdown, 6, 0);

    lv_obj_t* list = lv_dropdown_get_list(hatching_species_dropdown);
    if (list) {
        lv_obj_set_style_text_font(list, &fengyahei_s16_bpp4, (uint32_t)LV_PART_MAIN | (uint32_t)LV_STATE_DEFAULT);
        lv_obj_add_style(list, &style_text_white_custom_font, 0); 
        lv_obj_set_style_bg_color(list, PANEL_BG_COLOR, 0);
        lv_obj_set_style_border_color(list, TEXT_BTN_BOX_BG_COLOR, 0); 
        lv_obj_set_style_border_width(list, 2, 0);
        lv_obj_set_style_text_align(list, LV_TEXT_ALIGN_CENTER, 0);
        // 让列表居中显示
        lv_obj_align(list, LV_ALIGN_CENTER, 0, 0);
    }

    lv_obj_add_event_cb(hatching_species_dropdown, hatching_species_dropdown_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // --- 剩余天数标签 ---
    hatching_remaining_days_label = lv_label_create(content_area);
    lv_obj_add_style(hatching_remaining_days_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(hatching_remaining_days_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(hatching_remaining_days_label, LV_PCT(95));
    lv_obj_align(hatching_remaining_days_label, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_align(hatching_remaining_days_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(hatching_remaining_days_label, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_obj_set_style_pad_all(hatching_remaining_days_label, 8, 0);
    lv_obj_set_style_radius(hatching_remaining_days_label, 8, 0);
    lv_obj_add_flag(hatching_remaining_days_label, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_pad_top(hatching_remaining_days_label, 5, 0);

    // --- 参数调整区域 ---
    lv_obj_t *params_section = lv_obj_create(content_area);
    lv_obj_remove_style_all(params_section);
    lv_obj_set_width(params_section, LV_PCT(100));
    lv_obj_set_height(params_section, LV_SIZE_CONTENT);
    lv_obj_set_layout(params_section, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(params_section, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(params_section, 8, 0);
    lv_obj_set_flex_align(params_section, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_hor(params_section, 5, 0);
    lv_obj_set_style_pad_top(params_section, 10, 0);

    // ============== 温度行 ==============
    ::temp_row = lv_obj_create(params_section);
    lv_obj_remove_style_all(::temp_row);
    lv_obj_set_width(::temp_row, LV_PCT(100));
    lv_obj_set_height(::temp_row, 50);
    lv_obj_set_style_pad_bottom(::temp_row, 3, 0);
    lv_obj_add_flag(::temp_row, LV_OBJ_FLAG_HIDDEN);

    hatching_temp_slider = lv_slider_create(::temp_row);
    lv_obj_set_width(hatching_temp_slider, LV_PCT(64));
    lv_obj_set_height(hatching_temp_slider, 15);
    lv_obj_align(hatching_temp_slider, LV_ALIGN_CENTER, 0, 8);
    lv_slider_set_range(hatching_temp_slider, 200, 400);
    lv_slider_set_value(hatching_temp_slider, (int32_t)(custom_temp * 10), LV_ANIM_OFF);
    lv_obj_set_style_bg_color(hatching_temp_slider, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_color(hatching_temp_slider, TEMP_VALUE_COLOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(hatching_temp_slider, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_radius(hatching_temp_slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_add_event_cb(hatching_temp_slider, hatching_temp_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 温度减号按钮
    lv_obj_t *temp_minus_btn = lv_btn_create(::temp_row);
    lv_obj_set_size(temp_minus_btn, 30, 30);
    lv_obj_align_to(temp_minus_btn, hatching_temp_slider, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_obj_add_event_cb(temp_minus_btn, temp_minus_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *temp_minus_label = lv_label_create(temp_minus_btn);
    lv_label_set_text(temp_minus_label, "-");
    lv_obj_set_style_text_font(temp_minus_label, &fengyahei_s24_bpp4, 0);
    lv_obj_center(temp_minus_label);

    // 温度加号按钮
    lv_obj_t *temp_plus_btn = lv_btn_create(::temp_row);
    lv_obj_set_size(temp_plus_btn, 30, 30);
    lv_obj_align_to(temp_plus_btn, hatching_temp_slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(temp_plus_btn, temp_plus_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *temp_plus_label = lv_label_create(temp_plus_btn);
    lv_label_set_text(temp_plus_label, "+");
    lv_obj_set_style_text_font(temp_plus_label, &fengyahei_s24_bpp4, 0);
    lv_obj_center(temp_plus_label);

    // 温度数值标签
    hatching_temp_value_label = lv_label_create(::temp_row);
    lv_obj_add_style(hatching_temp_value_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_color(hatching_temp_value_label, TEMP_VALUE_COLOR, 0);
    lv_obj_set_style_text_font(hatching_temp_value_label, &fengyahei_s16_bpp4, 0);
    lv_obj_align(hatching_temp_value_label, LV_ALIGN_CENTER, 0, -13);
    set_temperature_label(hatching_temp_value_label, custom_temp);

    // ============== 湿度行 ==============
    ::humid_row = lv_obj_create(params_section);
    lv_obj_remove_style_all(::humid_row);
    lv_obj_set_width(::humid_row, LV_PCT(100));
    lv_obj_set_height(::humid_row, 50);
    lv_obj_set_style_pad_bottom(::humid_row, 6, 0);
    lv_obj_add_flag(::humid_row, LV_OBJ_FLAG_HIDDEN);

    hatching_humid_slider = lv_slider_create(::humid_row);
    lv_obj_set_width(hatching_humid_slider, LV_PCT(64));
    lv_obj_set_height(hatching_humid_slider, 15);
    lv_obj_align(hatching_humid_slider, LV_ALIGN_CENTER, 0, 8);
    lv_slider_set_range(hatching_humid_slider, 0, 100);
    lv_slider_set_value(hatching_humid_slider, custom_humidity, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(hatching_humid_slider, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_color(hatching_humid_slider, HUMID_VALUE_COLOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(hatching_humid_slider, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_radius(hatching_humid_slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_add_event_cb(hatching_humid_slider, hatching_humid_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 湿度减号按钮
    lv_obj_t *humid_minus_btn = lv_btn_create(::humid_row);
    lv_obj_set_size(humid_minus_btn, 30, 30);
    lv_obj_align_to(humid_minus_btn, hatching_humid_slider, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_obj_add_event_cb(humid_minus_btn, humid_minus_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *humid_minus_label = lv_label_create(humid_minus_btn);
    lv_label_set_text(humid_minus_label, "-");
    lv_obj_set_style_text_font(humid_minus_label, &fengyahei_s24_bpp4, 0);
    lv_obj_center(humid_minus_label);

    // 湿度加号按钮
    lv_obj_t *humid_plus_btn = lv_btn_create(::humid_row);
    lv_obj_set_size(humid_plus_btn, 30, 30);
    lv_obj_align_to(humid_plus_btn, hatching_humid_slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(humid_plus_btn, humid_plus_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *humid_plus_label = lv_label_create(humid_plus_btn);
    lv_label_set_text(humid_plus_label, "+");
    lv_obj_set_style_text_font(humid_plus_label, &fengyahei_s24_bpp4, 0);
    lv_obj_center(humid_plus_label);

    // 湿度数值标签
    hatching_humid_value_label = lv_label_create(::humid_row);
    lv_obj_add_style(hatching_humid_value_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_color(hatching_humid_value_label, HUMID_VALUE_COLOR, 0);
    lv_obj_set_style_text_font(hatching_humid_value_label, &fengyahei_s16_bpp4, 0);
    lv_obj_align(hatching_humid_value_label, LV_ALIGN_CENTER, 0, -13);
    lv_label_set_text_fmt(hatching_humid_value_label, "%d%%", custom_humidity);

    // ============== 孵化天数行 ==============
    hatching_days_container = lv_obj_create(params_section);
    lv_obj_remove_style_all(hatching_days_container);
    lv_obj_set_width(hatching_days_container, LV_PCT(100));
    lv_obj_set_height(hatching_days_container, 50);
    lv_obj_set_style_pad_bottom(hatching_days_container, 3, 0);
    lv_obj_add_flag(hatching_days_container, LV_OBJ_FLAG_HIDDEN);

    hatching_days_slider = lv_slider_create(hatching_days_container);
    lv_obj_set_width(hatching_days_slider, LV_PCT(64));
    lv_obj_set_height(hatching_days_slider, 15);
    lv_obj_align(hatching_days_slider, LV_ALIGN_CENTER, 0, 8);
    lv_slider_set_range(hatching_days_slider, 1, 40);
    lv_slider_set_value(hatching_days_slider, custom_days, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(hatching_days_slider, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_color(hatching_days_slider, lv_palette_main(LV_PALETTE_ORANGE), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(hatching_days_slider, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_radius(hatching_days_slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_add_event_cb(hatching_days_slider, hatching_days_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 天数减号按钮
    lv_obj_t *days_minus_btn = lv_btn_create(hatching_days_container);
    lv_obj_set_size(days_minus_btn, 30, 30);
    lv_obj_align_to(days_minus_btn, hatching_days_slider, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    lv_obj_add_event_cb(days_minus_btn, days_minus_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *days_minus_label = lv_label_create(days_minus_btn);
    lv_label_set_text(days_minus_label, "-");
    lv_obj_set_style_text_font(days_minus_label, &fengyahei_s24_bpp4, 0);
    lv_obj_center(days_minus_label);

    // 天数加号按钮
    lv_obj_t *days_plus_btn = lv_btn_create(hatching_days_container);
    lv_obj_set_size(days_plus_btn, 30, 30);
    lv_obj_align_to(days_plus_btn, hatching_days_slider, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(days_plus_btn, days_plus_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *days_plus_label = lv_label_create(days_plus_btn);
    lv_label_set_text(days_plus_label, "+");
    lv_obj_set_style_text_font(days_plus_label, &fengyahei_s24_bpp4, 0);
    lv_obj_center(days_plus_label);

    // 天数数值标签
    hatching_days_value_label = lv_label_create(hatching_days_container);
    lv_obj_add_style(hatching_days_value_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_color(hatching_days_value_label, lv_palette_main(LV_PALETTE_ORANGE), 0);
    lv_obj_set_style_text_font(hatching_days_value_label, &fengyahei_s16_bpp4, 0);
    lv_obj_align(hatching_days_value_label, LV_ALIGN_CENTER, 0, -13);
    lv_label_set_text_fmt(hatching_days_value_label, "%d天", custom_days);

    // --- 孵化状态区域 ---
    hatching_status_container = lv_obj_create(content_area);
    lv_obj_remove_style_all(hatching_status_container);
    lv_obj_set_width(hatching_status_container, LV_PCT(95)); 
    lv_obj_align(hatching_status_container, LV_ALIGN_TOP_MID, 0, 0); 
    lv_obj_set_height(hatching_status_container, LV_SIZE_CONTENT);
    lv_obj_set_layout(hatching_status_container, LV_LAYOUT_FLEX); 
    lv_obj_set_flex_flow(hatching_status_container, LV_FLEX_FLOW_COLUMN); 
    lv_obj_set_flex_align(hatching_status_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); 
    lv_obj_set_style_pad_row(hatching_status_container, 8, 0); 
    lv_obj_set_style_bg_color(hatching_status_container, lv_palette_darken(LV_PALETTE_GREEN, 3), 0); 
    lv_obj_set_style_bg_opa(hatching_status_container, LV_OPA_COVER, 0); 
    lv_obj_set_style_pad_all(hatching_status_container, 10, 0);
    lv_obj_set_style_radius(hatching_status_container, 8, 0);
    lv_obj_add_flag(hatching_status_container, LV_OBJ_FLAG_HIDDEN); 
    lv_obj_set_style_pad_top(hatching_status_container, 10, 0);

    lv_obj_t *status_title = lv_label_create(hatching_status_container);
    lv_label_set_text(status_title, "🥚 孵化进行中");
    lv_obj_add_style(status_title, &style_text_white_custom_font, 0); 
    lv_obj_set_style_text_color(status_title, lv_palette_lighten(LV_PALETTE_GREEN, 2), 0);
    lv_obj_set_style_text_align(status_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(status_title, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(status_title, LV_PCT(100));

    hatching_progress_bar = lv_bar_create(hatching_status_container);
    lv_obj_set_flex_grow(hatching_progress_bar, 0);
    lv_obj_set_width(hatching_progress_bar, LV_PCT(80));
    lv_obj_align(hatching_progress_bar, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_height(hatching_progress_bar, 12); 
    lv_obj_set_style_bg_color(hatching_progress_bar, ROLLER_COORDINATED_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_color(hatching_progress_bar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR); 
    lv_obj_set_style_radius(hatching_progress_bar, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_radius(hatching_progress_bar, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_bar_set_range(hatching_progress_bar, 0, 100);
    lv_bar_set_value(hatching_progress_bar, 0, LV_ANIM_OFF);
    
    hatching_status_label = lv_label_create(hatching_status_container);
    lv_obj_add_style(hatching_status_label, &style_text_white_custom_font, 0); 
    lv_obj_set_style_text_font(hatching_status_label, &fengyahei_s14_bpp4, 0);
    lv_obj_set_width(hatching_status_label, LV_PCT(100));
    lv_obj_set_style_text_align(hatching_status_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_long_mode(hatching_status_label, LV_LABEL_LONG_WRAP);

    // 初始化UI状态
    update_hatching_status();
    HatchingSettings_RegisterStatusUpdateCallback(update_hatching_status);
    Serial.println("孵化：设置弹出窗口已创建");
}
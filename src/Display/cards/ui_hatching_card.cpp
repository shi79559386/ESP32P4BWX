// File: src/Display/cards/ui_hatching_card.cpp
#include "ui_hatching_card.h"
#include "../UI_Styles.h"
#include "../../Config/Config.h"
#include "../../Peripherals/OutputControls.h"
#include "../../SettingsLogic/HatchingSettings.h"
#include "../font/fengyahei_s16_bpp4.h"
#include "../../App/AppGlobal.h"
#include "../UI_Common.h"
#include "../UI_ControlPage.h"
#include <Arduino.h>

// 注意：CAROUSEL_SWITCH_INTERVAL_MS 和 CAROUSEL_PAGE_COUNT 已在其他地方定义

// 简化的轮播结构体
typedef struct {
    lv_obj_t* content_container;  // 内容容器
    lv_obj_t* page_containers[2];  // 两个页面容器
    //lv_obj_t* page_indicators[2];  // 页面指示器（小点）
    
    // 页面1的标签（温度 + 湿度 + 状态）
    lv_obj_t* page1_temp_label;
    lv_obj_t* page1_humidity_label;
    lv_obj_t* page1_status_label;
    
    // 页面2的标签（状态 + 剩余时间 + 品种）
    lv_obj_t* page2_status_label;
    lv_obj_t* page2_remaining_label;
    lv_obj_t* page2_species_label;
    
    lv_timer_t* carousel_timer;    // 轮播定时器
    int current_page;              // 当前显示页面
    bool is_active;                // 是否激活轮播
    lv_coord_t viewport_height;    // 视窗高度
} hatching_carousel_t;

static hatching_carousel_t g_hatching_carousel = {0};
static lv_obj_t *card_hatching_main_switch = NULL;

// ============== 修复的滚动切换函数 ==============
static void hatching_carousel_slide_to_page(int page_index) {
    if (!g_hatching_carousel.content_container || !g_hatching_carousel.is_active) {
        return;
    }
    
    // 计算目标位置
    lv_coord_t target_y = -page_index * g_hatching_carousel.viewport_height;
    
    // 创建平移动画
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, g_hatching_carousel.content_container);
    lv_anim_set_values(&anim, lv_obj_get_y(g_hatching_carousel.content_container), target_y);
    lv_anim_set_time(&anim, 500);  // 500ms动画时长
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    lv_anim_start(&anim);
    
    /*// 更新指示器状态
    for (int i = 0; i < CAROUSEL_PAGE_COUNT; i++) {
        if (i == page_index) {
            lv_obj_add_state(g_hatching_carousel.page_indicators[i], LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(g_hatching_carousel.page_indicators[i], LV_STATE_CHECKED);
        }
    }*/
    
    g_hatching_carousel.current_page = page_index;
}

static void hatching_carousel_switch_page(lv_timer_t* timer) {
    if (!g_hatching_carousel.is_active) {
        return;
    }
    
    int next_page = (g_hatching_carousel.current_page + 1) % CAROUSEL_PAGE_COUNT;
    hatching_carousel_slide_to_page(next_page);
}

// ============== 创建左侧页面指示器 ==============
/*static void create_page_indicators(lv_obj_t* parent) {
    lv_obj_t* indicator_container = lv_obj_create(parent);
    lv_obj_remove_style_all(indicator_container);
    lv_obj_set_size(indicator_container, 15, 30);
    lv_obj_align(indicator_container, LV_ALIGN_BOTTOM_LEFT, 5, -20);
    lv_obj_set_layout(indicator_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(indicator_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(indicator_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(indicator_container, 6, 0);
    
    for (int i = 0; i < CAROUSEL_PAGE_COUNT; i++) {
        g_hatching_carousel.page_indicators[i] = lv_obj_create(indicator_container);
        lv_obj_remove_style_all(g_hatching_carousel.page_indicators[i]);
        lv_obj_set_size(g_hatching_carousel.page_indicators[i], 6, 6);
        lv_obj_set_style_radius(g_hatching_carousel.page_indicators[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(g_hatching_carousel.page_indicators[i], LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(g_hatching_carousel.page_indicators[i], lv_color_white(), 0);
        lv_obj_set_style_opa(g_hatching_carousel.page_indicators[i], LV_OPA_40, 0);
        lv_obj_set_style_opa(g_hatching_carousel.page_indicators[i], LV_OPA_100, LV_STATE_CHECKED);
    }
    
    // 默认显示第一页指示器
    lv_obj_add_state(g_hatching_carousel.page_indicators[0], LV_STATE_CHECKED);
}*/

// ============== 更新轮播内容函数 ==============
static void update_hatching_carousel_content() {
    if (!g_hatching_carousel.is_active) {
        return;
    }
    
    bool is_hatching_active = HatchingSettings_IsActive();
    
    if (is_hatching_active) {
        const hatching_status_t* status = HatchingSettings_GetStatus();
        if (status) {
            extern float g_current_temperature, g_current_temperature_2;
            extern int g_current_humidity, g_current_humidity_2;
            float avg_temp = (g_current_temperature + g_current_temperature_2) / 2.0f;
            int avg_humidity = (g_current_humidity + g_current_humidity_2) / 2;
            
            // 更新页面1内容（温度 + 湿度 + 状态）
            if (g_hatching_carousel.page1_temp_label && lv_obj_is_valid(g_hatching_carousel.page1_temp_label)) {
                lv_label_set_text_fmt(g_hatching_carousel.page1_temp_label, "温度: %.1f°C", avg_temp);
            }
            if (g_hatching_carousel.page1_humidity_label && lv_obj_is_valid(g_hatching_carousel.page1_humidity_label)) {
                lv_label_set_text_fmt(g_hatching_carousel.page1_humidity_label, "湿度: %d%%", avg_humidity);
            }
            if (g_hatching_carousel.page1_status_label && lv_obj_is_valid(g_hatching_carousel.page1_status_label)) {
                lv_label_set_text(g_hatching_carousel.page1_status_label, "孵化中");
            }
            
            // 更新页面2内容（状态 + 剩余时间 + 品种）
            const char* species_names[] = {
                "牡丹", "玄凤", "太阳锥尾", 
                "金太阳", "非洲灰", "和尚","蓝黄金刚","自定义"
            };
            
            char time_remaining[20];
            HatchingSettings_GetFormattedTimeRemaining(time_remaining, sizeof(time_remaining));
            
            if (g_hatching_carousel.page2_status_label && lv_obj_is_valid(g_hatching_carousel.page2_status_label)) {
                lv_label_set_text_fmt(g_hatching_carousel.page2_status_label, "第%d天", status->days_elapsed);
            }
            if (g_hatching_carousel.page2_remaining_label && lv_obj_is_valid(g_hatching_carousel.page2_remaining_label)) {
                lv_label_set_text_fmt(g_hatching_carousel.page2_remaining_label, "剩余: %s", time_remaining);
            }
            if (g_hatching_carousel.page2_species_label && lv_obj_is_valid(g_hatching_carousel.page2_species_label)) {
                lv_label_set_text_fmt(g_hatching_carousel.page2_species_label, "品种: %s", species_names[status->species]);
            }
        }
    } else {
        // 未激活状态
        if (g_hatching_carousel.page1_temp_label && lv_obj_is_valid(g_hatching_carousel.page1_temp_label)) {
            lv_label_set_text(g_hatching_carousel.page1_temp_label, "温度: --.-°C");
        }
        if (g_hatching_carousel.page1_humidity_label && lv_obj_is_valid(g_hatching_carousel.page1_humidity_label)) {
            lv_label_set_text(g_hatching_carousel.page1_humidity_label, "湿度: --%");
        }
        if (g_hatching_carousel.page1_status_label && lv_obj_is_valid(g_hatching_carousel.page1_status_label)) {
            lv_label_set_text(g_hatching_carousel.page1_status_label, "状态: 待机");
        }
        
        if (g_hatching_carousel.page2_status_label && lv_obj_is_valid(g_hatching_carousel.page2_status_label)) {
            lv_label_set_text(g_hatching_carousel.page2_status_label, "未孵化");
        }
        if (g_hatching_carousel.page2_remaining_label && lv_obj_is_valid(g_hatching_carousel.page2_remaining_label)) {
            lv_label_set_text(g_hatching_carousel.page2_remaining_label, "剩余: --天");
        }
        if (g_hatching_carousel.page2_species_label && lv_obj_is_valid(g_hatching_carousel.page2_species_label)) {
            lv_label_set_text(g_hatching_carousel.page2_species_label, "品种: 未选择");
        }
    }
}

static void hatching_card_main_toggle_event_handler(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    bool is_checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    set_heater_state(is_checked); // This might be HatchingSettings_SetActive(is_checked) or similar
    update_hatching_card_status();
}

void create_hatching_card_ui(lv_obj_t* parent_dashboard) {
    lv_obj_t* card = lv_obj_create(parent_dashboard);
    lv_obj_add_style(card, &style_card_main, 0);
    lv_obj_set_width(card, LV_PCT(48));
    lv_obj_set_height(card, CARD_FIXED_HEIGHT);
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(card, CARD_CONTENT_PADDING, 0);

    extern const char* card_titles[];
    extern const char* card_icons[];
    create_common_card_header(card, card_titles[CARD_ID_INCUBATION], card_icons[CARD_ID_INCUBATION], 
                              CARD_ID_INCUBATION, card_settings_event_handler_global);

    // 主内容容器
    lv_obj_t* content_area_card = lv_obj_create(card);
    lv_obj_remove_style_all(content_area_card);
    lv_obj_set_width(content_area_card, LV_PCT(100));
    lv_obj_set_flex_grow(content_area_card, 1);
    lv_obj_set_style_pad_all(content_area_card, CARD_CONTENT_INNER_PADDING, 0);

    // ============== 轮播视窗容器 ==============
    lv_obj_t* viewport_container = lv_obj_create(content_area_card);
    lv_obj_remove_style_all(viewport_container);
    lv_obj_set_pos(viewport_container, 0, 0);
    lv_obj_set_size(viewport_container, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_right(viewport_container, CARD_TEXT_RIGHT_MARGIN, 0);
    //lv_obj_set_style_pad_left(viewport_container, 0, 0);
    lv_obj_set_style_clip_corner(viewport_container, true, 0);
    lv_obj_clear_flag(viewport_container, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    
    // 强制更新布局并获取视窗高度
    lv_obj_update_layout(viewport_container);
    g_hatching_carousel.viewport_height = lv_obj_get_height(viewport_container);

    // ============== 轮播内容容器（修复版：不使用LV_LAYOUT_NONE） ==============
    g_hatching_carousel.content_container = lv_obj_create(viewport_container);
    lv_obj_remove_style_all(g_hatching_carousel.content_container);
    
    // 内容容器高度为2倍视窗高度
    lv_coord_t content_height = g_hatching_carousel.viewport_height * 2;
    lv_obj_set_size(g_hatching_carousel.content_container, lv_pct(100), content_height);
    lv_obj_set_pos(g_hatching_carousel.content_container, 0, 0);
    
    // ============== 创建页面1容器（绝对定位） ==============
    g_hatching_carousel.page_containers[0] = lv_obj_create(g_hatching_carousel.content_container);
    lv_obj_remove_style_all(g_hatching_carousel.page_containers[0]);
    lv_obj_set_size(g_hatching_carousel.page_containers[0], lv_pct(100), g_hatching_carousel.viewport_height);
    lv_obj_set_pos(g_hatching_carousel.page_containers[0], 0, 0);  // 页面1在顶部
    lv_obj_set_layout(g_hatching_carousel.page_containers[0], LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(g_hatching_carousel.page_containers[0], LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(g_hatching_carousel.page_containers[0], LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(g_hatching_carousel.page_containers[0], CARD_ROW_SPACING, 0);
    lv_obj_set_style_pad_all(g_hatching_carousel.page_containers[0], 5, 0);

    // 页面1的标签
    g_hatching_carousel.page1_temp_label = lv_label_create(g_hatching_carousel.page_containers[0]);
    lv_obj_add_style(g_hatching_carousel.page1_temp_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(g_hatching_carousel.page1_temp_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(g_hatching_carousel.page1_temp_label, LV_PCT(100));
    lv_label_set_text(g_hatching_carousel.page1_temp_label, "温度: --.-°C");

    g_hatching_carousel.page1_humidity_label = lv_label_create(g_hatching_carousel.page_containers[0]);
    lv_obj_add_style(g_hatching_carousel.page1_humidity_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(g_hatching_carousel.page1_humidity_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(g_hatching_carousel.page1_humidity_label, LV_PCT(100));
    lv_label_set_text(g_hatching_carousel.page1_humidity_label, "湿度: --%");

    g_hatching_carousel.page1_status_label = lv_label_create(g_hatching_carousel.page_containers[0]);
    lv_obj_add_style(g_hatching_carousel.page1_status_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(g_hatching_carousel.page1_status_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(g_hatching_carousel.page1_status_label, LV_PCT(100));
    lv_label_set_text(g_hatching_carousel.page1_status_label, "状态: 待机");

    // ============== 创建页面2容器（绝对定位） ==============
    g_hatching_carousel.page_containers[1] = lv_obj_create(g_hatching_carousel.content_container);
    lv_obj_remove_style_all(g_hatching_carousel.page_containers[1]);
    lv_obj_set_size(g_hatching_carousel.page_containers[1], lv_pct(100), g_hatching_carousel.viewport_height);
    lv_obj_set_pos(g_hatching_carousel.page_containers[1], 0, g_hatching_carousel.viewport_height);  // 页面2在底部
    lv_obj_set_layout(g_hatching_carousel.page_containers[1], LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(g_hatching_carousel.page_containers[1], LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(g_hatching_carousel.page_containers[1], LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(g_hatching_carousel.page_containers[1], CARD_ROW_SPACING, 0);
    lv_obj_set_style_pad_all(g_hatching_carousel.page_containers[1], 5, 0);

    // 页面2的标签
    g_hatching_carousel.page2_status_label = lv_label_create(g_hatching_carousel.page_containers[1]);
    lv_obj_add_style(g_hatching_carousel.page2_status_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(g_hatching_carousel.page2_status_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(g_hatching_carousel.page2_status_label, LV_PCT(100));
    lv_label_set_text(g_hatching_carousel.page2_status_label, "未孵化");

    g_hatching_carousel.page2_remaining_label = lv_label_create(g_hatching_carousel.page_containers[1]);
    lv_obj_add_style(g_hatching_carousel.page2_remaining_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(g_hatching_carousel.page2_remaining_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(g_hatching_carousel.page2_remaining_label, LV_PCT(100));
    lv_label_set_text(g_hatching_carousel.page2_remaining_label, "剩余: --天");

    g_hatching_carousel.page2_species_label = lv_label_create(g_hatching_carousel.page_containers[1]);
    lv_obj_add_style(g_hatching_carousel.page2_species_label, &style_text_white_custom_font, 0);
    lv_obj_set_style_text_font(g_hatching_carousel.page2_species_label, &fengyahei_s16_bpp4, 0);
    lv_obj_set_width(g_hatching_carousel.page2_species_label, LV_PCT(100));
    lv_label_set_text(g_hatching_carousel.page2_species_label, "品种: 未选择");

    // ============== 创建左侧页面指示器 ==============
    //create_page_indicators(content_area_card);

    // ============== 开关 ==============
    card_hatching_main_switch = lv_switch_create(content_area_card);
    lv_obj_add_style(card_hatching_main_switch, &style_switch_on_cyan, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_event_cb(card_hatching_main_switch, hatching_card_main_toggle_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_align(card_hatching_main_switch, LV_ALIGN_BOTTOM_RIGHT, -CARD_SWITCH_RIGHT_MARGIN, -CARD_SWITCH_BOTTOM_MARGIN);

    // ============== 初始化轮播 ==============
    g_hatching_carousel.current_page = 0;
    g_hatching_carousel.is_active = true;
    
    // 创建轮播定时器
    g_hatching_carousel.carousel_timer = lv_timer_create(hatching_carousel_switch_page, 
                                                         CAROUSEL_SWITCH_INTERVAL_MS, NULL);

    // 初始化内容
    update_hatching_carousel_content();
}

// ============== 更新函数 ==============
void update_hatching_card_status() {
    update_hatching_carousel_content();
    
    if (card_hatching_main_switch && lv_obj_is_valid(card_hatching_main_switch)) {
        bool is_active = HatchingSettings_IsActive();
        if (is_active) {
            lv_obj_add_state(card_hatching_main_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(card_hatching_main_switch, LV_STATE_CHECKED);
        }
    }
}

// ============== 轮播控制函数 ==============
void hatching_carousel_start() {
    if (g_hatching_carousel.carousel_timer) {
        lv_timer_resume(g_hatching_carousel.carousel_timer);
        g_hatching_carousel.is_active = true;
    }
}

void hatching_carousel_stop() {
    if (g_hatching_carousel.carousel_timer) {
        lv_timer_pause(g_hatching_carousel.carousel_timer);
        g_hatching_carousel.is_active = false;
    }
}

void hatching_carousel_cleanup() {
    if (g_hatching_carousel.carousel_timer) {
        lv_timer_del(g_hatching_carousel.carousel_timer);
        g_hatching_carousel.carousel_timer = NULL;
    }
    memset(&g_hatching_carousel, 0, sizeof(g_hatching_carousel));
}

void hatching_carousel_goto_page(int page_index) {
    if (page_index >= 0 && page_index < CAROUSEL_PAGE_COUNT) {
        hatching_carousel_slide_to_page(page_index);
    }
}
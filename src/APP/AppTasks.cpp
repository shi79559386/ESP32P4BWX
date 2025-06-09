#include "AppTasks.h"
#include "AppGlobal.h"
#include "../Config/Config.h"
#include "Display/UI_MainScreen.h"
#include "Peripherals/AHT20_Sensor.h"
#include "Peripherals/DS3231_Clock.h"
#include "../Peripherals/OutputControls.h"
#include "../SettingsLogic/LightingSettings.h"
#include "../SettingsLogic/ParrotSettings.h"
#include "../SettingsLogic/DryingSettings.h"
#include "../SettingsLogic/SterilizationSettings.h"
#include "../Display/cards/ui_lighting_card.h"
#include "../Display/cards/ui_voice_card.h"
#include "../Display/cards/ui_drying_card.h"
#include "../Display/cards/ui_fresh_air_card.h"
#include "../Display/cards/ui_sterilize_card.h"
#include "../Display/cards/ui_hatching_card.h"
#include "../Display/cards/ui_thermal_card.h"
#include "../Display/cards/ui_humidify_card.h"
#include "../Control/ThermalControl.h"
#include <Arduino.h>

static unsigned long last_sensor_read_millis = 0;
static const unsigned long SENSOR_READ_INTERVAL = 2000;
static unsigned long last_time_update_millis = 0;
static const unsigned long TIME_UPDATE_INTERVAL = 1000;
static unsigned long last_lighting_update_millis = 0;
static const unsigned long LIGHTING_UPDATE_INTERVAL = 50;
static unsigned long last_card_update_millis = 0;
static const unsigned long CARD_UI_UPDATE_INTERVAL = 500;
static unsigned long last_parrot_handle_millis = 0;
static const unsigned long PARROT_HANDLE_INTERVAL = 100; // 100ms 检查一次音频状态
static unsigned long last_drying_timed_check_millis = 0;
static const unsigned long DRYING_TIMED_CHECK_INTERVAL = 1000;
static unsigned long last_sterilization_timed_check_millis = 0;
static const unsigned long STERILIZATION_TIMED_CHECK_INTERVAL = 1000;


void AppTasks_Init(void) {
    last_sensor_read_millis = millis();
    last_time_update_millis = millis();
    last_lighting_update_millis = millis();
    last_card_update_millis = millis();
    last_parrot_handle_millis = millis();
    last_drying_timed_check_millis = millis();
    last_sterilization_timed_check_millis = millis();
    
    float temp_f;
    float humid_f;

    if(is_aht20_sensor_available(AHT20_SENSOR_1) && read_aht20_sensor_values(AHT20_SENSOR_1, &temp_f, &humid_f)){
        g_current_temperature = temp_f;
        g_current_humidity = (int)humid_f;
    } else {
        g_current_temperature = 25.0;
        g_current_humidity = 50;
    }
    if(is_aht20_sensor_available(AHT20_SENSOR_2) && read_aht20_sensor_values(AHT20_SENSOR_2, &temp_f, &humid_f)){
        g_current_temperature_2 = temp_f;
        g_current_humidity_2 = (int)humid_f;
    } else {
        g_current_temperature_2 = 25.0;
        g_current_humidity_2 = 50;
    }
}

static void read_real_sensor_data() {
    float temp_f, humid_f;
    
    #if ENABLE_SENSOR_MODULE
    if (!pca9548a_is_available()) {
        static float sim_temp1 = 25.0f;
        g_current_temperature = sim_temp1;
        g_current_humidity = 50;
        g_current_temperature_2 = sim_temp1 + 1.0f;
        g_current_humidity_2 = 55;
        return;
    }

    if (is_aht20_sensor_available(AHT20_SENSOR_1)) {
        if (read_aht20_sensor_values(AHT20_SENSOR_1, &temp_f, &humid_f)) {
            g_current_temperature = temp_f;
            g_current_humidity = (int)humid_f;
        }
    }

    if (is_aht20_sensor_available(AHT20_SENSOR_2)) {
        if (read_aht20_sensor_values(AHT20_SENSOR_2, &temp_f, &humid_f)) {
            g_current_temperature_2 = temp_f;
            g_current_humidity_2 = (int)humid_f;
        }
    }
#else
    static float sim_temp1 = 25.5f;
    sim_temp1 += (random(-20, 21) / 100.0f);
    sim_temp1 = constrain(sim_temp1, 22.0f, 30.0f);
    g_current_temperature = sim_temp1;
    g_current_humidity = 60;
    g_current_temperature_2 = sim_temp1 + 0.7f;
    g_current_humidity_2 = 58;
#endif

    if (lv_scr_act() == screen_main) {
        update_temperature_humidity_displays(g_current_temperature, g_current_humidity, 
                                             g_current_temperature_2, g_current_humidity_2);
    }
}

void AppTasks_Handler(void) {
    unsigned long current_millis = millis();

    if (current_millis - last_sensor_read_millis >= SENSOR_READ_INTERVAL) {
        last_sensor_read_millis = current_millis;
        read_real_sensor_data();
    }

    ThermalControl_Handler();
    
    if (current_millis - last_time_update_millis >= TIME_UPDATE_INTERVAL) {
        last_time_update_millis = current_millis;
        RtcDateTime now;
        if (is_ds3231_available() && get_current_datetime_rtc(&now)) {
            snprintf(g_current_date_str, sizeof(g_current_date_str), "%04u-%02u-%02u", now.Year(), now.Month(), now.Day());
            snprintf(g_current_time_str, sizeof(g_current_time_str), "%02u:%02u:%02u", now.Hour(), now.Minute(), now.Second());
        }
        if (lv_scr_act() == screen_main) {
            refresh_top_bar_info();
        }
    }

    if (current_millis - last_lighting_update_millis >= LIGHTING_UPDATE_INTERVAL) {
        last_lighting_update_millis = current_millis;
        LightingSettings_UpdateBrighteningState();
    }

    if (current_millis - last_drying_timed_check_millis >= DRYING_TIMED_CHECK_INTERVAL) {
        last_drying_timed_check_millis = current_millis;
        DryingSettings_UpdateTimedState();
    }

    if (current_millis - last_sterilization_timed_check_millis >= STERILIZATION_TIMED_CHECK_INTERVAL) {
        last_sterilization_timed_check_millis = current_millis;
        SterilizationSettings_UpdateTimedState();
    }
    
    // 重新启用 ParrotSettings_Handle
    if (current_millis - last_parrot_handle_millis >= PARROT_HANDLE_INTERVAL) {
        last_parrot_handle_millis = current_millis;
        ParrotSettings_Handle();
    }

    static unsigned long last_status_update_millis = 0;
    if (current_millis - last_status_update_millis >= 5000) {
        last_status_update_millis = current_millis;
        if (ThermalControl_IsEnabled()) {
            const char* state_desc = ThermalControl_GetStateDescription();
            snprintf(g_work_status_str, sizeof(g_work_status_str), "Control: %s", state_desc);
        } else {
            strcpy(g_work_status_str, "Status: Standby");
        }
        if (lv_scr_act() == screen_main) {
            refresh_top_bar_info();
        }
    }

    if (current_millis - last_card_update_millis >= CARD_UI_UPDATE_INTERVAL) {
        last_card_update_millis = current_millis;
        if (lv_scr_act() == screen_control) {
            update_lighting_card_status();
            update_voice_card_status();
            update_drying_card_status();
            update_fresh_air_card_status();
            update_sterilize_card_status();
            update_hatching_card_status();
            update_thermal_card_status();
            update_humidify_card_status();
        }
    }
}
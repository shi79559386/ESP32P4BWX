#include "OutputControls.h"
#include "../Config/Config.h" 
#include <Arduino.h>
#include <driver/ledc.h>

static bool heater_on = false;
static bool fan_on = false;
static uint8_t current_light_pwm = 0;
static bool fresh_air_on = false;
static bool sterilize_on = false;
static bool humidify_on = false;
static bool parrot_on = false;

void init_output_controls() {
    Serial.println("=== OutputControls Initialization (Direct Control) ===");
    
    // 初始化所有控制引脚
    pinMode(HEATER_CTRL_PIN, OUTPUT);
    pinMode(FAN_CTRL_PIN, OUTPUT);
    pinMode(HUMIDIFIER_CTRL_PIN, OUTPUT);   
    pinMode(STERILIZE_PIN, OUTPUT);
    pinMode(FRESH_AIR_PIN, OUTPUT);
    pinMode(PARROT_STATE_PIN, OUTPUT);
    pinMode(LIGHTING_PIN, OUTPUT);

    // 设置灯光PWM
    ledc_timer_config_t timer_cfg = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,              // 低速模式
        .duty_resolution  = LEDC_TIMER_8_BIT,                 // 8 位分辨率
        .timer_num        = (ledc_timer_t)LIGHTING_PWM_CHANNEL, // 对应通道即 timer 号
        .freq_hz          = 5000,                             // 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    esp_err_t err = ledc_timer_config(&timer_cfg);
    if (err != ESP_OK) {
      Serial.printf("⚠️ ledc_timer_config failed: %d\n", err);
    }

    // —— IDF 原生 LEDC 通道配置 —— 
    ledc_channel_config_t ch_cfg = {
        .gpio_num       = LIGHTING_PIN,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = (ledc_channel_t)LIGHTING_PWM_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = (ledc_timer_t)LIGHTING_PWM_CHANNEL,
        .duty           = 0,
        .hpoint         = 0
    };
    err = ledc_channel_config(&ch_cfg);
    if (err != ESP_OK) {
      Serial.printf("⚠️ ledc_channel_config failed: %d\n", err);
    }

    // 确保所有设备初始状态为关闭
    digitalWrite(HEATER_CTRL_PIN, LOW);
    digitalWrite(FAN_CTRL_PIN, LOW);
    digitalWrite(HUMIDIFIER_CTRL_PIN, LOW);
    digitalWrite(STERILIZE_PIN, LOW);
    digitalWrite(FRESH_AIR_PIN, LOW);
    digitalWrite(PARROT_STATE_PIN, LOW);
    // LEDC 的初始占空设置也要更新
    ledc_set_duty(LEDC_LOW_SPEED_MODE,
        (ledc_channel_t)LIGHTING_PWM_CHANNEL,
        0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE,
           (ledc_channel_t)LIGHTING_PWM_CHANNEL);
    
    Serial.println("✅ All output controls initialized and set to OFF.");
}

void set_heater_state(bool state) {
    digitalWrite(HEATER_CTRL_PIN, state ? HIGH : LOW);
    heater_on = state;
}

bool get_heater_state() { return heater_on; }

void set_fan_state(bool state) {
    digitalWrite(FAN_CTRL_PIN, state ? HIGH : LOW);
    fan_on = state;
}

bool get_fan_state() { return fan_on; }

void set_light_pwm(uint8_t pwm_percentage) {
    if (pwm_percentage > 100) pwm_percentage = 100;
    // 8 位分辨率最大值 255
    uint32_t duty = (uint32_t)pwm_percentage * 255 / 100;
    ledc_set_duty(LEDC_LOW_SPEED_MODE,
                  (ledc_channel_t)LIGHTING_PWM_CHANNEL,
                  duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE,
                     (ledc_channel_t)LIGHTING_PWM_CHANNEL);
    current_light_pwm = pwm_percentage;
}

uint8_t get_light_pwm() { return current_light_pwm; }

bool is_light_on_from_pwm() { return current_light_pwm > 0; }

void set_fresh_air_state(bool state) {
    digitalWrite(FRESH_AIR_PIN, state ? HIGH : LOW);
    fresh_air_on = state;
}

bool get_fresh_air_state() { return fresh_air_on; }

void set_sterilize_state(bool state) {
    digitalWrite(STERILIZE_PIN, state ? HIGH : LOW);
    sterilize_on = state;
}

bool get_sterilize_state() { return sterilize_on; }

void set_humidify_state(bool state) {
    digitalWrite(HUMIDIFIER_CTRL_PIN, state ? HIGH : LOW);
    humidify_on = state;
}

bool get_humidify_state() { return humidify_on; }

void set_parrot_state(bool state) {
    digitalWrite(PARROT_STATE_PIN, state ? HIGH : LOW);
    parrot_on = state;
}

bool get_parrot_state() {
    return parrot_on;
}
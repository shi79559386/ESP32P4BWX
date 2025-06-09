// File: src/SettingsLogic/ThermalControl.cpp
#include "ThermalControl.h"
#include "../Config/Config.h"
#include "../App/AppGlobal.h"
#include "../Peripherals/OutputControls.h"
#include "../Peripherals/AHT20_Sensor.h"
#include <Arduino.h>

// 静态变量
static thermal_config_t thermal_config;
static thermal_status_t thermal_status;
static bool debug_mode = false;

// 风扇转速检测变量
static volatile unsigned long pulse_count = 0;
static volatile unsigned long last_interrupt_time = 0;
static unsigned long calculated_rpm = 0;

// 时间相关变量
static unsigned long last_control_update = 0;
static unsigned long last_sensor_read = 0;
static unsigned long last_status_print = 0; // Still present, but print function can be empty if debug_mode is off

// 控制间隔常量
static const unsigned long CONTROL_UPDATE_INTERVAL = 1000;  // 1秒控制更新
static const unsigned long SENSOR_READ_INTERVAL = 2000;    // 2秒传感器读取
static const unsigned long STATUS_PRINT_INTERVAL = 10000;  // 10秒状态打印 (if debug_mode is on)

// 静态函数声明
static void ThermalControl_ProcessControl();
static void ThermalControl_ProcessFanDelay(unsigned long current_time);

// 带防抖的中断服务程序
void IRAM_ATTR fan_pulse_interrupt() {
    unsigned long current_time = micros();
    
    if (current_time - last_interrupt_time < RPM_DEBOUNCE_TIME) {
        return;
    }
    
    last_interrupt_time = current_time;
    pulse_count = pulse_count + 1;
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  // ← 看看 LED 是否闪烁
}

// 改进的RPM计算函数
unsigned long calculateRPM() {
    static unsigned long last_calc_time = 0;
    unsigned long current_time = millis();
    
    if (current_time - last_calc_time >= RPM_CALCULATION_INTERVAL) {
        noInterrupts();
        unsigned long pulses_in_period = pulse_count;
        pulse_count = 0;
        interrupts();
        
        unsigned long time_diff = current_time - last_calc_time;
        if (time_diff == 0) time_diff = 1;
        
        calculated_rpm = (pulses_in_period * 60000UL) / (time_diff * PULSES_PER_REVOLUTION);
        
        if (calculated_rpm > FAN_MAX_RPM) {
            // Optional: Log this critical event if a minimal logging system is in place
            calculated_rpm = 0; 
        }
        
        last_calc_time = current_time;
    }
    
    return calculated_rpm;
}

void ThermalControl_Init() {
    // 初始化配置参数
    thermal_config.enabled = false;
    thermal_config.target_temp_left = g_target_temperature_left;
    thermal_config.target_temp_right = g_target_temperature_right;
    thermal_config.heater_enabled = true;
    thermal_config.fan_enabled = true;
    thermal_config.humidifier_enabled = false;
    thermal_config.state = THERMAL_STATE_IDLE;
    
    // 初始化状态参数
    thermal_status.heater_active = false;
    thermal_status.fan_active = false;
    thermal_status.humidifier_active = false;
    thermal_status.fan_rpm = 0;
    thermal_status.fan_fault = false;
    thermal_status.overheat_protection = false;
    thermal_status.fan_shutdown_time = 0;
    strcpy(thermal_status.status_message, "System Ready"); // Changed to English for consistency
    
    pinMode(HUMIDIFIER_CTRL_PIN, OUTPUT);
    digitalWrite(HUMIDIFIER_CTRL_PIN, LOW);
    
    pinMode(FAN_CTRL_PIN, OUTPUT);
    digitalWrite(FAN_CTRL_PIN, LOW);
    pinMode(FAN_SPEED_PIN, INPUT_PULLUP);
    
    pulse_count = 0;
    last_interrupt_time = 0;
    calculated_rpm = 0;
    
    attachInterrupt(digitalPinToInterrupt(FAN_SPEED_PIN), fan_pulse_interrupt, RISING);
    
    set_heater_state(false);
    set_fan_state(false);
    digitalWrite(HUMIDIFIER_CTRL_PIN, LOW);
    
    last_control_update = millis();
    last_sensor_read = millis();
    last_status_print = millis();
    
    if (thermal_config.target_temp_left >= MIN_TARGET_TEMP && 
        thermal_config.target_temp_left <= MAX_TARGET_TEMP &&
        thermal_config.target_temp_right >= MIN_TARGET_TEMP && 
        thermal_config.target_temp_right <= MAX_TARGET_TEMP) {
        
        float current_temp_left = g_current_temperature;
        float current_temp_right = g_current_temperature_2;
        
        if (current_temp_left < thermal_config.target_temp_left - 1.0f ||
            current_temp_right < thermal_config.target_temp_right - 1.0f) {
            thermal_config.enabled = true;
        }
    }
    
    // Optional: Minimal init message
    // Serial.println("Thermal control initialized.");

    // Calibration call is kept, but its internal prints are removed.
    // Consider if calibration is still useful without serial feedback,
    // or if PULSES_PER_REVOLUTION should be a fixed config.
    delay(1000); // Reduced delay before calibration if kept
    ThermalControl_CalibrateRPM(); // This will now run silently

    noInterrupts();
    pulse_count = 0;
    interrupts();
    calculateRPM(); 
}

void ThermalControl_Handler() {
    unsigned long current_time = millis();
    
    thermal_status.fan_rpm = calculateRPM();
    
    if (thermal_status.fan_active) {
        if (thermal_status.fan_rpm < FAN_MIN_RPM) {
            if (!thermal_status.fan_fault) {
                thermal_status.fan_fault = true;
                strcpy(thermal_status.status_message, "Fan Fault");
                
                if (thermal_status.heater_active) {
                    set_heater_state(false);
                    thermal_status.heater_active = false;
                }
                thermal_config.state = THERMAL_STATE_FAN_FAULT;
            }
        } else if (thermal_status.fan_rpm >= FAN_MIN_RPM) {
            if (thermal_status.fan_fault) {
                thermal_status.fan_fault = false;
                thermal_config.state = THERMAL_STATE_IDLE;
                // Message will be updated by main control logic
            }
        }
    } else {
        if (thermal_status.fan_fault) {
            thermal_status.fan_fault = false;
        }
    }
    
    if (!thermal_config.enabled || thermal_status.fan_fault) {
        // If system disabled or fan fault, ensure outputs are off (except fan delay)
        if (thermal_status.heater_active) {
             set_heater_state(false);
             thermal_status.heater_active = false;
        }
        if (!thermal_config.enabled && thermal_status.fan_active && thermal_status.fan_shutdown_time == 0) {
            // If disabled, and fan not in shutdown delay, turn it off
            set_fan_state(false);
            thermal_status.fan_active = false;
        }
        // Process fan delay even if system is disabled to allow fan to complete its cycle
        ThermalControl_ProcessFanDelay(current_time);
        return;
    }
    
    if (current_time - last_sensor_read >= SENSOR_READ_INTERVAL) {
        last_sensor_read = current_time;
        
        float temp_left = g_current_temperature;
        float temp_right = g_current_temperature_2;
        
        bool overheat_left = temp_left > (thermal_config.target_temp_left + OVERHEAT_THRESHOLD);
        bool overheat_right = temp_right > (thermal_config.target_temp_right + OVERHEAT_THRESHOLD);
        
        if (overheat_left || overheat_right) {
            if (!thermal_status.overheat_protection) {
                thermal_status.overheat_protection = true;
                thermal_config.state = THERMAL_STATE_OVERHEAT;
                strcpy(thermal_status.status_message, "Overheat Protection");
                
                if (thermal_status.heater_active) {
                    set_heater_state(false);
                    thermal_status.heater_active = false;
                }
            }
        } else if (thermal_status.overheat_protection) {
            thermal_status.overheat_protection = false;
            thermal_config.state = THERMAL_STATE_IDLE;
             // Message will be updated by main control logic or if heating starts
        }
    }
    
    if (current_time - last_control_update >= CONTROL_UPDATE_INTERVAL) {
        last_control_update = current_time;
        
        if (!thermal_status.overheat_protection) {
            ThermalControl_ProcessControl();
        }
        
        ThermalControl_ProcessFanDelay(current_time);
    }
    
    if (debug_mode && current_time - last_status_print >= STATUS_PRINT_INTERVAL) {
        last_status_print = current_time;
        ThermalControl_PrintStatus(); // This function will now print minimal or no info
    }
}

void ThermalControl_CalibrateRPM() {
    // This function is now silent. Its utility without serial feedback is limited.
    // Consider removing if PULSES_PER_REVOLUTION is fixed or determined otherwise.
    noInterrupts();
    pulse_count = 0;
    last_interrupt_time = 0;
    interrupts();
    calculated_rpm = 0;
    
    set_fan_state(true);
    delay(2000); 
    
    // Dummy calculation loop - values are not used without prints
    // This section could be removed if calibration is not needed or done differently.
    int test_duration = 5000; // Example: one fixed duration
    int test_pulses_per_rev = PULSES_PER_REVOLUTION; // Use the configured one

    noInterrupts();
    pulse_count = 0;
    interrupts();
    
    unsigned long start_time = millis();
    delay(test_duration);
    unsigned long end_time = millis();
    
    noInterrupts();
    unsigned long pulse_diff = pulse_count;
    interrupts();
    
    unsigned long actual_time = end_time - start_time;
    if (actual_time == 0) actual_time = 1;
    
    // This calculation is done, but not reported via Serial here.
    // It might influence the global 'calculated_rpm' via calculateRPM() if called later.
    // unsigned long test_rpm = (pulse_diff * 60000UL) / (actual_time * test_pulses_per_rev);
        
    set_fan_state(false);
    
    noInterrupts();
    pulse_count = 0;
    interrupts();
    calculated_rpm = 0;
}

void ThermalControl_DiagnoseRPM() {
    // This function is largely non-functional without serial prints.
    // Kept as a shell if you want to add minimal, specific debugging later.
    noInterrupts();
    // unsigned long current_pulses = pulse_count; // Example of data you might fetch
    interrupts();
    // unsigned long current_calculated_rpm = calculated_rpm;
    // bool fan_is_active = thermal_status.fan_active;
    // int pin_level = digitalRead(FAN_SPEED_PIN);

    // Monitoring loop removed as it was purely for printing
}

void ThermalControl_ResetRPMCounter() {
    noInterrupts();
    pulse_count = 0;
    last_interrupt_time = 0;
    interrupts();
    calculated_rpm = 0;
}

static void ThermalControl_ProcessControl() {
    float temp_left = g_current_temperature;
    float temp_right = g_current_temperature_2;
    
    bool need_heating_left = temp_left < (thermal_config.target_temp_left - TEMP_HYSTERESIS);
    bool need_heating_right = temp_right < (thermal_config.target_temp_right - TEMP_HYSTERESIS);
    bool stop_heating_left = temp_left > (thermal_config.target_temp_left + TEMP_HYSTERESIS);
    bool stop_heating_right = temp_right > (thermal_config.target_temp_right + TEMP_HYSTERESIS);
    
    bool should_heat = (need_heating_left || need_heating_right) && 
                       thermal_config.heater_enabled && 
                       !thermal_status.fan_fault;
    
    bool should_stop = (stop_heating_left && stop_heating_right) || 
                       !thermal_config.heater_enabled;
    
    if (should_heat && !thermal_status.heater_active) {
        set_heater_state(true);
        thermal_status.heater_active = true;
        thermal_config.state = THERMAL_STATE_HEATING;
        strcpy(thermal_status.status_message, "Heating");
        
        if (thermal_config.fan_enabled && !thermal_status.fan_active) {
            set_fan_state(true);
            thermal_status.fan_active = true;
        }
    } else if (should_stop && thermal_status.heater_active) {
        set_heater_state(false);
        thermal_status.heater_active = false;
        thermal_config.state = THERMAL_STATE_COOLING;
        strcpy(thermal_status.status_message, "Cooling");
        thermal_status.fan_shutdown_time = millis() + FAN_SHUTDOWN_DELAY_MS;
    } else if (!thermal_status.heater_active && !thermal_status.fan_active && thermal_status.fan_shutdown_time == 0) {
        // If neither heating nor fan active (and not in fan shutdown delay), system is idle
        if (thermal_config.state != THERMAL_STATE_IDLE && 
            thermal_config.state != THERMAL_STATE_FAN_FAULT && // don't overwrite fault states
            thermal_config.state != THERMAL_STATE_OVERHEAT) {
            thermal_config.state = THERMAL_STATE_IDLE;
            strcpy(thermal_status.status_message, "Idle");
        }
    }
}

static void ThermalControl_ProcessFanDelay(unsigned long current_time) {
    if (thermal_status.fan_shutdown_time > 0 && 
        current_time >= thermal_status.fan_shutdown_time && 
        !thermal_status.heater_active) { // Ensure heater is not active before shutting fan
        
        if (thermal_status.fan_active) {
            set_fan_state(false);
            thermal_status.fan_active = false;
            if (thermal_config.state == THERMAL_STATE_COOLING) { // Only switch to IDLE if it was in COOLING
                 thermal_config.state = THERMAL_STATE_IDLE;
                 strcpy(thermal_status.status_message, "Idle");
            }
        }
        thermal_status.fan_shutdown_time = 0;
    }
}

void ThermalControl_SetEnabled(bool enabled) {
    thermal_config.enabled = enabled;
    
    if (!enabled) {
        set_heater_state(false);
        // Fan will be turned off by ProcessFanDelay or Handler logic if not in delay
        // digitalWrite(HUMIDIFIER_CTRL_PIN, LOW); // Humidifier not actively managed in core logic beyond manual set
        
        thermal_status.heater_active = false;
        // thermal_status.fan_active = false; // State managed by handler/delay
        thermal_status.humidifier_active = false; // If manually set
        // thermal_status.fan_shutdown_time = 0; // Don't reset if it's in a shutdown cycle
        
        // Only set to IDLE if no overriding fault condition exists
        if (!thermal_status.fan_fault && !thermal_status.overheat_protection) {
            thermal_config.state = THERMAL_STATE_IDLE;
            strcpy(thermal_status.status_message, "System Disabled");
        }
    } else {
         // If no fault, set to idle, otherwise retain fault message
        if (!thermal_status.fan_fault && !thermal_status.overheat_protection) {
            thermal_config.state = THERMAL_STATE_IDLE;
            strcpy(thermal_status.status_message, "System Enabled");
        }
    }
}

bool ThermalControl_IsEnabled() {
    return thermal_config.enabled;
}

bool ThermalControl_SetTargetTemp(thermal_zone_t zone, float target_temp) {
    if (target_temp < MIN_TARGET_TEMP || target_temp > MAX_TARGET_TEMP) {
        return false;
    }
    
    if (zone == THERMAL_ZONE_LEFT) {
        thermal_config.target_temp_left = target_temp;
    } else if (zone == THERMAL_ZONE_RIGHT) {
        thermal_config.target_temp_right = target_temp;
    } else {
        return false;
    }
    return true;
}

float ThermalControl_GetTargetTemp(thermal_zone_t zone) {
    return (zone == THERMAL_ZONE_LEFT) ? thermal_config.target_temp_left : thermal_config.target_temp_right;
}

thermal_state_t ThermalControl_GetState() {
    return thermal_config.state;
}

const thermal_status_t* ThermalControl_GetStatus() {
    return &thermal_status;
}

const thermal_config_t* ThermalControl_GetConfig() {
    return &thermal_config;
}

void ThermalControl_SetHeater(bool state) {
    set_heater_state(state);
    thermal_status.heater_active = state;
}

void ThermalControl_SetFan(bool state) {
    set_fan_state(state);
    thermal_status.fan_active = state;
    if (!state) { // If fan is manually turned off, cancel any pending shutdown delay
        thermal_status.fan_shutdown_time = 0;
    }
}

void ThermalControl_SetHumidifier(bool state) {
    digitalWrite(HUMIDIFIER_CTRL_PIN, state ? HIGH : LOW);
    thermal_status.humidifier_active = state;
}

unsigned long ThermalControl_GetFanRPM() {
    return thermal_status.fan_rpm;
}

bool ThermalControl_HasFault() {
    return thermal_status.fan_fault || thermal_status.overheat_protection;
}

void ThermalControl_ResetFault() {
    thermal_status.fan_fault = false;
    thermal_status.overheat_protection = false;
    // Re-evaluate state based on current conditions rather than forcing IDLE
    // For example, if temp is low, it might go to HEATING
    // For now, setting to idle and letting the handler correct if needed.
    thermal_config.state = THERMAL_STATE_IDLE; 
    strcpy(thermal_status.status_message, "Fault Reset");
}

const char* ThermalControl_GetStateDescription() {
    switch (thermal_config.state) {
        case THERMAL_STATE_IDLE: return "Idle";
        case THERMAL_STATE_HEATING: return "Heating";
        case THERMAL_STATE_COOLING: return "Cooling";
        case THERMAL_STATE_OVERHEAT: return "Overheat";
        case THERMAL_STATE_FAN_FAULT: return "Fan Fault";
        case THERMAL_STATE_ERROR: return "System Error";
        default: return "Unknown";
    }
}

void ThermalControl_PrintStatus() {
    if (!debug_mode) return; // Guard actual printing with debug_mode

    // Example of minimal debug output:
    // Serial.printf("State: %s, TempL: %.1f/%.1f, TempR: %.1f/%.1f, RPM: %lu\n",
    //     ThermalControl_GetStateDescription(),
    //     g_current_temperature, thermal_config.target_temp_left,
    //     g_current_temperature_2, thermal_config.target_temp_right,
    //     thermal_status.fan_rpm);
    // Or, keep it empty if no debug prints are desired at all.
}

void ThermalControl_SetDebugMode(bool debug) {
    debug_mode = debug;
}
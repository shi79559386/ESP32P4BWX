#ifndef OUTPUT_CONTROLS_H
#define OUTPUT_CONTROLS_H

#include <stdbool.h>
#include <stdint.h>

void init_output_controls();

void set_heater_state(bool state);
bool get_heater_state();

void set_fan_state(bool state);
bool get_fan_state();

void set_light_pwm(uint8_t pwm_percentage); // 0-100
uint8_t get_light_pwm();
bool is_light_on_from_pwm();

void set_fresh_air_state(bool state);
bool get_fresh_air_state();

void set_sterilize_state(bool state);
bool get_sterilize_state();

void set_humidify_state(bool state);
bool get_humidify_state();

void set_parrot_state(bool state); // Can be used for status indicator LED
bool get_parrot_state();

// This function is no longer needed in direct control model
// void handle_async_output_controls(); 

#endif // OUTPUT_CONTROLS_H
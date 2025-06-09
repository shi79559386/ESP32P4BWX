// FreshAirSettings.h - 简化版
#ifndef FRESH_AIR_SETTINGS_H
#define FRESH_AIR_SETTINGS_H

#include <stdbool.h>
#include <stdint.h>
#include "../App/AppGlobal.h"

typedef enum {
    FRESH_AIR_MODE_MANUAL = 0,
    FRESH_AIR_MODE_AUTO = 1,
    FRESH_AIR_MODE_SCHEDULED = 2
} fresh_air_mode_t;

typedef struct {
    bool enabled;
    uint8_t start_hour;
    uint8_t start_minute;
    uint8_t end_hour;
    uint8_t end_minute;
} fresh_air_schedule_t;

typedef void (*fresh_air_status_update_cb_t)(void);

void FreshAirSettings_Init();
void FreshAirSettings_SetEnabled(bool enabled);
bool FreshAirSettings_IsEnabled();
void FreshAirSettings_SetMode(fresh_air_mode_t mode);
fresh_air_mode_t FreshAirSettings_GetMode();
void FreshAirSettings_SetSchedule(const fresh_air_schedule_t* schedule);
const fresh_air_schedule_t* FreshAirSettings_GetSchedule();
void FreshAirSettings_UpdateAutoControl();
void FreshAirSettings_RegisterStatusUpdateCallback(fresh_air_status_update_cb_t cb);
const char* FreshAirSettings_GetStatusString();

#endif // FRESH_AIR_SETTINGS_H


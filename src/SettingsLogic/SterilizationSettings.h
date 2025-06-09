// File: src/SettingsLogic/SterilizationSettings.h
#ifndef STERILIZATION_SETTINGS_H
#define STERILIZATION_SETTINGS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "../APP/AppGlobal.h" // 包含 AppGlobal.h 以获取 timed_operation_config_t

typedef void (*sterilization_status_update_cb_t)(void);

void SterilizationSettings_Init();
void SterilizationSettings_SetEnabled(bool enabled);
bool SterilizationSettings_IsEnabled();
void SterilizationSettings_SetTimedOperation(const timed_operation_config_t* config); // 使用全局类型
const timed_operation_config_t* SterilizationSettings_GetTimedOperationSettings(); // 使用全局类型
void SterilizationSettings_UpdateTimedState();
bool SterilizationSettings_IsTimedSessionActive();
void SterilizationSettings_GetFormattedRemainingCountdown(char* buffer, size_t buffer_len);
void SterilizationSettings_RegisterStatusUpdateCallback(sterilization_status_update_cb_t cb);

#endif // STERILIZATION_SETTINGS_H
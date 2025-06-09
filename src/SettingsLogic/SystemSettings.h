// File: src/SettingsLogic/SystemSettings.h
#ifndef SYSTEM_SETTINGS_H
#define SYSTEM_SETTINGS_H

#include <stdbool.h>

void SystemSettings_Init();
void SystemSettings_SetBrightnessLevel(int level_idx); // level_idx: 0 (Low), 1 (Mid), 2 (High)
int SystemSettings_GetBrightnessLevel(); // Returns 0, 1, or 2

// Placeholder for WiFi logic if it were to be implemented here
// void SystemSettings_UpdateWifiStatus(bool connected, const char* ssid);
// const char* SystemSettings_GetWifiStatusString();
// const char* SystemSettings_GetWifiSSIDString();

#endif // SYSTEM_SETTINGS_H
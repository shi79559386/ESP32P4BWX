// 文件: src/APP/AppController.h

#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include <SD_MMC.h>
#include "Config/LGFX_Config.h"

// 这个函数的声明已更新，以接收一个 LGFX 对象指针
void AppController_Init(LGFX* lcd); 

void AppController_Init(void);
bool AppController_IsMainSDReady();
bool AppController_WriteSystemLog(const char* log_message);
bool AppController_WriteTemperatureLog(float temp1, int hum1, float temp2, int hum2);
bool AppController_LoadConfig(const char* config_name, char* buffer, size_t buffer_size);
bool AppController_SaveConfig(const char* config_name, const char* config_data);
bool AppController_CheckSDSpace(uint64_t* total_mb, uint64_t* used_mb, uint64_t* free_mb);
bool AppController_CleanupOldLogs();
void AppController_ListSDFiles(const char* dirname );

#endif // APP_CONTROLLER_H
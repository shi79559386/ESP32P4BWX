// File: src/APP/AppGlobal.cpp
#include "AppGlobal.h"
#include "lvgl.h" // For LV_SYMBOL_XXX
#include "../Config/Config.h"
#include <Arduino.h> // For Serial and strcpy
#include <string.h>  // For strcpy
#include "../Peripherals/AHT20_Sensor.h" // For sensor functions

// ============== 全局变量定义 ==============

// 传感器数据
SensorData_t sensor1Data = {0.0f, 0.0f, false};
SensorData_t sensor2Data = {0.0f, 0.0f, false};

// 温湿度全局变量
float g_current_temperature = 25.0f;   
int   g_current_humidity = 50;         
float g_current_temperature_2 = 26.0f; 
int   g_current_humidity_2 = 55;       

// 时间日期字符串
char  g_current_time_str[20] = "12:34:56";
char  g_current_date_str[20] = "2024-05-23";
char  g_work_status_str[50] = "工作状态:保温中...";

// 通用保温功能的目标值和开关
float g_target_temperature_left = 37.0f;    
int   g_target_humidity_left = 60;          
float g_target_temperature_right = 37.0f;   
int   g_target_humidity_right = 60;         
bool  g_generic_thermostat_active = false;  
bool  g_incubation_control_active = false;  

// LVGL屏幕对象
lv_obj_t *screen_main = NULL;
lv_obj_t *screen_control = NULL;
lv_obj_t *screen_setting = NULL;

// 卡片标题和图标的定义
const char* card_titles[CARD_ID_COUNT] = {
    "照  明", "鹦  语", "烘  干", "新  风", "杀  菌", "孵  化", "保  温", "加  湿"
};

const char* card_icons[CARD_ID_COUNT] = {
    LV_SYMBOL_POWER,    // 照明
    LV_SYMBOL_AUDIO,    // 鹦语
    LV_SYMBOL_REFRESH,  // 烘干
    LV_SYMBOL_GPS,      // 新风
    LV_SYMBOL_WARNING,  // 杀菌
    LV_SYMBOL_HOME,      // 孵化
    LV_SYMBOL_CHARGE,   // 保温 (或者用其他合适的图标)
    LV_SYMBOL_BELL      // 加湿 (或者用其他合适的图标)
};

// ============== 传感器读取和更新函数 ==============
void AppGlobal_UpdateSensorData() {
    static unsigned long last_sensor_update = 0;
    const unsigned long SENSOR_UPDATE_INTERVAL = 2000; // 每2秒更新一次
    
    unsigned long current_time = millis();
    if (current_time - last_sensor_update < SENSOR_UPDATE_INTERVAL) {
        return; // 还没到更新时间
    }
    last_sensor_update = current_time;
    
    // 读取传感器1（AHT20_SENSOR_1）
    if (is_aht20_sensor_available(AHT20_SENSOR_1)) {
        float temp1, hum1;
        if (read_aht20_sensor_values(AHT20_SENSOR_1, &temp1, &hum1)) {
            g_current_temperature = temp1;
            g_current_humidity = (int)hum1;
            sensor1Data.temperature = temp1;
            sensor1Data.humidity = hum1;
            sensor1Data.isValid = true;
            Serial.printf("Sensor 1: %.1f°C, %d%%\n", temp1, (int)hum1);
        } else {
            sensor1Data.isValid = false;
            Serial.println("Failed to read sensor 1");
        }
    } else {
        sensor1Data.isValid = false;
        // Serial.println("Sensor 1 not available"); // 注释掉避免过多输出
    }
    
    // 读取传感器2（AHT20_SENSOR_2）
    if (is_aht20_sensor_available(AHT20_SENSOR_2)) {
        float temp2, hum2;
        if (read_aht20_sensor_values(AHT20_SENSOR_2, &temp2, &hum2)) {
            g_current_temperature_2 = temp2;
            g_current_humidity_2 = (int)hum2;
            sensor2Data.temperature = temp2;
            sensor2Data.humidity = hum2;
            sensor2Data.isValid = true;
            Serial.printf("Sensor 2: %.1f°C, %d%%\n", temp2, (int)hum2);
        } else {
            sensor2Data.isValid = false;
            Serial.println("Failed to read sensor 2");
        }
    } else {
        sensor2Data.isValid = false;
        // Serial.println("Sensor 2 not available"); // 注释掉避免过多输出
    }
    UBaseType_t taskCount = uxTaskGetNumberOfTasks();
        TaskStatus_t* tasks = (TaskStatus_t*)pvPortMalloc(taskCount * sizeof(TaskStatus_t));
        if (tasks) {
            // 2) 获取所有任务的运行时统计（总滴答数写入 totalRunTime）
            uint32_t totalRunTime;
            uxTaskGetSystemState(tasks, taskCount, &totalRunTime);
            // 3) 找到「IDLE」任务的运行滴答
            uint32_t idleRunTime = 0;
            for (UBaseType_t i = 0; i < taskCount; i++) {
                if (strcmp(tasks[i].pcTaskName, "IDLE") == 0) {
                    idleRunTime = tasks[i].ulRunTimeCounter;
                    break;
                }
            }
            vPortFree(tasks);
            // 4) 计算繁忙百分比
            uint32_t busyPct = 0;
       if (totalRunTime > 0) {
            busyPct = (totalRunTime - idleRunTime) * 100 / totalRunTime;
            }
            // 5) 拼入全局状态字符串
            snprintf(g_work_status_str, sizeof(g_work_status_str),
                     "工作状态:保温中...  CPU:%u%%", (unsigned)busyPct);
        }
        

}

// ============== 初始化函数 ==============
void AppGlobal_Init() {
    // 初始化传感器数据结构体
    sensor1Data.temperature = 0.0f;
    sensor1Data.humidity = 0.0f;
    sensor1Data.isValid = false;

    sensor2Data.temperature = 0.0f;
    sensor2Data.humidity = 0.0f;
    sensor2Data.isValid = false;

    // 设置初始默认值（全局变量已经在定义时初始化，这里可选）
    g_current_temperature = 25.0f;
    g_current_humidity = 50;
    g_current_temperature_2 = 26.0f;
    g_current_humidity_2 = 55;
    
    strcpy(g_current_time_str, "12:34:56");
    strcpy(g_current_date_str, "2024-05-23");
    strcpy(g_work_status_str, "工作状态:保温中...");
    
    g_target_temperature_left = 37.0f;
    g_target_humidity_left = 60;
    g_target_temperature_right = 37.0f;
    g_target_humidity_right = 60;
    g_generic_thermostat_active = false;
    g_incubation_control_active = false;

    // 屏幕对象指针初始化为 NULL（已经在定义时设置）
    screen_main = NULL;
    screen_control = NULL;
    screen_setting = NULL;
    
    Serial.println("AppGlobal: Global variables initialized successfully.");
    Serial.println("AppGlobal: Remember to call AppGlobal_UpdateSensorData() in main loop.");
}
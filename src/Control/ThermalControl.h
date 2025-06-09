// File: src/SettingsLogic/ThermalControl.h
#ifndef THERMAL_CONTROL_H
#define THERMAL_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

// 温度控制范围
#define MIN_TARGET_TEMP 20.0f
#define MAX_TARGET_TEMP 40.0f
#define TEMP_HYSTERESIS 0.3f     // 温度滞后控制：±0.3°C
#define OVERHEAT_THRESHOLD 3.0f  // 过热保护阈值：+3°C

// 风扇控制参数
#define FAN_SHUTDOWN_DELAY_MS 45000  // 风扇延迟关闭时间：45秒
#define FAN_MIN_RPM 1000             // 🔥 降低最低正常转速阈值
#define FAN_MAX_RPM 5000             // 风扇最高正常转速
#define PULSES_PER_REVOLUTION 2      // 🔥 每转脉冲数，根据校准结果调整
#define RPM_CALCULATION_INTERVAL 2000  // RPM计算间隔2秒
#define RPM_DEBOUNCE_TIME 100         // 🔥 防抖时间50微秒

// 加湿器控制引脚
#define HUMIDIFIER_CTRL_PIN 37       // 加湿器控制引脚
#define FAN_SPEED_PIN       17       // 🔥 风扇转速检测引脚(从18改为36，避免与串口冲突)

// 系统状态枚举
typedef enum {
    THERMAL_STATE_IDLE,          // 空闲状态
    THERMAL_STATE_HEATING,       // 加热中
    THERMAL_STATE_COOLING,       // 冷却中（风扇延迟关闭）
    THERMAL_STATE_OVERHEAT,      // 过热保护
    THERMAL_STATE_FAN_FAULT,     // 风扇故障
    THERMAL_STATE_ERROR          // 系统错误
} thermal_state_t;

// 区域标识（左右分区）
typedef enum {
    THERMAL_ZONE_LEFT = 0,       // 左区（传感器1）
    THERMAL_ZONE_RIGHT = 1,      // 右区（传感器2）
    THERMAL_ZONE_COUNT = 2
} thermal_zone_t;

// 控制参数结构体
typedef struct {
    bool enabled;                // 温控系统总开关
    float target_temp_left;      // 左区目标温度
    float target_temp_right;     // 右区目标温度
    bool heater_enabled;         // 加热器开关
    bool fan_enabled;            // 风扇开关
    bool humidifier_enabled;     // 加湿器开关
    thermal_state_t state;       // 当前系统状态
} thermal_config_t;

// 运行状态结构体
typedef struct {
    bool heater_active;          // 加热器当前状态
    bool fan_active;             // 风扇当前状态
    bool humidifier_active;      // 加湿器当前状态
    unsigned long fan_rpm;       // 风扇转速
    bool fan_fault;              // 风扇故障标志
    bool overheat_protection;    // 过热保护激活
    unsigned long fan_shutdown_time; // 风扇计划关闭时间
    char status_message[64];     // 状态消息
} thermal_status_t;

// === 核心功能函数 ===
void ThermalControl_Init();
void ThermalControl_Handler();
void ThermalControl_SetEnabled(bool enabled);
bool ThermalControl_IsEnabled();
bool ThermalControl_SetTargetTemp(thermal_zone_t zone, float target_temp);
float ThermalControl_GetTargetTemp(thermal_zone_t zone);
thermal_state_t ThermalControl_GetState();
const thermal_status_t* ThermalControl_GetStatus();
const thermal_config_t* ThermalControl_GetConfig();
void ThermalControl_SetHeater(bool state);
void ThermalControl_SetFan(bool state);
void ThermalControl_SetHumidifier(bool state);
unsigned long ThermalControl_GetFanRPM();
bool ThermalControl_HasFault();
void ThermalControl_ResetFault();
const char* ThermalControl_GetStateDescription();
void ThermalControl_PrintStatus();
void ThermalControl_SetDebugMode(bool debug);

// RPM相关函数
void ThermalControl_CalibrateRPM();
void ThermalControl_DiagnoseRPM();
void ThermalControl_ResetRPMCounter();

#endif // THERMAL_CONTROL_H
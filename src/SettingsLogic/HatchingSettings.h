// File: src/SettingsLogic/HatchingSettings.h - 修正版本
#ifndef HATCHING_SETTINGS_H
#define HATCHING_SETTINGS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// 鸟类品种枚举 - 严格对应UI顺序
typedef enum {
    PARROT_SPECIES_BUDGIE = 0,       // 牡丹
    PARROT_SPECIES_COCKATIEL,        // 玄凤
    PARROT_SPECIES_SUN_CONURE,       // 太阳锥尾
    PARROT_SPECIES_GOLDEN_SUN,       // 金太阳
    PARROT_SPECIES_AFRICAN_GREY,     // 非洲灰
    PARROT_SPECIES_BLUE_GOLD_MACAW,  // 蓝黄金刚
    PARROT_SPECIES_MONK,             // 和尚
    PARROT_SPECIES_CUSTOM,           // 自定义
    PARROT_SPECIES_COUNT = 8         // 总数量
} parrot_species_t;

// 孵化阶段枚举
typedef enum {
    HATCH_STAGE_SETUP = 0,           // 准备中
    HATCH_STAGE_EARLY_INCUBATION,    // 早期孵化
    HATCH_STAGE_MID_INCUBATION,      // 中期孵化
    HATCH_STAGE_LATE_INCUBATION,     // 后期孵化
    HATCH_STAGE_HATCHING,            // 即将出雏
    HATCH_STAGE_COUNT
} hatch_stage_t;

// 孵化状态结构体
typedef struct {
    bool active;                     // 孵化是否激活
    parrot_species_t species;        // 鸟类品种
    hatch_stage_t current_stage;     // 当前孵化阶段
    
    // 时间相关
    unsigned long process_start_time; // 孵化开始时间
    uint8_t days_elapsed;            // 已经过天数
    uint8_t total_days;              // 总孵化天数
    
    // 目标参数
    float target_temp;               // 目标温度
    int target_humidity;             // 目标湿度
    
    // 统计信息
    uint32_t total_turns;            // 总翻蛋次数
    uint32_t total_ventilations;     // 总通风次数
} hatching_status_t;

// 状态更新回调函数类型
typedef void (*hatching_status_update_cb_t)(void);

// === 核心API函数 ===
void HatchingSettings_Init();
void HatchingSettings_StartIncubation(parrot_species_t species);
void HatchingSettings_StopIncubation();
bool HatchingSettings_IsActive();
void HatchingSettings_SetCustomParameters(float temp, int humidity);
const hatching_status_t* HatchingSettings_GetStatus();
const char* HatchingSettings_GetStageName(hatch_stage_t stage);
void HatchingSettings_GetFormattedTimeRemaining(char* buffer, size_t buffer_len);
void HatchingSettings_UpdateProcess();
void HatchingSettings_RegisterStatusUpdateCallback(hatching_status_update_cb_t cb);

// === 手动操作函数 ===
void HatchingSettings_SetAutoTurn(bool enabled);
void HatchingSettings_SetAutoVentilation(bool enabled);
void HatchingSettings_ManualTurn();
void HatchingSettings_ManualVentilation();
void HatchingSettings_GetNextActionTime(char* turn_buffer, char* vent_buffer, size_t buffer_len);

// === 新增函数 ===
bool HatchingSettings_SetCustomDays(int days);
bool HatchingSettings_IsCustomMode();
const char* HatchingSettings_GetSpeciesName(parrot_species_t species);
bool HatchingSettings_GetSpeciesDefaults(parrot_species_t species, float* temp, int* humidity, int* days);

#endif // HATCHING_SETTINGS_H
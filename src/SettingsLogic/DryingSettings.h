#ifndef DRYING_SETTINGS_H
#define DRYING_SETTINGS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "../App/AppGlobal.h" // 包含 AppGlobal.h 以获取 timed_operation_config_t

// 烘干状态更新回调函数类型
typedef void (*drying_status_update_cb_t)(void);

// 初始化烘干设置
void DryingSettings_Init();

// 设置/获取烘干启用状态
void DryingSettings_SetEnabled(bool enabled);
bool DryingSettings_IsEnabled();

// 定时操作相关函数
void DryingSettings_SetTimedOperation(const timed_operation_config_t* config);
const timed_operation_config_t* DryingSettings_GetTimedOperationSettings();
void DryingSettings_UpdateTimedState();
bool DryingSettings_IsTimedSessionActive();

// 获取格式化的剩余倒计时时间字符串
void DryingSettings_GetFormattedRemainingCountdown(char* buffer, size_t buffer_len);

// 注册状态更新回调
void DryingSettings_RegisterStatusUpdateCallback(drying_status_update_cb_t cb);

#endif // DRYING_SETTINGS_H
#ifndef FRAME_ANIMATION_H
#define FRAME_ANIMATION_H

#include "../Config/Config.h"
#include <stddef.h>
#include <stdbool.h>
#include <LovyanGFX.hpp>       // for LGFX reference
#include "../Config/LGFX_Config.h"  // <— 包含 LGFX 定义
#include <SD_MMC.h>            // for SD_MMC.open in implementation

// 初始化帧动画模块（申请双缓冲等）
bool FrameAnimation_Init(void);
// 启动后台播放任务（非阻塞）
void FrameAnimation_StartTask(LGFX& tft);
bool FrameAnimation_PlayBootSequence(LGFX& tft);

// 反初始化，释放资源
void FrameAnimation_DeInit(void);
void  FrameAnimation_Start(bool loop);
void  FrameAnimation_Stop(void);
bool  FrameAnimation_IsPlaying(void);

#endif // FRAME_ANIMATION_H

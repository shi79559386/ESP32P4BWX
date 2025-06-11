// 文件：src/Peripherals/FrameAnimation.h
// 版本：彻底修正，统一使用 Adafruit_ST7789

#ifndef FRAME_ANIMATION_H
#define FRAME_ANIMATION_H

#include <Adafruit_ST7789.h> // <--- 统一使用此头文件

// 初始化帧动画模块
bool FrameAnimation_Init();

// 核心修改：函数声明统一为 Adafruit_ST7789&
bool FrameAnimation_PlayBootSequence(Adafruit_ST7789& tft);

// 释放资源
void FrameAnimation_DeInit();

// 移除了其他库（如 LGFX）的函数声明，避免冲突
void FrameAnimation_Start(bool loop);
void FrameAnimation_Stop(void);
bool FrameAnimation_IsPlaying(void);
bool FrameAnimation_GetAndAdvanceToNextFramePath(char* buf, size_t len);


#endif // FRAME_ANIMATION_H

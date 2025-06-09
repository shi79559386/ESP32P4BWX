#ifndef FRAME_ANIMATION_H
#define FRAME_ANIMATION_H

#include "../Config/Config.h"
#include <stddef.h>
#include <stdbool.h>
#include "../Config/LGFX_Config.h" // 我们在这里引入新定义的 LGFX 类型


/**
 * @brief 初始化动画模块（只要调用一次即可）
 * @return true 如果初始化成功或已完成
 */
bool FrameAnimation_Init(void);

/**
 * @brief 播放预定义的开机动画序列
 * @param tft 传入要绘图的 LGFX 实例
 * @return 播放完成或失败后返回 false
 */
bool FrameAnimation_PlayBootSequence(LGFX& tft);

// 以下是可选的通用控制接口

/**
 * @brief 开始动画播放（但并不立刻绘制一帧），内部用于 FrameAnimation_PlayBootSequence
 * @param loop 如果为 true，播放到最后一帧后循环回到第一帧
 */
void FrameAnimation_Start(bool loop);

/** 停止播放 */
void FrameAnimation_Stop(void);

/** 判断当前是否正在播放 */
bool FrameAnimation_IsPlaying(void);

/**
 * @brief 获取下一帧的文件路径，并将内部索引前移
 * @param path_buffer 存放帧文件相对路径的缓冲区
 * @param buffer_len  缓冲区长度
 * @return 如果还有帧要播放返回 true，否则 false
 */
bool FrameAnimation_GetAndAdvanceToNextFramePath(char* path_buffer, size_t buffer_len);

/** 模块反初始化，恢复到未初始化状态（很少用到） */
void FrameAnimation_DeInit(void);

#endif // FRAME_ANIMATION_H

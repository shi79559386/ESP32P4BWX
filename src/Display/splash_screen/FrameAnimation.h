
 #pragma once


#include "MjpegClass.h"            // 用例程版的 MjpegClass
#include <SD_MMC.h>
#include "../Config/LGFX_Config.h"


// 播放MJPEG动画（基于例程的 tjpgdClass）
bool FrameAnimation_Play(LGFX* lcd, const char* path);

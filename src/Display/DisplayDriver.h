// 文件: src/Display/DisplayDriver.h

#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include "lvgl.h"

void display_init(void);
void lvgl_touch_read_cb(lv_indev_drv_t * drv, lv_indev_data_t * data);

#endif
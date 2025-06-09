// File: src/touch/TouchDriver.h
#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#include <lvgl.h> // <--- 添加这一行

// ✅ 厂家示例的函数声明
void touch_init(unsigned short int w, unsigned short int h, unsigned char r);
bool touch_touched(void);
bool touch_has_signal(void);
bool touch_released(void);

// ✅ 厂家示例的全局变量声明
extern int touch_last_x, touch_last_y;

// ✅ 保持原有接口兼容性
bool touch_hal_init(uint16_t width, uint16_t height, uint8_t rotation);
void lvgl_touch_read_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

#endif // TOUCH_DRIVER_H
// File: src/touch/TouchDriver.cpp
#include "TouchDriver.h"
#include "../Config/Config.h"
#include <Wire.h>
#include <Arduino.h>
#include <FT6336.h>

// ✅ 厂家示例的配置
#define TOUCH_MAP_X1 0
#define TOUCH_MAP_X2 320
#define TOUCH_MAP_Y1 0
#define TOUCH_MAP_Y2 480

// ✅ 厂家示例的全局变量
int touch_last_x = 0, touch_last_y = 0;
unsigned short int width = 0, height = 0, rotation = 0;
unsigned short int min_x = 0, max_x = 0, min_y = 0, max_y = 0;

// ✅ 厂家原始的FT6336对象
static FT6336 ts = FT6336(TOUCH_SDA_PIN, TOUCH_SCL_PIN, TOUCH_INT_PIN, TOUCH_RST_PIN, 
                          max(TOUCH_MAP_X1, TOUCH_MAP_X2), max(TOUCH_MAP_Y1, TOUCH_MAP_Y2));

// ✅ 厂家原始的touch_init函数
void touch_init(unsigned short int w, unsigned short int h, unsigned char r) {
    width = w; 
    height = h;
    rotation = r;
    
    switch (r) {
        case ROTATION_NORMAL:
        case ROTATION_INVERTED:
            min_x = TOUCH_MAP_X1;
            max_x = TOUCH_MAP_X2;
            min_y = TOUCH_MAP_Y1;
            max_y = TOUCH_MAP_Y2;
            break;
        case ROTATION_LEFT:
        case ROTATION_RIGHT:
            min_x = TOUCH_MAP_Y1;
            max_x = TOUCH_MAP_Y2;
            min_y = TOUCH_MAP_X1;
            max_y = TOUCH_MAP_X2;
            break;
        default:
            break;
    }
    
    Serial.printf("TouchDriver: 厂家原始touch_init %dx%d, 旋转:%d\n", w, h, r);
    // 1) 初始化 I2C
    Wire.begin(TOUCH_SDA_PIN, TOUCH_SCL_PIN);

    // 2) 复位脚（如果有）
    if (TOUCH_RST_PIN >= 0) {
        pinMode(TOUCH_RST_PIN, OUTPUT);
        digitalWrite(TOUCH_RST_PIN, HIGH);
    }

    // 3) 中断脚（如果有）
    if (TOUCH_INT_PIN >= 0) {
        pinMode(TOUCH_INT_PIN, INPUT_PULLUP);
        // 如需 attachInterrupt，可在这里添加
    }
    ts.begin();
    ts.setRotation(r);
    
    Serial.println("TouchDriver: ✅ 厂家原始初始化完成（占用Wire到5,6）");
}

// ✅ 厂家原始的touch_touched函数
bool touch_touched(void) {
    ts.read();
    if (ts.isTouched) {
        touch_last_x = map(ts.points[0].x, min_x, max_x, 0, width - 1);
        touch_last_y = map(ts.points[0].y, min_y, max_y, 0, height - 1);
        
        // 调试输出
        static unsigned long last_debug = 0;
        if (millis() - last_debug > 500) {
            last_debug = millis();
            Serial.printf("厂家触摸: (%d,%d)\n", touch_last_x, touch_last_y);
        }
        
        return true;
    }
    return false;
}

// ✅ 厂家示例的其他函数
bool touch_has_signal(void) {
    return true;
}

bool touch_released(void) {
    return true;
}

// ✅ LVGL回调
void lvgl_touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    if (touch_touched()) {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_last_x;
        data->point.y = touch_last_y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

// ✅ 保持接口兼容
bool touch_hal_init(uint16_t w, uint16_t h, uint8_t r) {
    touch_init(w, h, r);
    return true;
}
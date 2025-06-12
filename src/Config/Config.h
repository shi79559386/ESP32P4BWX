// File: src/Config/Config.
#ifndef CONFIG_H
#define CONFIG_H

#include "lv_conf.h"
#include "../Peripherals/PCA9548A.h"

#define ENABLE_SENSOR_MODULE 0  // 0=跳过传感器，1=启用传感器
#define ENABLE_SD_CARD    1   // 板载动画
#define SKIP_BOOT_ANIMATION    1      // 🆕 新增：设置为 0 可跳过动画
#define USE_SOFTWARE_I2C  1

// --- LEDC (PWM) 配置 ---
#define LIGHTING_PWM_CHANNEL 1 // 使用 LEDC 通道 0
#define BACKLIGHT_PWM_CHANNEL 0 // 专门留给背光用通道 


// --- I2S 引脚定义 (用于 MAX98357A) ---
// 请根据你的实际接线修改这些引脚
#define I2S_BCLK_PIN   47 // Bit Clock
#define I2S_LRC_PIN    48 // Left/Right Clock (or Word Clock)
#define I2S_DOUT_PIN   49 // Data Out

// --- Screen Dimensions (固定为 480x320 横屏) ---
#define screenWidth 480
#define screenHeight 320


// --- I2C Addresses ---
#define AHT20_I2C_ADDR 0x38
#define DS3231_I2C_ADDR 0x68
#define PCA9548A_I2C_ADDR 0x70  // 新增：PCA9548A多路复用器地址


// --- Boot Animation Configuration ---
#define LVGL_INTERNAL_RAM_BUFFER_LINES 20
#define ANIM_FRAME_WIDTH          480
#define ANIM_FRAME_HEIGHT         320
#define ANIM_TOTAL_FRAMES         47
#define ANIM_FRAME_BUFFER_LINES   20
#define TARGET_TOTAL_DURATION_MS  4000


// --- UI布局常量 ---
#define STATUS_BAR_HEIGHT 30
#define NAV_BAR_HEIGHT    50

// --- LVGL Buffer Configuration ---
#define LVGL_INTERNAL_RAM_BUFFER_LINES 20

#define PARROT_DEFAULT_VOLUME 50  // 默认音量 (0-100)

#define TFT_BL   53            // LED back-light control pin
#define TFT_MISO 31
#define TFT_MOSI 29
#define TFT_SCLK 30
#define TFT_CS   28  // Chip select control pin
#define TFT_DC   22  // Data Command control pin
#define TFT_RST  52  // Reset pin (could connect to RST pin)

// --- Touch Panel FT6336 Configuration ---
#define TOUCH_SCL_PIN 19
#define TOUCH_SDA_PIN 18
#define TOUCH_INT_PIN 23
#define TOUCH_RST_PIN 17


// --- I2C1 Pins (Wire1) - AHT20 & DS3231 ---
#define I2C1_SDA_PIN  8    //I2C模块
#define I2C1_SCL_PIN  9    //I2C模块


// --- PCA9548A通道配置（新增PCF8574通道）---
#define PCA9548A_CHANNEL_DS3231_AHT20_1    PCA9548A_CHANNEL_0
#define PCA9548A_CHANNEL_AHT20_2           PCA9548A_CHANNEL_1 // 通道1：AHT20传感器2
//#define PCF8574_PCA_CHANNEL       PCA9548A_CHANNEL_2 // VS1053 用的 I2C 多路复用通道


#define HEATER_CTRL_PIN     41
#define FAN_CTRL_PIN        42
#define FAN_SPEED_PIN       34
#define HUMIDIFIER_CTRL_PIN 5
#define LIGHTING_PIN        13  // 假设灯光控制接到GPIO 1
#define STERILIZE_PIN       2  // 假设杀菌灯接到GPIO 2
#define FRESH_AIR_PIN       4  // 假设新风扇接到GPIO 4
#define PARROT_STATE_PIN    15 // 假设鹦语状态指示灯接到GPIO 15



// --- 板载 microSD（开机动画 用 SDMMC）---
#define BOOT_SD_MOUNT_POINT    "/sdcard"

// --- 外接音频 SD（SPI 模式）---
#define AUDIO_SD_CS_PIN        38
#define AUDIO_SD_SCLK_PIN      37
#define AUDIO_SD_MISO_PIN      36
#define AUDIO_SD_MOSI_PIN      35

#define TOUCH_PANEL_NATIVE_WIDTH  320 // 触摸屏IC未经旋转时的物理X最大值
#define TOUCH_PANEL_NATIVE_HEIGHT 480 // 触摸屏IC未经旋转时的物理Y最大值

// --- Sensor/Operational Ranges ---
#define ACTUAL_TEMP_MIN 20.0f
#define ACTUAL_TEMP_MAX 40.0f // As used in UI_MainScreen.cpp color logic and main.cpp mock data
#define ACTUAL_HUMID_MIN 0
#define ACTUAL_HUMID_MAX 100

// --- 颜色定义 ---
#define SCREEN_BG_COLOR         lv_color_hex(0x001E3C)
#define PANEL_BG_COLOR          lv_color_hex(0x061A2E)
#define TEXT_BTN_BOX_BG_COLOR   lv_color_hex(0x1A3F6C)
#define BUTTON_BG_COLOR         lv_color_hex(0x3C64DC)
#define TEXT_COLOR_WHITE        lv_color_white()
#define METER_SCALE_TEXT_COLOR  lv_color_hex(0xFFFFFF)
#define METER_TICK_MAJOR_COLOR  lv_color_hex(0xFFFFFF)
#define METER_TICK_MINOR_COLOR  lv_color_hex(0xA0A0A0)
#define TEMP_VALUE_COLOR        lv_color_hex(0xFF6B6B)
#define HUMID_VALUE_COLOR       lv_color_hex(0x4DB6AC)

#define POPUP_CONTENT_BG_COLOR  lv_color_hex(0x6A9FCE)
#define POPUP_HEADER_BG_COLOR   lv_color_hex(0xA0B0E0)

#define SWITCH_COLOR_ON_CYAN    lv_color_hex(0x48D1CC)


#define ROLLER_TEXT_MUTED_COLOR_BRIGHTER lv_color_hex(0xAFAFAF)// 未选中项文字 - 调亮后的浅灰色 (原为 0xC8C8C8)
#define ROLLER_SELECTED_BG_LIGHT_BLUE    lv_color_hex(0x7CB9E8) // 选中项背景 - 淡蓝色 (例如：柔和的天蓝色)
#define ROLLER_BORDER_COLOR              lv_color_hex(0x4682C8)   // 滚轮边框颜色 (保持或按需调整)

#define ROLLER_COORDINATED_BG_COLOR lv_color_hex(0x1A2B3C)



// --- Audio ---
#define AUDIO_ROOT_PATH "/audio"
#define MAX_AUDIO_FILES_IN_PLAYLIST 50 
#define MAX_AUDIO_FILENAME_LENGTH 256
#define PARROT_DEFAULT_VOLUME 50



#endif
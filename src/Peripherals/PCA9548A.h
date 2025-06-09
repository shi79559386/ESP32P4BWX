// File: src/Peripherals/PCA9548A.h
#ifndef PCA9548A_H
#define PCA9548A_H

#include <Wire.h>
#include <stdbool.h>
#include <stdint.h>

// PCA9548A配置
#define PCA9548A_I2C_ADDR   0x70    // PCA9548A的I2C地址
#define PCA9548A_MAX_CHANNELS 8     // PCA9548A支持8个通道

// 通道定义
typedef enum {
    PCA9548A_CHANNEL_0 = 0,  // DS3231 + AHT20传感器1
    PCA9548A_CHANNEL_1 = 1,  // AHT20传感器2
    PCA9548A_CHANNEL_2 = 2,
    PCA9548A_CHANNEL_3 = 3,
    PCA9548A_CHANNEL_4 = 4,
    PCA9548A_CHANNEL_5 = 5,
    PCA9548A_CHANNEL_6 = 6,
    PCA9548A_CHANNEL_7 = 7,
    PCA9548A_CHANNEL_NONE = 0xFF  // 关闭所有通道
} pca9548a_channel_t;

// 设备通道映射
#define DS3231_CHANNEL      PCA9548A_CHANNEL_0  // DS3231时钟模块在通道0
#define AHT20_SENSOR_1_CHANNEL PCA9548A_CHANNEL_0  // AHT20传感器1在通道0
#define AHT20_SENSOR_2_CHANNEL PCA9548A_CHANNEL_1  // AHT20传感器2在通道1
#define PCA9548A_TOUCH_CHANNEL 0   // 触摸芯片所在通道

/**
 * 初始化PCA9548A多路复用器
 * @param wire_bus I2C总线指针
 * @return true 初始化成功，false 失败
 */
bool pca9548a_init(TwoWire* wire_bus);

/**
 * 检查PCA9548A是否可用
 * @return true 可用，false 不可用
 */
bool pca9548a_is_available();

/**
 * 选择PCA9548A通道
 * @param channel 要选择的通道
 * @return true 选择成功，false 失败
 */
bool pca9548a_select_channel(pca9548a_channel_t channel);

/**
 * 关闭所有PCA9548A通道
 * @return true 成功，false 失败
 */
bool pca9548a_disable_all_channels();

/**
 * 获取当前激活的通道
 * @return 当前激活的通道掩码（位域）
 */
uint8_t pca9548a_get_current_channels();

/**
 * 扫描指定通道上的I2C设备
 * @param channel 要扫描的通道
 * @param bus_name 总线名称（用于调试输出）
 */
void pca9548a_scan_channel(pca9548a_channel_t channel, const char* bus_name);

/**
 * 扫描所有通道上的I2C设备
 */
void pca9548a_scan_all_channels();

/**
 * 多通道选择（可以同时激活多个通道）
 * @param channel_mask 通道掩码（位0-7对应通道0-7）
 * @return true 成功，false 失败
 */
bool pca9548a_select_channels(uint8_t channel_mask);

/**
 * 检查指定通道是否激活
 * @param channel 要检查的通道
 * @return true 通道激活，false 通道未激活
 */
bool pca9548a_is_channel_active(pca9548a_channel_t channel);

/**
 * 获取PCA9548A状态信息（调试用）
 */
void pca9548a_print_status();

#endif // PCA9548A_H
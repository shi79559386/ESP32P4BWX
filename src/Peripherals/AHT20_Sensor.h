// File: src/Peripherals/AHT20_Sensor.h
#ifndef AHT20_SENSOR_H
#define AHT20_SENSOR_H

#include <stdbool.h>
#include <Wire.h> // Required for TwoWire class parameter (虽然现在用软件I2C，但保持兼容)

// 传感器ID枚举
typedef enum {
    AHT20_SENSOR_1 = 0,  // 第一个传感器（软件I²C: SDA=21, SCL=20）
    AHT20_SENSOR_2 = 1,  // 第二个传感器（软件I²C: SDA=8, SCL=9）
    NUM_AHT20_SENSORS    // 传感器总数
} aht20_sensor_id_t;

// === 核心功能函数 ===

/**
 * 初始化所有AHT20传感器（使用软件I²C）
 * @param i2c_bus_1 硬件I2C总线指针 (现在会被忽略，使用软件I²C)
 * @param i2c_bus_2 硬件I2C总线指针 (现在会被忽略，使用软件I²C)
 * @return true 如果所有传感器初始化成功，false 如果有任何传感器失败
 * 
 * 注意：此函数现在内部使用软件I²C，传入的硬件I²C参数会被忽略
 */
bool init_all_aht20_sensors(TwoWire* i2c_bus_1 = nullptr, TwoWire* i2c_bus_2 = nullptr);

/**
 * 检查指定传感器是否可用
 * @param sensor_id 传感器ID
 * @return true 如果传感器可用，false 如果不可用
 */
bool is_aht20_sensor_available(aht20_sensor_id_t sensor_id);

/**
 * 读取指定传感器的温湿度数据
 * @param sensor_id 传感器ID
 * @param temperature 温度指针（输出参数，单位：摄氏度）
 * @param humidity 湿度指针（输出参数，单位：百分比）
 * @return true 读取成功，false 读取失败
 */
bool read_aht20_sensor_values(aht20_sensor_id_t sensor_id, float *temperature, float *humidity);

// === 调试和诊断函数 ===

/**
 * 扫描指定I2C总线上的所有设备
 * @param wire_bus I2C总线指针
 * @param bus_name 总线名称（用于调试输出）
 */
void scan_i2c_bus(TwoWire* wire_bus, const char* bus_name);

/**
 * 测试I2C总线通信状态
 * @param wire_bus I2C总线指针
 * @param bus_name 总线名称（用于调试输出）
 * @return true 总线通信正常，false 通信异常
 */
bool test_i2c_bus(TwoWire* wire_bus, const char* bus_name);

/**
 * 打印所有传感器的状态信息（调试用）
 */
void print_aht20_sensor_status();

/**
 * 单独初始化指定传感器（调试用）
 * @param sensor_id 传感器ID
 * @return true 初始化成功，false 失败
 */
bool init_single_aht20_sensor(aht20_sensor_id_t sensor_id);

/**
 * 重置指定传感器（调试用）
 * @param sensor_id 传感器ID
 * @return true 重置成功，false 失败
 */
bool reset_aht20_sensor(aht20_sensor_id_t sensor_id);

#endif // AHT20_SENSOR_H
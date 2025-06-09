// File: src/Peripherals/DS3231_Clock.h
#ifndef DS3231_CLOCK_H
#define DS3231_CLOCK_H

#include <stdbool.h>
#include <RtcDS3231.h> // 使用 Rtc by Makuna 库

#define DS3231_I2C_ADDR 0x68  // DS3231 默认 I2C 地址

/**
 * 初始化DS3231时钟模块（通过PCA9548A）
 * @return true 初始化成功，false 失败
 */
bool init_ds3231();

/**
 * 检查DS3231是否可用
 * @return true 可用，false 不可用
 */
bool is_ds3231_available();

/**
 * 获取当前日期时间（通过PCA9548A）
 * @param dt 输出参数，存储获取的时间
 * @return true 获取成功，false 失败
 */
bool get_current_datetime_rtc(RtcDateTime* dt);

/**
 * 获取DS3231内置温度传感器的温度
 * @return 温度值（摄氏度），失败时返回NAN
 */
float get_temperature_ds3231();

/**
 * 设置DS3231时间（通过PCA9548A）
 * @param dt 要设置的时间
 * @return true 设置成功，false 失败
 */
bool set_current_datetime_rtc(const RtcDateTime& dt);

/**
 * 打印DS3231状态信息（调试用）
 */
void print_ds3231_status();

#endif // DS3231_CLOCK_H
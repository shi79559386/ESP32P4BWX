// File: src/Peripherals/DS3231_Clock.cpp
#include "DS3231_Clock.h"
#include "PCA9548A.h"  // 新增：包含PCA9548A头文件
#include "../Config/Config.h"
#include <Wire.h>
#include <Arduino.h>

static RtcDS3231<TwoWire> *rtc_instance = nullptr;
static bool ds3231_is_ready = false;
static TwoWire* ds3231_i2c_bus = nullptr;

bool init_ds3231() {
    Serial.println("=== DS3231时钟模块初始化（通过PCA9548A）===");
    
    // 检查PCA9548A是否可用
    if (!pca9548a_is_available()) {
        Serial.println("❌ PCA9548A不可用，无法初始化DS3231");
        ds3231_is_ready = false;
        return false;
    }
    
    // 使用Wire1总线（与PCA9548A连接的总线）
    ds3231_i2c_bus = &Wire1;
    
    // 选择DS3231所在的PCA9548A通道（通道0）
    if (!pca9548a_select_channel(DS3231_CHANNEL)) {
        Serial.printf("❌ 无法选择DS3231通道%d\n", DS3231_CHANNEL);
        ds3231_is_ready = false;
        return false;
    }
    
    delay(50); // 等待通道切换稳定
    
    // 第一次调用时创建RTC实例
    if (rtc_instance == nullptr) {
        rtc_instance = new RtcDS3231<TwoWire>(*ds3231_i2c_bus);
    }
    
    // 初始化RTC
    rtc_instance->Begin();
    
    // 测试DS3231通信
    Serial.printf("🔍 测试DS3231通信（地址0x%02X，通道%d）...\n", DS3231_I2C_ADDR, DS3231_CHANNEL);
    ds3231_i2c_bus->beginTransmission(DS3231_I2C_ADDR);
    int error = ds3231_i2c_bus->endTransmission();
    
    if (error == 0) {
        Serial.println("✅ DS3231在PCA9548A通道上响应成功");
        ds3231_is_ready = true;
    } else {
        Serial.printf("❌ DS3231通信失败，错误代码: %d\n", error);
        Serial.println("请检查:");
        Serial.printf("  - DS3231是否正确连接到PCA9548A通道%d\n", DS3231_CHANNEL);
        Serial.println("  - DS3231供电是否正常");
        Serial.println("  - I2C接线是否正确");
        ds3231_is_ready = false;
        return false;
    }
    
    // 检查RTC运行状态
    if (!rtc_instance->GetIsRunning()) {
        Serial.println("⚠️ RTC未运行，设置编译时间并启动...");
        
        // 确保通道仍然选中
        pca9548a_select_channel(DS3231_CHANNEL);
        delay(10);
        
        RtcDateTime compileTime(__DATE__, __TIME__);
        rtc_instance->SetDateTime(compileTime);
        
        if (!rtc_instance->GetIsRunning()) {
            Serial.println("❌ 设置后RTC仍未运行！");
            ds3231_is_ready = false;
        } else {
            Serial.println("✅ RTC已设置为编译时间并开始运行");
            Serial.printf("   设置时间: %04u-%02u-%02u %02u:%02u:%02u\n", 
                         compileTime.Year(), compileTime.Month(), compileTime.Day(),
                         compileTime.Hour(), compileTime.Minute(), compileTime.Second());
        }
    } else {
        Serial.println("✅ RTC正在运行");
    }
    
    // 检查时间有效性
    if (ds3231_is_ready) {
        // 确保通道选中
        pca9548a_select_channel(DS3231_CHANNEL);
        delay(10);
        
        if (!rtc_instance->IsDateTimeValid()) {
            Serial.println("⚠️ RTC时间无效，重设编译时间...");
            RtcDateTime compileTime(__DATE__, __TIME__);
            rtc_instance->SetDateTime(compileTime);
            
            if (!rtc_instance->IsDateTimeValid()) {
                Serial.println("❌ 时间设置后仍然无效！");
            } else {
                Serial.println("✅ RTC时间已更新为编译时间");
            }
        } else {
            // 显示当前时间
            RtcDateTime now = rtc_instance->GetDateTime();
            Serial.printf("✅ 当前RTC时间: %04u-%02u-%02u %02u:%02u:%02u\n", 
                         now.Year(), now.Month(), now.Day(),
                         now.Hour(), now.Minute(), now.Second());
        }
    }
    
    // 测试温度读取功能
    if (ds3231_is_ready) {
        pca9548a_select_channel(DS3231_CHANNEL);
        delay(10);
        
        float temp = rtc_instance->GetTemperature().AsFloatDegC();
        if (!isnan(temp)) {
            Serial.printf("✅ DS3231温度读取成功: %.2f°C\n", temp);
        } else {
            Serial.println("⚠️ DS3231温度读取失败");
        }
    }
    
    Serial.printf("=== DS3231初始化完成: %s ===\n", ds3231_is_ready ? "成功" : "失败");
    Serial.println();
    
    return ds3231_is_ready;
}

bool is_ds3231_available() {
    return ds3231_is_ready;
}

bool get_current_datetime_rtc(RtcDateTime* dt) {
    if (!ds3231_is_ready || dt == nullptr) {
        return false;
    }
    
    // 检查PCA9548A是否可用
    if (!pca9548a_is_available()) {
        return false;
    }
    
    // 选择DS3231通道
    if (!pca9548a_select_channel(DS3231_CHANNEL)) {
        return false;
    }
    
    delay(5); // 短暂延迟确保通道切换
    
    // 读取时间
    *dt = rtc_instance->GetDateTime();
    return true;
}

float get_temperature_ds3231() {
    if (!ds3231_is_ready) {
        return NAN;
    }
    
    // 检查PCA9548A是否可用
    if (!pca9548a_is_available()) {
        return NAN;
    }
    
    // 选择DS3231通道
    if (!pca9548a_select_channel(DS3231_CHANNEL)) {
        return NAN;
    }
    
    delay(5); // 短暂延迟确保通道切换
    
    // 读取温度
    return rtc_instance->GetTemperature().AsFloatDegC();
}

bool set_current_datetime_rtc(const RtcDateTime& dt) {
    if (!ds3231_is_ready) {
        return false;
    }
    
    // 检查PCA9548A是否可用
    if (!pca9548a_is_available()) {
        return false;
    }
    
    // 选择DS3231通道
    if (!pca9548a_select_channel(DS3231_CHANNEL)) {
        return false;
    }
    
    delay(10); // 较长延迟确保写入稳定
    
    // 设置时间
    rtc_instance->SetDateTime(dt);
    
    Serial.printf("DS3231时间已设置为: %04u-%02u-%02u %02u:%02u:%02u\n", 
                 dt.Year(), dt.Month(), dt.Day(),
                 dt.Hour(), dt.Minute(), dt.Second());
    
    return true;
}

void print_ds3231_status() {
    Serial.println("=== DS3231状态报告（通过PCA9548A）===");
    Serial.printf("DS3231就绪状态: %s\n", ds3231_is_ready ? "就绪" : "未就绪");
    Serial.printf("DS3231通道: %d\n", DS3231_CHANNEL);
    
    if (ds3231_is_ready && pca9548a_is_available()) {
        RtcDateTime now;
        if (get_current_datetime_rtc(&now)) {
            Serial.printf("当前时间: %04u-%02u-%02u %02u:%02u:%02u\n", 
                         now.Year(), now.Month(), now.Day(),
                         now.Hour(), now.Minute(), now.Second());
                         
            float temp = get_temperature_ds3231();
            if (!isnan(temp)) {
                Serial.printf("DS3231温度: %.2f°C\n", temp);
            } else {
                Serial.println("温度读取失败");
            }
        } else {
            Serial.println("时间读取失败");
        }
    } else {
        Serial.println("DS3231或PCA9548A不可用");
    }
    
    Serial.println("=== DS3231状态报告结束 ===\n");
}
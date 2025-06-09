// File: src/Peripherals/AHT20_Sensor.cpp
#include "AHT20_Sensor.h"
#include "PCA9548A.h"  // 新增：包含PCA9548A头文件
#include "../Config/Config.h"
#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include <Arduino.h>

static Adafruit_AHTX0 aht_sensors[NUM_AHT20_SENSORS];
static bool           aht20_is_ready[NUM_AHT20_SENSORS] = {false};
static TwoWire*       aht20_i2c_bus = nullptr;  // 统一使用一个I2C总线

// 传感器通道映射
static const pca9548a_channel_t sensor_channels[NUM_AHT20_SENSORS] = {
    AHT20_SENSOR_1_CHANNEL,  // 传感器1在通道0
    AHT20_SENSOR_2_CHANNEL   // 传感器2在通道1
};

// 传感器名称（用于调试输出）
static const char* sensor_names[NUM_AHT20_SENSORS] = {
    "AHT20传感器1(通道0+DS3231)",
    "AHT20传感器2(通道1)"
};

// I2C总线扫描功能（现在通过PCA9548A）
void scan_i2c_bus_via_pca9548a(pca9548a_channel_t channel, const char* bus_name) {
    Serial.printf("=== 通过PCA9548A扫描通道%d (%s) ===\n", channel, bus_name);
    
    if (!pca9548a_is_available()) {
        Serial.println("PCA9548A不可用，无法扫描");
        return;
    }
    
    // 选择通道
    if (!pca9548a_select_channel(channel)) {
        Serial.printf("无法选择PCA9548A通道%d\n", channel);
        return;
    }
    
    delay(50); // 等待通道切换稳定
    
    int device_count = 0;
    for (int address = 1; address < 127; address++) {
        aht20_i2c_bus->beginTransmission(address);
        int error = aht20_i2c_bus->endTransmission();
        
        if (error == 0) {
            Serial.printf("  📍 发现设备地址: 0x%02X", address);
            device_count++;
            
            if (address == AHT20_I2C_ADDR) {
                Serial.print(" <- AHT20传感器!");
            }
            if (address == DS3231_I2C_ADDR) {
                Serial.print(" <- DS3231时钟!");
            }
            Serial.println();
        }
    }
    
    if (device_count == 0) {
        Serial.printf("  ❌ 通道%d上未发现任何设备\n", channel);
    } else {
        Serial.printf("  ✅ 通道%d上发现%d个设备\n", channel, device_count);
    }
    Serial.println();
}

bool init_all_aht20_sensors(TwoWire* i2c_bus_1, TwoWire* i2c_bus_2) {
    // 现在只使用一个I2C总线，通过PCA9548A切换设备
    // i2c_bus_2参数保留兼容性，但会被忽略
    aht20_i2c_bus = i2c_bus_1;  // 使用Wire1 (SDA=8, SCL=9)
    
    if (aht20_i2c_bus == nullptr) {
        Serial.println("错误: AHT20传感器I2C总线指针为空！");
        return false;
    }
    
    Serial.println("=== AHT20传感器初始化（通过PCA9548A）===");
    Serial.printf("预期的AHT20 I2C地址: 0x%02X\n", AHT20_I2C_ADDR);
    Serial.printf("使用I2C总线: SDA=%d, SCL=%d\n", I2C1_SDA_PIN, I2C1_SCL_PIN);
    Serial.println("通过PCA9548A多路复用器访问传感器");
    
    // 检查PCA9548A是否可用
    if (!pca9548a_is_available()) {
        Serial.println("❌ PCA9548A不可用，无法初始化AHT20传感器");
        Serial.println("请确保PCA9548A已正确初始化");
        return false;
    }
    
    Serial.println("✅ PCA9548A可用，开始初始化传感器");
    
    // 扫描所有相关通道
    Serial.println("🔍 扫描设备分布...");
    for (int i = 0; i < NUM_AHT20_SENSORS; i++) {
        scan_i2c_bus_via_pca9548a(sensor_channels[i], sensor_names[i]);
    }
    
    // 等待系统稳定
    delay(200);
    
    // 初始化每个AHT20传感器
    bool all_ok = true;
    Serial.println("🌡️ 开始初始化AHT20传感器...");
    
    for (int i = 0; i < NUM_AHT20_SENSORS; i++) {
        Serial.printf("正在初始化%s...\n", sensor_names[i]);
        
        // 选择对应的PCA9548A通道
        if (!pca9548a_select_channel(sensor_channels[i])) {
            Serial.printf("❌ 无法选择通道%d，跳过传感器%d\n", sensor_channels[i], i + 1);
            aht20_is_ready[i] = false;
            all_ok = false;
            continue;
        }
        
        delay(50); // 等待通道切换稳定
        
        // 初始化传感器
        if (!aht_sensors[i].begin(aht20_i2c_bus)) {
            Serial.printf("❌ %s在通道%d上初始化失败！\n", sensor_names[i], sensor_channels[i]);
            Serial.printf("   请检查传感器在通道%d上的连接\n", sensor_channels[i]);
            aht20_is_ready[i] = false;
            all_ok = false;
        } else {
            Serial.printf("✅ %s初始化成功！\n", sensor_names[i]);
            aht20_is_ready[i] = true;
            
            // 测试读取
            delay(100);
            sensors_event_t hum_event, temp_event;
            
            if (aht_sensors[i].getEvent(&hum_event, &temp_event)) {
                Serial.printf("   📊 测试读取: 温度 %.1f°C, 湿度 %.1f%%\n", 
                              temp_event.temperature, hum_event.relative_humidity);
                
                // 验证数据合理性
                if (temp_event.temperature < -40 || temp_event.temperature > 85) {
                    Serial.printf("   ⚠️ 温度读数超出正常范围\n");
                }
                if (hum_event.relative_humidity < 0 || hum_event.relative_humidity > 100) {
                    Serial.printf("   ⚠️ 湿度读数超出正常范围\n");
                }
            } else {
                Serial.printf("   ⚠️ 传感器已初始化但测试读取失败\n");
            }
        }
        Serial.println();
    }
    
    // 初始化完成后关闭所有通道
    pca9548a_disable_all_channels();
    
    // 输出最终结果
    Serial.printf("=== AHT20初始化完成: %s ===\n", all_ok ? "完全成功" : "部分失败");
    for (int i = 0; i < NUM_AHT20_SENSORS; i++) {
        Serial.printf("%s状态: %s\n", sensor_names[i], aht20_is_ready[i] ? "就绪" : "未就绪");
    }
    Serial.println();
    
    return all_ok;
}

bool is_aht20_sensor_available(aht20_sensor_id_t sensor_id) {
    if (sensor_id >= NUM_AHT20_SENSORS) {
        Serial.printf("错误: 无效的传感器ID %d\n", sensor_id);
        return false;
    }
    return aht20_is_ready[sensor_id];
}

bool read_aht20_sensor_values(aht20_sensor_id_t sensor_id, float *temperature, float *humidity) {
    // 参数验证
    if (sensor_id >= NUM_AHT20_SENSORS) {
        Serial.printf("错误: 无效的传感器ID %d\n", sensor_id);
        return false;
    }
    
    if (temperature == nullptr || humidity == nullptr) {
        Serial.printf("错误: 温度或湿度指针为空\n");
        return false;
    }
    
    // 检查传感器是否就绪
    if (!aht20_is_ready[sensor_id]) {
        return false;
    }
    
    // 检查PCA9548A是否可用
    if (!pca9548a_is_available()) {
        Serial.printf("PCA9548A不可用，无法读取传感器%d\n", sensor_id + 1);
        return false;
    }
    
    // 选择对应的PCA9548A通道
    if (!pca9548a_select_channel(sensor_channels[sensor_id])) {
        Serial.printf("无法选择传感器%d的通道%d\n", sensor_id + 1, sensor_channels[sensor_id]);
        return false;
    }
    
    // 短暂延迟确保通道切换稳定
    delay(10);
    
    // 读取传感器数据
    sensors_event_t humidity_event, temp_event;
    if (!aht_sensors[sensor_id].getEvent(&humidity_event, &temp_event)) {
        Serial.printf("传感器%d读取数据失败\n", sensor_id + 1);
        return false;
    }
    
    // 验证数据合理性
    if (temp_event.temperature < -40 || temp_event.temperature > 85 ||
        humidity_event.relative_humidity < 0 || humidity_event.relative_humidity > 100) {
        Serial.printf("传感器%d数据超出正常范围 - 温度:%.1f°C, 湿度:%.1f%%\n", 
                      sensor_id + 1, temp_event.temperature, humidity_event.relative_humidity);
        return false;
    }
    
    // 返回数据
    *temperature = temp_event.temperature;
    *humidity = humidity_event.relative_humidity;
    
    return true;
}

void print_aht20_sensor_status() {
    Serial.println("=== AHT20传感器状态报告（通过PCA9548A）===");
    
    if (!pca9548a_is_available()) {
        Serial.println("❌ PCA9548A不可用，无法获取传感器状态");
        return;
    }
    
    for (int i = 0; i < NUM_AHT20_SENSORS; i++) {
        Serial.printf("%s: ", sensor_names[i]);
        
        if (aht20_is_ready[i]) {
            float temp, humid;
            if (read_aht20_sensor_values((aht20_sensor_id_t)i, &temp, &humid)) {
                Serial.printf("正常工作 - 温度:%.1f°C, 湿度:%.1f%%\n", temp, humid);
            } else {
                Serial.printf("初始化成功但读取失败\n");
            }
        } else {
            Serial.printf("未就绪或初始化失败\n");
        }
    }
    
    // 显示PCA9548A状态
    pca9548a_print_status();
    
    Serial.println("=== 状态报告结束 ===\n");
}
// File: src/Peripherals/PCA9548A.cpp
#include "PCA9548A.h"
#include "../Config/Config.h"
#include <Arduino.h>

static TwoWire* pca9548a_wire = nullptr;
static bool pca9548a_ready = false;
static uint8_t current_channel_mask = 0x00; // 当前激活的通道掩码

bool pca9548a_init(TwoWire* wire_bus) {
    if (wire_bus == nullptr) {
        return false;
    }
    
    pca9548a_wire = wire_bus;
    
    pca9548a_wire->beginTransmission(PCA9548A_I2C_ADDR);
    int error = pca9548a_wire->endTransmission();
    
    if (error == 0) {
        pca9548a_ready = true;
        
        if (pca9548a_disable_all_channels()) {
            // All channels disabled
        } else {
            // Problem disabling channels
        }
        
        // Test channel selection
        if (pca9548a_select_channel(PCA9548A_CHANNEL_0)) {
            pca9548a_disable_all_channels();
        } else {
            // Channel 0 selection failed
        }
    } else {
        pca9548a_ready = false;
    }
    
    return pca9548a_ready;
}



bool pca9548a_is_available() {
    #if !ENABLE_SENSOR_MODULE
        return false;  // 调试模式直接返回false
    #endif
        return pca9548a_ready;
    }

bool pca9548a_select_channel(pca9548a_channel_t channel) {
    if (!pca9548a_ready || pca9548a_wire == nullptr) {
        return false;
    }
    
    uint8_t channel_mask;
    
    if (channel == PCA9548A_CHANNEL_NONE) {
        channel_mask = 0x00; // 关闭所有通道
    } else if (channel < PCA9548A_MAX_CHANNELS) {
        channel_mask = 1 << channel; // 选择单个通道
    } else {
        return false;
    }
    
    pca9548a_wire->beginTransmission(PCA9548A_I2C_ADDR);
    pca9548a_wire->write(channel_mask);
    int error = pca9548a_wire->endTransmission();
    
    if (error == 0) {
        current_channel_mask = channel_mask;
        return true;
    } else {
        return false;
    }
}

bool pca9548a_disable_all_channels() {
    return pca9548a_select_channel(PCA9548A_CHANNEL_NONE);
}

bool pca9548a_select_channels(uint8_t channel_mask) {
    if (!pca9548a_ready || pca9548a_wire == nullptr) {
        return false;
    }
    
    pca9548a_wire->beginTransmission(PCA9548A_I2C_ADDR);
    pca9548a_wire->write(channel_mask);
    int error = pca9548a_wire->endTransmission();
    
    if (error == 0) {
        current_channel_mask = channel_mask;
        return true;
    } else {
        return false;
    }
}

uint8_t pca9548a_get_current_channels() {
    if (!pca9548a_ready || pca9548a_wire == nullptr) {
        return 0x00;
    }
    
    pca9548a_wire->requestFrom(PCA9548A_I2C_ADDR, 1);
    if (pca9548a_wire->available()) {
        current_channel_mask = pca9548a_wire->read();
    }
    
    return current_channel_mask;
}

bool pca9548a_is_channel_active(pca9548a_channel_t channel) {
    if (channel >= PCA9548A_MAX_CHANNELS) {
        return false;
    }
    
    uint8_t current_mask = pca9548a_get_current_channels();
    return (current_mask & (1 << channel)) != 0;
}

void pca9548a_scan_channel(pca9548a_channel_t channel, const char* bus_name) {
    if (!pca9548a_ready) {
        return;
    }
    
    if (!pca9548a_select_channel(channel)) {
        return;
    }
    
    delay(10); // Wait for channel switch to stabilize
    
    // int device_count = 0; // No longer used without prints
    for (int address = 1; address < 127; address++) {
        pca9548a_wire->beginTransmission(address);
        int error = pca9548a_wire->endTransmission();
        
        if (error == 0) {
           
        }
    }
}

void pca9548a_scan_all_channels() {

    
    for (int i = 0; i < PCA9548A_MAX_CHANNELS; i++) {
        pca9548a_scan_channel((pca9548a_channel_t)i, nullptr /* channel_names[i] */);
        delay(100); // Inter-channel delay
    }
    
    pca9548a_disable_all_channels();
}

void pca9548a_print_status() {
    
}
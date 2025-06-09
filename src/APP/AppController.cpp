// 文件: src/APP/AppController.cpp

#include "AppController.h"
#include "AppGlobal.h"
#include "AppTasks.h"
#include "../Config/Config.h"
#include "Display/DisplayDriver.h"
#include "Display/UI_Styles.h"
#include "Display/UI_MainScreen.h"
#include "Display/UI_ControlPage.h"
#include "../Display/UI_SystemSettingPage.h"
#include "Peripherals/AHT20_Sensor.h"
#include "Peripherals/DS3231_Clock.h"
#include "Peripherals/PCA9548A.h"
#include "Peripherals/OutputControls.h"
#include "Peripherals/FrameAnimation.h"
#include "SettingsLogic/FreshAirSettings.h"
#include "SettingsLogic/SystemSettings.h"
#include "SettingsLogic/LightingSettings.h"
#include "SettingsLogic/ParrotSettings.h"
#include "SettingsLogic/HatchingSettings.h"
#include "SettingsLogic/HumidifySettings.h"
#include "SettingsLogic/ThermalSettings.h"
#include "Control/ThermalControl.h"
#include <Arduino.h>
#include <Wire.h>
#include "driver/spi_master.h"
#include <SD.h>
#include <FS.h>
#include "../Config/LGFX_Config.h" // 引入LGFX配置

// 声明全局的LGFX对象
extern LGFX lcd;

static bool audio_board_comm_ok = false;
static bool main_sd_is_initialized_and_tested = false;

static bool checkAudioBoard_Safe();
static void initSensors_Safe();

static bool checkAudioBoard_Safe() {
    Serial.println("=== 检查音频板通信 ===");
#if ENABLE_AUDIO_BOARD
    Serial.printf("初始化 Serial1 (RX=%d, TX=%d)\n", AUDIO_BOARD_RX_PIN, AUDIO_BOARD_TX_PIN);
    AUDIO_BOARD_COMM_SERIAL.begin(AUDIO_BOARD_COMM_BAUD, SERIAL_8N1, AUDIO_BOARD_RX_PIN, AUDIO_BOARD_TX_PIN);
    delay(100);
    for (int attempt = 0; attempt < 2; attempt++) {
        Serial.printf("PING 尝试 %d/2\n", attempt + 1);
        AUDIO_BOARD_COMM_SERIAL.println("PING");
        AUDIO_BOARD_COMM_SERIAL.flush();
        unsigned long start = millis();
        while (millis() - start < 1000) {
            if (AUDIO_BOARD_COMM_SERIAL.available()) {
                String line = AUDIO_BOARD_COMM_SERIAL.readStringUntil('\n');
                line.trim();
                if (line.startsWith("ACK_PING:PONG")) {
                    Serial.println("✅ 音频板通信成功");
                    return true;
                }
            }
            delay(10);
            yield();
        }
        delay(100);
    }
    Serial.println("❌ 音频板通信失败");
    return false;
#else
    Serial.println("📝 音频板已禁用");
    return false;
#endif
}

static void initSensors_Safe() {
    Serial.println("=== 初始化传感器 ===");
#if ENABLE_SENSOR_MODULE
    Serial.printf("初始化 I2C1 (SDA=%d, SCL=%d)\n", I2C1_SDA_PIN, I2C1_SCL_PIN);
    Wire1.begin(I2C1_SDA_PIN, I2C1_SCL_PIN, 100000);
    Serial.println("✅ I2C1 总线已初始化 (用于传感器).");
    if (!pca9548a_init(&Wire1)) {
        Serial.println("⚠️ PCA9548A 初始化失败.");
        return;
    }
    Serial.println("✅ PCA9548A 初始化成功.");
    init_all_aht20_sensors(&Wire1, nullptr);
    init_ds3231();
#else
    Serial.println("📝 传感器模块已禁用.");
#endif
}


void AppController_Init(void) {
    Serial.println("\n========================================");
    Serial.println("=== AppController Initialization Started... ===");
    Serial.println("========================================");

    #if ENABLE_SD_CARD
    spi_bus_config_t sd_bus_config = {};
    sd_bus_config.mosi_io_num = MAIN_SD_MOSI_PIN;
    sd_bus_config.miso_io_num = MAIN_SD_MISO_PIN;
    sd_bus_config.sclk_io_num = MAIN_SD_SCLK_PIN;
    sd_bus_config.quadwp_io_num = -1;
    sd_bus_config.quadhd_io_num = -1;
    sd_bus_config.max_transfer_sz = 4092;
    esp_err_t ret = spi_bus_initialize(SPI3_HOST, &sd_bus_config, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        Serial.printf("❌ SD Card SPI Bus (SPI3_HOST) init failed: %s\n", esp_err_to_name(ret));
        main_sd_is_initialized_and_tested = false;
    } else {
        Serial.println("✅ SD Card SPI Bus (SPI3_HOST) initialized.");
        if (!SD.begin(MAIN_SD_CS_PIN, SPI3_HOST)) {
            Serial.println("❌ SD.begin() failed.");
            main_sd_is_initialized_and_tested = false;
        } else {
            main_sd_is_initialized_and_tested = (SD.cardType() != CARD_NONE);
            if(main_sd_is_initialized_and_tested) {
                Serial.printf("✅ SD Card mounted. Type: %d, Size: %llu MB\n", SD.cardType(), SD.cardSize() / (1024 * 1024));
            } else {
                 Serial.println("❌ SD Card found but type is unknown.");
            }
        }
    }
    #else
    main_sd_is_initialized_and_tested = false;
    #endif
    
    ::delay(200); yield();

    lv_init();
    display_init(); 
    ui_styles_init();
    
    if (main_sd_is_initialized_and_tested) {
        if (FrameAnimation_Init()) {
            FrameAnimation_PlayBootSequence(lcd); // <--- 修改: tft -> lcd
            FrameAnimation_DeInit();
        }
    } else {
        lcd.fillScreen(TFT_BLACK); // <--- 修改: tft -> lcd
    }
    ::delay(200); yield();

    initSensors_Safe();
    init_output_controls();
    audio_board_comm_ok = checkAudioBoard_Safe();

    SystemSettings_Init();
    LightingSettings_Init();
    
    if(audio_board_comm_ok) {
        ParrotSettings_Init();
    } else {
        Serial.println("Skipping ParrotSettings init due to communication failure.");
    }

    FreshAirSettings_Init();
    HatchingSettings_Init();
    HumidifySettings_Init();
    ThermalSettings_Init();
    AppTasks_Init();
    
    Serial.println("\n========================================");
    Serial.println("=== AppController Initialization Finished ===");
    Serial.println("========================================\n");
}

bool AppController_IsMainSDReady() {
    return main_sd_is_initialized_and_tested;
}


bool AppController_WriteSystemLog(const char* log_message) {
    if (!main_sd_is_initialized_and_tested || !log_message) return false;
    File logFile = SD.open("/logs/system.log", FILE_APPEND);
    if (!logFile) { return false; }
    char timestamp[32] = "----/--/-- --:--:--";
    if (is_ds3231_available() && pca9548a_select_channel((pca9548a_channel_t)PCA9548A_CHANNEL_DS3231_AHT20_1)) {
        ::delay(5);
        RtcDateTime now_rtc;
        if (get_current_datetime_rtc(&now_rtc)) {
            snprintf(timestamp, sizeof(timestamp), "%04u/%02u/%02u %02u:%02u:%02u",
                     now_rtc.Year(), now_rtc.Month(), now_rtc.Day(),
                     now_rtc.Hour(), now_rtc.Minute(), now_rtc.Second());
        }
    } else {
        snprintf(timestamp, sizeof(timestamp), "T+%lu", ::millis());
    }
    logFile.printf("[%s] %s\n", timestamp, log_message);
    logFile.close();
    return true;
}

bool AppController_WriteTemperatureLog(float temp1, int hum1, float temp2, int hum2) {
    if (!main_sd_is_initialized_and_tested) return false;
    File tempLog = SD.open("/logs/temperature.log", FILE_APPEND);
    if (!tempLog) { return false; }
    char timestamp[32] = "----/--/-- --:--:--";
    if (is_ds3231_available()) {
        RtcDateTime now_rtc;
        if (pca9548a_select_channel((pca9548a_channel_t)PCA9548A_CHANNEL_DS3231_AHT20_1)) {
            ::delay(5);
            if (get_current_datetime_rtc(&now_rtc)) {
                snprintf(timestamp, sizeof(timestamp), "%04u/%02u/%02u %02u:%02u:%02u",
                         now_rtc.Year(), now_rtc.Month(), now_rtc.Day(), now_rtc.Hour(), now_rtc.Minute(), now_rtc.Second());
            }
        }
    } else { snprintf(timestamp, sizeof(timestamp), "T+%lu", ::millis()); }
    tempLog.printf("[%s] T1:%.1f°C,%d%% T2:%.1f°C,%d%%\n", timestamp, temp1, hum1, temp2, hum2);
    tempLog.close();
    return true;
}

bool AppController_LoadConfig(const char* config_name, char* buffer, size_t buffer_size) {
    if (!main_sd_is_initialized_and_tested || !config_name || !buffer || buffer_size == 0) return false;
    char filepath[64]; snprintf(filepath, sizeof(filepath), "/config/%s", config_name);
    File configFile = SD.open(filepath, FILE_READ);
    if (!configFile) { return false; }
    size_t bytesRead = configFile.readBytes(buffer, buffer_size - 1);
    buffer[bytesRead] = '\0'; configFile.close();
    return true;
}

bool AppController_SaveConfig(const char* config_name, const char* config_data) {
    if (!main_sd_is_initialized_and_tested || !config_name || !config_data) return false;
    char filepath[64]; snprintf(filepath, sizeof(filepath), "/config/%s", config_name);
    File configFile = SD.open(filepath, FILE_WRITE);
    if (!configFile) { return false; }
    configFile.print(config_data);
    configFile.close();
    char log_msg[128]; snprintf(log_msg, sizeof(log_msg), "配置文件已保存: %s", config_name);
    AppController_WriteSystemLog(log_msg);
    return true;
}

bool AppController_CheckSDSpace(uint64_t* total_mb, uint64_t* used_mb, uint64_t* free_mb) {
    if (!main_sd_is_initialized_and_tested) return false;
    if (total_mb) *total_mb = SD.totalBytes() / (1024 * 1024);
    if (used_mb) *used_mb = SD.usedBytes() / (1024 * 1024);
    if (free_mb) *free_mb = (SD.totalBytes() - SD.usedBytes()) / (1024 * 1024);
    return true;
}

bool AppController_CleanupOldLogs() {
    if (!main_sd_is_initialized_and_tested) return false;
    File sysLog = SD.open("/logs/system.log", FILE_READ);
    if (sysLog) {
        size_t fileSize = sysLog.size(); sysLog.close();
        if (fileSize > 1024 * 1024) {
            Serial.println("系统日志文件过大，进行备份...");
            char backup_name[64]; snprintf(backup_name, sizeof(backup_name), "/logs/system_backup_%lu.log", ::millis());
            if (SD.rename("/logs/system.log", backup_name)) {
                Serial.printf("✅ 日志已备份为: %s\n", backup_name);
                AppController_WriteSystemLog("日志文件已备份并重新开始"); return true;
            } else { Serial.println("❌ 日志备份失败"); }
        }
    }
    File tempLog = SD.open("/logs/temperature.log", FILE_READ);
    if (tempLog) {
        size_t fileSize = tempLog.size(); tempLog.close();
        if (fileSize > 2 * 1024 * 1024) {
            Serial.println("温度日志文件过大，进行备份...");
            char backup_name[64]; snprintf(backup_name, sizeof(backup_name), "/logs/temperature_backup_%lu.log", ::millis());
            if (SD.rename("/logs/temperature.log", backup_name)) {
                Serial.printf("✅ 温度日志已备份为: %s\n", backup_name); return true;
            }
        }
    }
    return false;
}

void AppController_ListSDFiles(const char* dirname) {
    if (!main_sd_is_initialized_and_tested) { Serial.println("SD卡未就绪"); return; }
    File root = SD.open(dirname);
    if (!root) { Serial.printf("无法打开目录: %s\n", dirname); return; }
    if (!root.isDirectory()) { Serial.printf("%s 不是目录\n", dirname); root.close(); return; }
    Serial.printf("=== 目录内容: %s ===\n", dirname);
    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) { Serial.printf("  📁 %s/\n", file.name()); }
        else { Serial.printf("  📄 %s (%lu bytes)\n", file.name(), file.size()); }
        file = root.openNextFile();
    }
    root.close(); Serial.println("=== 目录列表结束 ===");
}
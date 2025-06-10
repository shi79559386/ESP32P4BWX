// 文件: src/APP/AppController.cpp (最终版 - 适配 AppGlobal)
#include "AppController.h"
#include "AppGlobal.h" // <--- 关键：包含全局变量头文件
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
#include "Peripherals/AudioPlayer.h"
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
#include "esp_task_wdt.h"
#include <SD_MMC.h>    // ← 用 SDMMC 驱动，全局取代 <SD.h>/<FS.h>
#include "LGFX_Config.h"



static bool main_sd_is_initialized_and_tested = false;
static bool audio_sd_is_initialized_and_tested = false;

SPIClass spi_audio(HSPI);


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

void AppController_Init() {
    Serial.println("=== AppController Init…");

    // 1. 全局 + LVGL init
    AppGlobal_Init();
    display_init();

    // 2. 板载 SDMMC 挂载（用于动画/日志/配置）
    #if ENABLE_SD_CARD
    // 挂载到 /sdcard
    if (SD_MMC.begin(BOOT_SD_MOUNT_POINT, false)) {
      main_sd_is_initialized_and_tested = true;
      Serial.printf("✅ SDMMC mounted at %s\n", BOOT_SD_MOUNT_POINT);
    } else {
      main_sd_is_initialized_and_tested = false;
      Serial.printf("❌ SDMMC mount failed at %s\n", BOOT_SD_MOUNT_POINT);
    }
      // ← 在这里加入目录遍历，确保卡里 video_frames 存在
    if (main_sd_is_initialized_and_tested) {
        Serial.println(">>> SD 根目录列表 <<<");
        File root = SD_MMC.open(BOOT_SD_MOUNT_POINT);
        if (root && root.isDirectory()) {
            File f = root.openNextFile();
            while (f) {
                Serial.printf("  %s%s\n", f.name(), f.isDirectory() ? "/" : "");
                f = root.openNextFile();
            }
            root.close();
        }
        Serial.println(">>> video_frames 子目录列表 <<<");
        File vf = SD_MMC.open(String(BOOT_SD_MOUNT_POINT) + "/video_frames");
        if (vf && vf.isDirectory()) {
            File ff = vf.openNextFile();
            while (ff) {
                Serial.println(String("  ") + ff.name());
                ff = vf.openNextFile();
            }
            vf.close();
        }
    }
  #else
    main_sd_is_initialized_and_tested = false;
  #endif
  



    // 4. 其他模块初始化
    initSensors_Safe();
    init_output_controls();
    SystemSettings_Init();
    LightingSettings_Init();



    // 只有外接 SD 准备好了才初始化音频系统
    if (audio_sd_is_initialized_and_tested) {
        Serial.println("Audio SD ready, initializing audio system...");
        AudioPlayer_Init();
        ParrotSettings_Init();
    } else {
        Serial.println("Audio SD not ready, skipping audio initialization.");
    }

    FreshAirSettings_Init();
    HatchingSettings_Init();
    HumidifySettings_Init();
    ThermalSettings_Init();
    ThermalControl_Init();
    AppTasks_Init();

    #if ENABLE_SD_CARD
    if (main_sd_is_initialized_and_tested) {
      Serial.println(">>> Before Boot Animation");
      FrameAnimation_Init();
      Serial.println(">>> After Init, about to call PlayBootSequence()");
      if (!FrameAnimation_PlayBootSequence(lcd)) {
        Serial.println("!!! PlayBootSequence returned false");
      }
      Serial.println(">>> After PlayBootSequence()");
      FrameAnimation_DeInit();
      Serial.println(">>> After DeInit()");
    }
  #endif
  


  // 4. 创建并加载 UI
  ui_styles_init();

    screen_main    = lv_obj_create(nullptr);
    screen_control = lv_obj_create(nullptr);
    screen_setting = lv_obj_create(nullptr);

    create_main_ui(screen_main);
    create_control_page_ui(screen_control);
    create_setting_page_ui(screen_setting);

    lv_disp_load_scr(screen_main);

    Serial.println("\n========================================");
    Serial.println("=== AppController Initialization Finished ===");
    Serial.println("========================================\n");
}



bool AppController_IsMainSDReady() {
    return main_sd_is_initialized_and_tested;
}


bool AppController_WriteSystemLog(const char* log_message) {
    if (!main_sd_is_initialized_and_tested || !log_message) return false;
    File logFile = SD_MMC.open("/logs/system.log", FILE_APPEND);
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
    File tempLog = SD_MMC.open("/logs/temperature.log", FILE_APPEND);
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
    File configFile = SD_MMC.open(filepath, FILE_READ);
    if (!configFile) { return false; }
    size_t bytesRead = configFile.readBytes(buffer, buffer_size - 1);
    buffer[bytesRead] = '\0'; configFile.close();
    return true;
}

bool AppController_SaveConfig(const char* config_name, const char* config_data) {
    if (!main_sd_is_initialized_and_tested || !config_name || !config_data) return false;
    char filepath[64]; snprintf(filepath, sizeof(filepath), "/config/%s", config_name);
    File configFile = SD_MMC.open(filepath, FILE_WRITE);
    if (!configFile) { return false; }
    configFile.print(config_data);
    configFile.close();
    char log_msg[128]; snprintf(log_msg, sizeof(log_msg), "配置文件已保存: %s", config_name);
    AppController_WriteSystemLog(log_msg);
    return true;
}

bool AppController_CheckSDSpace(uint64_t* total_mb, uint64_t* used_mb, uint64_t* free_mb) {
    if (!main_sd_is_initialized_and_tested) return false;
    if (total_mb) *total_mb = SD_MMC.totalBytes() / (1024 * 1024);
    if (used_mb) *used_mb = SD_MMC.usedBytes() / (1024 * 1024);
    if (free_mb) *free_mb = (SD_MMC.totalBytes() - SD_MMC.usedBytes()) / (1024 * 1024);
    return true;
}

bool AppController_CleanupOldLogs() {
    if (!main_sd_is_initialized_and_tested) return false;
    File sysLog = SD_MMC.open("/logs/system.log", FILE_READ);
    if (sysLog) {
        size_t fileSize = sysLog.size(); sysLog.close();
        if (fileSize > 1024 * 1024) {
            Serial.println("系统日志文件过大，进行备份...");
            char backup_name[64]; snprintf(backup_name, sizeof(backup_name), "/logs/system_backup_%lu.log", ::millis());
            if (SD_MMC.rename("/logs/system.log", backup_name)) {
                Serial.printf("✅ 日志已备份为: %s\n", backup_name);
                AppController_WriteSystemLog("日志文件已备份并重新开始"); return true;
            } else { Serial.println("❌ 日志备份失败"); }
        }
    }
    File tempLog = SD_MMC.open("/logs/temperature.log", FILE_READ);
    if (tempLog) {
        size_t fileSize = tempLog.size(); tempLog.close();
        if (fileSize > 2 * 1024 * 1024) {
            Serial.println("温度日志文件过大，进行备份...");
            char backup_name[64]; snprintf(backup_name, sizeof(backup_name), "/logs/temperature_backup_%lu.log", ::millis());
            if (SD_MMC.rename("/logs/temperature.log", backup_name)) {
                Serial.printf("✅ 温度日志已备份为: %s\n", backup_name); return true;
            }
        }
    }
    return false;
}

void AppController_ListSDFiles(const char* dirname) {
    if (!main_sd_is_initialized_and_tested) {
        Serial.println("SD卡未就绪");
        return;
    }
    File root = SD_MMC.open(dirname);
    if (!root) {
        Serial.printf("无法打开目录: %s\n", dirname);
        return;
    }
    if (!root.isDirectory()) {
        Serial.printf("%s 不是目录\n", dirname);
        root.close();
        return;
    }
    Serial.printf("=== 目录内容: %s ===\n", dirname);
    File file = root.openNextFile();        // 正确定义 file
    while (file) {
        if (file.isDirectory()) {
            Serial.printf("  📁 %s/\n", file.name());
        } else {
            Serial.printf("  📄 %s (%lu bytes)\n", file.name(), file.size());
        }
        file = root.openNextFile();         // 继续遍历
    }
    root.close();
    Serial.println("=== 目录列表结束 ===");
}

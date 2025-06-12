    #include "AppController.h"
    #include "AppGlobal.h"
    #include "AppTasks.h"
    #include "../Config/Config.h"
    #include "splash_screen/FrameAnimation.h"
    #include "Display/UI_Styles.h"
    #include "Display/UI_MainScreen.h"
    #include "Display/UI_ControlPage.h"
    #include "../Display/UI_SystemSettingPage.h"
    #include "Peripherals/AHT20_Sensor.h"
    #include "Peripherals/DS3231_Clock.h"
    #include "Peripherals/PCA9548A.h"
    #include "Peripherals/OutputControls.h"
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
    #include <SD_MMC.h>

    extern lv_obj_t *screen_main;
    extern lv_obj_t *screen_control;
    extern lv_obj_t *screen_setting;
    extern lv_disp_t* lvgl_disp;  // LVGL 默认屏幕指针
    static bool main_sd_is_initialized_and_tested = false;
    lv_disp_t* lvgl_disp = nullptr;  // 真正定义变量

    static void initSensors_Safe() {
    #if ENABLE_SENSOR_MODULE == 1
        Serial.println("=== Initializing Sensors ===");
        Wire1.begin(I2C1_SDA_PIN, I2C1_SCL_PIN, 100000);
        if (!pca9548a_init(&Wire1)) { Serial.println("⚠️ PCA9548A init failed."); return; }
        init_all_aht20_sensors(&Wire1, nullptr);
        init_ds3231();
    #else
        Serial.println("Sensors are disabled in Config.h.");
    #endif
    }

    void AppController_Init(LGFX* lcd) {
        Serial.println("=== AppController Init…");
    
        // 1. 屏幕初始化
        lcd->init();
        lcd->setSwapBytes(true);
        lcd->fillScreen(TFT_BLACK);
    
        // 全局变量等初始化
        AppGlobal_Init();
    
        #if ENABLE_SD_CARD == 1
        Serial.println("Attempting to mount on-board SDMMC...");
        if (SD_MMC.begin()) {
            main_sd_is_initialized_and_tested = true;
            Serial.println("✅ On-board SDMMC mounted successfully.");
    
            uint8_t cardType = SD_MMC.cardType();
            if (cardType == CARD_NONE) {
                Serial.println("⚠️ No SD Card attached");
                main_sd_is_initialized_and_tested = false;
            } else {
                Serial.print("SD Card Type: ");
                if (cardType == CARD_MMC)  Serial.println("MMC");
                else if (cardType == CARD_SD)   Serial.println("SDSC");
                else if (cardType == CARD_SDHC) Serial.println("SDHC");
                else                            Serial.println("UNKNOWN");
                Serial.printf("SD Card Size: %lluMB\n", SD_MMC.cardSize()/(1024*1024));
            }
    
            // —— 播放动画（条件） ——
            #if SKIP_BOOT_ANIMATION == 1
            // 暂停 LVGL 刷新
            auto disp = lv_disp_get_default();
            lv_disp_set_default(NULL);
    
            lcd->fillScreen(TFT_BLACK);
            Serial.println("▶ Playing boot animation…");
    
        File vFile = SD_MMC.open("/boot.mjpeg", FILE_READ);
        if (vFile && !vFile.isDirectory()) {
            // 分配 PSRAM 缓冲
            uint8_t* buf = (uint8_t*)heap_caps_malloc(MJPEG_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
            if (buf) {
                MjpegClass player;
                // multiTask=false 保证在主任务中解码
                player.setup(vFile, buf, lcd, /*multiTask=*/false);
                // 严格照例：同步循环读帧+解码+绘制
                while (player.readMjpegBuf()) {
                    player.drawJpg();
                    delay(40); // 每帧延时，可根据需要调整
                }
                heap_caps_free(buf);
            } else {
                Serial.println("Error: buf malloc failed");
            }
            vFile.close();
            Serial.println("▶ Boot animation finished");
        } else {
            Serial.println("Error: open /boot.mjpeg failed");
        }

        // 恢复 LVGL 刷新
        lv_disp_set_default(disp);
    
        } else {
            main_sd_is_initialized_and_tested = false;
            Serial.println("❌ On-board SDMMC mount failed.");
            lcd->fillScreen(TFT_RED);
            lcd->setCursor(10, 10);
            lcd->println("Error: SD_MMC mount failed!");
            delay(3000);
        }
    #else
        Serial.println("On-board SDMMC is disabled in Config.h. Skipping video.");
        main_sd_is_initialized_and_tested = false;
    #endif
    
    
        // 2. 其他模块初始化
        Serial.println("Initializing other modules...");
        AudioPlayer_Init();
        initSensors_Safe();
        init_output_controls();
        SystemSettings_Init();
        LightingSettings_Init();
        FreshAirSettings_Init();
        HatchingSettings_Init();
        HumidifySettings_Init();
        ThermalSettings_Init();
        ThermalControl_Init();
        AppTasks_Init();
    
        // 3. 构建 LVGL UI
        Serial.println("Creating LVGL UI...");
        lcd->fillScreen(TFT_BLACK);
        ui_styles_init();
        screen_main    = lv_obj_create(nullptr);
        screen_control = lv_obj_create(nullptr);
        screen_setting = lv_obj_create(nullptr);
        create_main_ui(screen_main);
        create_control_page_ui(screen_control);
        create_setting_page_ui(screen_setting);
        lv_disp_load_scr(screen_main);
    
        Serial.println("\n=== AppController Initialization Finished ===\n");
        
        lv_timer_handler();  // 💡 立即刷新画面
    }
    

    bool AppController_IsMainSDReady() { return main_sd_is_initialized_and_tested; }

    // 注意：所有文件操作函数都应检查 main_sd_is_initialized_and_tested
    // 并且在 File 类型前加上 fs:: 避免歧义

    bool AppController_WriteSystemLog(const char* log_message) {
        if (!main_sd_is_initialized_and_tested || !log_message) return false;
        fs::File logFile = SD_MMC.open("/logs/system.log", FILE_APPEND);
        if (!logFile) return false;
        // ... (获取时间戳的逻辑保持不变) ...
        logFile.printf("[%s] %s\n", "timestamp_placeholder", log_message);
        logFile.close();
        return true;
    }

    bool AppController_LoadConfig(const char* config_name, char* buffer, size_t buffer_size) {
        if (!main_sd_is_initialized_and_tested || !config_name || !buffer) return false;
        char filepath[64];
        snprintf(filepath, sizeof(filepath), "/config/%s", config_name);
        fs::File configFile = SD_MMC.open(filepath, FILE_READ);
        if (!configFile) return false;
        size_t bytesRead = configFile.readBytes(buffer, buffer_size - 1);
        buffer[bytesRead] = '\0';
        configFile.close();
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

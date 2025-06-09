#include "UI_FeatureSettings.h"

// 包含所有具体弹窗模块的头文件
#include "Popups/UI_LightingSettingsPopup.h"
#include "Popups/UI_ParrotSettingsPopup.h"
#include "Popups/UI_DryingSettingsPopup.h"         // 确保此文件和函数已创建
#include "Popups/UI_FreshAirSettingsPopup.h"       // 确保此文件和函数已创建
#include "Popups/UI_HatchingSettingsPopup.h"       // 确保此文件和函数已创建
#include "Popups/UI_SterilizationSettingsPopup.h"  // 新增的杀菌弹窗
#include "Popups/UI_ThermalSettingsPopup.h"    // 新增
#include "Popups/UI_HumidifySettingsPopup.h"   // 新增

void show_feature_settings_popup(feature_id_t feature_id, lv_obj_t *parent_scr) {
    switch (feature_id) {
        case FEATURE_ID_LIGHTING:
            create_lighting_settings_popup(parent_scr);
            break;
        case FEATURE_ID_PARROT:
            create_parrot_settings_popup(parent_scr);
            break;
        case FEATURE_ID_DRYING:
            create_drying_settings_popup(parent_scr); // 需要您实现此函数
            break;
        case FEATURE_ID_FRESH_AIR:
            create_fresh_air_settings_popup(parent_scr); // 需要您实现此函数
            break;
        case FEATURE_ID_HATCHING:
            create_hatching_settings_popup(parent_scr); // 需要您实现此函数
            break;
        case FEATURE_ID_STERILIZATION:
            create_sterilization_settings_popup(parent_scr);
        case FEATURE_ID_THERMAL:               // 新增
            create_thermal_settings_popup(parent_scr);
            break;
        case FEATURE_ID_HUMIDIFY:              // 新增
            create_humidify_settings_popup(parent_scr);
            break;
        default:
            break;
    }
}
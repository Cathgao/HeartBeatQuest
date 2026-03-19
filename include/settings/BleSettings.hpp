#pragma once
#include "settings/Settings.hpp"
#include "i18n.hpp"

namespace HeartBeat {
    
    class BleSettings : public Settings {
        BSML::CustomListTableData *ble_list = nullptr;
        std::vector<std::string> ble_mac;

        void UpdateSelectedBLEScrollList();
        void UpdateSelectedBLEValue(int idx);

    public:
        BleSettings():Settings("Bluetooth Device", LANG->heart_devices, "<3") { }
        void CreateElements() override;
    };

    
}
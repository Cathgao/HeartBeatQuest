#pragma once
#include "HMUI/CurvedTextMeshPro.hpp"
#include "data_sources/Bluetooth.hpp"
#include "settings/Settings.hpp"
#include "i18n.hpp"

namespace HeartBeat {
    
    class BleSettings : public Settings {
        BSML::CustomListTableData *ble_list = nullptr;
        std::vector<std::string> ble_mac;

        HeartBeat::HeartBeatBleDataSource * bleDataSource = nullptr;
        HMUI::CurvedTextMeshPro * scanStatusText;
        void UpdateSelectedBLEScrollList();
        void UpdateSelectedBLEValue(int idx);

    public:
        BleSettings():Settings("Bluetooth Device", LANG->heart_devices, "<3") { }
        void CreateElements() override;

        void Open() override;
        void Close() override;
        void Update() override;
    };

    
}
#pragma once
#include "HMUI/CurvedTextMeshPro.hpp"
#include "settings/Settings.hpp"
#include "SSL10nGenerated.hpp"

namespace HeartBeat {

class HeartBeatBleDataSource;

class BleSettings : public Settings {
    BSML::CustomListTableData *ble_list = nullptr;

    HeartBeat::HeartBeatBleDataSource *bleDataSource = nullptr;
    HMUI::CurvedTextMeshPro *scanStatusText;
    void UpdateSelectedBLEScrollList();
    void UpdateSelectedBLEValue(int idx);

  public:
    BleSettings() : Settings("Bluetooth Device", SSL10nGen::STR::heart_devices(), "<3") {}
    void CreateElements() override;

    void Open() override;
    void Close() override;
    void Update() override;
};
} // namespace HeartBeat

// clang-format off

DECLARE_CLASS_CODEGEN(
    HeartBeat, BluetoothDeviceItem, BSML::CustomCellInfo
#if defined(GAME_VER_1_28_0) || defined(GAME_VER_1_35_0) || defined(GAME_VER_1_37_0)
    ,
#else
) {
#endif
    DECLARE_DEFAULT_CTOR();
    DECLARE_SIMPLE_DTOR();
public:
    bool dirty = true;
    bool selected = false;
    std::string devName, devMac;
    bool isNone = false;
    bool m_private_ui;

    bool Update(std::string devName, std::string devMac, bool selected) {
        if (isNone) {
            if (this->selected != selected || dirty) {
                this->selected = selected;
                this->text = selected ? SSL10nGen::STR::ble_none_selected() : SSL10nGen::STR::ble_none_not_selected();
                dirty = false;
                return true;
            } else {
                return false;
            }
        }
        if (!dirty && this->selected == selected && this->devName == devName && this->devMac == devMac &&
            this->m_private_ui == private_ui)
            return false;
        dirty = false;
        this->selected = selected;
        this->devName = devName;
        this->devMac = devMac;
        this->text =
            std::string(selected ? ">> " : "") + devName + "(" + (private_ui ? "XX-XX-XX-XX-XX-XX" : devMac) + ")";
        return true;
    }

    static BluetoothDeviceItem* construct();
#if defined(GAME_VER_1_28_0) || defined(GAME_VER_1_35_0) || defined(GAME_VER_1_37_0)
);
#else
};
#endif

// clang-format on

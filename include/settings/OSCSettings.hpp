#pragma once
#include "HMUI/CurvedTextMeshPro.hpp"
#include "settings/Settings.hpp"
#include "i18n.hpp"

namespace HeartBeat {
class HeartBeatOSCDataSource;
class OSCSettings : public Settings {
    BSML::CustomListTableData *osc_list = nullptr;
    HeartBeatOSCDataSource *oscDataSource;

    void UpdateOscScrollList();
    void UpdateSelectedOscValue(int idx);

  public:
    OSCSettings() : Settings("OSC Source", LANG::heart_osc_senders(), "<3") {}
    void CreateElements() override;
    void Update() override;

    HMUI::CurvedTextMeshPro *mDnsNameText = nullptr;
};

} // namespace HeartBeat

// clang-format off

DECLARE_CLASS_CODEGEN(
    HeartBeat, OSCDeviceItem, BSML::CustomCellInfo
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
    std::string devAddress;
    bool isNone = false;

    bool Update(std::string devAddress, bool selected) {
        if (isNone) {
            if (this->selected != selected || dirty) {
                this->selected = selected;
                this->text = selected ? LANG::ble_none_selected() : LANG::ble_none_not_selected();
                dirty = false;
                return true;
            } else {
                return false;
            }
        }
        if (!dirty && this->selected == selected && this->devAddress == devAddress)
            return false;
        dirty = false;
        this->selected = selected;
        this->devAddress = devAddress;
        this->text =
            std::string(selected ? ">> " : "") + devAddress;
        return true;
    }

    static OSCDeviceItem* construct();
#if defined(GAME_VER_1_28_0) || defined(GAME_VER_1_35_0) || defined(GAME_VER_1_37_0)
);
#else
};
#endif

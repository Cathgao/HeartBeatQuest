#include "UnityEngine/Vector2.hpp"
#include "bsml/shared/BSML-Lite/Creation/Layout.hpp"
#include "bsml/shared/BSML-Lite/Creation/Text.hpp"
#include "data_sources/Bluetooth.hpp"
#include "settings/BleSettings.hpp"
#include "settings/Settings.hpp"

void HeartBeat::BleSettings::CreateElements(){
    auto *container = BSML::Lite::CreateVerticalLayoutGroup(controller->get_transform());
    bleDataSource=HeartBeat::DataSource::getInstance()->as<HeartBeat::HeartBeatBleDataSource>(); 

    auto *scanBtnContainer = BSML::Lite::CreateHorizontalLayoutGroup(container->get_transform());

    BSML::Lite::CreateUIButton(scanBtnContainer->get_transform(), LANG->scan_start, UnityEngine::Vector2{}, UnityEngine::Vector2{25, 8}, [this](){
        bleDataSource->StartScan();
    });
    BSML::Lite::CreateUIButton(scanBtnContainer->get_transform(), LANG->scan_stop, UnityEngine::Vector2{}, UnityEngine::Vector2{25, 8}, [this](){
        bleDataSource->StopScan();
    });

    BSML::Lite::CreateUIButton(container->get_transform(), LANG->turn_location_on, UnityEngine::Vector2{}, UnityEngine::Vector2{50, 8}, [this](){
        OpenWebpage("https://www.meta.com/help/quest/1202271140482151/");
    });

    scanStatusText = BSML::Lite::CreateText(container->get_transform(), LANG->no_scan, UnityEngine::Vector2{}, UnityEngine::Vector2{50, 8});

    ble_list = BSML::Lite::CreateScrollableList(container->get_transform(), {70,60}, [this](int idx){
        UpdateSelectedBLEValue(idx);
    });
    ble_list->set_listStyle(BSML::CustomListTableData::ListStyle::Simple);
    ble_list->tableView->set_selectionType(HMUI::TableViewSelectionType::Single);
    ble_mac.push_back("");
    UpdateSelectedBLEScrollList();
}

void HeartBeat::BleSettings::Open(){
    // bleDataSource->StartScan();
}
void HeartBeat::BleSettings::Close(){
    bleDataSource->StopScan();
}
void HeartBeat::BleSettings::Update(){
    static int slow_down = 0;
    if(slow_down++ % 5 == 0){
        UpdateSelectedBLEScrollList();
    }
    scanStatusText->set_text(bleDataSource->isAutoConnecting()? LANG->auto_scaning : bleDataSource->isScanning() ? LANG->scaning : LANG->no_scan);
}
void HeartBeat::BleSettings::UpdateSelectedBLEScrollList(){
    auto * i = bleDataSource;
    bool any_data_changed = false;
    int the_selected = -1;
    {
        std::set<std::string> already_in(ble_mac.begin(), ble_mac.end());
        auto& devs = i->avaliable_devices;

        for(auto it = devs.begin(), end = devs.end(); it != end; ++it){
            if(already_in.count(it->first))
                continue;
            ble_mac.push_back(it->first);
            already_in.insert(it->first);
        }

        while(ble_list->data.size() > ble_mac.size()){
            ble_list->data->RemoveAt(ble_list->data.size() - 1);
            any_data_changed = true;
        }
        while(ble_list->data.size() < ble_mac.size()){
            ble_list->data->Add(BSML::CustomCellInfo::construct(""));
            any_data_changed = true;
        }

        for(int j=0;j<ble_mac.size();j++){
            bool selected = (ble_mac[j] == i->GetSelectedBleMac());
            std::string name;

            if(ble_mac[j] == ""){
                name = selected ? ">>None" : "  None";
            }else{
                auto it = devs.find(ble_mac[j]);
                if(it != devs.end()){
                    name = std::string(selected ? ">>" : "  ") + (it->second.name) + "(" + 
                        (private_ui ? "XX-XX-XX-XX-XX-XX" : ble_mac[j])
                        + ")";
                }else{
                    name = std::string(selected ? ">>" : "  ") + (LANG->unknown_left_quote) + ble_mac[j] + ")";
                }
            }
            if(ble_list->data[j]->text != name){
                ble_list->data[j]->text = name;
                any_data_changed = true;
            }
            if(selected){
                the_selected = j;
            }
        }
    }
    if(any_data_changed)
        ble_list->tableView->ReloadData();
    if(the_selected >= 0){
        ble_list->tableView->SelectCellWithIdx(the_selected, false);
    }
}
void HeartBeat::BleSettings::UpdateSelectedBLEValue(int idx){
    bleDataSource->SetSelectedBleMac(ble_mac[idx]);
    UpdateSelectedBLEScrollList();
}

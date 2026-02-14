#include "data_sources/OSC.hpp"
#include "settings/OSCSettings.hpp"
void HeartBeat::OSCSettings::CreateElements(){
    // Create a container that has a scroll bar
    auto *container = BSML::Lite::CreateScrollableSettingsContainer(controller->get_transform());

    osc_list = BSML::Lite::CreateScrollableList(container->get_transform(), {70,60}, [this](int idx){
        UpdateSelectedOscValue(idx);
    });
    osc_list->set_listStyle(BSML::CustomListTableData::ListStyle::Simple);
    osc_list->tableView->set_selectionType(HMUI::TableViewSelectionType::Single);
    osc_addr.push_back("None");
    UpdateOscScrollList();

}
void HeartBeat::OSCSettings::UpdateSelectedOscValue(int idx){
    HeartBeat::DataSource::getInstance()->as<HeartBeat::HeartBeatOSCDataSource>()->SetSelectedAddr(osc_addr[idx]);
    UpdateOscScrollList();
}
void HeartBeat::OSCSettings::UpdateOscScrollList(){
    auto * i = HeartBeat::DataSource::getInstance()->as<HeartBeat::HeartBeatOSCDataSource>();
    bool any_data_changed = false;
    int the_selected = -1;
    {
        std::set<std::string> already_in(osc_addr.begin(), osc_addr.end());
        auto& devs = i->received_addresses;

        for(auto it = devs.begin(), end = devs.end(); it != end; ++it){
            if(already_in.count(*it))
                continue;
            osc_addr.push_back(*it);
            already_in.insert(*it);
        }

        while(osc_list->data.size() > osc_addr.size()){
            osc_list->data->RemoveAt(osc_list->data.size() - 1);
            any_data_changed = true;
        }
        while(osc_list->data.size() < osc_addr.size()){
            osc_list->data->Add(BSML::CustomCellInfo::construct(""));
            any_data_changed = true;
        }

        for(int j=0;j<osc_addr.size();j++){
            bool selected = (osc_addr[j] == i->GetSelectedAddress());
            std::string name;

            name = std::string(selected ? ">>" : "  ") + osc_addr[j];
            if(osc_list->data[j]->text != name){
                osc_list->data[j]->text = name;
                any_data_changed = true;
            }
            if(selected){
                the_selected = j;
            }
        }
    }
    if(any_data_changed)
        osc_list->tableView->ReloadData();
    if(the_selected >= 0){
        osc_list->tableView->SelectCellWithIdx(the_selected, false);
    }
}

void HeartBeat::OSCSettings::Update(){
    UpdateOscScrollList();
}
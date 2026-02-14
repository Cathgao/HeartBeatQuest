#pragma once

#include "DataSource.hpp"
#include <string>
#include <map>

namespace HeartBeat{

struct HeartBeatBleDevice {
    std::string name;
    std::string mac;
    int last_data;
    int last_data_time;
};

class HeartBeatBleDataSource:public DataSource{
private:
    std::string selected_mac = "";
    bool has_new_data;
    int heartbeat = 0;
    std::atomic_llong energy = 0;
    std::atomic_llong persistent_energy = 0;
public:
    HeartBeatBleDataSource();
    bool GetData(int& heartbeat) override;
    long long GetEnergy() override;

    void ScanDevice();

    std::string& GetSelectedBleMac(){ return selected_mac; }
    void SetSelectedBleMac(const std::string mac);

    std::map<std::string/*mac*/, HeartBeatBleDevice> avaliable_devices;

    //called from java
    void InformNativeDevice(const std::string& macAddr, const std::string& name);
    void OnDataCome(const std::string& macAddr, int heartRate, long energy);
    void OnEnergyReset();
};


}
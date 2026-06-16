#pragma once
#include "UnityEngine/GameObject.hpp"
#include "settings/Settings.hpp"
#include "SSL10nGenerated.hpp"

namespace HeartBeat {

class MainSettings : public Settings {
  public:
    MainSettings() : Settings("HeartBeatQuest Main Config", SSL10nGen::STR::heart_config(), "<3") {}
    void CreateElements() override;

    UnityEngine::UI::Button *private_public_btn = nullptr;

    void UpdateContent();
};

} // namespace HeartBeat
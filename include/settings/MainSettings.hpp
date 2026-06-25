#pragma once
#include "UnityEngine/GameObject.hpp"
#include "settings/Settings.hpp"
#include "SSL10nGenerated.hpp"

namespace HeartBeat {

class MainSettings : public Settings {
  public:
    MainSettings()
        : Settings("HEART_BEAT_QUEST_SETTHINGS_MAIN_TITLE", "HEART_BEAT_QUEST_SETTHINGS_MAIN_BUTTON", "<3") {}
    void CreateElements() override;

    UnityEngine::UI::Button *private_public_btn = nullptr;

    void UpdateContent();
};

} // namespace HeartBeat
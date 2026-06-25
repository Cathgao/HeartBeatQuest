#pragma once
#include "HMUI/CurvedTextMeshPro.hpp"
#include "settings/Settings.hpp"
#include "SSL10nGenerated.hpp"

namespace HeartBeat {

class HypeRateSettings : public Settings {
  public:
    HypeRateSettings()
        : Settings("HEART_BEAT_QUEST_SETTHINGS_HYPERATE_TITLE", "HEART_BEAT_QUEST_SETTHINGS_HYPERATE_BUTTON", "<3") {}
    void CreateElements() override;

    std::string hyperate_id;

    void Update() override;

  private:
    std::vector<UnityEngine::UI::Button *> buttons;
    void disableBtns();
    void enableBtns();

    HMUI::CurvedTextMeshPro *statusText;
    HMUI::CurvedTextMeshPro *serverMessageText;
};

} // namespace HeartBeat
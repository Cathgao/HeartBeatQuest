#pragma once
#include "settings/Settings.hpp"
#include "i18n.hpp"

namespace HeartBeat {
    
    class PulsoidSettings : public Settings {
        HMUI::CurvedTextMeshPro* tokenText;
        bool tokenTextIsDirty = false;

        UnityEngine::UI::Button *PairInBrowserBtn, *BrowserCompleteBtn, *CancelBrowserPairBtn;

        HMUI::CurvedTextMeshPro* errMsgText;


    public:
        PulsoidSettings():Settings("Pulsoid Connect", LANG->pulsoid, "<3") { }
        void CreateElements() override;
        void Update() override;
        void Close() override;
    };

    
}
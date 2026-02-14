#pragma once

#include "UnityEngine/GameObject.hpp"
#include "HeartBeat.hpp"
namespace HeartBeat {
    struct MainMenuPreviewer{
        MainMenuPreviewer();

        UnityEngine::GameObject* MainMenuPreviewObject;
        HeartBeat::HeartBeatObj *MainMenuPreviewObjectComp;

        static MainMenuPreviewer* getInstance();  

        void Show();
        void Hide();
        void Reload();

        HMUI::CurvedTextMeshPro * serverMessageDisplayer = nullptr;
    };

}
#include "ModConfig.hpp"
#include "settings/HypeRateSettings.hpp"
#include "data_sources/Hyperate.hpp"

    namespace HeartBeat{

        void HypeRateSettings::CreateElements(){
            auto *container = BSML::Lite::CreateScrollableSettingsContainer(controller->get_transform());

            hyperate_id = getModConfig().HypeRateId.GetValue();
            BSML::Lite::CreateText(container->get_transform(), LANG->hyperate_input_hint, 4, UnityEngine::Vector2{}, UnityEngine::Vector2{50, 4});
            // BSML::Lite::CreateText(container->get_transform(), LANG->hyperate_input_hint2, 4, UnityEngine::Vector2{}, UnityEngine::Vector2{50, 4});


            static HMUI::InputFieldView * hyperate_id_input;
            hyperate_id_input = BSML::Lite::CreateStringSetting(container->get_transform(), "HypeRate ID", hyperate_id, [this](StringW v){
                hyperate_id = std::string(v);
            });
            BSML::Lite::CreateUIButton(container->get_transform(), LANG->hyperate_reset, UnityEngine::Vector2{}, UnityEngine::Vector2{50, 8}, [this](){
                {
                    hyperate_id = getModConfig().HypeRateId.GetValue();
                    hyperate_id_input->set_text(hyperate_id.c_str());
                }
                hyperate_id_input->set_text(hyperate_id);
            });
            BSML::Lite::CreateUIButton(container->get_transform(), LANG->hyperate_save_and_connect, UnityEngine::Vector2{}, UnityEngine::Vector2{50, 8}, [this](){
                getModConfig().HypeRateId.SetValue(hyperate_id);
                HeartBeat::DataSource::getInstance()->as<HeartBeat::HeartBeatHypeRateDataSource>()->ResetConnection();
            });

            MainMenuPreviewer::getInstance()->serverMessageDisplayer = BSML::Lite::CreateText(container->get_transform(), LANG->no_message_from_server, 4, UnityEngine::Vector2{}, UnityEngine::Vector2{100, 32});
        }
    }

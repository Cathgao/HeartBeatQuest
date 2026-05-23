#ifdef WITH_QOUNTERS

#include "UnityEngine/UI/Graphic.hpp"
#include "BeatLeaderRecorder.hpp"
#include "config-utils/shared/config-utils.hpp"
#include "qppopt/shared/sources.hpp"
#include "qppopt/shared/types.hpp"
#include <cstddef>

#include "QountersDriver.hpp"
#include "ModConfig.hpp"
#include "UnityEngine/Color.hpp"
#include "UnityEngine/GameObject.hpp"
#include "bsml/shared/BSML-Lite/Creation/Misc.hpp"
#include "bsml/shared/BSML-Lite/Creation/Settings.hpp"
#include "bsml/shared/BSML/Components/Settings/ColorSetting.hpp"
#include "main.hpp"
#include "rapidjson-macros/shared/macros.hpp"
#include "UIManager.hpp"
#include "DataHub.hpp"
#include <dlfcn.h>
#include "metacore/shared/events.hpp"
#include "qppopt/shared/api.hpp"
#include "qppopt/shared/events.hpp"
#include <vector>
#include "HeartBeat.hpp"

DECLARE_CLASS_CODEGEN(HeartBeat, HeartBeatQountersDriver, UnityEngine::MonoBehaviour) {
    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
    DECLARE_INSTANCE_METHOD(void, Update);

  private:
    bool isAddedToUIManager = false;
};
DEFINE_TYPE(HeartBeat, HeartBeatQountersDriver);

void HeartBeat::Qounters::CreateDriverObject() {
    UnityEngine::GameObject::New_ctor()->AddComponent<HeartBeat::HeartBeatQountersDriver *>();
}

void HeartBeat::HeartBeatQountersDriver::Start() {
    if (isAddedToUIManager)
        return;
    isAddedToUIManager = true;
    UIManager::getInstance()->addReader();
}
void HeartBeat::HeartBeatQountersDriver::Update() {
    DataHub::getInstance()->Update();
    int data;
    if (DataHub::getInstance()->GetData(data))
        HeartBeat::Qounters::DisplayData(data);
    // there is no asset bundle UI for qounters, just return.
}

void HeartBeat::HeartBeatQountersDriver::OnDestroy() {
    if (isAddedToUIManager) {
        isAddedToUIManager = false;
        UIManager::getInstance()->decReader();
    }
}

namespace HeartBeat {
namespace Qounters {

#define METACORE_EVENT_MOD "HeartBeatQuest"
#define METACORE_EVENT_ID_HEART_RATE 1
#define METACORE_EVENT_ID_REPLAY_STATUS 2

static int qounters_hr = 0;

DECLARE_JSON_STRUCT(HeartRateTextOption) {
    VALUE_DEFAULT(bool, heartBeforeText, false);
    VALUE_DEFAULT(bool, heartAfterText, false);
};

UnityEngine::UI::Graphic *hrBundledUIPremade(UnityEngine::GameObject *parent, UnparsedJSON) {
    auto ret = UnityEngine::GameObject::New_ctor()->AddComponent<UnityEngine::UI::Graphic *>();
    ret->get_transform()->set_parent(parent->get_transform());

    // create the UI selected by player from HeartBeatQuest menu
    std::string SelectedUI = getModConfig().SelectedUI.GetValue();
    if (!HeartBeat::assetBundleMgr.loadedBundles.contains(SelectedUI))
        SelectedUI = "Default";
    if (!HeartBeat::assetBundleMgr.loadedBundles.contains(SelectedUI)) {
        getLogger().error("Can't find ui asset bundle '{}' to load!", SelectedUI);
        return ret;
    }

    HeartBeat::AssetBundleInstinateInformation result;
    if (!HeartBeat::assetBundleMgr.Instantiate(SelectedUI, parent->get_transform(), result)) {
        getLogger().error("The UI Can't loaded.");
        return ret;
    }
    auto comp = result.gameObject->AddComponent<HeartBeat::HeartBeatObj *>();
    comp->loadedComponents = result;

    // the created UI root is result.gameObject
    result.gameObject->get_transform()->set_parent(ret->get_transform());
    getLogger().info("loaded bundle for qounters premade");
    return ret;
}

std::string hrTextSource(UnparsedJSON unparsed) {
    static HeartRateTextOption opts;
    opts = unparsed.Parse<HeartRateTextOption>();

    char buff[128];
    sprintf(buff,
            opts.heartBeforeText  ? opts.heartAfterText ? "♥ %d♥" : "♥ %d"
            : opts.heartAfterText ? "%d♥"
                                  : "%d",
            qounters_hr);
    return buff;
}

void hrTextSourceUI(UnityEngine::GameObject *parent, UnparsedJSON unparsed) {
    static HeartRateTextOption opts;
    opts = unparsed.Parse<HeartRateTextOption>();
    BSML::Lite::CreateToggle(parent, "Heart Icon Before Text", opts.heartBeforeText, [](bool v) {
        static int id = ::Qounters::API::GetActionId();
        opts.heartBeforeText = v;
        ::Qounters::API::SetSourceOptions(id, opts);
        ::Qounters::API::FinalizeAction();
    });
    BSML::Lite::CreateToggle(parent, "Heart Icon After Text", opts.heartAfterText, [](bool v) {
        static int id = ::Qounters::API::GetActionId();
        opts.heartAfterText = v;
        ::Qounters::API::SetSourceOptions(id, opts);
        ::Qounters::API::FinalizeAction();
    });
}

DECLARE_JSON_STRUCT(HeartRatePercentOption) { VALUE_DEFAULT(bool, alignedTo5Range, true); };

float hrPercentSource(UnparsedJSON unparsed) {
    static HeartRatePercentOption opts;
    opts = unparsed.Parse<HeartRatePercentOption>();

    int Maximum = getModConfig().MaxHeart.GetValue();
    float percent = ((float)qounters_hr) / Maximum;

    if (opts.alignedTo5Range) {
        /*
        50% - 60% : 1
        60% - 70% : 2
        70% - 80% : 3
        80% - 90% : 4
        90% - 100%: 5
        */
        int level = percent * 10 - 4;
        if (level < 0)
            level = 0;
        if (level > 5)
            level = 5;

        percent = ((float)level) / 5;
    }

    return percent;
}

void hrPercentSourceUI(UnityEngine::GameObject *parent, UnparsedJSON unparsed) {
    static HeartRatePercentOption opts;
    opts = unparsed.Parse<HeartRatePercentOption>();
    BSML::Lite::CreateToggle(parent, "Align to 5 level range", opts.alignedTo5Range, [](bool v) {
        static int id = ::Qounters::API::GetActionId();
        opts.alignedTo5Range = v;
        ::Qounters::API::SetSourceOptions(id, opts);
        ::Qounters::API::FinalizeAction();
    });
}

DECLARE_JSON_STRUCT(HeartRateRangeEnableOption) {
    VALUE_DEFAULT(bool, range__5, false);
    VALUE_DEFAULT(bool, range_5_6, false);
    VALUE_DEFAULT(bool, range_6_7, false);
    VALUE_DEFAULT(bool, range_7_8, false);
    VALUE_DEFAULT(bool, range_8_9, false);
    VALUE_DEFAULT(bool, range_9_, false);
};

bool hrRangeEnable(UnparsedJSON unparsed) {
    static HeartRateRangeEnableOption opts;
    opts = unparsed.Parse<HeartRateRangeEnableOption>();

    int Maximum = getModConfig().MaxHeart.GetValue();
    float percent = ((float)qounters_hr) / Maximum;
    if (percent < 0.5)
        return opts.range__5;
    if (percent < 0.6)
        return opts.range_5_6;
    if (percent < 0.7)
        return opts.range_6_7;
    if (percent < 0.8)
        return opts.range_7_8;
    if (percent < 0.9)
        return opts.range_8_9;
    return opts.range_9_;
}
void hrRangeEnableUI(UnityEngine::GameObject *parent, UnparsedJSON unparsed) {
    static HeartRateRangeEnableOption opts;
    opts = unparsed.Parse<HeartRateRangeEnableOption>();
    BSML::Lite::CreateToggle(parent, "Enable if range lower than 50%", opts.range__5, [](bool v) {
        static int id = ::Qounters::API::GetActionId();
        opts.range__5 = v;
        ::Qounters::API::SetEnableOptions(id, opts);
        ::Qounters::API::FinalizeAction();
    });
    BSML::Lite::CreateToggle(parent, "Enable if range in 50% - 60%", opts.range_5_6, [](bool v) {
        static int id = ::Qounters::API::GetActionId();
        opts.range_5_6 = v;
        ::Qounters::API::SetEnableOptions(id, opts);
        ::Qounters::API::FinalizeAction();
    });
    BSML::Lite::CreateToggle(parent, "Enable if range in 60% - 70%", opts.range_6_7, [](bool v) {
        static int id = ::Qounters::API::GetActionId();
        opts.range_6_7 = v;
        ::Qounters::API::SetEnableOptions(id, opts);
        ::Qounters::API::FinalizeAction();
    });
    BSML::Lite::CreateToggle(parent, "Enable if range in 70% - 80%", opts.range_7_8, [](bool v) {
        static int id = ::Qounters::API::GetActionId();
        opts.range_7_8 = v;
        ::Qounters::API::SetEnableOptions(id, opts);
        ::Qounters::API::FinalizeAction();
    });
    BSML::Lite::CreateToggle(parent, "Enable if range in 80% - 90%", opts.range_8_9, [](bool v) {
        static int id = ::Qounters::API::GetActionId();
        opts.range_8_9 = v;
        ::Qounters::API::SetEnableOptions(id, opts);
        ::Qounters::API::FinalizeAction();
    });
    BSML::Lite::CreateToggle(parent, "Enable if range larger than 90%", opts.range_9_, [](bool v) {
        static int id = ::Qounters::API::GetActionId();
        opts.range_9_ = v;
        ::Qounters::API::SetEnableOptions(id, opts);
        ::Qounters::API::FinalizeAction();
    });
}

DECLARE_JSON_STRUCT(HeartRateRangeColorOption) {
    VALUE_DEFAULT(ConfigUtils::Color, range__5, ConfigUtils::Color(1, 1, 1, 1));
    VALUE_DEFAULT(ConfigUtils::Color, range_5_6, ConfigUtils::Color(1, 1, 1, 1));
    VALUE_DEFAULT(ConfigUtils::Color, range_6_7, ConfigUtils::Color(1, 1, 1, 1));
    VALUE_DEFAULT(ConfigUtils::Color, range_7_8, ConfigUtils::Color(1, 1, 1, 1));
    VALUE_DEFAULT(ConfigUtils::Color, range_8_9, ConfigUtils::Color(1, 1, 1, 1));
    VALUE_DEFAULT(ConfigUtils::Color, range_9_, ConfigUtils::Color(1, 1, 1, 1));

    VALUE_DEFAULT(bool, use_replay_color, false);
    VALUE_DEFAULT(ConfigUtils::Color, range_replay, ConfigUtils::Color(1, 1, 1, 1));
};

UnityEngine::Color hrRangeColor(UnparsedJSON unparsed) {
    static HeartRateRangeColorOption opts;
    opts = unparsed.Parse<HeartRateRangeColorOption>();

    int Maximum = getModConfig().MaxHeart.GetValue();
    float percent = ((float)qounters_hr) / Maximum;
    if (percent < 0.5)
        return opts.range__5;
    if (percent < 0.6)
        return opts.range_5_6;
    if (percent < 0.7)
        return opts.range_6_7;
    if (percent < 0.8)
        return opts.range_7_8;
    if (percent < 0.9)
        return opts.range_8_9;
    return opts.range_9_;
}

void hrRangeColorUI(UnityEngine::GameObject *parent, UnparsedJSON unparsed) {
    static HeartRateRangeColorOption opts;
    opts = unparsed.Parse<HeartRateRangeColorOption>();

    BSML::ColorSetting *sptr;
    sptr = ::Qounters::API::CreateColorPicker(
        parent, "Color when lower than 50%", opts.range__5,
        [](UnityEngine::Color val) {
            static int id = ::Qounters::API::GetActionId();
            opts.range__5 = val;
            ::Qounters::API::SetColorOptions(id, opts);
        },
        ::Qounters::API::FinalizeAction);
    BSML::Lite::AddHoverHint(sptr, "Pickup a color");
    sptr = ::Qounters::API::CreateColorPicker(
        parent, "Color when range in 50% - 60%", opts.range_5_6,
        [](UnityEngine::Color val) {
            static int id = ::Qounters::API::GetActionId();
            opts.range_5_6 = val;
            ::Qounters::API::SetColorOptions(id, opts);
        },
        ::Qounters::API::FinalizeAction);
    BSML::Lite::AddHoverHint(sptr, "Pickup a color");
    sptr = ::Qounters::API::CreateColorPicker(
        parent, "Color when range in 60% - 70%", opts.range_6_7,
        [](UnityEngine::Color val) {
            static int id = ::Qounters::API::GetActionId();
            opts.range_6_7 = val;
            ::Qounters::API::SetColorOptions(id, opts);
        },
        ::Qounters::API::FinalizeAction);
    BSML::Lite::AddHoverHint(sptr, "Pickup a color");
    sptr = ::Qounters::API::CreateColorPicker(
        parent, "Color when range in 70% - 80%", opts.range_7_8,
        [](UnityEngine::Color val) {
            static int id = ::Qounters::API::GetActionId();
            opts.range_7_8 = val;
            ::Qounters::API::SetColorOptions(id, opts);
        },
        ::Qounters::API::FinalizeAction);
    BSML::Lite::AddHoverHint(sptr, "Pickup a color");
    sptr = ::Qounters::API::CreateColorPicker(
        parent, "Color when range in 80% - 90%", opts.range_8_9,
        [](UnityEngine::Color val) {
            static int id = ::Qounters::API::GetActionId();
            opts.range_8_9 = val;
            ::Qounters::API::SetColorOptions(id, opts);
        },
        ::Qounters::API::FinalizeAction);
    BSML::Lite::AddHoverHint(sptr, "Pickup a color");
    sptr = ::Qounters::API::CreateColorPicker(
        parent, "Color when range more than 90%", opts.range_9_,
        [](UnityEngine::Color val) {
            static int id = ::Qounters::API::GetActionId();
            opts.range_9_ = val;
            ::Qounters::API::SetColorOptions(id, opts);
        },
        ::Qounters::API::FinalizeAction);
    BSML::Lite::AddHoverHint(sptr, "Pickup a color");

    auto *rptr = BSML::Lite::CreateToggle(parent, "Set color for replay", opts.use_replay_color, [](bool v) {
        static int id = ::Qounters::API::GetActionId();
        opts.use_replay_color = v;
        ::Qounters::API::SetColorOptions(id, opts);
    });
    BSML::Lite::AddHoverHint(rptr, "Enable this will use replay color when playing replay.");

    sptr = ::Qounters::API::CreateColorPicker(
        parent, "Replay color", opts.range_replay,
        [](UnityEngine::Color val) {
            static int id = ::Qounters::API::GetActionId();
            opts.range_replay = val;
            ::Qounters::API::SetColorOptions(id, opts);
        },
        ::Qounters::API::FinalizeAction);
    BSML::Lite::AddHoverHint(sptr, "Pickup a color");
}

bool hrIsInReplayEnable(UnparsedJSON unparsed) { return Recorder::isReplaying(); }
void hrIsInReplayEnableUI(UnityEngine::GameObject *parent, UnparsedJSON unparsed) {}
void informIsReplayUpdated() { MetaCore::Events::Broadcast(METACORE_EVENT_MOD, METACORE_EVENT_ID_REPLAY_STATUS); }
void DisplayData(int heartrate) {
    qounters_hr = heartrate;
    MetaCore::Events::Broadcast(METACORE_EVENT_MOD, METACORE_EVENT_ID_HEART_RATE);
}

static bool enabled = false;

void Init() {

    if (::Qounters::API::IsInstalled()) {
        getLogger().info("Qounters detected, will load.");

        ::Qounters::Sources::premades["HeartBeatQuest"] = {
            {"HeartRateUI", ::Qounters::Types::PremadeFn(HeartBeat::Qounters::hrBundledUIPremade)}};

        ::Qounters::Sources::RegisterText("HeartRate", ::Qounters::Types::SourceFn<std::string>(hrTextSource),
                                          hrTextSourceUI);
        ::Qounters::Sources::RegisterShape("HeartRatePrecent", ::Qounters::Types::SourceFn<float>(hrPercentSource),
                                           hrPercentSourceUI);
        ::Qounters::Sources::RegisterEnable("HeartRatePercentRange", ::Qounters::Types::SourceFn<bool>(hrRangeEnable),
                                            hrRangeEnableUI);
        ::Qounters::Sources::RegisterColor(
            "HeartRateRangeColor", ::Qounters::Types::SourceFn<UnityEngine::Color>(hrRangeColor), hrRangeColorUI);

        MetaCore::Events::RegisterEvent(METACORE_EVENT_MOD, METACORE_EVENT_ID_HEART_RATE);
        ::Qounters::Events::RegisterToEvent(::Qounters::Types::Sources::Text, "HeartRate", METACORE_EVENT_MOD,
                                            METACORE_EVENT_ID_HEART_RATE);
        ::Qounters::Events::RegisterToEvent(::Qounters::Types::Sources::Shape, "HeartRatePrecent", METACORE_EVENT_MOD,
                                            METACORE_EVENT_ID_HEART_RATE);
        ::Qounters::Events::RegisterToEvent(::Qounters::Types::Sources::Enable, "HeartRatePercentRange",
                                            METACORE_EVENT_MOD, METACORE_EVENT_ID_HEART_RATE);
        ::Qounters::Events::RegisterToEvent(::Qounters::Types::Sources::Color, "HeartRateRangeColor",
                                            METACORE_EVENT_MOD, METACORE_EVENT_ID_HEART_RATE);

        ::Qounters::Sources::RegisterEnable(
            "HeartRateIsInReplay", ::Qounters::Types::SourceFn<bool>(hrIsInReplayEnable), hrIsInReplayEnableUI);

        MetaCore::Events::RegisterEvent(METACORE_EVENT_MOD, METACORE_EVENT_ID_REPLAY_STATUS);
        ::Qounters::Events::RegisterToEvent(::Qounters::Types::Sources::Enable, "HeartRateIsInReplay",
                                            METACORE_EVENT_MOD, METACORE_EVENT_ID_REPLAY_STATUS);

        enabled = true;
        return;
    }
    getLogger().info("Qounters not detected.");
}

bool Enabled() { return enabled; }

} // namespace Qounters
} // namespace HeartBeat

#endif // WITH_QOUNTERS
#pragma once

#include "sslocalization/shared/SSL10n.hpp"

namespace LANG {

#define V(key, v) inline const char *KEY_##key = "HEART_BEAT_QUEST_" #key;
#include "langs/english.inc"
#undef V

#define V(key, v)                                                                                                      \
    inline std::string key() { return SSL10n::Get("HEART_BEAT_QUEST_" #key); };
#include "langs/english.inc"
#undef V
}; // namespace LANG

namespace I18N {
void Setup();
}

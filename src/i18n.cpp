#include "i18n.hpp"
#include "sslocalization/shared/SSL10n.hpp"

namespace I18N {
void Setup() {
    SSL10n::Database::Helper()
#define V(key, value) .v("HEART_BEAT_QUEST_" #key, value)
#include "langs/english.inc"
        ;

    SSL10n::Database::Helper(SSL10n::L_Simplified_Chinese)
#define V(key, value) .v("HEART_BEAT_QUEST_" #key, value)
#include "langs/chinese.inc"
        ;
}
} // namespace I18N
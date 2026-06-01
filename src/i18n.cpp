#include "i18n.hpp"
#include "sslocalization/shared/SSL10n.hpp"
#include "asset.hpp"
namespace I18N {
void Setup() {
    SSL10n::Database::PolyglotFormat::AddCSVContent((char *)AssetGenerated::HeartBeatQuest_csv,
                                                    sizeof(AssetGenerated::HeartBeatQuest_csv));
}
} // namespace I18N
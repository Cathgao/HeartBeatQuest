#pragma once

#ifdef WITH_QOUNTERS

namespace HeartBeat {
namespace Qounters {
void Init();
void DisplayData(int heartrate);

bool Enabled();
} // namespace Qounters
} // namespace HeartBeat

#endif // WITH_QOUNTERS
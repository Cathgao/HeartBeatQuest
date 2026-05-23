#pragma once

#ifdef WITH_QOUNTERS

namespace HeartBeat {
namespace Qounters {
void Init();
void DisplayData(int heartrate);
void informIsReplayUpdated();
bool Enabled();

void CreateDriverObject();

} // namespace Qounters
} // namespace HeartBeat

#endif // WITH_QOUNTERS
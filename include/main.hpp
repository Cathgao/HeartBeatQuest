#pragma once

#include "paper2_scotland2/shared/logger.hpp"
#include <string>

namespace HeartBeat{
class HeartBeatObj;
}
Paper::ConstLoggerContext<21> & getLogger();

extern std::string modConfigFilePath;

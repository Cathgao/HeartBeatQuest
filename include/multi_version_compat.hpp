#if defined(GAME_VER_1_28_0) || defined(GAME_VER_1_35_0) || defined(GAME_VER_1_37_0)
#include "paper/shared/logger.hpp"
#else
#include "paper2_scotland2/shared/logger.hpp"
#endif

#if defined(GAME_VER_1_28_0) || defined(GAME_VER_1_35_0) || defined(GAME_VER_1_37_0) \
    || defined(GAME_VER_1_40_4) || defined(GAME_VER_1_40_6) || defined(GAME_VER_1_40_7) \
    || defined(GAME_VER_1_40_8)
#define UNITY_ACTIVITY_CLASS "com/unity3d/player/UnityPlayer"
#else
#define UNITY_ACTIVITY_CLASS "com/unity3d/player/UnityPlayerActivity"
#endif
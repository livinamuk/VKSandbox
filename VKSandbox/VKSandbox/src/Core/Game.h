#pragma once
#include "HellEnums.h"
#include "HellTypes.h"
#include <vector>
#include "Camera/Camera.h"
#include "Player/Player.h"

namespace Game {
    void Create();
    void Update();
    float GetDeltaTime();

    Player* GetLocalPlayerByIndex(uint32_t index);
    void NextSplitScreenMode();
    void SetSplitscreenMode(SplitscreenMode mode);
    const SplitscreenMode& GetSplitscreenMode();
    int32_t GetLocalPlayerCount();
    int32_t GetOnlinePlayerCount();
    Camera* GetLocalPlayerCameraByIndex(uint32_t index);
}
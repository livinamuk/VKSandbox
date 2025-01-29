#pragma once
#include "HellEnums.h"
#include "HellTypes.h"
#include <vector>
#include "../Player/Player.h"
#include "../Renderer/Types/Viewport.h"

namespace Game {
    void Create();
    void Update();
    float GetDeltaTime();

    Player* GetPlayerByIndex(int index);
    void NextSplitScreenMode();
    void SetSplitscreenMode(SplitscreenMode mode);
    const SplitscreenMode& GetSplitscreenMode();
    uint16_t GetPlayerCount();
    uint16_t GetActiveViewportCount();
    uint16_t GetLocalPlayerCount();
    uint16_t GetOnlinePlayerCount();
    Viewport* GetViewportByIndex(int16_t index);
    const std::vector<uint16_t>& GetActiveViewportPlayerIndices();
}
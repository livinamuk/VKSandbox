#include "Editor.h"
#include "Util.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Input/Input.h"
#include "UI/UIBackEnd.h"
#include "Viewport/ViewportManager.h"

namespace Editor {

    float g_dividerThickness = 4.0f;

    void UpdateUI() {
        const Resolutions& resolutions = Config::GetResolutions();
        int gBufferWidth = resolutions.gBuffer.x;
        int gBufferHeight = resolutions.gBuffer.y;

        // Dividers
        UIBackEnd::BlitTexture("Black", glm::ivec2(GetVerticalDividerXPos() * gBufferWidth, 0), Alignment::CENTERED_HORIZONTAL, WHITE, glm::ivec2(g_dividerThickness, gBufferHeight));
        UIBackEnd::BlitTexture("Black", glm::ivec2(0, GetHorizontalDividerYPos() * gBufferHeight), Alignment::CENTERED_VERTICAL, WHITE, glm::ivec2(gBufferWidth, g_dividerThickness));
    }
}
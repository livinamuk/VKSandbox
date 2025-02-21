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
        if (Editor::GetEditorViewportSplitMode() != EditorViewportSplitMode::SINGLE) {
            UIBackEnd::BlitTexture("Black", glm::ivec2(GetVerticalDividerXPos() * gBufferWidth, 0), Alignment::CENTERED_HORIZONTAL, WHITE, glm::ivec2(g_dividerThickness, gBufferHeight));
            UIBackEnd::BlitTexture("Black", glm::ivec2(0, GetHorizontalDividerYPos() * gBufferHeight), Alignment::CENTERED_VERTICAL, WHITE, glm::ivec2(gBufferWidth, g_dividerThickness));
        }

        // Resize rectangle
        SelectionRectangleState& rectangleState = GetSelectionRectangleState();
        if (GetEditorState() == EditorState::DRAGGING_SELECT_RECT) {
            glm::ivec2 location = glm::ivec2(0, 0);
            location.x = rectangleState.beginX;
            location.y = rectangleState.beginY;
            glm::ivec2 size = glm::ivec2(0, 0);
            size.x = rectangleState.currentX - rectangleState.beginX;
            size.y = rectangleState.currentY - rectangleState.beginY;
            UIBackEnd::BlitTexture("White", location, Alignment::TOP_LEFT, ORANGE * glm::vec4(1, 1, 1, 0.05f), size);
            UIBackEnd::BlitTexture("White", location, Alignment::TOP_LEFT, ORANGE, glm::ivec2(1, size.y));
            UIBackEnd::BlitTexture("White", location, Alignment::TOP_LEFT, ORANGE, glm::ivec2(size.x, 1));
            UIBackEnd::BlitTexture("White", location + size, Alignment::BOTTOM_RIGHT, ORANGE, glm::ivec2(1, size.y));
            UIBackEnd::BlitTexture("White", location + size, Alignment::BOTTOM_RIGHT, ORANGE, glm::ivec2(size.x, 1));
        }
    }
}
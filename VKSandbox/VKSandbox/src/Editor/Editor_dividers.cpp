#include "Editor.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"

namespace Editor {
    bool g_horizontalDividerHovered = false;
    bool g_verticalDividerHovered = false;
    int g_dividerHoverThreshold = 10;

    void UpdateDividers() {
        // const Resolutions& resolutions = Config::GetResolutions();
        int mouseX = Input::GetMouseX();
        int mouseY = Input::GetMouseY();
        int windowWidth = BackEnd::GetCurrentWindowWidth();
        int windowHeight = BackEnd::GetCurrentWindowHeight();
        //int gBufferWidth = resolutions.gBuffer.x;
        //int gBufferHeight = resolutions.gBuffer.y;

        // Hover
        g_horizontalDividerHovered = (std::abs(mouseX - (GetVerticalDividerXPos() * windowWidth)) < g_dividerHoverThreshold);
        g_verticalDividerHovered = (std::abs(mouseY - (GetHorizontalDividerYPos() * windowHeight)) < g_dividerHoverThreshold);

        // Start drag
        if (Input::LeftMousePressed()) {
            if (IsHorizontalDividerHovered() && IsVerticalDividerHovered()) {
                SetViewportResizeState(ViewportResizeState::RESIZING_HORIZONTAL_VERTICAL);
            }
            else if (IsHorizontalDividerHovered()) {
                SetViewportResizeState(ViewportResizeState::RESIZING_HORIZONTAL);
            }
            else if (IsVerticalDividerHovered()) {
                SetViewportResizeState(ViewportResizeState::RESIZING_VERTICAL);
            }
        }
        // End drag
        if (!Input::LeftMouseDown() && GetViewportResizeState() != ViewportResizeState::IDLE) {
            SetViewportResizeState(ViewportResizeState::IDLE);
        }
        if (GetViewportResizeState() == ViewportResizeState::RESIZING_HORIZONTAL ||
            GetViewportResizeState() == ViewportResizeState::RESIZING_HORIZONTAL_VERTICAL) {
            float xPos = Util::MapRange(mouseX, 0.0f, windowWidth, 0.0f, 1.0f);
            xPos = glm::clamp(xPos, 0.0f, 1.0f);
            Editor::SetSplitX(xPos);
        }
        // Update drag
        if (GetViewportResizeState() == ViewportResizeState::RESIZING_VERTICAL ||
            GetViewportResizeState() == ViewportResizeState::RESIZING_HORIZONTAL_VERTICAL) {
            float yPos = Util::MapRange(mouseY, 0.0f, windowHeight, 0.0f, 1.0f);
            yPos = glm::clamp(yPos, 0.0f, 1.0f);
            Editor::SetSplitY(yPos);
        }
    }

    bool IsHorizontalDividerHovered() {
        return g_horizontalDividerHovered;
    }

    bool IsVerticalDividerHovered() {
        return g_verticalDividerHovered;
    }
}
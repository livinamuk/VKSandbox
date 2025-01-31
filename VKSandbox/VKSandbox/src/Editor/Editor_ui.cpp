#include "Editor.h"
#include "Util.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Input/Input.h"
#include "UI/UIBackEnd.h"
#include "Viewport/ViewportManager.h"

namespace Editor {

    void UpdateUI() {

        const Resolutions& resolutions = Config::GetResolutions();

        int windowWidth = BackEnd::GetCurrentWindowWidth();
        int windowHeight = BackEnd::GetCurrentWindowHeight();

        int gBufferWidth = resolutions.gBuffer.x;
        int gBufferHeight = resolutions.gBuffer.y;

        int lineThickness = 4;
        int threshold = 10;

        int mouseX = Input::GetMouseX();
        int mouseY = Input::GetMouseY();

        float splitXGBufferSpace = GetSplitX() * gBufferWidth;
        float splitYGBufferSpace = GetSplitY() * gBufferHeight;
        float splitXWindowSpace = GetSplitX() * windowWidth;
        float splitYWindowSpace = GetSplitY() * windowHeight;

        float centerLineXPos = splitXGBufferSpace - (lineThickness / 2);
        float centerLineYPos = splitYGBufferSpace - (lineThickness / 2);

        UIBackEnd::BlitTexture("Black", glm::ivec2(centerLineXPos, 0), Alignment::TOP_LEFT, WHITE, glm::ivec2(lineThickness, gBufferHeight));
        UIBackEnd::BlitTexture("Black", glm::ivec2(0, centerLineYPos), Alignment::TOP_LEFT, WHITE, glm::ivec2(gBufferWidth, lineThickness));

        bool xHover = (mouseX < splitXWindowSpace + threshold && mouseX > splitXWindowSpace - threshold);
        bool yHover = (mouseY < splitYWindowSpace + threshold && mouseY > splitYWindowSpace - threshold);

        if (xHover && yHover) {
            BackEnd::SetCursor(HELL_CURSOR_CROSSHAIR);
        }
        else if (xHover) {
            BackEnd::SetCursor(HELL_CURSOR_HRESIZE);
        }
        else if (yHover) {
            BackEnd::SetCursor(HELL_CURSOR_VRESIZE);
        }
        else {
            BackEnd::SetCursor(HELL_CURSOR_ARROW);
        }

        if (Input::LeftMousePressed()) {
            if (xHover && yHover) {
                SetViewportResizeState(ViewportResizeState::RESIZING_HORIZONTAL_VERTICAL);
            }
            else if (xHover) {
                SetViewportResizeState(ViewportResizeState::RESIZING_HORIZONTAL);
            }
            else if (yHover) {
                SetViewportResizeState(ViewportResizeState::RESIZING_VERTICAL);
            }
        }

        if (!Input::LeftMouseDown()) {
            SetViewportResizeState(ViewportResizeState::IDLE);
        }

        if (GetViewportResizeState() == ViewportResizeState::RESIZING_HORIZONTAL || 
            GetViewportResizeState() == ViewportResizeState::RESIZING_HORIZONTAL_VERTICAL) {
            float splitX = Util::MapRange(mouseX, 0.0f, windowWidth, 0.0f, 1.0f);
            splitX = glm::clamp(splitX, 0.0f, 1.0f);
            Editor::SetSplitX(splitX);
        }
        if (GetViewportResizeState() == ViewportResizeState::RESIZING_VERTICAL ||
            GetViewportResizeState() == ViewportResizeState::RESIZING_HORIZONTAL_VERTICAL) {
            float splitY = Util::MapRange(mouseY, 0.0f, windowHeight, 0.0f, 1.0f);
            splitY = glm::clamp(splitY, 0.0f, 1.0f);
            Editor::SetSplitY(splitY);
        }
    }
}
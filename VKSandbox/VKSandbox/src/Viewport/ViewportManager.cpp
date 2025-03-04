#pragma once
#include "ViewportManager.h"
#include "HellDefines.h"
#include "Util.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Core/Game.h"
#include "Imgui/ImguiBackEnd.h"
#include "Input/Input.h"
#include "Editor/Editor.h"
#include "Renderer/Renderer.h"

namespace ViewportManager {
    std::vector<Viewport> g_viewports;

    void Init() {
        g_viewports.resize(4);
        g_viewports[0].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
        g_viewports[1].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
        g_viewports[2].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
        g_viewports[3].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
        Update();
    }

    void Update() {
        const Resolutions& resolutions = Config::GetResolutions();

        // Set state / recreate matrices based on editor/splitscreen mode
        // Clean this up when you have a moment.
        if (Editor::IsEditorOpen()) {
            float splitX = Editor::GetVerticalDividerXPos();
            float splitY = Editor::GetHorizontalDividerYPos();
                      
            float editorPanelRightEdgeNormalized = EDITOR_LEFT_PANEL_WIDTH / BackEnd::GetCurrentWindowWidth();
            float fileMenuBottomNormalized = (float)ImGuiBackend::GetFileMenuHeight() / (float)BackEnd::GetCurrentWindowHeight();

            float beginX = editorPanelRightEdgeNormalized;
            float beginY = fileMenuBottomNormalized;

            // 4 way split
            if (Editor::GetEditorViewportSplitMode() == EditorViewportSplitMode::FOUR_WAY_SPLIT) {
                g_viewports[0].SetPosition(glm::vec2(beginX, 1.0f - splitY));  // Top-left
                g_viewports[1].SetPosition(glm::vec2(splitX, 1.0f - splitY));  // Top-right
                g_viewports[2].SetPosition(glm::vec2(beginX + 0.0f, 0.0f));             // Bottom-left
                g_viewports[3].SetPosition(glm::vec2(splitX, 0.0f));                    // Bottom-right
                g_viewports[0].SetSize(glm::vec2(splitX - beginX, splitY - beginY));
                g_viewports[1].SetSize(glm::vec2(1.0f - splitX, splitY - beginY));
                g_viewports[2].SetSize(glm::vec2(splitX - beginX, 1.0f - splitY));
                g_viewports[3].SetSize(glm::vec2(1.0f - splitX, 1.0f - splitY));
                g_viewports[0].Show();
                g_viewports[1].Show();
                g_viewports[2].Show();
                g_viewports[3].Show();
            }
            else if (Editor::GetEditorViewportSplitMode() == EditorViewportSplitMode::SINGLE) {
                g_viewports[0].SetPosition(glm::vec2(beginX, 0.0f));  // Top-left
                g_viewports[1].SetPosition(glm::vec2(beginX, 0.0f));  // Top-right
                g_viewports[2].SetPosition(glm::vec2(beginX, 0.0f));  // Bottom-left
                g_viewports[3].SetPosition(glm::vec2(beginX, 0.0f));  // Bottom-right
                g_viewports[0].SetSize(glm::vec2(1.0f - beginX, 1.0f - beginY));
                g_viewports[1].SetSize(glm::vec2(1.0f - beginX, 1.0f - beginY));
                g_viewports[2].SetSize(glm::vec2(1.0f - beginX, 1.0f - beginY));
                g_viewports[3].SetSize(glm::vec2(1.0f - beginX, 1.0f - beginY));
                g_viewports[0].Show();
                g_viewports[1].Hide();
                g_viewports[2].Hide();
                g_viewports[3].Hide();
            }
            for (int i = 0; i < 4; i++) {
                float orthoNear = 0.1f;
                float orthoFar = 1000.0f;

                g_viewports[i].SetViewportMode(Editor::GetViewportModeByIndex(i));
                Editor::IsViewportOrthographic(i)
                    ? g_viewports[i].SetOrthographic(g_viewports[i].GetOrthoSize(), orthoNear, orthoFar)
                    : g_viewports[i].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
            }
        }
        // When not in the editor
        else {
            for (int i = 0; i < 4; i++) {
                g_viewports[i].SetViewportMode(ShadingMode::SHADED);
            }
            if (Game::GetSplitscreenMode() == SplitscreenMode::FULLSCREEN) {
                g_viewports[0].SetPosition(glm::vec2(0.0f, 0.0f));  // Fullscreen
                g_viewports[0].SetSize(glm::vec2(1.0f, 1.0f));
                g_viewports[0].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
                g_viewports[0].Show();
                g_viewports[1].Hide();
                g_viewports[2].Hide();
                g_viewports[3].Hide();
            }
            else if (Game::GetSplitscreenMode() == SplitscreenMode::TWO_PLAYER) {
                g_viewports[0].SetPosition(glm::vec2(0.0f, 0.5f));  // Top
                g_viewports[1].SetPosition(glm::vec2(0.0f, 0.0f));  // Bottom
                g_viewports[0].SetSize(glm::vec2(1.0f, 0.5f));
                g_viewports[1].SetSize(glm::vec2(1.0f, 0.5f));
                g_viewports[0].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
                g_viewports[1].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
                g_viewports[0].Show();
                g_viewports[1].Show();
                g_viewports[2].Hide();
                g_viewports[3].Hide();
            }
            else if (Game::GetSplitscreenMode() == SplitscreenMode::FOUR_PLAYER) {
                g_viewports[0].SetPosition(glm::vec2(0.0f, 0.5f));  // Top-left
                g_viewports[1].SetPosition(glm::vec2(0.5f, 0.5f));  // Top-right
                g_viewports[2].SetPosition(glm::vec2(0.0f, 0.0f));  // Bottom-left
                g_viewports[3].SetPosition(glm::vec2(0.5f, 0.0f));  // Bottom-right
                g_viewports[0].SetSize(glm::vec2(0.5f, 0.5f));
                g_viewports[1].SetSize(glm::vec2(0.5f, 0.5f));
                g_viewports[2].SetSize(glm::vec2(0.5f, 0.5f));
                g_viewports[3].SetSize(glm::vec2(0.5f, 0.5f));
                g_viewports[0].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
                g_viewports[1].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
                g_viewports[2].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
                g_viewports[3].SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
                g_viewports[0].Show();
                g_viewports[1].Show();
                g_viewports[2].Show();
                g_viewports[3].Show();
            }
        }

        for (Viewport& viewport : g_viewports) {
            viewport.Update();
        }

    }

    Viewport* GetViewportByIndex(int32_t viewportIndex) {
        if (viewportIndex >= 0 && viewportIndex < g_viewports.size()) {
            return &g_viewports[viewportIndex];
        }
        else {
            std::cout << "ViewportManager::GetViewportByIndex(int index) failed. " << viewportIndex << " out of range of size " << g_viewports.size() << "\n";
            return nullptr;
        }
    }

    std::vector<Viewport>& GetViewports() {
        return g_viewports;
    }
}
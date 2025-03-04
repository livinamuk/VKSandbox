#include "Renderer.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/Vulkan/Renderer/VK_renderer.h"
#include "BackEnd/BackEnd.h"
#include "Editor/Editor.h"

namespace Renderer {

    struct RendererSettingsSet {
        RendererSettings game;
        RendererSettings heightMapEditor;
        RendererSettings mapEditor;
        RendererSettings sectorEditor;
    } g_rendererSettingsSet;

    DebugLineRenderMode g_debugLineRenderMode = DebugLineRenderMode::SHOW_NO_LINES;

    void InitMain() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::InitMain();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO: VulkanRenderer::InitMain();
        }

        g_rendererSettingsSet.mapEditor.rendererOverrideState = RendererOverrideState::NORMALS;
        g_rendererSettingsSet.mapEditor.drawGrass = false;

        g_rendererSettingsSet.heightMapEditor.rendererOverrideState = RendererOverrideState::CAMERA_NDOTL;
        g_rendererSettingsSet.heightMapEditor.drawGrass = false;

        g_rendererSettingsSet.sectorEditor.rendererOverrideState = RendererOverrideState::BASE_COLOR;
        g_rendererSettingsSet.sectorEditor.drawGrass = true;
    }

    void RenderLoadingScreen() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::RenderLoadingScreen();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            VulkanRenderer::RenderLoadingScreen();
        }
    }

    void RenderGame() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::RenderGame();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO: VulkanRenderer::RenderLoadingScreen();
        }
    }

    void HotloadShaders() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::HotloadShaders();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            //VulkanRenderer::HotloadShaders();
        }
    }

    void DrawPoint(glm::vec3 position, glm::vec3 color) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawPoint(position, color);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO
        }
    }

    void DrawLine(glm::vec3 begin, glm::vec3 end, glm::vec3 color) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawLine(begin, end, color);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO
        }
    }

    void DrawAABB(AABB& aabb, glm::vec3 color) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawAABB(aabb, color);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO
        }
    }

    void DrawAABB(AABB& aabb, glm::vec3 color, glm::mat4 worldTransform) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::DrawAABB(aabb, color, worldTransform);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO
        }
    }

    void Renderer::NextDebugLineRenderMode() {
        std::vector<DebugLineRenderMode> allowedDebugLineRenderModes = {
            SHOW_NO_LINES,
            PHYSX_ALL,
            //PATHFINDING,
            //PHYSX_COLLISION,
            //PATHFINDING_RECAST,
            //RTX_LAND_TOP_LEVEL_ACCELERATION_STRUCTURE,
            //RTX_LAND_BOTTOM_LEVEL_ACCELERATION_STRUCTURES,
            //BOUNDING_BOXES,
        };

        g_debugLineRenderMode = (DebugLineRenderMode)(int(g_debugLineRenderMode) + 1);
        if (g_debugLineRenderMode == DEBUG_LINE_MODE_COUNT) {
            g_debugLineRenderMode = (DebugLineRenderMode)0;
        }
        // If mode isn't in available modes list, then go to next
        bool allowed = false;
        for (auto& avaliableMode : allowedDebugLineRenderModes) {
            if (g_debugLineRenderMode == avaliableMode) {
                allowed = true;
                break;
            }
        }
        if (!allowed && g_debugLineRenderMode != DebugLineRenderMode::SHOW_NO_LINES) {
            NextDebugLineRenderMode();
        }
    }

    DebugLineRenderMode GetDebugLineRenderMode() {
        return g_debugLineRenderMode;
    }

    RendererSettings& GetCurrentRendererSettings() {
        if (Editor::IsEditorOpen()) {
            switch (Editor::GetEditorMode()) {
                case EditorMode::HEIGHTMAP_EDITOR: return g_rendererSettingsSet.heightMapEditor;
                case EditorMode::SECTOR_EDITOR:    return g_rendererSettingsSet.sectorEditor;
                case EditorMode::MAP_EDITOR:       return g_rendererSettingsSet.mapEditor;
            }
        }
        return g_rendererSettingsSet.game;
    }

    void NextRendererOverrideState() {
        RendererSettings& rendererSettings = GetCurrentRendererSettings();
        int i = static_cast<int>(rendererSettings.rendererOverrideState);
        i = (i + 1) % static_cast<int>(RendererOverrideState::STATE_COUNT);
        rendererSettings.rendererOverrideState = static_cast<RendererOverrideState>(i);
    }
}
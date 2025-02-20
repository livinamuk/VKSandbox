#include "Renderer.h"
#include "../API/OpenGL/Renderer/GL_renderer.h"
#include "../API/Vulkan/Renderer/VK_renderer.h"
#include "../BackEnd/BackEnd.h"

namespace Renderer {

    DebugLineRenderMode g_debugLineRenderMode = DebugLineRenderMode::SHOW_NO_LINES;

    void InitMain() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::InitMain();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            // TODO: VulkanRenderer::InitMain();
        }
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
}
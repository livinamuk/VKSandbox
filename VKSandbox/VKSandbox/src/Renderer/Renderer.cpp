#include "Renderer.h"
#include "../API/OpenGL/Renderer/GL_renderer.h"
#include "../API/Vulkan/Renderer/VK_renderer.h"
#include "../BackEnd/BackEnd.h"

namespace Renderer {

    void InitMain() {
        if (BackEnd::GetAPI() == API::OPENGL) {
            OpenGLRenderer::InitMain();
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            //VulkanRenderer::InitMain();
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
            //VulkanRenderer::RenderLoadingScreen();
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
}
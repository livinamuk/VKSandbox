#include "API/OpenGL/Renderer/GL_renderer.h"

#include "BackEnd/GLFWIntegration.h"
#include "Input/Input.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "AssetManagement/AssetManager.h"
#include "BackEnd/BackEnd.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"
#include "UI/UIBackEnd.h"
#include "Config/Config.h"
#include "Core/Audio.h"
#include "ImGui/EditorImgui.h"

#include <fstream>
#include <string>

namespace OpenGLRenderer {

    GLint g_quadVAO2 = 0;
    GLuint g_linearSampler2 = 0;
    GLuint g_nearestSampler2 = 0;
     
    void ImGuiPass() {

        const Resolutions& resolutions = Config::GetResolutions();
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        int mouseX = Input::GetMouseX();
        int mouseY = Input::GetMouseY();
        int windowWidth = BackEnd::GetCurrentWindowWidth();
        int windowHeight = BackEnd::GetCurrentWindowHeight();
        int fullScreenWidth = BackEnd::GetFullScreenWidth();
        int fullScreenHeight = BackEnd::GetFullScreenHeight();


        if (g_linearSampler2 == 0) {
            glGenSamplers(1, &g_linearSampler2);
            glSamplerParameteri(g_linearSampler2, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glSamplerParameteri(g_linearSampler2, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenSamplers(1, &g_nearestSampler2);
            glSamplerParameteri(g_nearestSampler2, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glSamplerParameteri(g_nearestSampler2, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        } 

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, BackEnd::GetCurrentWindowWidth(), BackEnd::GetCurrentWindowHeight());

        ImGuiStyle& style = ImGui::GetStyle();
        style.FramePadding = ImVec2(10.0f, 10.0f);
        style.ItemSpacing = ImVec2(10.0f, 10.0f);


        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        EditorImGui::CreateImguiElements();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
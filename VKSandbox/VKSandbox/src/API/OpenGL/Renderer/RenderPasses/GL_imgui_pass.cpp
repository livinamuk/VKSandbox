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
#include "Editor/Editor.h"

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

        OpenGLFrameBuffer* uiFrameBuffer = GetFrameBuffer("UI");

        if (!uiFrameBuffer) return;

        if (g_linearSampler2 == 0) {
            glGenSamplers(1, &g_linearSampler2);
            glSamplerParameteri(g_linearSampler2, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glSamplerParameteri(g_linearSampler2, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenSamplers(1, &g_nearestSampler2);
            glSamplerParameteri(g_nearestSampler2, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glSamplerParameteri(g_nearestSampler2, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        uiFrameBuffer->Bind();
        uiFrameBuffer->SetViewport();
        uiFrameBuffer->ClearAttachment("Color", 0.0f, 0.0f, 0.0f, 1.0f);
        uiFrameBuffer->DrawBuffer("Color");


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, BackEnd::GetCurrentWindowWidth(), BackEnd::GetCurrentWindowHeight());

        ImGuiStyle& style = ImGui::GetStyle();
        style.FramePadding = ImVec2(10.0f, 10.0f);
        style.ItemSpacing = ImVec2(10.0f, 10.0f);


        float scaleX = uiFrameBuffer->GetWidth() / (float)windowWidth;
        float scaleY = uiFrameBuffer->GetHeight() / (float)fullScreenHeight;

        ImGuiIO& io = ImGui::GetIO();



        bool uiHovered = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
        if (uiHovered) {
            //std::cout << "ImGui my beloved has hover\n";
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        Editor::UpdateImGui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

       //glEnable(GL_BLEND);
       //
       //// Create QUAD if it don't exist
       //if (g_quadVAO2 == 0) {
       //    g_quadVAO2 = OpenGLRenderer::CreateQuadVAO();
       //}
       //glBindFramebuffer(GL_FRAMEBUFFER, 0);
       //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
       //
       //glBindFramebuffer(GL_FRAMEBUFFER, 0);
       //glBindSampler(0, g_nearestSampler2);
       ////glBindSampler(0, g_linearSampler2);
       //glViewport(0, 0, BackEnd::GetCurrentWindowWidth(), BackEnd::GetCurrentWindowHeight());
       //glActiveTexture(GL_TEXTURE0);
       //glBindTexture(GL_TEXTURE_2D, uiFrameBuffer->GetColorAttachmentHandleByName("Color"));
       //glBindVertexArray(g_quadVAO2);
       //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
       //
       //// Cleanup
       //glDisable(GL_BLEND);
       //glBindSampler(0, 0);
    }
}
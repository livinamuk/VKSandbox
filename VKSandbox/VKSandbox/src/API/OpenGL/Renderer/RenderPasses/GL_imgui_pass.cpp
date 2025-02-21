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

    bool g_initilized = false;
    
    void UpdateStyle();
    void ShowFileMenu();
    void UpdateImGuiMouseState();
    void UpdateImGuiKeyboardState();
    void Test();
    void ShowEditorSelectMenu();
    void ShowLeftPanel();
    void ShowNewFileWindow();
    void ShowHeightMapSettings(float leftPanelWidth, float& propertiesHeight, float panelHeight, float minSize, float maxSize);

    void InitImGui() {
        const Resolutions& resolutions = Config::GetResolutions();
        GLFWwindow* window = (GLFWwindow*)GLFWIntegration::GetWindowPointer();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.FontGlobalScale = 1.0f;
        io.IniFilename = nullptr;
        io.DisplaySize = ImVec2(resolutions.ui.x, resolutions.ui.y);
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f); 
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        io.Fonts->TexGlyphPadding = 1;

      

        ImGui::GetMainViewport()->Size = io.DisplaySize;
        ImGui::GetMainViewport()->Pos = ImVec2(0, 0);

        ImGui_ImplOpenGL3_Init();
        g_initilized = true;
    }

    void UpdateStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.FrameRounding = 0.0f;
        style.GrabRounding = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.TabRounding = 0.0f;
        style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.4f, 0.4f, 0.4f, 1.00f);

        ImVec4 baseColor = style.Colors[ImGuiCol_MenuBarBg];
        ImVec4 fadedColor = ImVec4(baseColor.x * 0.5f, baseColor.y * 0.5f, baseColor.z * 0.5f, baseColor.w * 1.0f);
        style.Colors[ImGuiCol_MenuBarBg] = fadedColor;
        style.Colors[ImGuiCol_PopupBg] = fadedColor;
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0);  // Fully transparent

        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);  
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);   // Red grab
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); 
    }

    void ImGuiPass() {

        if (!Editor::IsOpen()) {
            return;
        }

        if (!g_initilized) {
            InitImGui();
        }

        UpdateStyle();

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

        ImGuiStyle& style = ImGui::GetStyle();
        style.FramePadding = ImVec2(10.0f, 10.0f);
        style.ItemSpacing = ImVec2(10.0f, 10.0f);


        float scaleX = uiFrameBuffer->GetWidth() / (float)windowWidth;
        float scaleY = uiFrameBuffer->GetHeight() / (float)fullScreenHeight;

        UpdateImGuiMouseState();
        UpdateImGuiKeyboardState();
        ImGuiIO& io = ImGui::GetIO();



        bool uiHovered = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
        if (uiHovered) {
            //std::cout << "ImGui my beloved has hover\n";
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        //ShowNewFileWindow();
        //ShowEditorSelectMenu(); 

        ShowFileMenu();
        ShowLeftPanel();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

       glEnable(GL_BLEND);

        // Create QUAD if it don't exist
        if (g_quadVAO2 == 0) {
            g_quadVAO2 = OpenGLRenderer::CreateQuadVAO();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindSampler(0, g_nearestSampler2);
        glBindSampler(0, g_linearSampler2);
        glViewport(0, 0, BackEnd::GetCurrentWindowWidth(), BackEnd::GetCurrentWindowHeight());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uiFrameBuffer->GetColorAttachmentHandleByName("Color"));
        glBindVertexArray(g_quadVAO2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Cleanup
        glDisable(GL_BLEND);
        glBindSampler(0, 0);
    }



    void ShowFileMenu() {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        float fixedMenuHeight = EDITOR_FILE_MENU_HEIGHT;
        style.FramePadding.y = (fixedMenuHeight - io.Fonts->Fonts[0]->FontSize) * 0.5f;
        float minWidth = 250.0f;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {

                if (ImGui::MenuItem("  New    ")) { Test(); }

                if (ImGui::MenuItem("  Open    ")) { std::cout << "Open file dialog\n"; }

                if (ImGui::MenuItem("  Save    ")) { std::cout << "Save file\n"; }

                if (ImGui::MenuItem("  Exit    ")) { std::cout << "Exit application\n"; }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Insert")) {

                if (ImGui::MenuItem("  Tree    ")) { Test(); }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

    }


    void UpdateImGuiMouseState() {
        const Resolutions& resolutions = Config::GetResolutions();
        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        int windowWidth = BackEnd::GetCurrentWindowWidth();
        int windowHeight = BackEnd::GetCurrentWindowHeight();
        int fullScreenWidth = BackEnd::GetFullScreenWidth();
        int fullScreenHeight = BackEnd::GetFullScreenHeight();
        int mouseX = Input::GetMouseX();
        int mouseY = Input::GetMouseY();
        float scaleX = resolutions.ui.x / (float)windowWidth;
        float scaleY = resolutions.ui.y / (float)windowHeight;
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(mouseX * scaleX, mouseY * scaleY);
        io.MouseDown[0] = Input::LeftMouseDown();
        io.MouseDown[1] = Input::GetMouseWheelValue() != 0;
        io.MouseDown[2] = Input::RightMouseDown();
    }

    void ShowEditorSelectMenu() {
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 windowSize = ImVec2(400, 298);
        ImVec2 centerPos = ImVec2(
            (io.DisplaySize.x - windowSize.x) * 0.5f,
            (io.DisplaySize.y - windowSize.y) * 0.5f
        );

        // Set window position before creating it
        ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

        ImGui::Begin("EDITOR SELECT", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

        if (ImGui::Button("New Run", ImVec2(-FLT_MIN, 40))) {
            std::cout << "New Run selected\n";
        }

        if (ImGui::Button("Heightmap Editor", ImVec2(-FLT_MIN, 40))) {
            std::cout << "Heightmap Editor selected\n";
        }

        if (ImGui::Button("Sector Editor", ImVec2(-FLT_MIN, 40))) {
            std::cout << "Sector Editor selected\n";
        }

        if (ImGui::Button("Weapon Editor", ImVec2(-FLT_MIN, 40))) {
            std::cout << "Weapon Editor selected\n";
        }
        if (ImGui::Button("Cancel", ImVec2(-FLT_MIN, 40))) {
            std::cout << "Weapon Editor selected\n";
        }

        ImGui::End();
    }

    void ShowOutliner(float panelHeight, float propertiesHeight);
    void ShowPropertiesPanel(float leftPanelWidth, float& propertiesHeight, float panelHeight, float minSize, float maxSize);

    void ShowLeftPanel() {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        style.ChildBorderSize = 0.001f;

        float panelHeight = io.DisplaySize.y - EDITOR_FILE_MENU_HEIGHT; 

        static float propertiesHeight = EDITOR_FILE_MENU_HEIGHT + (panelHeight * 0.35f);
        float minSize = 50.0f;
        float maxSize = panelHeight - minSize;

        ImGui::SetNextWindowPos(ImVec2(0, EDITOR_FILE_MENU_HEIGHT), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(EDITOR_LEFT_PANEL_WIDTH, panelHeight), ImGuiCond_Always);

        if (ImGui::Begin("LeftPanel", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {
            //ShowPropertiesPanel(EDITOR_LEFT_PANEL_WIDTH, propertiesHeight, panelHeight, minSize, maxSize);
            ShowHeightMapSettings(EDITOR_LEFT_PANEL_WIDTH, propertiesHeight, panelHeight, minSize, maxSize);
            ShowOutliner(panelHeight, propertiesHeight);
        }
        ImGui::End();
    }

    

    void ShowPropertiesPanel(float leftPanelWidth, float& propertiesHeight, float panelHeight, float minSize, float maxSize) {

        static glm::vec3 position = glm::vec3(0.0f);
        static glm::vec3 rotation = glm::vec3(0.0f);
        static glm::vec3 scale = glm::vec3(1.0f);
        static char nameBuffer[128] = "Default Name";
        static int selectedMaterial = 0; 
        static std::vector<std::string> materials = { "Material A", "Material B", "Material C" };


        ImGuiIO& io = ImGui::GetIO();

        if (ImGui::BeginChild("Properties", ImVec2(0, propertiesHeight - EDITOR_FILE_MENU_HEIGHT), true)) {
            ImGui::Text("Properties");
            ImGui::Separator();

            ImGui::Text("Name:");
            float nameInputFieldWidth = 220.0f;
            ImGui::PushItemWidth(nameInputFieldWidth);
            if (ImGui::InputText("##Name", nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
                std::string cleanedName = std::string(nameBuffer).c_str();
                std::cout << "'" << cleanedName << "'\n";
            }
            ImGui::PopItemWidth();

            float inputFieldWidth = 220.0f;

            ImGui::Text("Position:");
            ImGui::PushItemWidth(inputFieldWidth);
            ImGui::InputFloat3("##Position", &position.x, "%.3f");
            ImGui::PopItemWidth();

            ImGui::Text("Rotation:");
            ImGui::PushItemWidth(inputFieldWidth);
            ImGui::InputFloat3("##Rotation", &rotation.x, "%.3f");
            ImGui::PopItemWidth();

            ImGui::Text("Scale:");
            ImGui::PushItemWidth(inputFieldWidth);
            ImGui::InputFloat3("##Scale", &scale.x, "%.3f");
            ImGui::PopItemWidth();


            ImGui::Text("Material:");
            if (ImGui::BeginCombo("##Material", materials[selectedMaterial].c_str())) {
                for (int i = 0; i < materials.size(); i++) {
                    bool isSelected = (selectedMaterial == i);
                    if (ImGui::Selectable(materials[i].c_str(), isSelected)) {
                        selectedMaterial = i;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
        }
        ImGui::EndChild();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.4f));

        ImGui::SetCursorPosY(propertiesHeight - EDITOR_FILE_MENU_HEIGHT);
        ImGui::Button("##Divider", ImVec2(leftPanelWidth - 10, 5));

        ImGui::PopStyleColor(3);

        // this is broken
        if (ImGui::IsItemActive()) {
            propertiesHeight += io.MouseDelta.y; 
            propertiesHeight = glm::clamp(propertiesHeight, EDITOR_FILE_MENU_HEIGHT + minSize, EDITOR_FILE_MENU_HEIGHT + maxSize);
        }
    }
    
    void ShowHeightMapSettings(float leftPanelWidth, float& propertiesHeight, float panelHeight, float minSize, float maxSize) {

        ImGuiIO& io = ImGui::GetIO();

        if (ImGui::BeginChild("Properties", ImVec2(0, propertiesHeight - EDITOR_FILE_MENU_HEIGHT), true)) {

            ImGui::Text("Properties");
            ImGui::Separator();
                        
            // Brush Type Selection
            static int selectedBrush = 0;
            const char* brushNames[] = { "Smooth Brush", "Noise Brush" };
            for (int i = 0; i < 2; i++) {
                bool isSelected = (selectedBrush == i);
                if (ImGui::Checkbox(brushNames[i], &isSelected)) {
                    selectedBrush = i;
                    std::cout << "Selected Brush: " << i << "\n";
                }
            }

            ImGui::Separator();

            // Sliders
            static float sliderValues[2] = { 0.5f, 10.0f };
            if (ImGui::SliderFloat("Brush strength", &sliderValues[0], 0.0f, 1.0f)) {
                std::cout << "Brush strength: " << sliderValues[0] << "\n";
            }
            if (ImGui::SliderFloat("Noise strength", &sliderValues[1], 1.0f, 50.0f)) {
                std::cout << "Noise strength: " << sliderValues[1] << "\n";
            }

            // Sliders
            ImGui::Separator();
            static bool checkboxState = false;
            if (ImGui::Checkbox("Auto Update", &checkboxState)) {
                std::cout << "Checkbox state: " << (checkboxState ? "Checked" : "Unchecked") << "\n";
            }
        }
        ImGui::EndChild();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.2f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.4f));

        ImGui::SetCursorPosY(propertiesHeight - EDITOR_FILE_MENU_HEIGHT);
        ImGui::Button("##Divider", ImVec2(leftPanelWidth - 10, 5));

        ImGui::PopStyleColor(3);

        if (ImGui::IsItemActive()) {
            propertiesHeight += io.MouseDelta.y; // Move with mouse
            propertiesHeight = glm::clamp(propertiesHeight, EDITOR_FILE_MENU_HEIGHT + minSize, EDITOR_FILE_MENU_HEIGHT + maxSize);
        }
    }


    void ShowOutliner(float panelHeight, float propertiesHeight) {
        float objectIndent = 10.0f;

        ImGuiStyle& style = ImGui::GetStyle();
        // style.IndentSpacing = 10.0f;
        // style.FramePadding = ImVec2(2.0f, 2.0f);
        // style.ItemSpacing = ImVec2(4.0f, 2.0f);

        if (ImGui::BeginChild("Outliner", ImVec2(0, panelHeight - propertiesHeight - 10), true, ImGuiWindowFlags_HorizontalScrollbar)) {
            ImGui::TextUnformatted("Outliner");
            ImGui::Separator();

            ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
            if (ImGui::TreeNodeEx("Game Objects", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
                ImGui::Indent(objectIndent);
                for (int i = 0; i < 5; i++) {
                    std::string label = "Game Object " + std::to_string(i);
                    if (ImGui::Selectable(label.c_str())) {
                        std::cout << "Selected: " << i << "\n";
                    }
                }
                ImGui::Unindent(objectIndent);
                ImGui::TreePop();
            }


            if (ImGui::TreeNodeEx("Trees", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
                ImGui::Indent(objectIndent);
                for (int i = 0; i < 5; i++) {
                    ImGui::Text("Tree %d", i);
                }
                ImGui::Unindent(objectIndent);
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Lights", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
                ImGui::Indent(objectIndent);
                for (int i = 0; i < 5; i++) {
                    ImGui::Text("Light %d", i);
                }
                ImGui::Unindent(objectIndent);
                ImGui::TreePop();
            }
            ImGui::PopStyleVar();

        }
        ImGui::EndChild();
    }


    void ShowHeightMapSettings() {

    }

    void ShowNewFileWindow() {

        static char textBuffer[256] = "";
        static bool newFileWindowOpen = true;

        if (!newFileWindowOpen) return;

        ImGui::OpenPopup("Create New File");

        if (ImGui::BeginPopupModal("Create New File", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
            ImGui::Text("Enter file name:");
            ImGui::InputText("##newfile", textBuffer, IM_ARRAYSIZE(textBuffer));

            if (ImGui::Button("Create")) {
                std::string filename = textBuffer;
                std::ofstream file(filename);
                if (file) {
                    file.close();
                    newFileWindowOpen = false;
                    ImGui::CloseCurrentPopup();
                    std::cout << "Simulated creating a file: " << filename << "\n";
                }
                else {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to create file.");
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                newFileWindowOpen = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void Test() {
        Audio::PlayAudio("UI_Select.wav", 1.0f);
    }

    void UpdateImGuiKeyboardState() {
        ImGuiIO& io = ImGui::GetIO();
        static std::unordered_map<int, ImGuiKey> keyMap;

        if (keyMap.empty()) {
            keyMap = {
                {HELL_KEY_A, ImGuiKey_A}, {HELL_KEY_B, ImGuiKey_B}, {HELL_KEY_C, ImGuiKey_C},
                {HELL_KEY_D, ImGuiKey_D}, {HELL_KEY_E, ImGuiKey_E}, {HELL_KEY_F, ImGuiKey_F},
                {HELL_KEY_G, ImGuiKey_G}, {HELL_KEY_H, ImGuiKey_H}, {HELL_KEY_I, ImGuiKey_I},
                {HELL_KEY_J, ImGuiKey_J}, {HELL_KEY_K, ImGuiKey_K}, {HELL_KEY_L, ImGuiKey_L},
                {HELL_KEY_M, ImGuiKey_M}, {HELL_KEY_N, ImGuiKey_N}, {HELL_KEY_O, ImGuiKey_O},
                {HELL_KEY_P, ImGuiKey_P}, {HELL_KEY_Q, ImGuiKey_Q}, {HELL_KEY_R, ImGuiKey_R},
                {HELL_KEY_S, ImGuiKey_S}, {HELL_KEY_T, ImGuiKey_T}, {HELL_KEY_U, ImGuiKey_U},
                {HELL_KEY_V, ImGuiKey_V}, {HELL_KEY_W, ImGuiKey_W}, {HELL_KEY_X, ImGuiKey_X},
                {HELL_KEY_Y, ImGuiKey_Y}, {HELL_KEY_Z, ImGuiKey_Z},

                {HELL_KEY_0, ImGuiKey_0}, {HELL_KEY_1, ImGuiKey_1}, {HELL_KEY_2, ImGuiKey_2},
                {HELL_KEY_3, ImGuiKey_3}, {HELL_KEY_4, ImGuiKey_4}, {HELL_KEY_5, ImGuiKey_5},
                {HELL_KEY_6, ImGuiKey_6}, {HELL_KEY_7, ImGuiKey_7}, {HELL_KEY_8, ImGuiKey_8},
                {HELL_KEY_9, ImGuiKey_9},

                {HELL_KEY_SPACE, ImGuiKey_Space},
                {HELL_KEY_BACKSPACE, ImGuiKey_Backspace},
                {HELL_KEY_ENTER, ImGuiKey_Enter},
                //{HELL_KEY_TAB, ImGuiKey_Tab},
                {HELL_KEY_LEFT, ImGuiKey_LeftArrow}, {HELL_KEY_RIGHT, ImGuiKey_RightArrow},
                {HELL_KEY_UP, ImGuiKey_UpArrow}, {HELL_KEY_DOWN, ImGuiKey_DownArrow},
                {HELL_KEY_ESCAPE, ImGuiKey_Escape},      // broken
                {HELL_KEY_CAPS_LOCK, ImGuiKey_CapsLock}, // broken
                {HELL_KEY_LEFT_SHIFT, ImGuiKey_LeftShift},
                {HELL_KEY_PERIOD, ImGuiKey_Period},
                {HELL_KEY_DELETE, ImGuiKey_Delete}
            };
        }

        // Input keys
        for (const auto& [hellKey, imguiKey] : keyMap) {
            bool pressed = Input::KeyPressed(hellKey);
            io.AddKeyEvent(imguiKey, pressed);
            if (pressed) {
                if (imguiKey >= ImGuiKey_A && imguiKey <= ImGuiKey_Z) {
                    if (Input::KeyDown(HELL_KEY_LEFT_SHIFT_GLFW)) {
                        io.AddInputCharacter('A' + (imguiKey - ImGuiKey_A));
                    }
                    else {
                        io.AddInputCharacter('a' + (imguiKey - ImGuiKey_A));
                    }
                }
                else if (imguiKey >= ImGuiKey_0 && imguiKey <= ImGuiKey_9) {
                    io.AddInputCharacter('0' + (imguiKey - ImGuiKey_0));
                }
                else if (imguiKey == ImGuiKey_Space) {
                    io.AddInputCharacter(' ');
                }
                else if (imguiKey == ImGuiKey_Period) {
                    io.AddInputCharacter('.');
                }
            }
        }
    }



}
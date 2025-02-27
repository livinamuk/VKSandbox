#include "Editor.h"
#include "Core/Audio.h"
#include "Core/JSON.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "Input/Input.h"
#include "imgui/imgui.h"
#include <fstream>

namespace Editor {

    enum InsertState {
        IDLE,
        INSERTING
    };

    InsertState g_InsertState;


    // "Callbacks"
    void LoadSector();
    void SaveSector();

    void InsertTree();
    void ShowFileMenu();
    void Test();
    void CreateEditorSelectMenuUI();
    void ShowLeftPanel();
    void ShowNewFileWindow();
    void ShowHeightMapSettings(float leftPanelWidth, float& propertiesHeight, float panelHeight, float minSize, float maxSize);

    void ShowOutliner(float panelHeight, float propertiesHeight);
    void ShowPropertiesPanel(float leftPanelWidth, float& propertiesHeight, float panelHeight, float minSize, float maxSize);

    void ShowLightingSettings(float panelHeight, float propertiesHeight);

    void UpdateImGui() {

        if (IsEditorSelectMenuVisible()) {
            CreateEditorSelectMenuUI();
        }
        if (IsEditorOpen()){
            ShowFileMenu();
            ShowLeftPanel();
        }
    }


    void ShowFileMenu() {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        float fixedMenuHeight = EDITOR_FILE_MENU_HEIGHT;
        style.FramePadding.y = (fixedMenuHeight - io.Fonts->Fonts[0]->FontSize) * 0.5f;
        float minWidth = 250.0f;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {

                if (ImGui::MenuItem("  New        ")) { Test(); }

                if (ImGui::MenuItem("  Load        ")) { LoadSector(); }

                if (ImGui::MenuItem("  Save        ")) { SaveSector(); }

                if (ImGui::MenuItem("  Exit        ")) { std::cout << "Exit application\n"; }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Insert")) {

                if (ImGui::MenuItem("  Tree    ")) { InsertTree(); }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

    }


    void CreateEditorSelectMenuUI() {
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
            Audio::PlayAudio(AUDIO_SELECT, 1.00f);
            CloseEditorSelectMenu();
            std::cout << "New Run selected\n";
        }

        if (ImGui::Button("Heightmap Editor", ImVec2(-FLT_MIN, 40))) {
            Audio::PlayAudio(AUDIO_SELECT, 1.00f);
            SetEditorMode(EditorMode::HEIGHTMAP_EDITOR);
            CloseEditorSelectMenu();
            if (!IsEditorOpen()) {
                OpenEditor();
            }
        }

        if (ImGui::Button("Sector Editor", ImVec2(-FLT_MIN, 40))) {
            Audio::PlayAudio(AUDIO_SELECT, 1.00f);
            SetEditorMode(EditorMode::SECTOR_EDITOR);
            CloseEditorSelectMenu();
            if (!IsEditorOpen()) {
                OpenEditor();
            }
        }

        if (ImGui::Button("Weapon Editor", ImVec2(-FLT_MIN, 40))) {
            Audio::PlayAudio(AUDIO_SELECT, 1.00f);
            std::cout << "Weapon editor selected\n";
            CloseEditorSelectMenu();
        }
        if (ImGui::Button("Cancel", ImVec2(-FLT_MIN, 40))) {
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            CloseEditorSelectMenu();
        }

        ImGui::End();
    }

    void ShowLeftPanel() {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        style.ChildBorderSize = 0.001f;

        float panelHeight = io.DisplaySize.y - EDITOR_FILE_MENU_HEIGHT;

        static float propertiesHeight = EDITOR_FILE_MENU_HEIGHT + (panelHeight * 0.35f);
        float minSize = 150.0f;
        float maxSize = panelHeight - minSize;

        ImGui::SetNextWindowPos(ImVec2(0, EDITOR_FILE_MENU_HEIGHT), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(EDITOR_LEFT_PANEL_WIDTH, panelHeight), ImGuiCond_Always);

        if (ImGui::Begin("LeftPanel", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {

            if (GetEditorMode() == EditorMode::HEIGHTMAP_EDITOR) {
                ShowHeightMapSettings(EDITOR_LEFT_PANEL_WIDTH, propertiesHeight, panelHeight, minSize, maxSize);
                ShowLightingSettings(panelHeight, propertiesHeight);
            }
            else if (GetEditorMode() == EditorMode::SECTOR_EDITOR) {
                ShowPropertiesPanel(EDITOR_LEFT_PANEL_WIDTH, propertiesHeight, panelHeight, minSize, maxSize);
                ShowOutliner(panelHeight, propertiesHeight);
            }

        }
        ImGui::End();
    }

    void RightJustifiedText(const std::string& text, float right_margin) {
        ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
        float cursor_x = right_margin - text_size.x;
        ImGui::SetCursorPosX(cursor_x);
        ImGui::TextUnformatted(text.c_str());
    }

    void Vec3Input(const std::string& label, glm::vec3& value) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 3)); 


        float padding = 40;
        float valueMargin = EDITOR_LEFT_PANEL_WIDTH * 0.4f;
        float labelMargin = valueMargin - 12.0f;
        float inputFieldWidth = EDITOR_LEFT_PANEL_WIDTH - valueMargin - padding;
        ImGui::PushItemWidth(inputFieldWidth);

        // X Input
        RightJustifiedText(label + " X", labelMargin);
        ImGui::SameLine(valueMargin);
        ImGui::InputFloat(std::string("##X" + label).c_str(), &value[0], 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);

        // Y Input
        RightJustifiedText("Y", labelMargin);
        ImGui::SameLine(valueMargin);
        ImGui::InputFloat(std::string("##Y" + label).c_str(), &value[1], 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);

        // Z Input
        RightJustifiedText("Z", labelMargin);
        ImGui::SameLine(valueMargin);
        ImGui::InputFloat(std::string("##Z" + label).c_str(), &value[2], 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);

        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::PopStyleVar(3);
        ImGui::PopItemWidth();

    }

    void StringInput(const std::string& label, const std::string& value) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);

        float padding = 40;
        float valueMargin = EDITOR_LEFT_PANEL_WIDTH * 0.4f;
        float labelMargin = valueMargin - 12.0f;
        float inputFieldWidth = EDITOR_LEFT_PANEL_WIDTH - valueMargin - padding;
        ImGui::PushItemWidth(inputFieldWidth);


        RightJustifiedText(label, labelMargin);
        ImGui::SameLine(valueMargin);
        //ImGui::InputFloat("##X", &value[0], 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);

        static char nameBuffer[128] = "Default Name";

        float nameInputFieldWidth = 220.0f;
        //ImGui::PushItemWidth(nameInputFieldWidth);
        if (ImGui::InputText(std::string("##" + label).c_str(), nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
            std::string cleanedName = std::string(nameBuffer).c_str();
            std::cout << "'" << cleanedName << "'\n";
        }

        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        ImGui::PopStyleVar(2);
        ImGui::PopItemWidth();
    }


    void ShowPropertiesPanel(float leftPanelWidth, float& propertiesHeight, float panelHeight, float minSize, float maxSize) {

        static glm::vec3 position = glm::vec3(0.0f);
        static glm::vec3 rotation = glm::vec3(0.0f);
        static glm::vec3 scale = glm::vec3(1.0f);
        static char nameBuffer[128] = "Default Name";
        static int selectedMaterial = 0;
        static std::vector<std::string> materials = { "Material A", "Material B", "Material C" };

        static std::string name0;
        static std::string name1;

        ImGuiIO& io = ImGui::GetIO();

        if (ImGui::BeginChild("Properties", ImVec2(0, propertiesHeight - EDITOR_FILE_MENU_HEIGHT), true)) {
            ImGui::Text("Properties");
            ImGui::Separator();

           // ImGui::Text("Name:");
           // float nameInputFieldWidth = 220.0f;
           // ImGui::PushItemWidth(nameInputFieldWidth);
           // if (ImGui::InputText("##Name", nameBuffer, IM_ARRAYSIZE(nameBuffer))) {
           //     std::string cleanedName = std::string(nameBuffer).c_str();
           //     std::cout << "'" << cleanedName << "'\n";
           // }
           // ImGui::PopItemWidth();
           //
           // float inputFieldWidth = 220.0f;


            StringInput("Name", name0);
            StringInput("Cunt", name1);
            Vec3Input("Position", position);
            Vec3Input("Rotation", rotation);
            Vec3Input("Scale", scale);


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


    void ShowLightingSettings(float panelHeight, float propertiesHeight) {
        RendererSettings& rendererSettings = Renderer::GetCurrentRendererSettings();

        if (ImGui::BeginChild("Lighting Settings", ImVec2(0, panelHeight - propertiesHeight - 10), true, ImGuiWindowFlags_HorizontalScrollbar)) {
            ImGui::TextUnformatted("Lighting Settings");
            ImGui::Separator();
            //ImGui::Checkbox("Lighting", &lightingSettings.lightingEnabled);            
            ImGui::Checkbox("Grass", &rendererSettings.drawGrass);
        }
        ImGui::EndChild();
    }

    void ShowOutliner(float panelHeight, float propertiesHeight) {
        float objectIndent = 10.0f;

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

    void LoadSector() {
        SectorCreateInfo sectorCreateInfo = JSON::LoadSector("res/sectors/TestSector.json");
        World::LoadSingleSector(sectorCreateInfo);
    }

    void SaveSector() {
        SectorCreateInfo& sectorCreateInfo = World::GetEditorSectorCreateInfo();
        JSON::SaveSector("res/sectors/TestSector2.json", sectorCreateInfo);
    }

    void InsertTree() {
        // nothing
    }
}
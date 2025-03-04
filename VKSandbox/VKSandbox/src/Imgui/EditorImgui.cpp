#pragma once
#include "EditorImgui.h"

#include "Editor/Editor.h"
#include "Callbacks/Callbacks.h"
#include "Core/Audio.h"
#include "Core/JSON.h"
#include "Imgui/ImguiBackEnd.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "Input/Input.h"
#include "imgui/imgui.h"
#include <fstream>

namespace EditorImGui {

    //enum InsertState {
    //    IDLE,
    //    INSERTING
    //};
    //
    //InsertState g_InsertState;

    std::unordered_map<std::string, NewFileWindow> g_newFileWindows;
    std::unordered_map<std::string, OpenFileWindow> g_openFileWindows;
    std::unordered_map<std::string, EditorWindow> g_editorWindows;
    std::unordered_map<std::string, FileMenu> g_fileMenus;

    bool g_hasHover = false;
    bool g_typingInTextField = false;
    bool g_imguiContentIsVisible = false;
    bool g_imguiContentWasVisibleLastFrame = false;
    bool g_floatingWindowsAreVisible = false;

    void ShowLeftPanel();
    void ShowHeightMapSettings(float leftPanelWidth, float& propertiesHeight, float panelHeight, float minSize, float maxSize);

    void ShowOutliner(float panelHeight, float propertiesHeight);
    void ShowPropertiesPanel(float leftPanelWidth, float& propertiesHeight, float panelHeight, float minSize, float maxSize);

    void ShowLightingSettings(float panelHeight, float propertiesHeight);

    void ClickedHeightMapEditorSelectButton() {
        Editor::SetEditorMode(EditorMode::HEIGHTMAP_EDITOR);
        if (!Editor::IsEditorOpen()) {
            Editor::OpenEditor();
        }
    }

    void ClickedSectorEditorSelectButton() {
        Editor::SetEditorMode(EditorMode::SECTOR_EDITOR);
        if (!Editor::IsEditorOpen()) {
            Editor::OpenEditor();
        }
    }

    void AddEditorMenuNode(FileMenu& fileMenu) {
        FileMenuNode& editor = fileMenu.AddMenuNode("Editor");
        editor.AddChild("House", &Callbacks::OpenHouseMapEditor, "F4");
        editor.AddChild("Height Map", &Callbacks::OpenHeightMapEditor, "F5");
        editor.AddChild("Map", &Callbacks::OpenMapEditor, "F6");
        editor.AddChild("Sector", &Callbacks::OpenSectorEditor, "F7");
        editor.AddChild("Weapons", &Callbacks::OpenWeaponsEditor, "F8");
    }

    void Init() {
        g_fileMenus.clear();
        g_editorWindows.clear();

        // Editor select
        EditorWindow* editorSelectWindow = CreateEditorWindow("EditorSelect");
        editorSelectWindow->SetTitle("EDITOR SELECT");
        editorSelectWindow->AddButton("New Run", nullptr);
        editorSelectWindow->AddButton("Heightmap Editor", (&ClickedHeightMapEditorSelectButton));
        editorSelectWindow->AddButton("Sector Editor", (&ClickedSectorEditorSelectButton));
        editorSelectWindow->AddButton("Weapon Editor", nullptr);
        editorSelectWindow->AddButton("Cancel", nullptr);

        // "New File" windows
        CreateNewFileWindow("NewHeightMap", "Create New Height Map", &Callbacks::NewHeightMap);
        CreateNewFileWindow("NewMap", "Create Map", &Callbacks::NewMap);
        CreateNewFileWindow("NewSector", "Create New Sector", &Callbacks::NewSector);

        // "Open File" windows
        CreateOpenFileWindow("OpenHeightMap", "Open Height Map", "res/height_maps", &Callbacks::OpenHeightMap);
        CreateOpenFileWindow("OpenMap", "Open Map", "res/maps", &Callbacks::OpenMap);
        CreateOpenFileWindow("OpenSector", "Open Sector", "res/sectors", &Callbacks::OpenSector);
               
        /* Height Map Editor file menu */ {
            FileMenu& fileMenu = CreateFileMenu("HeightMapEditor");
            FileMenuNode& file = fileMenu.AddMenuNode("File", nullptr);

            file.AddChild("New", []() { ShowNewFileWindow("NewHeightMap"); }, "F2");
            file.AddChild("Open", []() { ShowOpenFileWindow("OpenHeightMap"); }, "F3");
            file.AddChild("Save", nullptr);
            file.AddChild("Revert", nullptr);
            file.AddChild("Delete", nullptr);
            file.AddChild("Duplicate", nullptr);
            file.AddChild("Quit", &Callbacks::QuitProgram, "Esc");

            AddEditorMenuNode(fileMenu);

            FileMenuNode& run = fileMenu.AddMenuNode("Run");
            run.AddChild("New Run", nullptr, "F1");
            run.AddChild("Test Height Map", nullptr);
        }

        /* Map Editor file menu */ {
            FileMenu& fileMenu = CreateFileMenu("MapEditor");
            FileMenuNode& file = fileMenu.AddMenuNode("File", nullptr);

            file.AddChild("New", []() { ShowNewFileWindow("NewMap"); }, "F2");
            file.AddChild("Open", []() { ShowOpenFileWindow("OpenMap"); }, "F3");
            file.AddChild("Save", nullptr);
            file.AddChild("Revert", nullptr);
            file.AddChild("Delete", nullptr);
            file.AddChild("Duplicate", nullptr);
            file.AddChild("Quit", &Callbacks::QuitProgram, "Esc");

            AddEditorMenuNode(fileMenu);

            FileMenuNode& run = fileMenu.AddMenuNode("Run");
            run.AddChild("New Run", nullptr, "F1");
            run.AddChild("Test Map", &Callbacks::TestCurrentMap);
        }


        /* Sector Editor menu */ {
            FileMenu& fileMenu = CreateFileMenu("SectorEditor");
            FileMenuNode& file = fileMenu.AddMenuNode("File", nullptr);

            file.AddChild("New", []() { ShowNewFileWindow("NewSector"); }, "F2");
            file.AddChild("Open", []() { ShowOpenFileWindow("OpenSector"); }, "F3");
            file.AddChild("Save", nullptr);
            file.AddChild("Revert", nullptr);
            file.AddChild("Delete", nullptr);
            file.AddChild("Duplicate", nullptr);
            file.AddChild("Quit", &Callbacks::QuitProgram, "Esc");

            AddEditorMenuNode(fileMenu);

            FileMenuNode& insert = fileMenu.AddMenuNode("Insert");

            FileMenuNode& nature = insert.AddChild("Nature", nullptr);
            nature.AddChild("Tree", nullptr);

            FileMenuNode& pickups = insert.AddChild("Pick Ups", nullptr);

            FileMenuNode& weapons = pickups.AddChild("Weapons", nullptr);
            weapons.AddChild("AKS74U", nullptr);
            weapons.AddChild("FN-P90", nullptr);
            weapons.AddChild("Glock", nullptr);
            weapons.AddChild("Golden Glock", nullptr);
            weapons.AddChild("Remington 870", nullptr);
            weapons.AddChild("SPAS", nullptr);
            weapons.AddChild("Tokarev", nullptr);

            FileMenuNode& ammo = pickups.AddChild("Ammo", nullptr);
            ammo.AddChild("AKS74U", nullptr);
            ammo.AddChild("FN-P90", nullptr);
            ammo.AddChild("Glock", nullptr);
            ammo.AddChild("Shotgun Shells Buckshot", nullptr);
            ammo.AddChild("Shotgun Shells Slug", nullptr);
            ammo.AddChild("Tokarev", nullptr);

            FileMenuNode& run = fileMenu.AddMenuNode("Run");
            run.AddChild("New Run", nullptr, "F1");
            run.AddChild("Test Sector", nullptr);
        }

    }

    void Update() {

        //if (Input::KeyPressed(HELL_KEY_K)) {
        //    Init();
        //}

        // Toggle editor select menu
        //if (Input::KeyPressed(HELL_KEY_F1)) {
        //    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
        //    CloseAnyOpenContent();
        //    EditorWindow* window = GetEditorWindowByName("EditorSelect");
        //    if (window) {
        //        //window->ToggleOpenState();
        //        window->Show();
        //    }
        //}
        // New file
        if (Input::KeyPressed(HELL_KEY_F2)) {
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            if (Editor::IsEditorOpen()) {
                if (Editor::GetEditorMode() == EditorMode::HEIGHTMAP_EDITOR) ShowNewFileWindow("NewHeightMap");
                if (Editor::GetEditorMode() == EditorMode::MAP_EDITOR)       ShowNewFileWindow("Map");
                if (Editor::GetEditorMode() == EditorMode::SECTOR_EDITOR)    ShowNewFileWindow("NewSector");
            }
        }
        // Open file
        if (Input::KeyPressed(HELL_KEY_F3)) {
            Audio::PlayAudio(AUDIO_SELECT, 1.0f);
            if (Editor::IsEditorOpen()) {
                if (Editor::GetEditorMode() == EditorMode::HEIGHTMAP_EDITOR) ShowOpenFileWindow("OpenHeightMap");
                if (Editor::GetEditorMode() == EditorMode::MAP_EDITOR)       ShowOpenFileWindow("OpenMap");
                if (Editor::GetEditorMode() == EditorMode::SECTOR_EDITOR)    ShowOpenFileWindow("OpenSector");
            }
        }

        // Calculate visibility, this frame and last frame
        g_imguiContentWasVisibleLastFrame = g_imguiContentIsVisible;
        g_imguiContentIsVisible = false;
        for (auto& [key, editorWindow] : g_editorWindows) {
            if (editorWindow.IsVisible()) {
                g_imguiContentIsVisible = true;
                break;
            }
        }
    }

    void CreateImguiElements() {
        g_floatingWindowsAreVisible = false;

        // "New File" windows
        for (auto& [key, window] : g_newFileWindows) {
            if (window.IsVisible()) {
                window.CreateImGuiElements();
                g_floatingWindowsAreVisible = true;
            }
        }
        // "Open File" windows
        for (auto& [key, window] : g_openFileWindows) {
            if (window.IsVisible()) {
                window.CreateImGuiElements();
                g_floatingWindowsAreVisible = true;
            }
        }
        // Hovering windows
        for (auto& [key, window] : g_editorWindows) {
            if (window.IsVisible()) {
                window.CreateImGuiElements();
                g_floatingWindowsAreVisible = true;
            }
        }

        // File menus and fixed windows
        if (Editor::IsEditorOpen()) {
            if (Editor::GetEditorMode() == EditorMode::HEIGHTMAP_EDITOR) {
                GetFileMenuByName("HeightMapEditor")->CreateImguiElements();
            }
            if (Editor::GetEditorMode() == EditorMode::MAP_EDITOR) {
                GetFileMenuByName("MapEditor")->CreateImguiElements();
            }
            if (Editor::GetEditorMode() == EditorMode::SECTOR_EDITOR) {
                GetFileMenuByName("SectorEditor")->CreateImguiElements();
            }
            ShowLeftPanel();
        }


        // Update hover state
        ImGuiIO& io = ImGui::GetIO();
        g_hasHover = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);    

        // Determine if any text field has input focus
        g_typingInTextField = ImGui::GetIO().WantTextInput;
    }

    void CloseAnyOpenContent() {
        for (auto& [key, window] : g_newFileWindows) {
            window.Close();
        }
        for (auto& [key, window] : g_openFileWindows) {
            window.Close();
        }
        for (auto& [key, window] : g_editorWindows) {
            window.Close();
        }
    }

    void ShowLeftPanel() {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        style.ChildBorderSize = 0.001f;

        float panelHeight = io.DisplaySize.y - ImGuiBackend::GetFileMenuHeight();

        static float propertiesHeight = ImGuiBackend::GetFileMenuHeight() + (panelHeight * 0.35f);
        float minSize = 150.0f;
        float maxSize = panelHeight - minSize;

        ImGui::SetNextWindowPos(ImVec2(0, ImGuiBackend::GetFileMenuHeight()), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(EDITOR_LEFT_PANEL_WIDTH, panelHeight), ImGuiCond_Always);

        if (ImGui::Begin("LeftPanel", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar)) {

            if (Editor::GetEditorMode() == EditorMode::HEIGHTMAP_EDITOR) {
                ShowHeightMapSettings(EDITOR_LEFT_PANEL_WIDTH, propertiesHeight, panelHeight, minSize, maxSize);
                ShowLightingSettings(panelHeight, propertiesHeight);
            }
            else if (Editor::GetEditorMode() == EditorMode::SECTOR_EDITOR) {
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

        if (ImGui::BeginChild("Properties", ImVec2(0, propertiesHeight - ImGuiBackend::GetFileMenuHeight()), true)) {
            ImGui::Text("Properties");
            ImGui::Separator();

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

        ImGui::SetCursorPosY(propertiesHeight - ImGuiBackend::GetFileMenuHeight());
        ImGui::Button("##Divider", ImVec2(leftPanelWidth - 10, 5));

        ImGui::PopStyleColor(3);

        // this is broken
        if (ImGui::IsItemActive()) {
            propertiesHeight += io.MouseDelta.y;
            propertiesHeight = glm::clamp(propertiesHeight, ImGuiBackend::GetFileMenuHeight() + minSize, ImGuiBackend::GetFileMenuHeight() + maxSize);
        }
    }

    void ShowHeightMapSettings(float leftPanelWidth, float& propertiesHeight, float panelHeight, float minSize, float maxSize) {

        ImGuiIO& io = ImGui::GetIO();

        if (ImGui::BeginChild("Properties", ImVec2(0, propertiesHeight - ImGuiBackend::GetFileMenuHeight()), true)) {

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

        ImGui::SetCursorPosY(propertiesHeight - ImGuiBackend::GetFileMenuHeight());
        ImGui::Button("##Divider", ImVec2(leftPanelWidth - 10, 5));

        ImGui::PopStyleColor(3);

        if (ImGui::IsItemActive()) {
            propertiesHeight += io.MouseDelta.y; // Move with mouse
            propertiesHeight = glm::clamp(propertiesHeight, ImGuiBackend::GetFileMenuHeight() + minSize, ImGuiBackend::GetFileMenuHeight() + maxSize);
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

    bool ImGuiContentIsVisible() {
        return g_imguiContentIsVisible;
    }

    bool ImGuiContentWasVisibleLastFrame() {
        return g_imguiContentWasVisibleLastFrame;
    }

    void ShowNewFileWindow(const std::string& name) {
        CloseAnyOpenContent();
        NewFileWindow* window = GetNewFileWindow(name);
        if (window) {
            window->Show();
        }
    }    
    
    void ShowOpenFileWindow(const std::string& name) {
        CloseAnyOpenContent();
        OpenFileWindow* window = GetOpenFileWindow(name);
        if (window) {
            window->Show();
        }
    }

    NewFileWindow& CreateNewFileWindow(const std::string& name, const std::string& title, NewFileCallback callback) {
        g_newFileWindows[name] = NewFileWindow(title, callback);
        return g_newFileWindows[name];
    }

    NewFileWindow* GetNewFileWindow(const std::string& name) {
        auto it = g_newFileWindows.find(name);
        return (it != g_newFileWindows.end()) ? &it->second : nullptr;
    }

    OpenFileWindow& CreateOpenFileWindow(const std::string& name, const std::string& title, const std::string filepath, OpenFileCallback callback) {
        g_openFileWindows[name] = OpenFileWindow(title, filepath, callback);
        return g_openFileWindows[name];
    }

    OpenFileWindow* GetOpenFileWindow(const std::string& name) {
        auto it = g_openFileWindows.find(name);
        return (it != g_openFileWindows.end()) ? &it->second : nullptr;
    }

    FileMenu& CreateFileMenu(const std::string& name) {
        g_fileMenus[name] = FileMenu();
        return g_fileMenus[name];
    }

    FileMenu* GetFileMenuByName(const std::string& name) {
        auto it = g_fileMenus.find(name);
        return (it != g_fileMenus.end()) ? &it->second : nullptr;
    }

    EditorWindow* CreateEditorWindow(const std::string& windowName) {
        auto  it = g_editorWindows.find(windowName);
        if (it != g_editorWindows.end()) {
            return &it->second;
        }
        auto result = g_editorWindows.emplace(windowName, EditorWindow());
        return &result.first->second;
    }

    EditorWindow* GetEditorWindowByName(const std::string& windowName) {
        auto it = g_editorWindows.find(windowName);
        return (it != g_editorWindows.end()) ? &it->second : nullptr;
    }

    bool AnyElementHasHover() {
        return g_hasHover;
    }

    bool HasKeyboardFocus() {
        return g_typingInTextField;
    }

    bool FloatingWindowsAreVisible() {
        return g_floatingWindowsAreVisible;
    }
}
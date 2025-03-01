#include "Types.h"
#include "BackEnd/BackEnd.h"
#include "Core/Audio.h"
#include "Imgui/ImguiBackEnd.h"
#include "Util/Util.h"

#include <Imgui/imgui.h>

FileMenuNode::FileMenuNode(const std::string& text, std::function<void()> callback, bool addPadding) {
    m_callback = callback;
    if (addPadding) {
        m_text = "   " + text + "        ";
    }
    else {
        m_text = " " + text + " ";
    }
}

FileMenuNode& FileMenuNode::AddChild(const std::string& text, std::function<void()> callback, bool addPadding)
{
    m_children.emplace_back(text, callback, addPadding);
    return m_children.back();
}

void FileMenuNode::CreateImguiElement()
{
    // If this node has children, treat it as a submenu.
    if (!m_children.empty()) {
        if (ImGui::BeginMenu(m_text.c_str())) {
            for (auto& child : m_children) {
                child.CreateImguiElement();
            }
            ImGui::EndMenu();
        }
    }
    else {
        // No children? Then it's a leaf item.
        if (ImGui::MenuItem(m_text.c_str())) {
            if (m_callback) {
                m_callback();
            }
        }
    }
}

FileMenuNode& FileMenu::AddMenuNode(const std::string& text, std::function<void()> callback, bool addPadding)
{
    m_menuNodes.emplace_back(text, callback, addPadding);
    return m_menuNodes.back();
}

void FileMenu::CreateImguiElements()
{
    if (ImGui::BeginMainMenuBar()) {
        ImGuiBackend::RecordFileMenuHeight(ImGui::GetWindowSize().y);
        for (auto& node : m_menuNodes) {
            node.CreateImguiElement();
        }
        ImGui::EndMainMenuBar();
    }
}

NewFileWindow::NewFileWindow(const std::string& title, NewFileCallback callback) {
    m_title = title;
    m_callback = callback;
}

void NewFileWindow::Show() {
    m_isVisible = true;
    m_openedThisFrame = true;
    m_textBuffer[0] = '\0';
}

void NewFileWindow::Close() {
    m_isVisible = false;
    m_openedThisFrame = false;
    m_textBuffer[0] = '\0';
}

void NewFileWindow::CreateImGuiElements() {
    ImVec2 padding(40.0f, 30.0f);
    float buttonWidth = 150.0f;

    float windowWidth = (float)BackEnd::GetCurrentWindowWidth();
    float windowHeight = (float)BackEnd::GetCurrentWindowHeight();

    ImGui::SetNextWindowPos(ImVec2(windowWidth * 0.5f, windowHeight * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
    ImGui::OpenPopup(m_title.c_str());

    if (ImGui::BeginPopupModal(m_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("Enter file name:");
        if (m_openedThisFrame) {
            ImGui::SetKeyboardFocusHere();
            m_openedThisFrame = false;
        }
        ImGui::InputText("##newfile", m_textBuffer, IM_ARRAYSIZE(m_textBuffer)); 

        // Compute space to center 2 buttons side by side
        float availWidth = ImGui::GetContentRegionAvail().x;
        float spacing = ImGui::GetStyle().ItemSpacing.x;
        float totalButtonsWidth = (buttonWidth * 2.0f) + spacing;
        float offsetX = (availWidth - totalButtonsWidth) * 0.5f;

        ImGui::Dummy(ImVec2(0, 10));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 8.0f));

        // "Create" button
        if (ImGui::Button("Create", ImVec2(buttonWidth, 0.0f))) {
            std::string filename = m_textBuffer;
            if (m_callback) {
                m_callback(filename);
            }
            Close();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        // "Cancel" button
        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
            Close();
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleVar();

        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
}


bool NewFileWindow::IsVisible() {
    return m_isVisible;
}

OpenFileWindow::OpenFileWindow(const std::string& title, const std::string& path, OpenFileCallback callback) {
    m_title = title;
    m_path = path;
    m_callback = callback;
}

void OpenFileWindow::Show() {
    m_isVisible = true;
    m_openedThisFrame = true;
    m_selectedIndex = -1;
}

void OpenFileWindow::Close() {
    m_isVisible = false;
    m_openedThisFrame = false;
    m_selectedIndex = -1;
}

void OpenFileWindow::CreateImGuiElements() {
    ImVec2 padding(40.0f, 30.0f);
    float buttonWidth = 150.0f;
    float listBoxWidth = 600;

    float windowWidth = (float)BackEnd::GetCurrentWindowWidth();
    float windowHeight = (float)BackEnd::GetCurrentWindowHeight();

    ImGui::SetNextWindowPos(ImVec2(windowWidth * 0.5f, windowHeight * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding);
    ImGui::OpenPopup(m_title.c_str());

    if (ImGui::BeginPopupModal(m_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
       
        std::vector<std::string> filenames;
        std::vector<FileInfo> files = Util::IterateDirectory(m_path);
        for (FileInfo& file : files) {
            filenames.push_back(file.name);
        }
        if (m_openedThisFrame) {
            if (filenames.size()) {
                m_selectedIndex = 0;
            }
            m_openedThisFrame = false;
        }

        int numItems = static_cast<int>(filenames.size());
        float lineHeight = ImGui::GetTextLineHeightWithSpacing();
        float spacingY = ImGui::GetStyle().ItemSpacing.y;
        float framePadY = ImGui::GetStyle().FramePadding.y * 2.0f;
        float desiredHeight = (numItems > 0) ? (numItems * lineHeight + (numItems - 1) * spacingY + framePadY)  : lineHeight + framePadY;
        float maxHeight = 800.0f;
        desiredHeight = std::min(desiredHeight, maxHeight);

        if (ImGui::BeginListBox("##FileListBox", ImVec2(listBoxWidth, desiredHeight))) {
            for (int i = 0; i < (int)filenames.size(); i++)  {
                bool isSelected = (m_selectedIndex == i);

                // Render a selectable item
                if (ImGui::Selectable(filenames[i].c_str(), isSelected)) {
                    m_selectedIndex = i;
                }

                // Set the focus on the newly selected item
                if (isSelected)  {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }

        //ImGui::InputText("##Openfile", m_textBuffer, IM_ARRAYSIZE(m_textBuffer));

        // Compute space to center 2 buttons side by side
        float availWidth = ImGui::GetContentRegionAvail().x;
        float spacing = ImGui::GetStyle().ItemSpacing.x;
        float totalButtonsWidth = (buttonWidth * 2.0f) + spacing;
        float offsetX = (availWidth - totalButtonsWidth) * 0.5f;
        
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 8.0f));

        // "Create" button
        if (ImGui::Button("Open", ImVec2(buttonWidth, 0.0f))) {
            if (m_callback && m_selectedIndex >= 0 && m_selectedIndex < filenames.size()) {
                m_callback(filenames[m_selectedIndex]);
            }
            Close();
            ImGui::CloseCurrentPopup();
        }
        // "Cancel" button
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
            Close();
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
}


bool OpenFileWindow::IsVisible() {
    return m_isVisible;
}




// Button

EditorButton::EditorButton(const std::string& text, void (*callback)(), EditorWindow* parentWindow) {
    std::string padding = "       ";
    m_text = padding + text + padding;
    m_callback = callback;
    m_parentWindow = parentWindow;
}

void EditorButton::CreateImguiElement() {
    if (ImGui::Button(m_text.c_str())) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        if (m_callback) {
            m_callback();
        }
        if (m_parentWindow) {
            m_parentWindow->Close();
        }
    }
}

void EditorButton::CreateCenteredImguiElement(int centerXPos) {
    glm::ivec2 buttonSize = CalculateSize();
    float leftX = centerXPos - (buttonSize.x * 0.5f);
    ImGui::SetCursorPosX(leftX); 
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    if (ImGui::Button(m_text.c_str(), ImVec2(static_cast<float>(buttonSize.x), 0))) {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
        if (m_callback) {
            m_callback();
        }
        if (m_parentWindow) {
            m_parentWindow->Close();
        }
    }
    ImGui::PopStyleVar();
}

glm::ivec2 EditorButton::CalculateSize() {
    ImVec2 textSize = ImGui::CalcTextSize(m_text.c_str());
    ImVec2 padding = ImGui::GetStyle().FramePadding;
    int width = static_cast<int>(textSize.x + padding.x * 2);
    int height = static_cast<int>(textSize.y + padding.y * 2);
    return glm::ivec2(width, height);
}

void EditorWindow::SetTitle(const std::string& title) {
    m_title = title;
}

void EditorWindow::AddButton(std::string text, void (*callback)()) {
    EditorButton button = EditorButton(text, callback, this);
    m_buttons.emplace_back(button);
}

void EditorWindow::CreateImGuiElements() {
    int extraPaddingX = 80;
    int extraPaddingY = 20;

    ImGuiIO& io = ImGui::GetIO(); 
    ImVec2 windowPadding = ImGui::GetStyle().WindowPadding;

    // Calculate required width
    float buttonMaxWidth = 0.0f;
    for (EditorButton& button : m_buttons) {
        float width = static_cast<float>(button.CalculateSize().x);
        buttonMaxWidth = std::max(buttonMaxWidth, width);
    }
    float finalWindowWidth = buttonMaxWidth + (windowPadding.x * 2.0f) + extraPaddingX;

    // Center the window on the screen
    ImGui::SetNextWindowPos(
        ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );
    ImGui::SetNextWindowSize(ImVec2(finalWindowWidth, 0.0f), ImGuiCond_Always);

    // Create the window
    ImGui::Begin(m_title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    int centerXPos = ImGui::GetWindowSize().x * 0.5f;

    // Top padding
    ImGui::Dummy(ImVec2(0, extraPaddingY));

    // Create elements
    for (EditorButton& button : m_buttons) {
        button.CreateCenteredImguiElement(centerXPos);
    }

    // Bottom padding
    ImGui::Dummy(ImVec2(0, extraPaddingY));

    // End the window
    ImGui::End();
}

void EditorWindow::Show() {
    m_visible = true;
}

void EditorWindow::Close() {
    m_visible = false;
}

void EditorWindow::ToggleOpenState() {
    Audio::PlayAudio(AUDIO_SELECT, 1.0f);
    m_visible = !m_visible;
}

bool EditorWindow::IsVisible() {
    return m_visible;
}

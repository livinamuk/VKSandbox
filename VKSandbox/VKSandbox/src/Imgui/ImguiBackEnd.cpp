#include "ImguiBackend.h"
#include "BackEnd/GLFWIntegration.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Imgui/EditorImgui.h"
#include "Input/Input.h"
#include "Util.h"

#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <unordered_map>

namespace ImGuiBackend {

    void UpdateImGuiMouseState();
    void UpdateImGuiKeyboardState();    
    void UpdateStyle();

    uint32_t g_fileMenuHeight = 0;

    void Init() {
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

        io.Fonts->Clear();        
        std::string fontPath = "res/fonts/ttf/ClearSans-Regular.ttf";
        float fontSize = 34.0f;
        ImFont* customFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize);
        io.FontGlobalScale = 1.0f;
        io.Fonts->Build();

        ImGui::GetMainViewport()->Size = io.DisplaySize;
        ImGui::GetMainViewport()->Pos = ImVec2(0, 0);

        ImGui_ImplOpenGL3_Init();
    }

    void Update() {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)BackEnd::GetCurrentWindowWidth(), (float)BackEnd::GetCurrentWindowHeight());

        UpdateImGuiMouseState();
        UpdateImGuiKeyboardState();
        UpdateStyle();
    }

    struct KeyState {
        bool pressed;         // Current state (held down)
        double lastEventTime; // Time when the last event was added
        bool firstPress;      // true if this is the first event for this key
    };

    void UpdateImGuiKeyboardState() {
        ImGuiIO& io = ImGui::GetIO();
        static std::unordered_map<int, ImGuiKey> keyMap;
        static std::unordered_map<int, KeyState> keyStates;

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
                {HELL_KEY_LEFT, ImGuiKey_LeftArrow}, {HELL_KEY_RIGHT, ImGuiKey_RightArrow},
                {HELL_KEY_UP, ImGuiKey_UpArrow}, {HELL_KEY_DOWN, ImGuiKey_DownArrow},
                {HELL_KEY_ESCAPE, ImGuiKey_Escape},      // broken
                {HELL_KEY_CAPS_LOCK, ImGuiKey_CapsLock}, // broken
                {HELL_KEY_LEFT_SHIFT, ImGuiKey_LeftShift},
                {HELL_KEY_PERIOD, ImGuiKey_Period},
                {HELL_KEY_DELETE, ImGuiKey_Delete}
            };
        }

        double initialDelay = 0.4;    // seconds before repeat starts
        double repeatInterval = 0.035; // seconds between repeated events

        // Input keys
        for (const auto& [hellKey, imguiKey] : keyMap) {
            bool isDown = Input::KeyDown(hellKey);
            KeyState& state = keyStates[hellKey];

            // Get the current time in seconds.
            double currentTime = Util::GetCurrentTime();
            bool sendEvent = false;

            if (isDown && !state.pressed) {
                // First press
                state.pressed = true;
                state.firstPress = true;
                state.lastEventTime = currentTime;
                sendEvent = true;
            }
            else if (isDown && state.pressed) {
                double elapsed = currentTime - state.lastEventTime;
                if (state.firstPress && elapsed >= initialDelay) {
                    sendEvent = true;
                    state.lastEventTime = currentTime;
                    state.firstPress = false;
                }
                else if (!state.firstPress && elapsed >= repeatInterval) {
                    sendEvent = true;
                    state.lastEventTime = currentTime;
                }
            }
            else if (!isDown && state.pressed) {
                // Key released
                state.pressed = false;
                // Optionally send a "key up" event:
                io.AddKeyEvent(imguiKey, false);
            }

            // Now send a "key down" event if necessary.
            // (Note: You may need to adjust how you handle repeated events.)
            io.AddKeyEvent(imguiKey, sendEvent);

            if (sendEvent) {
                // Optionally add input characters for textual keys.
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

    void UpdateImGuiMouseState() {
        int mouseX = Input::GetMouseX();
        int mouseY = Input::GetMouseY();
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(mouseX, mouseY);
        io.MouseDown[0] = Input::LeftMouseDown();
        io.MouseDown[1] = Input::GetMouseWheelValue() != 0;
        io.MouseDown[2] = Input::RightMouseDown();
    }

    void UpdateStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.FrameRounding = 0.0f;
        style.GrabRounding = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.TabRounding = 0.0f;
        style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4f, 0.4f, 0.4f, 1.00f);
        
        // Window
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f); 

        // Buttons
        style.Colors[ImGuiCol_Button] = ImVec4(0.35f, 0.38f, 0.47f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.31f, 0.33f, 0.42f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.27f, 0.29f, 0.39f, 1.00f);

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

        style.Colors[ImGuiCol_Header] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
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

        // Scrollbar
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
        style.ScrollbarSize = 20.0f;

        // Slider
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        style.GrabMinSize = 20.0f;

        //style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);   // Red grab
        //style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        //style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

        
        // Zero padding. It's all handed manually with pop/push
        /*style.WindowPadding = ImVec2(0.0f, 0.0f);
        style.FramePadding = ImVec2(0.0f, 0.0f);
        style.ItemSpacing = ImVec2(0.0f, 0.0f);

        style.WindowPadding = ImVec2(0.0f, 0.0f);
        style.FramePadding = ImVec2(0.0f, 0.0f);
        style.CellPadding = ImVec2(0.0f, 0.0f);
        style.ItemSpacing = ImVec2(0.0f, 0.0f);
        style.ItemInnerSpacing = ImVec2(0.0f, 0.0f);
        style.TouchExtraPadding = ImVec2(0.0f, 0.0f);

        // Indentation, scrollbar, and grab sizes
        style.IndentSpacing = 0.0f;
        style.ScrollbarSize = 0.0f;
        style.GrabMinSize = 0.0f;

        // Border sizes
        style.WindowBorderSize = 0.0f;
        style.ChildBorderSize = 0.0f;
        style.PopupBorderSize = 0.0f;
        style.FrameBorderSize = 0.0f;
        style.TabBorderSize = 0.0f;

        // Rounding
        style.WindowRounding = 0.0f;
        style.ChildRounding = 0.0f;
        style.FrameRounding = 0.0f;
        style.PopupRounding = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.GrabRounding = 0.0f;
        style.TabRounding = 0.0f;
        */

        // Misc
        style.WindowMenuButtonPosition = ImGuiDir_None; // Removes the window menu button
        // style.WindowTitleAlign      = ImVec2(0.0f, 0.5f); // If you want to shift the window title
        // style.LogSliderDeadzone    = 0.0f;               // For slider snapping

        ImGuiIO& io = ImGui::GetIO();
        //io.FontGlobalScale = 2.0f; // Scale everything up
    }

    void RecordFileMenuHeight(uint32_t height) {
        g_fileMenuHeight = height;
    }

    bool ImGuiOwnsMouse() {
        return (EditorImGui::AnyElementHasHover() || EditorImGui::FloatingWindowsAreVisible());
    }

    uint32_t GetFileMenuHeight() {
        return g_fileMenuHeight;
    }
}
#include "Game.h"
#include "HellTypes.h"


#include "../Renderer/Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Backend/Backend.h"
#include "../API/OpenGL/GL_backend.h"
#include "../API/OpenGL/GL_renderer.h"
#include "../Core/Audio.h"
#include "../Core/Scene.h"
#include "../Player/Player.h"
#include "../Editor/Editor.h"
#include "../File/File.h"
#include "../Input/Input.h"
#include "../Renderer/Types/Viewport.h"
#include "../UI/UIBackEnd.h"
#include "../Tools/ImageTools.h"

namespace Game {

    int g_currentLocalPlayerIndex = 0;
    float g_deltaTime = 0;
    std::vector<Player> g_players;
    std::vector<Viewport> g_viewports;
    std::vector<uint16_t> g_activeViewportPlayerIndices;
    std::vector<uint16_t> g_localPlayerIndices;
    std::vector<uint16_t> g_onlinePlayerIndices;
    SplitscreenMode g_splitscreenMode = SplitscreenMode::FULLSCREEN;
    
    void AddPlayer(glm::vec3 position, glm::vec3 rotation) {
        Player& player = g_players.emplace_back();
        player.Init(position, rotation, g_viewports.size());
        Viewport& viewport = g_viewports.emplace_back(); 
        viewport.SetPerspective(1.0f, NEAR_PLANE, FAR_PLANE);
    }

    void AddOnlinePlayer(glm::vec3 position, glm::vec3 rotation) {
        Player& player = g_players.emplace_back();
        player.Init(position, rotation, -1);
    }

    void Create() {
        g_localPlayerIndices.clear();
        g_onlinePlayerIndices.clear();

        Scene::Init();
        Scene::CreateGameObjects();

        AddPlayer(glm::vec3(3.18f, 0.55f, 6.06f), glm::vec3(-0.40f, 3.34f, 0.0f));
        AddPlayer(glm::vec3(0.18f, 0.55f, 2.06f), glm::vec3(-0.40f, 3.34f, 0.0f));
        AddPlayer(glm::vec3(2.18f, 0.55f, 4.06f), glm::vec3(-0.40f, 3.34f, 0.0f));
        //AddPlayer(glm::vec3(1.18f, 0.55f, 0.06f), glm::vec3(-0.40f, 3.34f, 0.0f));
        AddOnlinePlayer(glm::vec3(1.18f, 0.55f, 0.06f), glm::vec3(-0.40f, 3.34f, 0.0f));
        AddOnlinePlayer(glm::vec3(1.18f, 0.55f, 0.06f), glm::vec3(-0.40f, 3.34f, 0.0f));

        // Cache local and online player indices
        for (int i = 0; i < g_players.size(); i++) {
            Player* player = GetPlayerByIndex(i);
            if (player->IsLocal()) {
                g_localPlayerIndices.push_back(i);
            } 
            else if (player->IsOnline()) {
                g_onlinePlayerIndices.push_back(i);
            }
        }

        SetSplitscreenMode(SplitscreenMode::FULLSCREEN);

        std::cout << "Created players\n";
    }

    void Update() {
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        g_deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        if (Input::KeyPressed(HELL_KEY_ESCAPE)) {
            BackEnd::ForceCloseWindow();
        }
        if (Input::KeyPressed(HELL_KEY_F)) {
            BackEnd::ToggleFullscreen();
        }
        if (Input::KeyPressed(HELL_KEY_H)) {
            Renderer::HotloadShaders();
        }
        if (Input::KeyPressed(HELL_KEY_TAB)) {
            Editor::ToggleOpenState();
        }
        if (Input::KeyPressed(HELL_KEY_V)) {
            NextSplitScreenMode();
        }
        if (Input::KeyPressed(HELL_KEY_C)) {
            g_currentLocalPlayerIndex++;
            g_currentLocalPlayerIndex = g_currentLocalPlayerIndex % GetLocalPlayerCount();
        }

        std::string debugText = "Mousepick: ";
        debugText += std::to_string(BackEnd::GetMousePickR()) + ", ";
        debugText += std::to_string(BackEnd::GetMousePickG()) + "\n";
        debugText += "\n";
        debugText += std::to_string(Game::GetDeltaTime()) + "\n";
        debugText += "\n";
       // debugText += Debug::GetText();

        // Update UI
        glm::ivec2 viewportSize = glm::ivec2(1920 / 1080);
        UIBackEnd::BlitText("Might leave in a body bag,", "StandardFont", 0, 0, 2.0f);
        UIBackEnd::BlitText("Never in cuffs.", "StandardFont", 0, 30, 2.0f);
        UIBackEnd::BlitText(debugText, "StandardFont", 0, 90, 2.0f);
        UIBackEnd::Update();


        for (Player& player : g_players) {
            //player.Update(g_deltaTime);
        }

        g_activeViewportPlayerIndices.clear();
        for (int playerIndex : g_localPlayerIndices) {
            Player* player = GetPlayerByIndex(playerIndex);
            if (player->IsViewportVisible()) {
                g_activeViewportPlayerIndices.push_back(playerIndex);
            }
        }

        std::cout << "\n";
        for (auto i : g_activeViewportPlayerIndices) {
            std::cout << i << " ";
        }
        std::cout << "\n";

        uint16_t playerIndex = g_localPlayerIndices[g_currentLocalPlayerIndex];
        g_players[playerIndex].Update(g_deltaTime);
    }

    float GetDeltaTime() {
        return g_deltaTime;
    }

    Player* GetPlayerByIndex(int index) {
        if (index >= 0 && index < g_players.size()) {
            return &g_players[index];
        }
        else {
            std::cout << "Game::GetPlayerByIndex(int index) failed. " << index << " out of range of size " << g_players.size() << "\n";
            return nullptr;
        }
    }

    void NextSplitScreenMode() {
        int nextSplitscreenMode = ((int)(g_splitscreenMode) + 1) % ((int)(SplitscreenMode::SPLITSCREEN_MODE_COUNT));
        SetSplitscreenMode((SplitscreenMode)nextSplitscreenMode);
        std::cout << "Splitscreen mode: " << nextSplitscreenMode << "\n";
    }

    void SetSplitscreenMode(SplitscreenMode mode) {
        g_splitscreenMode = mode;
        if (g_splitscreenMode == SplitscreenMode::FULLSCREEN) {
            g_viewports[0].SetPosition(glm::vec2(0.0f, 0.0f));
            g_viewports[0].SetSize(glm::vec2(1.0f, 1.0f));
        }
        else if (g_splitscreenMode == SplitscreenMode::TWO_PLAYER) {
            g_viewports[0].SetPosition(glm::vec2(0.0f, 0.5f)); // Top
            g_viewports[1].SetPosition(glm::vec2(0.0f, 0.0f)); // Bottom
            g_viewports[0].SetSize(glm::vec2(1.0f, 0.5f));
            g_viewports[1].SetSize(glm::vec2(1.0f, 0.5f));
        }
        else if (g_splitscreenMode == SplitscreenMode::FOUR_PLAYER) {
            g_viewports[0].SetPosition(glm::vec2(0.0f, 0.5f)); // Top-left
            g_viewports[1].SetPosition(glm::vec2(0.5f, 0.5f)); // Top-right
            g_viewports[2].SetPosition(glm::vec2(0.0f, 0.0f)); // Bottom-left
            g_viewports[3].SetPosition(glm::vec2(0.5f, 0.0f)); // Bottom-right
            g_viewports[0].SetSize(glm::vec2(0.5f, 0.5f));
            g_viewports[1].SetSize(glm::vec2(0.5f, 0.5f));
            g_viewports[2].SetSize(glm::vec2(0.5f, 0.5f));
            g_viewports[3].SetSize(glm::vec2(0.5f, 0.5f));
        }
    }

    const SplitscreenMode& GetSplitscreenMode() {
        return g_splitscreenMode;
    }

    uint16_t GetPlayerCount() {
        return g_players.size();
    }

    uint16_t GetActiveViewportCount() {
        if (g_splitscreenMode == SplitscreenMode::FULLSCREEN) {
            return std::min(GetLocalPlayerCount(), (uint16_t)1);
        }
        else if (g_splitscreenMode == SplitscreenMode::TWO_PLAYER) {
            return std::min(GetLocalPlayerCount(), (uint16_t)2);
        }
        else if (g_splitscreenMode == SplitscreenMode::FOUR_PLAYER) {
            return std::min(GetLocalPlayerCount(), (uint16_t)4);
        }
    }

    uint16_t GetLocalPlayerCount() {
        return g_localPlayerIndices.size();
    }

    uint16_t GetOnlinePlayerCount() {
        return g_onlinePlayerIndices.size();
    }

    Viewport* GetViewportByIndex(int16_t index) {
        if (index >= 0 && index < g_viewports.size()) {
            return &g_viewports[index];
        }
        else {
            std::cout << "Game::GetViewportByIndex(int index) failed. " << index << " out of range of size " << g_viewports.size() << "\n";
            return nullptr;
        }
    }

    const std::vector<uint16_t>& GetActiveViewportPlayerIndices() {
        return g_activeViewportPlayerIndices;
    }
}
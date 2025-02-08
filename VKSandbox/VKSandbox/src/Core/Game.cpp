#include "Game.h"
#include "HellTypes.h"


#include "../Renderer/Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Backend/Backend.h"
#include "../Core/Audio.h"
#include "../Core/Scene.h"
#include "../Player/Player.h"
#include "../Editor/Editor.h"
#include "../File/File.h"
#include "../Input/Input.h"
#include "../Viewport/ViewportManager.h"
#include "../UI/UIBackEnd.h"
#include "../Tools/ImageTools.h"

namespace Game {
    float g_deltaTime = 0;
    std::vector<Player> g_localPlayers;
    std::vector<Player> g_onlinePlayers;
    SplitscreenMode g_splitscreenMode = SplitscreenMode::FULLSCREEN;

    void UpdateLazyKeypresses();

    void AddLocalPlayer(glm::vec3 position, glm::vec3 rotation) {
        if (g_localPlayers.size() == 4) {
            return;
        }
        Player& player = g_localPlayers.emplace_back();
        player.Init(position, rotation, g_localPlayers.size() - 1);
    }

    void AddOnlinePlayer(glm::vec3 position, glm::vec3 rotation) {
        Player& player = g_onlinePlayers.emplace_back();
        player.Init(position, rotation, -1);
    }

    void Create() {
        Scene::Init();
        Scene::CreateGameObjects();

        // Create players
        AddLocalPlayer(glm::vec3(-2.09f, 1.45f, 0.68f), glm::vec3(-0.30f, -1.32f, 0.0f));
        AddLocalPlayer(glm::vec3(-2.09f, 1.45f, 0.68f), glm::vec3(-0.30f, -1.32f, 0.0f));
        AddLocalPlayer(glm::vec3(-2.09f, 1.45f, 0.68f), glm::vec3(-0.30f, -1.32f, 0.0f));
        AddLocalPlayer(glm::vec3(-2.09f, 1.45f, 0.68f), glm::vec3(-0.30f, -1.32f, 0.0f));

        SetPlayerKeyboardAndMouseIndex(0, 0, 0);
        SetPlayerKeyboardAndMouseIndex(1, 1, 1);
        SetPlayerKeyboardAndMouseIndex(2, 1, 1);
        SetPlayerKeyboardAndMouseIndex(3, 1, 1);

        AddLocalPlayer(glm::vec3(0.0f, 1.2f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f));
        AddLocalPlayer(glm::vec3(0.0f, 2.5f, 0.0f), glm::vec3(-HELL_PI / 2, -HELL_PI / 2, 0.0f));
        AddLocalPlayer(glm::vec3(-2.5f, 1.2f, 0.0f), glm::vec3(0.0f, -HELL_PI / 2, 0.0f));
        //AddOnlinePlayer(glm::vec3(1.18f, 0.55f, 0.06f), glm::vec3(-0.40f, 3.34f, 0.0f));
        //AddOnlinePlayer(glm::vec3(1.18f, 0.55f, 0.06f), glm::vec3(-0.40f, 3.34f, 0.0f));

        // Give control to player 0 only
        for (Player& player : g_localPlayers) {
            player.DisableControl();
        }
        g_localPlayers[0].EnableControl();

        SetSplitscreenMode(SplitscreenMode::FULLSCREEN);

        std::cout << "Created players\n";
    }

    void Update() {
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        g_deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        UpdateLazyKeypresses();

        std::string debugText = "Mousepick: ";
        debugText += std::to_string(BackEnd::GetMousePickR()) + ", ";
        debugText += std::to_string(BackEnd::GetMousePickG()) + "\n";
        debugText += "\n";
        debugText += std::to_string(Game::GetDeltaTime()) + "\n";
        debugText += "\n";
        // debugText += Debug::GetText();

         // Update UI
         //UIBackEnd::BlitText("Might leave in a body bag,", "StandardFont", 0, 0, 2.0f);
         //UIBackEnd::BlitText("Never in cuffs.", "StandardFont", 0, 30, 2.0f);
         //UIBackEnd::BlitText(debugText, "StandardFont", 0, 90, 2.0f);


        for (Player& player : g_localPlayers) {
            player.Update(g_deltaTime);
        }
    }

    void UpdateLazyKeypresses() {

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
            ViewportManager::UpdateViewports();
        }
        if (Input::KeyPressed(HELL_KEY_GRAVE_ACCENT)) {
            Audio::PlayAudio(AUDIO_SELECT, 1.00f);
            Debug::ToggleDebugText();
        }
        if (!Editor::IsOpen()) {
            if (Input::KeyPressed(HELL_KEY_V)) {
                NextSplitScreenMode();
            }
            if (Input::KeyPressed(HELL_KEY_1) && GetLocalPlayerCount() >= 1) {
                for (Player& player : g_localPlayers) {
                    player.DisableControl();
                }
                g_localPlayers[0].EnableControl();
            }
            if (Input::KeyPressed(HELL_KEY_2) && GetLocalPlayerCount() >= 2) {
                for (Player& player : g_localPlayers) {
                    player.DisableControl();
                }
                g_localPlayers[1].EnableControl();
            }
            if (Input::KeyPressed(HELL_KEY_3) && GetLocalPlayerCount() >= 3) {
                for (Player& player : g_localPlayers) {
                    player.DisableControl();
                }
                g_localPlayers[2].EnableControl();
            }
            if (Input::KeyPressed(HELL_KEY_4) && GetLocalPlayerCount() >= 4) {
                for (Player& player : g_localPlayers) {
                    player.DisableControl();
                }
                g_localPlayers[3].EnableControl();
            }
        }
    }


    float GetDeltaTime() {
        return g_deltaTime;
    }

    Player* GetLocalPlayerByIndex(uint32_t index) {
        if (index >= 0 && index < g_localPlayers.size()) {
            return &g_localPlayers[index];
        }
        else {
            std::cout << "Game::GetPlayerByIndex(int index) failed. " << index << " out of range of size " << g_localPlayers.size() << "\n";
            return nullptr;
        }
    }

    Camera* GetLocalPlayerCameraByIndex(uint32_t index) {
        if (index >= 0 && index < g_localPlayers.size()) {
            return &g_localPlayers[index].GetCamera();
        }
        else {
            std::cout << "Game::GetLocalPlayerCameraByIndex(int index) failed. " << index << " out of range of local player count " << g_localPlayers.size() << "\n";
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
        ViewportManager::UpdateViewports();
    }

    const SplitscreenMode& GetSplitscreenMode() {
        return g_splitscreenMode;
    }

    int32_t GetLocalPlayerCount() {
        return g_localPlayers.size();
    }

    int32_t GetOnlinePlayerCount() {
        return g_onlinePlayers.size();
    }

    int32_t GetActiveViewportCount() {
        if (g_splitscreenMode == SplitscreenMode::FULLSCREEN) {
            return std::min(GetLocalPlayerCount(), (int32_t)1);
        }
        else if (g_splitscreenMode == SplitscreenMode::TWO_PLAYER) {
            return std::min(GetLocalPlayerCount(), (int32_t)2);
        }
        else if (g_splitscreenMode == SplitscreenMode::FOUR_PLAYER) {
            return std::min(GetLocalPlayerCount(), (int32_t)4);
        } 
        else{
            return 1;
        }
    }

    void SetPlayerKeyboardAndMouseIndex(int playerIndex, int keyboardIndex, int mouseIndex) {
        if (playerIndex >= 0 && playerIndex < g_localPlayers.size()) {
            g_localPlayers[playerIndex].SetKeyboardIndex(keyboardIndex);
            g_localPlayers[playerIndex].SetMouseIndex(mouseIndex);
        }
    }
}
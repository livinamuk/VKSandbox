#include "Game.h"
#include "HellTypes.h"


#include "../Renderer/Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Backend/Backend.h"
#include "Core/Audio.h"
#include "Editor/Editor.h"
#include "File/File.h"
#include "Input/Input.h"
#include "Player/Player.h"
#include "Physics/Physics.h"
#include "Tools/ImageTools.h"
#include "UI/UIBackEnd.h"
#include "Viewport/ViewportManager.h"

// Get me out of here
#include "World/World.h"
// Get me out of here

namespace Game {
    float g_deltaTime = 0;
    float g_totalTime = 0;
    double g_deltaTimeAccumulator = 0.0;
    double g_fixedDeltaTime = 1.0 / 60.0;
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


        //Scene::Init();
        //Scene::CreateGameObjects();

        // Create players
        AddLocalPlayer(glm::vec3(12.82, -4.5f, 18.27f), glm::vec3(-0.13f, -1.46f, 0.0f));
        AddLocalPlayer(glm::vec3(15.21, -4.5f, 19.57), glm::vec3(-0.49f, -0.74f, 0.0f));
        AddLocalPlayer(glm::vec3(10, -4.5f, 22), glm::vec3(-0.30f, -4.5f, 0.0f));
        AddLocalPlayer(glm::vec3(12, -4.5f, 22), glm::vec3(-0.30f, -4.5f, 0.0f));

        SetPlayerKeyboardAndMouseIndex(0, 0, 0);
        SetPlayerKeyboardAndMouseIndex(1, 1, 1);
        SetPlayerKeyboardAndMouseIndex(2, 1, 1);
        SetPlayerKeyboardAndMouseIndex(3, 1, 1);

        SetSplitscreenMode(SplitscreenMode::FULLSCREEN);
        //SetSplitscreenMode(SplitscreenMode::TWO_PLAYER);

        std::cout << "Created players\n";
        Audio::PlayAudio("Glock_Equip.wav", 0.5f);
    }

    void Update() {

        if (Input::KeyPressed(HELL_KEY_SPACE)) {
            World::CreateHardCodedTestSector();
        }            
      
        if (Input::KeyPressed(HELL_KEY_BACKSPACE)) {
            SectorData sectorData = World::LoadSectorData("res/sectors/TestSector.json");
            World::LoadSingleSector(sectorData);
        }

        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        g_deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        g_deltaTimeAccumulator += g_deltaTime;

        // Total time
        g_totalTime += g_deltaTime;
        if (g_totalTime > TIME_WRAP) {
            g_totalTime -= TIME_WRAP; // Keep it continuous
        }

        // Physics
        while (g_deltaTimeAccumulator >= g_fixedDeltaTime) {
            g_deltaTimeAccumulator -= g_fixedDeltaTime;
            Physics::StepPhysics(g_fixedDeltaTime);
        }


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


        // Editor select menu open?
        if (Editor::IsEditorSelectMenuVisible()) {
            for (Player& player : g_localPlayers) {
                player.DisableControl();
            }
        } else{
            for (Player& player : g_localPlayers) {
                player.EnableControl();
            }
        }

        for (Player& player : g_localPlayers) {
            player.Update(g_deltaTime);
        }
    }

    float GetDeltaTime() {
        return g_deltaTime;
    }

    float GetTotalTime() {
        return g_totalTime;
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
        //ViewportManager::UpdateViewports();
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


    void PlayFootstepIndoorAudio() {
        const std::vector<const char*> indoorFootstepFilenames = {
                    "player_step_1.wav",
                    "player_step_2.wav",
                    "player_step_3.wav",
                    "player_step_4.wav",
        };
        int random = rand() % 4;
        Audio::PlayAudio(indoorFootstepFilenames[random], 0.5f);
    }

    void PlayFootstepOutdoorAudio() {
        const std::vector<const char*> indoorFootstepFilenames = {
                "player_step_grass_1.wav",
                "player_step_grass_2.wav",
                "player_step_grass_3.wav",
                "player_step_grass_4.wav",
        };
        int random = rand() % 4;
        Audio::PlayAudio(indoorFootstepFilenames[random], 0.5f);
    }
}
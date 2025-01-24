#include "Game.h"
#include "HellTypes.h"


#include "../Renderer/Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Backend/Backend.h"
#include "../API/OpenGL/GL_backend.h"
#include "../API/OpenGL/GL_renderer.h"
#include "../Core/Audio.h"
#include "../Core/Scene.hpp"
#include "../Core/Camera.h"
#include "../Editor/EditorNew.h"
#include "../Input/Input.h"
#include "../File/File.h"
#include "../UI/UIBackEnd.h"
#include "../Tools/ImageTools.h"

namespace Game {

    float g_deltaTime = 0;

    void Create() {
        Scene::Init();
        Camera::Init(BackEnd::GetWindowPointer());
        Scene::CreateGameObjects();
    }

    void Update() {
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        g_deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        OpenGLBackEnd::UpdateTextureBaking();
        Scene::SetMaterials();
        Camera::Update(g_deltaTime);
        Scene::Update(g_deltaTime);
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
            EditorNew::ToggleOpenState();
        }

        std::string debugText = "Mousepick: ";
        debugText += std::to_string(BackEnd::GetMousePickR()) + ", ";
        debugText += std::to_string(BackEnd::GetMousePickG()) + "\n";
        debugText += "\n";
        debugText += std::to_string(Game::GetDeltaTime()) + "\n";

        // Update UI
        glm::ivec2 viewportSize = glm::ivec2(1920 / 1080);
        //UIBackEnd::BlitText("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", "StandardFont", 60, 60, viewportSize, 2.0f);
        //UIBackEnd::BlitTexture("ui_test", glm::ivec2(0, 0), viewportSize, Alignment::TOP_LEFT);
        UIBackEnd::BlitText("Might leave in a body bag,", "StandardFont", 0, 0, 2.0f);
        UIBackEnd::BlitText("Never in cuffs.", "StandardFont", 0, 30, 2.0f);
        UIBackEnd::BlitText(debugText, "StandardFont", 0, 90, 2.0f);
        UIBackEnd::Update();
    }

    float GetDeltaTime() {
        return g_deltaTime;
    }
}
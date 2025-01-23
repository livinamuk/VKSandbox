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
#include "../Input/Input.h"
#include "../File/File.h"
#include "../UI/Textblitter.h"
#include "../Tools/ImageTools.h"

namespace Game {

    void Create() {
        Scene::Init();
        Camera::Init(BackEnd::GetWindowPointer());
        Scene::CreateGameObjects();
    }

    void Update() {
        static float deltaTime = 0;
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        OpenGLBackEnd::UpdateTextureBaking();
        Scene::SetMaterials();
        Camera::Update(deltaTime);
        Scene::Update(deltaTime);
        if (Input::KeyPressed(HELL_KEY_ESCAPE)) {
            BackEnd::ForceCloseWindow();
        }
        if (Input::KeyPressed(HELL_KEY_F)) {
            BackEnd::ToggleFullscreen();
        }
        if (Input::KeyPressed(HELL_KEY_H)) {
            Renderer::HotloadShaders();
        }
    }
}
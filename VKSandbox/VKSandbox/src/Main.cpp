/*

███    █▄  ███▄▄▄▄    ▄█        ▄██████▄   ▄█    █▄     ▄████████ ████████▄
███    ███ ███▀▀▀██▄ ███       ███    ███ ███    ███   ███    ███ ███   ▀███
███    ███ ███   ███ ███       ███    ███ ███    ███   ███    █▀  ███    ███
███    ███ ███   ███ ███       ███    ███ ███    ███  ▄███▄▄▄     ███    ███
███    ███ ███   ███ ███       ███    ███ ███    ███ ▀▀███▀▀▀     ███    ███
███    ███ ███   ███ ███       ███    ███ ███    ███   ███    █▄  ███    ███
███    ███ ███   ███ ███▌    ▄ ███    ███ ███    ███   ███    ███ ███   ▄███
████████▀   ▀█   █▀  █████▄▄██  ▀██████▀   ▀██████▀    ██████████ ████████▀

*/

#include "AssetManagement/AssetManager.h"
#include "Backend/Backend.h"
#include "Game/Game.h"
#include "Editor/EditorNew.h"
#include "Renderer/Renderer.h"
#include "UI/UIBackEnd.h"
#include <iostream>

int main() {
    std::cout << "We are all alone on life's journey, held captive by the limitations of human consciousness.\n";

    // Init the back-end, sub-systems, and the minimum to render loading screen
    if (!BackEnd::Init(API::OPENGL, 1920 * 1.5, 1080 * 1.5, WindowedMode::WINDOWED)) {
        std::cout << "BackEnd::Init() FAILED!\n";
        return -1;
    }

    // Program loop
    while (BackEnd::WindowIsOpen()) {

        BackEnd::BeginFrame();
        BackEnd::UpdateSubSystems();

        // Render loading screen
        if (!AssetManager::LoadingComplete()) {
            AssetManager::UpdateLoading();
            Renderer::RenderLoadingScreen();
        
            // Loading complete?
            if (AssetManager::LoadingComplete()) {
                Game::Create();
            }
        }
        // Update/render game
        else {
            Game::Update();
            EditorNew::Update();
            UIBackEnd::Update();
            Renderer::RenderGame();
        }

        BackEnd::EndFrame();
    }
    return 0;
}
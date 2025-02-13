#include "../GL_renderer.h" 
#include "../../GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"
#include "UI/UIBackEnd.h"

namespace OpenGLRenderer {

    void RenderLoadingScreen() {
        // Clear the swapchain
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render UI
        UIBackEnd::Update();
        UIPass();
    }
}
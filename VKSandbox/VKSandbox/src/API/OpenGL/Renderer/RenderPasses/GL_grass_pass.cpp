#include "API/OpenGL/Renderer/GL_renderer.h" 
#include "AssetManagement/AssetManager.h"
#include "Renderer/Types/GrassMesh.h"

#include "API/OpenGL/GL_backend.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"
#include "Input/Input.h"
#include "Core/Game.h"

namespace OpenGLRenderer {

    void GrassPassNew();
    void GrassPassOld();

    void GrassPass() {
        //GrassPassNew();
        GrassPassOld();
    }

    void GrassPassNew() {

        static OpenGLDetachedMesh grassMesh;

        Player* player = Game::GetLocalPlayerByIndex(0);
        Viewport* viewport = ViewportManager::GetViewportByIndex(0);
        Frustum& frustum = viewport->GetFrustum();

        frustum.GetFrustumCorners();


    }

    void GrassPassOld() {
        OpenGLShader* shader = GetShader("Grass");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        static GrassMesh grassMesh;
        if (Input::KeyPressed(HELL_KEY_SPACE) || grassMesh.glMesh.vertices.size() == 0) {
            grassMesh.Create();
        }

        glm::vec3 offset = glm::vec3(17, -4, 18);

        glm::mat4 modelMatrix;
        for (int x = 0; x < GRASS_SIZE; x++) {
            for (int z = 0; z < GRASS_SIZE; z++) {
                //   DrawPoint(grassMesh.m_bladePoints[x][z] + offset, RED);
            }
        }

        for (Vertex& v : grassMesh.glMesh.vertices) {
            // DrawPoint(v.position, BLUE);
        }



        shader->Use();

        UpdateDebugLinesMesh();
        UpdateDebugPointsMesh();

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);

            gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick", "WorldSpacePosition", "Emissive" });
            SetRasterizerState("GeometryPass_NonBlended");

            glm::mat4 projectionMatrix = viewportData[i].projection;
            glm::mat4 viewMatrix = viewportData[i].view;

            shader->SetMat4("projection", projectionMatrix);
            shader->SetMat4("view", viewMatrix);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("Perlin")->GetGLTexture().GetHandle());

            int grassGridSize = 50; // Number of patches per axis (200 - 20)
            float scale = 0.5f;

            shader->SetFloat("scale", scale);
            shader->SetInt("gridSize", grassGridSize);
            shader->SetVec3("basePosition", glm::vec3(10, -5, 10)); // Starting offset

            glBindVertexArray(grassMesh.glMesh.GetVAO());
            glDrawElementsInstanced(GL_TRIANGLE_STRIP, grassMesh.glMesh.GetIndexCount(), GL_UNSIGNED_INT, 0, grassGridSize * grassGridSize);
        }

    }
}
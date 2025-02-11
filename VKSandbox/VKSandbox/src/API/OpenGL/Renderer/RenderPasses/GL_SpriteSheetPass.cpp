#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "API/OpenGL/Renderer/GL_renderer_util.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"

namespace OpenGLRenderer {

    void SpriteSheetPass() {

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        OpenGLShader* shader = GetShader("SpriteSheet");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        gBuffer->Bind();
        gBuffer->DrawBuffer("FinalLighting");
        SetRasterizerState("SpriteSheetPass");
        shader->Use();

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRendererUtil::SetViewport(gBuffer, viewport);

            glm::mat4 projectionMatrix = viewportData[i].projection;
            glm::mat4 viewMatrix = viewportData[i].view;

            Transform transform;
            transform.position = glm::vec3(16, -3, 25);

            shader->SetMat4("projection", projectionMatrix);
            shader->SetMat4("view", viewMatrix);
            shader->SetMat4("model", transform.to_mat4());

            //Texture* texture = AssetManager::GetTextureByName("MuzzleFlash_4x4");
            Texture* texture = AssetManager::GetTextureByName("WaterSplash0_Color_4x4");
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture->GetGLTexture().GetHandle());


            // Test mesh
            Mesh* mesh = AssetManager::GetMeshByModelNameMeshIndex("Quad", 0);
            glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());
            glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), mesh->baseVertex);

        }
    }
}
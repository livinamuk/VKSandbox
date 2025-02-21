#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "AssetManagement/AssetManager.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "Util/Util.h"

#include "Core/Game.h"

namespace OpenGLRenderer {

    void SpriteSheetPass() {

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        OpenGLShader* shader = GetShader("SpriteSheet");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        Mesh* mesh = AssetManager::GetMeshByModelNameMeshIndex("Quad", 0);

        gBuffer->Bind();
        gBuffer->DrawBuffer("FinalLighting");
        shader->Use();
        SetRasterizerState("SpriteSheetPass");

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);

            const std::vector<SpriteSheetRenderItem>& renderItems = Game::GetLocalPlayerByIndex(i)->GetSpriteSheetRenderItems();
            for (const SpriteSheetRenderItem& renderItem : renderItems) {

                Texture* texture = AssetManager::GetTextureByIndex(renderItem.textureIndex);

                shader->SetInt("u_rowCount", renderItem.rowCount);
                shader->SetInt("u_columnCount", renderItem.columnCount);
                shader->SetInt("u_frameIndex", renderItem.frameIndex);
                shader->SetInt("u_frameNextIndex", renderItem.frameIndexNext);
                shader->SetFloat("u_mixFactor", renderItem.mixFactor);
                shader->SetVec4("u_position", renderItem.position);
                shader->SetVec4("u_rotation", renderItem.rotation);
                shader->SetVec4("u_scale", renderItem.scale);
                shader->SetInt("u_isBillboard", renderItem.isBillboard);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture->GetGLTexture().GetHandle());
                glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid*)(mesh->baseIndex * sizeof(GLuint)), 1, mesh->baseVertex, i);
            }           
        }
    }
}
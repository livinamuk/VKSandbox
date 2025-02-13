#include "../GL_renderer.h" 
#include "../../GL_backend.h"
#include "../../Types/GL_heightmapMesh.h"
#include "../Config/Config.h"
#include "AssetManagement/AssetManager.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"

namespace OpenGLRenderer {

    void HeightMapPass() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
        OpenGLShader* imageDataShader = GetShader("HeightMapImageGeneration");
        OpenGLShader* vertexBufferShader = GetShader("HeightMapVertexGeneration");
        OpenGLShader* colorShader = GetShader("HeightMapColor");

        if (!gBuffer) return;
        if (!heightmapFBO) return;
        if (!imageDataShader) return;
        if (!vertexBufferShader) return;

        // Create vertex buffer if it doesn't exist
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
        if (heightMapMesh.GetVAO() == 0) {
            heightMapMesh.Create(HEIGHTMAP_SIZE);
        }

        // Create height map data
        imageDataShader->Use();
        glBindImageTexture(0, heightmapFBO->GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
        glDispatchCompute((heightmapFBO->GetWidth() + 7) / 8, (heightmapFBO->GetHeight() + 7) / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Write the data to a vertex buffer buffer
        vertexBufferShader->Use();
        glBindImageTexture(0, heightmapFBO->GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, heightMapMesh.GetVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, heightMapMesh.GetEBO());

        // Dispatch compute shader
        glDispatchCompute((HEIGHTMAP_SIZE + 15) / 16, (HEIGHTMAP_SIZE + 15) / 16, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick", "WorldSpacePosition" });
        colorShader->Use();

        Transform transform;
        transform.position = glm::vec3(0, -5, 0);
        transform.scale = glm::vec3(0.25f, 5.0, 0.25f);
        glm::mat4 modelMatrix = transform.to_mat4();
        glm::mat4 inverseModelMatrix = glm::inverse(modelMatrix);

        colorShader->SetMat4("modelMatrix", modelMatrix);
        colorShader->SetMat4("inverseModelMatrix", inverseModelMatrix);

        SetRasterizerState("GeometryPass_NonBlended");

        Material* material = AssetManager::GetDefaultMaterial();
        int materialIndex = AssetManager::GetMaterialIndexByName("Ground_MudVeg");
        material = AssetManager::GetMaterialByIndex(materialIndex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_basecolor)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_normal)->GetGLTexture().GetHandle());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_rma)->GetGLTexture().GetHandle());

        int indexCount = (HEIGHTMAP_SIZE - 1) * (HEIGHTMAP_SIZE - 1) * 6;
        int vertexCount = HEIGHTMAP_SIZE * HEIGHTMAP_SIZE;
        glBindVertexArray(heightMapMesh.GetVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);
                glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 0), 1, 0, i);
            }
        }
        glBindVertexArray(0);

        OpenGLBackEnd::ReadBackHeightmapMeshData();
    }
}
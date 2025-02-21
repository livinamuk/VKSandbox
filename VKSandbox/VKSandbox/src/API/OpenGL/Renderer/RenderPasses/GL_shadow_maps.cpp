#include "API/OpenGL/GL_backend.h"
#include "API/OpenGL/Renderer/GL_renderer.h"
#include "Core/Game.h"

// Get me out of here
#include "World/World.h"
// Get me out of here

namespace OpenGLRenderer {

    void RenderFlashLightShadowMaps();

    void RenderShadowMaps() {
        RenderFlashLightShadowMaps();
    }

    void RenderFlashLightShadowMaps() {
        OpenGLShader* shader = GetShader("ShadowMapGeometry");
        OpenGLFrameBuffer* frameBuffer = GetFrameBuffer("FlashlightShadowMap");
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();

        glEnable(GL_DEPTH_TEST);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        frameBuffer->Bind();
        frameBuffer->SetViewport();
        frameBuffer->ClearDepthAttachment();

        glm::mat4 lightProjectionView = Game::GetLocalPlayerByIndex(0)->GetFlashlightProjectionView();

        shader->Use();
        shader->SetMat4("u_projectionView", lightProjectionView);

        // Heightmap
        // Scene geometry
        glCullFace(GL_FRONT);
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());
        for (RenderItem& renderItem : World::GetRenderItems()) {
            shader->SetMat4("u_modelMatrix", renderItem.modelMatrix);
            Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
            glDrawElementsBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), mesh->baseVertex);
        }


        Transform transform;
        transform.position = glm::vec3(0, HEIGHTMAP_BEGIN_Y, 0);
        transform.scale = glm::vec3(HEIGHTMAP_SCALE_XZ, HEIGHTMAP_SCALE_Y, HEIGHTMAP_SCALE_XZ);
        glm::mat4 modelMatrix = transform.to_mat4();
        glm::mat4 inverseModelMatrix = glm::inverse(modelMatrix);
        shader->SetMat4("u_modelMatrix", modelMatrix);
        int indexCount = (HEIGHT_MAP_SIZE - 1) * (HEIGHT_MAP_SIZE - 1) * 6;
        int vertexCount = HEIGHT_MAP_SIZE * HEIGHT_MAP_SIZE;
        int i = 0; // viewport index !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        glBindVertexArray(heightMapMesh.GetVAO());
        glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 0), 1, 0, i);


        glCullFace(GL_BACK);
        glFinish();
    }
}
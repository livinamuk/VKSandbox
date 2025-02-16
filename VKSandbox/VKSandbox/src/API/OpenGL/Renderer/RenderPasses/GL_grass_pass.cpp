#include "API/OpenGL/Renderer/GL_renderer.h" 
#include "AssetManagement/AssetManager.h"
#include "Renderer/Types/GrassMesh.h"

#include "API/OpenGL/GL_backend.h"
#include "BackEnd/BackEnd.h"
#include "Config/Config.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Core/Game.h"
#include "Util/Util.h"

namespace OpenGLRenderer {

    OpenGLDetachedMesh g_grassMesh;
    GLuint g_indirectBuffer = 0;

#define GRASS_TILE_WORLDSPACE_SIZE 8.0
#define GRASS_TILE_SPACING 0.0155f

    void GenerateGrass(float xOffset, float zOffset);
    void RenderGrass();

    void GrassPass() {


        glFinish();
        glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* wipBuffer = GetFrameBuffer("WIP");
        BlitFrameBuffer(gBuffer, wipBuffer, "WorldSpacePosition", "WorldSpacePosition", GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);


        //glm::vec3 worldMin = glm::vec3(0, HEIGHTMAP_BEGIN_Y, 0);
        //glm::vec3 worldMax = glm::vec3(HEIGHTMAP_SIZE * HEIGHTMAP_SCALE_XZ, HEIGHTMAP_BEGIN_Y, HEIGHTMAP_SIZE * HEIGHTMAP_SCALE_XZ);

        int viewportIndex = 0;

        Viewport* viewport = ViewportManager::GetViewportByIndex(viewportIndex);
        Frustum& frustum = viewport->GetFrustum();        

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();
        glm::vec3 viewPos = viewportData[viewportIndex].inverseView[3];

        float yThreshold = 1.0f;
        float grassTileSize = GRASS_TILE_WORLDSPACE_SIZE;
        float y = HEIGHTMAP_BEGIN_Y;

        struct GrassTileData {
            float xOffset = 0;
            float zOffset = 0;
            float distanceFromCamera = 0;
        };

        std::vector<GrassTileData> grassTileDataList;

        for (int x = 0; x < 8; x++) {
            for (int z = 0; z < 8; z++) {
                float xOffset = x * GRASS_TILE_WORLDSPACE_SIZE;
                float zOffset = z * GRASS_TILE_WORLDSPACE_SIZE;
                glm::vec3 tileBoundsMin = glm::vec3(xOffset, y - yThreshold, zOffset);
                glm::vec3 tileBoundsMax = glm::vec3(xOffset + grassTileSize, y + yThreshold, zOffset + grassTileSize);
                AABB tileAabb = AABB(tileBoundsMin, tileBoundsMax);
                if (frustum.IntersectsAABB(tileAabb)) {

                    glm::vec3 center = (tileBoundsMin + tileBoundsMax) * glm::vec3(0.5f);
                    float distanceToCamera = glm::distance(viewPos, center);

                    GrassTileData& grassTileData = grassTileDataList.emplace_back();
                    grassTileData.xOffset = xOffset;
                    grassTileData.zOffset = zOffset;
                    grassTileData.distanceFromCamera = distanceToCamera;
                }
            }
        }

        // Sort tiles by closest to camera
        std::sort(grassTileDataList.begin(), grassTileDataList.end(),
            [](const GrassTileData& a, const GrassTileData& b) {
            return a.distanceFromCamera > b.distanceFromCamera;
        });

        // Generate and draw
        for (GrassTileData& grassTileData : grassTileDataList) {
            //glFinish();
            GenerateGrass(grassTileData.xOffset, grassTileData.zOffset);
            //glFinish();
            RenderGrass();
        }

        /*
        for (int x = 0; x < 8; x++) {

            for (int z = 0; z < 8; z++) {

                float xOffset = x * GRASS_TILE_WORLDSPACE_SIZE;
                float zOffset = z * GRASS_TILE_WORLDSPACE_SIZE;

                Viewport* viewport = ViewportManager::GetViewportByIndex(0);
                Frustum& frustum = viewport->GetFrustum();

                glm::vec3 tileBoundsMin = glm::vec3(xOffset, y - yThreshold, zOffset);
                glm::vec3 tileBoundsMax = glm::vec3(xOffset + grassTileSize, y + yThreshold, zOffset + grassTileSize);

                AABB tileAabb = AABB(tileBoundsMin, tileBoundsMax);

                if (frustum.IntersectsAABB(tileAabb)) {
                    GenerateGrass(xOffset, zOffset);
                    RenderGrass();
                   // DrawAABB(tileAabb, WHITE);
                    //std::cout << x << ": YES\n";
                }
                else {
                   // std::cout << x << ": NO\n";
                }
            }
        }*/
    }

    void GenerateGrass(float xOffset, float zOffset) {
        OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");

        int gridSize = static_cast<int>(GRASS_TILE_WORLDSPACE_SIZE / GRASS_TILE_SPACING) + 1;
        int bladeCount = gridSize * gridSize;
        int maxVertexCount = bladeCount * 6;
        int maxIndexCount = bladeCount * 12;

        // Allocate mesh memory
        if (g_grassMesh.GetVAO() == 0) {
            std::cout << "estimated: " << maxVertexCount << " vertices\n";
            std::cout << "estimated: " << maxIndexCount << " indices\n";
            g_grassMesh.AllocateBuffers(maxVertexCount, maxIndexCount);
        }

        // Create indirect buffer
        if (g_indirectBuffer == 0) {
            glGenBuffers(1, &g_indirectBuffer);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer);
            glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawIndexedIndirectCommand), NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        }

        // Zero out indirect buffer
        DrawIndexedIndirectCommand initialCmd;
        initialCmd.instanceCount = 1;
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer);
        glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawIndexedIndirectCommand), &initialCmd);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

        // Bind buffers
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, g_indirectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, g_grassMesh.GetVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, g_grassMesh.GetEBO());

        // Bind worldspace position texture (for occlusion culling)
        OpenGLFrameBuffer* wipBuffer = GetFrameBuffer("WIP");
        glBindTextureUnit(0, heightmapFBO->GetColorAttachmentHandleByName("Color"));
        glBindTextureUnit(1, wipBuffer->GetColorAttachmentHandleByName("WorldSpacePosition"));

        // Uniforms
        OpenGLShader* generationShader = GetShader("GrassGeneration");
        generationShader->Use();
        generationShader->SetInt("gridSize", gridSize);
        generationShader->SetFloat("spacing", GRASS_TILE_SPACING);
        generationShader->SetVec3("offset", glm::vec3(xOffset, 0.0f, zOffset));
        generationShader->SetFloat("u_heightMapWorldSpaceSize", HEIGHTMAP_SIZE * HEIGHTMAP_SCALE_XZ);
        

        

        // Dispatch compute shader
        const int workGroupSize = 16;
        int workGroupsX = (gridSize + workGroupSize - 1) / workGroupSize;
        int workGroupsY = (gridSize + workGroupSize - 1) / workGroupSize;
        glDispatchCompute(workGroupsX, workGroupsY, 1);
    }

    void RenderGrass() {

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLShader* geometryShader = GetShader("GrassToshima");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        Player* player = Game::GetLocalPlayerByIndex(0);
        Viewport* viewport = ViewportManager::GetViewportByIndex(0);
        Frustum& frustum = viewport->GetFrustum();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByName("Perlin")->GetGLTexture().GetHandle());

        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT); 
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        geometryShader->Use();

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            OpenGLRenderer::SetViewport(gBuffer, viewport);

            gBuffer->Bind();
            gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick", "WorldSpacePosition", "Emissive" });
            SetRasterizerState("GeometryPass_NonBlended");
           // glDisable(GL_DEPTH_TEST);

            glm::mat4 projectionMatrix = viewportData[i].projection;
            glm::mat4 viewMatrix = viewportData[i].view;

            geometryShader->SetMat4("projection", projectionMatrix);
            geometryShader->SetMat4("view", viewMatrix);
            geometryShader->SetMat4("model", glm::mat4(1));

            glDisable(GL_CULL_FACE);
            //glEnable(GL_CULL_FACE);

            glBindVertexArray(g_grassMesh.GetVAO());
            glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer); 
            glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0);
        }

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
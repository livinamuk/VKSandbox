#include "API/OpenGL/Renderer/GL_renderer.h" 
#include "AssetManagement/AssetManager.h"
#include "Renderer/Types/GrassMesh.h"

#include "API/OpenGL/GL_backend.h"
#include "Config/Config.h"
#include "Editor/Editor.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Core/Game.h"
#include "Util/Util.h"

struct GrassVertex {
    glm::vec3 position;
    glm::vec3 normal;
};

struct OpenGLGrassMesh {

private:
    unsigned int VBO = 0;
    unsigned int VAO = 0;
    unsigned int EBO = 0;

public:

    int GetVAO() {
        return VAO;
    }

    int GetVBO() {
        return VBO;
    }

    int GetEBO() {
        return EBO;
    }

    void AllocateBuffers(size_t vertexCount, size_t indexCount) {
        if (vertexCount == 0 || indexCount == 0) {
            if (VAO != 0) {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
                glDeleteBuffers(1, &EBO);
                VAO = VBO = EBO = 0;
            }
            return;
        }

        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GrassVertex), nullptr, GL_DYNAMIC_DRAW); // Allocate, no data

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW); // Allocate, no data

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GrassVertex), (void*)offsetof(GrassVertex, normal));

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};

namespace OpenGLRenderer {

    OpenGLGrassMesh g_grassGeometryMesh;
    GLuint g_indirectBuffer = 0;

#define GRASS_TILE_WORLDSPACE_SIZE 8.0
#define BLADE_SPACING 0.0185185185185185f
#define BLADES_PER_TILE_AXIS 432

    void GrassInit();
    void GenerateBladePositions(float xOffset, float zOffset, int viewportIndex);
    void RenderGrass(int viewportIndex);

    void GrassInit() {
        int bladesPerHeightMapAxis = HEIGHT_MAP_SIZE * HEIGHTMAP_SCALE_XZ / BLADE_SPACING;
        int bufferSize = bladesPerHeightMapAxis * bladesPerHeightMapAxis * sizeof(glm::vec4);
        //std::cout << "Grass SSBO allocated: " << Util::BytesToMBString(bufferSize) << "\n";
        CreateSSBO("BladePositions", bufferSize, GL_DYNAMIC_STORAGE_BIT);
        CreateGrassGeometry();

        // Create indirect buffer
        if (g_indirectBuffer == 0) {
            glGenBuffers(1, &g_indirectBuffer);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer);
            glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawIndexedIndirectCommand), NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        }
    }

    void CreateGrassGeometry() {        
        int bladeCount = 360;

        // Allocate mesh memory
        if (g_grassGeometryMesh.GetVAO() == 0) {
            int maxVertexCount = bladeCount * 6 * 2;
            int maxIndexCount = bladeCount * 12 * 2;
            g_grassGeometryMesh.AllocateBuffers(maxVertexCount, maxIndexCount);
        }

        OpenGLShader* geometryShader = GetShader("GrassGeometryGeneration");
        geometryShader->Use();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_grassGeometryMesh.GetVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_grassGeometryMesh.GetEBO());
        glDispatchCompute(bladeCount, 1, 1);
    }

    void GrassPass() {        
        RendererSettings& rendererSettings = Renderer::GetCurrentRendererSettings();
        if (!rendererSettings.drawGrass) return;

        if (Input::KeyPressed(HELL_KEY_X)) {
            CreateGrassGeometry();
        }

        static bool runOnce = true;
        if (runOnce) {
            GrassInit();
            runOnce = false;
        }

        OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLFrameBuffer* wipBuffer = GetFrameBuffer("WIP");
        OpenGLSSBO* bladeositionsSSBO = GetSSBO("BladePositions");

        BlitFrameBuffer(gBuffer, wipBuffer, "WorldSpacePosition", "WorldSpacePosition", GL_COLOR_BUFFER_BIT, GL_NEAREST);
       
        //glm::vec3 worldMin = glm::vec3(0, HEIGHTMAP_BEGIN_Y, 0);
        //glm::vec3 worldMax = glm::vec3(HEIGHTMAP_SIZE * HEIGHTMAP_SCALE_XZ, HEIGHTMAP_BEGIN_Y, HEIGHTMAP_SIZE * HEIGHTMAP_SCALE_XZ);

        

        // Sort tiles by closest to camera
        //std::sort(grassTileDataList.begin(), grassTileDataList.end(),
        //    [](const GrassTileData& a, const GrassTileData& b) {
        //    return a.distanceFromCamera > b.distanceFromCamera;
        //});

        //grassTileDataList.clear();
        //GrassTileData& grassTileData = grassTileDataList.emplace_back();
        //grassTileData.xOffset = GRASS_TILE_WORLDSPACE_SIZE * 2;
        //grassTileData.zOffset = GRASS_TILE_WORLDSPACE_SIZE * 2;


        // Bindings
        glBindVertexArray(g_grassGeometryMesh.GetVAO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, g_indirectBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, bladeositionsSSBO->GetHandle());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, g_grassGeometryMesh.GetVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, g_grassGeometryMesh.GetEBO());
        glBindTextureUnit(0, heightmapFBO->GetColorAttachmentHandleByName("Color"));
        glBindTextureUnit(1, wipBuffer->GetColorAttachmentHandleByName("WorldSpacePosition"));
        glBindTextureUnit(2, AssetManager::GetTextureByName("Perlin")->GetGLTexture().GetHandle());

        // GL State
        SetRasterizerState("GeometryPass_NonBlended");

        // Generate and draw
        static bool optimized = true;
        if (optimized) {
            for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {   // CHANGE TO VIEWPORT NOT PLAYER!!!!

                int viewportIndex = i;
                
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

                            // DrawAABB(tileAabb, WHITE);
                        }
                    }
                }

                // Zero out indirect buffer
                DrawIndexedIndirectCommand initialCmd;
                initialCmd.instanceCount = 1;
                glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer);
                glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawIndexedIndirectCommand), &initialCmd);
                                
                // Generate grass positions
                for (GrassTileData& grassTileData : grassTileDataList) {
                    GenerateBladePositions(grassTileData.xOffset, grassTileData.zOffset, viewportIndex);
                }
                // Then render for the current viewport
                RenderGrass(viewportIndex);
            }
        }
        else {
            //for (GrassTileData& grassTileData : grassTileDataList) {
            //    DrawIndexedIndirectCommand initialCmd;
            //    initialCmd.instanceCount = 1;
            //    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer);
            //    glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawIndexedIndirectCommand), &initialCmd);
            //    GenerateBladePositions(grassTileData.xOffset, grassTileData.zOffset);
            //    RenderGrass();
            //}
        }
        //if (Input::KeyPressed(HELL_KEY_O)) {
        //    optimized = !optimized;
        //    std::cout << "optimized: " << Util::BoolToString(optimized) << "\n";
        //}
    }

    void GenerateBladePositions(float xOffset, float zOffset, int viewportIndex) {
        int bladeCount = BLADES_PER_TILE_AXIS * BLADES_PER_TILE_AXIS;
        int maxVertexCount = bladeCount * 6;
        int maxIndexCount = bladeCount * 12;

        // Uniforms
        OpenGLShader* generationShader = GetShader("GrassPositionGeneration");
        generationShader->Use();
        generationShader->SetInt("gridSize", BLADES_PER_TILE_AXIS);
        generationShader->SetInt("u_viewportIndex", viewportIndex);
        generationShader->SetFloat("spacing", BLADE_SPACING);
        generationShader->SetVec3("offset", glm::vec3(xOffset, 0.0f, zOffset));
        generationShader->SetFloat("u_heightMapWorldSpaceSize", HEIGHT_MAP_SIZE * HEIGHTMAP_SCALE_XZ);
        
        // Dispatch compute shader
        const int workGroupSize = 16;
        int workGroupsX = BLADES_PER_TILE_AXIS / 16;
        int workGroupsY = BLADES_PER_TILE_AXIS / 16;
        glDispatchCompute(workGroupsX, workGroupsY, 1);
    }

    void RenderGrass(int viewportIndex) {

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        OpenGLSSBO* bladeositionsSSBO = GetSSBO("BladePositions");
        OpenGLShader* geometryShader = GetShader("Grass");
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");

        Viewport* viewport = ViewportManager::GetViewportByIndex(viewportIndex);
        if (!viewport->IsVisible()) return;

        OpenGLRenderer::SetViewport(gBuffer, viewport);

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick", "WorldSpacePosition", "Emissive" });
           
        glm::mat4 projectionView = viewportData[viewportIndex].projectionView;

        geometryShader->Use();
        geometryShader->SetMat4("projectionView", projectionView);

        glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, g_indirectBuffer); 
        glDrawArraysIndirect(GL_TRIANGLES, 0);


        //float bladeCount = 360;            
        //glm::mat4 projectionMatrix = viewportData[i].projection;
        //glm::mat4 viewMatrix = viewportData[i].view;            
        //Transform transform;
        //transform.position = glm::vec3(17, -4.1, 19);
        //transform.scale = glm::vec3(5);            
        //OpenGLShader* shader = GetShader("SolidColor");
        //shader->Use();
        //shader->SetMat4("projection", projectionMatrix);
        //shader->SetMat4("view", viewMatrix);
        //shader->SetMat4("model", transform.to_mat4());
        //shader->SetBool("useUniformColor", false);            
        //glBindVertexArray(g_grassGeometryMesh.GetVAO());
        //glEnable(GL_CULL_FACE);
        //glDrawElements(GL_TRIANGLES, bladeCount * 24, GL_UNSIGNED_INT, 0);
      
    }
}
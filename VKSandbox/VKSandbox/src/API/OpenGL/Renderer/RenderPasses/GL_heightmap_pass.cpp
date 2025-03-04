#include "../GL_renderer.h" 
#include "../../GL_backend.h"
#include "../../Types/GL_heightmapMesh.h"
#include "../Config/Config.h"
#include "AssetManagement/AssetManager.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"

#include "API/OpenGL/Types/GL_texture_readback.h"
#include "BackEnd/BackEnd.h"
#include "Editor/Editor.h"
#include "Editor/Gizmo.h"
#include "Imgui/ImguiBackEnd.h"
#include "Input/Input.h"
#include "World/HeightMapManager.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"
#include "World/MapManager.h"
#include "World/World.h"

#include "lodepng/lodepng.h"

#include "Core/Audio.h"

namespace OpenGLRenderer {

    //void AllocateHeightMapGPUMemoryIfItDoesntExist();
    void BlitHeightMapWorld();
    void GenerateHeightMapImageData();
    void PaintHeightMap();
    void GenerateHeightMapVertexData();
    void GeneratePhysXTextures();
    void DrawHeightMap();
    void SaveHeightMap();

    struct HeightMapGPUData {
        int layerIndex;
        int layerIndexN;
        int layerIndexS;
        int layerIndexE;
        int layerIndexW;
        int mapCellX;
        int mapCellZ;
    };

    std::vector<HeightMapGPUData> heightMapGPUDataSet;

    void HeightMapPass() {

        if (heightMapGPUDataSet.empty()) {

            HeightMapGPUData data;
            data.layerIndex = 0;
            data.layerIndexN = 1;
            data.layerIndexS = -1;
            data.layerIndexE = -1;
            data.layerIndexW = -1;
            data.mapCellX = 0;
            data.mapCellZ = 0;
            heightMapGPUDataSet.push_back(data);

            data.layerIndex = 1;
            data.layerIndexN = 0;
            data.layerIndexS = 0;
            data.layerIndexE = 0;
            data.layerIndexW = 0;
            data.mapCellX = 0;
            data.mapCellZ = 1;
            heightMapGPUDataSet.push_back(data);

            data.layerIndex = 1;
            data.layerIndexN = -1;
            data.layerIndexS = 0;
            data.layerIndexE = -1;
            data.layerIndexW = -1;
            data.mapCellX = 0;
            data.mapCellZ = 2;
            heightMapGPUDataSet.push_back(data);

            data.layerIndex = 1;
            data.layerIndexN = -1;
            data.layerIndexS = -1;
            data.layerIndexE = 0;
            data.layerIndexW = -1;
            data.mapCellX = -1;
            data.mapCellZ = 1;
            heightMapGPUDataSet.push_back(data);

            data.layerIndex = 1;
            data.layerIndexN = -1;
            data.layerIndexS = -1;
            data.layerIndexE = -1;
            data.layerIndexW = 0;
            data.mapCellX = 1;
            data.mapCellZ = 1;
            heightMapGPUDataSet.push_back(data);
        }

        BlitHeightMapWorld();

        GenerateHeightMapImageData();
        PaintHeightMap();
        GenerateHeightMapVertexData();
        GeneratePhysXTextures();
        DrawHeightMap();

        if (Editor::IsEditorOpen() && Editor::GetEditorMode() == EditorMode::HEIGHTMAP_EDITOR) {

            if (Input::KeyPressed(HELL_KEY_L)) {                
                HeightMapData heightMapData = File::LoadHeightMap("TEST.heightmap");
                OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
                GLuint textureHandle = heightmapFBO->GetColorAttachmentHandleByName("Color");
            }
            if (Input::KeyPressed(HELL_KEY_S)) {
                SaveHeightMap();
            }
        }

        if (Input::KeyPressed(HELL_KEY_U)) {
            if (Util::RenameFile("res/shit.txt", "res/fuck.txt")) {
                std::cout << "rename successful\n";
            }
        }
    }

    void BlitHeightMapWorld() {

        OpenGLFrameBuffer* worldFramebuffer = GetFrameBuffer("World");
        OpenGLFrameBuffer* heightMapBlitBuffer = GetFrameBuffer("HeightMapBlitBuffer");
        OpenGLTextureArray& heightmapTextureArray = HeightMapManager::GetGLTextureArray();
        MapCreateInfo* mapCreateInfo = World::GetCurrentMapCreateInfo();

        if (!mapCreateInfo) return;

        int textureWidth = mapCreateInfo->width * 256;
        int textureHeight = mapCreateInfo->depth * 256;

        if (worldFramebuffer->GetWidth() != textureWidth || worldFramebuffer->GetHeight() != textureHeight) {
            worldFramebuffer->Resize(textureWidth, textureHeight);
        }

        worldFramebuffer->ClearAttachment("HeightMap", 0.0);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, heightMapBlitBuffer->GetHandle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, worldFramebuffer->GetHandle());

        for (int x = 0; x < mapCreateInfo->width; x++) {
            for (int z = 0; z < mapCreateInfo->depth; z++) {
                if (MapManager::IsSectorAtLocation(mapCreateInfo, x, z)) {
                    int layerIndex = 0;
                    int srcX0 = 0;
                    int srcY0 = 0;
                    int srcX1 = HEIGHT_MAP_SIZE;
                    int srcY1 = HEIGHT_MAP_SIZE;
                    int dstX0 = x * HEIGHT_MAP_SIZE;
                    int dstY0 = z * HEIGHT_MAP_SIZE;
                    int dstX1 = dstX0 + HEIGHT_MAP_SIZE;
                    int dstY1 = dstY0 + HEIGHT_MAP_SIZE;
                    glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, heightmapTextureArray.GetHandle(), 0, layerIndex);
                    glDrawBuffer(worldFramebuffer->GetColorAttachmentSlotByName("HeightMap"));
                    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, GL_COLOR_BUFFER_BIT, GL_LINEAR);
                }
            }
        }
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }


    void GenerateHeightMapImageData() {
        static bool runOnce = true;
        if (!runOnce) return;
        runOnce = false;
        return;

        OpenGLTextureArray& heightmapTextureArray = HeightMapManager::GetGLTextureArray();
        OpenGLShader* shader = GetShader("HeightMapImageGeneration");

        shader->Use();

        glBindImageTexture(0, heightmapTextureArray.GetHandle(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_R16F);
        glDispatchCompute(HEIGHT_MAP_SIZE / 16, HEIGHT_MAP_SIZE / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    void PaintHeightMap() {
        if (!IsMouseRayWorldPositionReadBackReady()) return;
        if (!Editor::IsEditorOpen()) return;
        if (Editor::GetEditorMode() != EditorMode::HEIGHTMAP_EDITOR) return;
        if (ImGuiBackend::ImGuiOwnsMouse()) return;

        OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
        OpenGLShader* shader = GetShader("HeightMapPaint");

        int paintX = static_cast<int>(GetMouseRayWorldPostion().x / HEIGHTMAP_SCALE_XZ);
        int paintY = static_cast<int>(GetMouseRayWorldPostion().z / HEIGHTMAP_SCALE_XZ);
        int brushSize = 16;
        int maxBrushSize = 32;
        int mode = 1;
        float strength = 1.0f;
        float noiseStrength = 0.5;

        if (Input::RightMouseDown()) {
            strength *= -1;
        }

        if (Input::LeftMouseDown() || Input::RightMouseDown()) {
            shader->Use();
            shader->SetInt("u_paintX", paintX);
            shader->SetInt("u_paintY", paintY);
            shader->SetInt("u_mode", mode);
            shader->SetFloat("u_brushSize", brushSize);
            shader->SetFloat("u_strength", strength);
            shader->SetFloat("u_noiseStrength", noiseStrength);

            //glBindImageTexture(0, heightmapFBO->GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R16F);

            OpenGLTextureArray& heightmapTextureArray = HeightMapManager::GetGLTextureArray();
            glBindImageTexture(0, heightmapTextureArray.GetHandle(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_R16F);

            glDispatchCompute(maxBrushSize, maxBrushSize, 1);
        }

        DrawPoint(GetMouseRayWorldPostion(), RED);
    }

    void GenerateHeightMapVertexData() {
        OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
        OpenGLShader* shader = GetShader("HeightMapVertexGeneration");
        MapCreateInfo* mapCreateInfo = World::GetCurrentMapCreateInfo();

        int heightMapCount = MapManager::GetHeightMapCount(mapCreateInfo);
        heightMapMesh.AllocateMemory(heightMapCount);
        
        int layerIndex = 0;

        shader->Use();
        shader->SetInt("u_layerIndex", layerIndex);

        OpenGLTextureArray& heightmapTextureArray = HeightMapManager::GetGLTextureArray();        
        glBindImageTexture(0, heightmapTextureArray.GetHandle(), 0, GL_TRUE, layerIndex, GL_READ_ONLY, GL_R16F);


        for (int h = 0; h < heightMapGPUDataSet.size(); h++) {

            HeightMapGPUData& heightMapGPUData = heightMapGPUDataSet[h];
            int heightMapIndex = h;

            int layerIndexN = 0;
            int layerIndexS = 0;
            int layerIndexE = 0;
            int layerIndexW = 0;

            shader->SetInt("u_layerIndex", heightMapGPUData.layerIndex);
            shader->SetInt("u_layerIndexN", heightMapGPUData.layerIndexN);
            shader->SetInt("u_layerIndexS", heightMapGPUData.layerIndexS);
            shader->SetInt("u_layerIndexE", heightMapGPUData.layerIndexE);
            shader->SetInt("u_layerIndexW", heightMapGPUData.layerIndexW);
            shader->SetInt("u_mapCellX", heightMapGPUData.mapCellX);
            shader->SetInt("u_mapCellZ", heightMapGPUData.mapCellZ);
            shader->SetInt("u_heightMapIndex", heightMapIndex);

            //std::cout << heightMapIndex << ": " << heightMapGPUData.mapCellX << ", " << heightMapGPUData.mapCellZ << "\n";

            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, heightMapMesh.GetVBO());
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, heightMapMesh.GetEBO());
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

            for (int cx = 0; cx < 8; cx++) {
                for (int cz = 0; cz < 8; cz++) {
                    shader->SetInt("u_chunkIndexX", cx);
                    shader->SetInt("u_chunkIndexZ", cz);

                    int chunkSize = HEIGHT_MAP_SIZE / 8;
                    int chunkWidth = chunkSize + 1;
                    int chunkDepth = chunkSize + 1;
                    int groupSizeX = (chunkWidth + 16 - 1) / 16;
                    int groupSizeY = (chunkDepth + 16 - 1) / 16;
                    glDispatchCompute(groupSizeX, groupSizeY, 1);
                }
            }
        }
    }

    void GeneratePhysXTextures() {
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
        OpenGLShader* shader = GetShader("HeightMapPhysxTextureGeneration");

        int textureWidth = 264;  // 8 chunks * 33
        int textureHeight = 264; // 8 chunks * 33

        // Create a "working" texture if it does not exist yet
        static GLuint textureID = 0;
        if (textureID == 0) {
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 33 * 8, 33 * 8, 0, GL_RED, GL_HALF_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Run a compute shader to write the heightmap data into a texture
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
        glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R16F);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, heightMapMesh.GetVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, heightMapMesh.GetEBO());
        int localSize = 16;
        int groupSizeX = (textureWidth + localSize - 1) / localSize;
        int groupSizeY = (textureHeight + localSize - 1) / localSize;
        shader->Use();
        glDispatchCompute(groupSizeX, groupSizeY, 1);

        // Readback from that texture into a float array
        int depth = 1;
        GLsizei dataSize = textureWidth * textureHeight * sizeof(float);
        std::vector<float> heightMapData(textureWidth * textureHeight);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glGetTextureSubImage(textureID, 0,0, 0, 0, textureWidth, textureHeight, depth, GL_RED, GL_FLOAT, dataSize, heightMapData.data());

        // Iterate that float array and extract the height values for PhysX
        for (int chunkZ = 0; chunkZ < 8; chunkZ++) {
            for (int chunkX = 0; chunkX < 8; chunkX++) {
                for (int x = 0; x < 33; x++) {
                    for (int z = 0; z < 33; z++) {
                        int globalX = chunkX * 33 + x;
                        int globalZ = chunkZ * 33 + z;
                        float y = heightMapData[globalZ * textureWidth + globalX];
                        y = (y * 25) - 5;

                        float worldX = chunkX * 32 + x;
                        float worldZ = chunkZ * 32 + z;

                        glm::vec3 position(worldX * 0.25f, y, worldZ * 0.25f);

                        glm::vec4 color = glm::vec4(0, 0, 1, 1);
                        color.x = chunkX / 8.0f;
                        color.y = chunkZ / 8.0f;
                        //DrawPoint(position, color);
                    }
                }
            }
        }
    }

    void DrawHeightMap() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
        OpenGLShader* shader = GetShader("HeightMapColor");

        Transform transform;
        transform.scale = glm::vec3(HEIGHTMAP_SCALE_XZ, HEIGHTMAP_SCALE_Y, HEIGHTMAP_SCALE_XZ);
        glm::mat4 modelMatrix = transform.to_mat4();
        glm::mat4 inverseModelMatrix = glm::inverse(modelMatrix);

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick", "WorldSpacePosition" });

        shader->Use();
        shader->SetMat4("modelMatrix", modelMatrix);
        shader->SetMat4("inverseModelMatrix", inverseModelMatrix);

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

        int indexCount = (HEIGHT_MAP_SIZE - 1) * (HEIGHT_MAP_SIZE - 1) * 6;
        int vertexCount = HEIGHT_MAP_SIZE * HEIGHT_MAP_SIZE;
        glBindVertexArray(heightMapMesh.GetVAO());

        int verticesPerChunk = 33 * 33;
        int verticesPerHeightMap = verticesPerChunk * 8 * 8;
        int indicesPerChunk = 32 * 32 * 6;
        int indicesPerHeightMap = indicesPerChunk * 8 * 8;

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);

                for (int h = 0; h < heightMapGPUDataSet.size(); h++) {

                    for (int c = 0; c < 64; c++) {
                        int chunkIndexCount = 32 * 32 * 6;
                        int baseVertex = 0;// verticesPerHeightMap* h;
                        int baseIndex = c * indicesPerChunk + (indicesPerHeightMap * h);
                        void* indexOffset = (GLvoid*)(baseIndex * sizeof(GLuint));
                        int instanceCount = 1;
                        int viewportIndex = i;
                        glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, chunkIndexCount, GL_UNSIGNED_INT, indexOffset, instanceCount, baseVertex, viewportIndex);

                        // Draw indices as points
                        //glDrawElementsInstancedBaseVertexBaseInstance(GL_POINTS, chunkIndexCount, GL_UNSIGNED_INT, indexOffset, instanceCount, baseVertex, viewportIndex);

                        // Draw chunk vertices as points
                        //glDrawArraysInstancedBaseInstance(GL_POINTS, baseVertex, 1089, instanceCount, viewportIndex);
                    }
                } 
            }
        }
        glBindVertexArray(0);

        //OpenGLBackEnd::ReadBackHeightmapMeshData();
    }

    void SaveHeightMap() {
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);

        OpenGLTextureArray& heightmapTextureArray = HeightMapManager::GetGLTextureArray();

        GLuint textureHandle = heightmapTextureArray.GetHandle();
        GLuint width = heightmapTextureArray.GetWidth();
        GLuint height = heightmapTextureArray.GetHeight();
        int layerIndex = 0;
        size_t dataSize = width * height * sizeof(float);

        HeightMapData heightMapData;
        heightMapData.name = "TEST";
        heightMapData.width = width;
        heightMapData.height = height;
        heightMapData.data.resize(heightMapData.width * heightMapData.height);

        // Readback
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        glGetTextureSubImage(textureHandle, 0, 0, 0, layerIndex, width, height, 1, GL_RED, GL_FLOAT, dataSize, heightMapData.data.data());

        // Write file
        File::SaveHeightMap(heightMapData);
    }
}
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

#include "lodepng/lodepng.h"

#include "Core/Audio.h"

namespace OpenGLRenderer {

    void AllocateHeightMapGPUMemoryIfItDoesntExist();
    void GenerateHeightMapImageData();
    void PaintHeightMap();
    void GenerateHeightMapVertexData();
    void DrawHeightMap();
    void SaveHeightMap();

    void HeightMapPass() {
        AllocateHeightMapGPUMemoryIfItDoesntExist();
        GenerateHeightMapImageData();
        PaintHeightMap();
        GenerateHeightMapVertexData();
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

    void AllocateHeightMapGPUMemoryIfItDoesntExist() {
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
        if (heightMapMesh.GetVAO() == 0) {
            heightMapMesh.Create(HEIGHT_MAP_SIZE);
        }
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
        
        int layerIndex = 0;

        shader->Use();
        shader->SetInt("u_layerIndex", layerIndex);

        OpenGLTextureArray& heightmapTextureArray = HeightMapManager::GetGLTextureArray();        
        glBindImageTexture(0, heightmapTextureArray.GetHandle(), 0, GL_TRUE, layerIndex, GL_READ_ONLY, GL_R16F);
      
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, heightMapMesh.GetVBO());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, heightMapMesh.GetEBO());
        glDispatchCompute((HEIGHT_MAP_SIZE + 15) / 16, (HEIGHT_MAP_SIZE + 15) / 16, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    }

    void DrawHeightMap() {
        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
        OpenGLShader* shader = GetShader("HeightMapColor");

        Transform transform;
        transform.position = glm::vec3(0, HEIGHTMAP_BEGIN_Y, 0);
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
        glGetTextureSubImage(textureHandle, 0, 0, 0, layerIndex, width, height, 1, GL_RED, GL_FLOAT, dataSize, heightMapData.data.data());

        // Write file
        File::SaveHeightMap(heightMapData);
    }
}
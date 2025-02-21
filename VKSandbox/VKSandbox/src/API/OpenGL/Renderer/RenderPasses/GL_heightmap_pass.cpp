#include "../GL_renderer.h" 
#include "../../GL_backend.h"
#include "../../Types/GL_heightmapMesh.h"
#include "../Config/Config.h"
#include "AssetManagement/AssetManager.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/RenderDataManager.h"

#include "API/OpenGL/Types/GL_texture_readback.h"
#include "BackEnd/BackEnd.h"
#include "Input/Input.h"
#include "Tools/ImageTools.h"
#include "Util/Util.h"
#include "Editor/Editor.h"
#include "Editor/Gizmo.h"
#include "World/HeightMapManager.h"

#include "lodepng/lodepng.h"

namespace OpenGLRenderer {

    void AllocateHeightMapGPUMemoryIfItDoesntExist();
    void GenerateHeightMapImageData();
    void PaintHeightMap();
    void GenerateHeightMapVertexData();
    void DrawHeightMap();
    void SaveHeightMapRGBA8();

    void HeightMapPass() {
        AllocateHeightMapGPUMemoryIfItDoesntExist();
        GenerateHeightMapImageData();
        PaintHeightMap();
        GenerateHeightMapVertexData();
        DrawHeightMap();
        SaveHeightMapRGBA8();

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

        OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
        OpenGLShader* shader = GetShader("HeightMapImageGeneration");

        shader->Use();

        glBindImageTexture(0, heightmapFBO->GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R16F);
        glDispatchCompute((heightmapFBO->GetWidth() + 7) / 8, (heightmapFBO->GetHeight() + 7) / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    void PaintHeightMap() {
        if (!IsMouseRayWorldPositionReadBackReady()) return;
        if (!Editor::IsOpen()) return;
        if (Editor::GetEditorMode() != EditorMode::HEIGHTMAP_EDITOR) return;

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

            glBindImageTexture(0, heightmapFBO->GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R16F);
            glDispatchCompute(maxBrushSize, maxBrushSize, 1);
        }

        DrawPoint(GetMouseRayWorldPostion(), RED);
    }

    void GenerateHeightMapVertexData() {
        OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
        OpenGLHeightMapMesh& heightMapMesh = OpenGLBackEnd::GetHeightMapMesh();
        OpenGLShader* shader = GetShader("HeightMapVertexGeneration");

        shader->Use();

        glBindImageTexture(0, heightmapFBO->GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R16F);

        // Array version: untested...
        OpenGLTextureArray& heightmapTextureArray = HeightMapManager::GetGLTextureArray();
        int layerIndex = 0;
        glBindImageTexture(1, heightmapTextureArray.GetHandle(), 0, GL_TRUE, layerIndex, GL_READ_ONLY, GL_R16F);
        //glBindImageTexture(1, heightmapTextureArray.GetHandle(), 0, GL_FALSE, layerIndex, GL_READ_ONLY, GL_R16F); // single layer only?
        glFinish();

        // Test heightmap array
        //OpenGLTextureArray& textureArray = HeightMapManager::GetGLTextureArray();
        //glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray.GetHandle());
        //uniform sampler2DArray heightmapArray;


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

    void SaveHeightMap2() {
        static OpenGLTextureReadBack readback;
        static bool savedFile = false;

        if (Input::KeyPressed(HELL_KEY_Y)) {
            OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
            GLuint fboHandle = heightmapFBO->GetHandle();
            GLuint attachment = heightmapFBO->GetColorAttachmentSlotByName("Color");
            int xOffset = 0;
            int yOffset = 0;
            int width = heightmapFBO->GetWidth();
            int height = heightmapFBO->GetHeight();
            readback.IssueDataRequest(fboHandle, attachment, xOffset, yOffset, width, height, GL_R16F);
            //savedFile = false;
        }
        if (readback.IsRequestInProgress()) {
            readback.Update();
        }
        if (readback.IsResultReady() && !savedFile) {
            ImageTools::SaveBitmap("heightmap_export.bmp", readback.GetUint16Data().data(), readback.GetWidth(), readback.GetHeight(), GL_R16F);
            //ImageTools::SaveHeightMapR16F("heightmap_export.png", readback.GetUint16Data().data(), readback.GetWidth(), readback.GetHeight());
            std::cout << "saved r16f file\n";
            readback.Reset();
            //savedFile = true;
        }
    }


#include "stb_image_write.h"

    void SaveHeightMapRGBA8() {
        if (Input::KeyPressed(HELL_KEY_Y)) {
            OpenGLFrameBuffer* heightmapFBO = GetFrameBuffer("HeightMap");
            GLuint textureHandle = heightmapFBO->GetColorAttachmentHandleByName("Color");
            int width = heightmapFBO->GetWidth();
            int height = heightmapFBO->GetHeight();
            std::string filename = "HEIGHTMAP_RGBA8.png";

            glBindTexture(GL_TEXTURE_2D, textureHandle);

            // Allocate buffer for RGBA8 data
            std::vector<uint8_t> rawData(width * height * 4); // 4 channels (RGBA)

            // Read pixels from the currently bound texture
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawData.data());

            // Verify min/max values (only checking the R channel for debug)
            uint8_t minVal = 255, maxVal = 0;
            for (size_t i = 0; i < rawData.size(); i += 4) { // Step 4 since RGBA
                uint8_t r = rawData[i]; // Red channel
                minVal = std::min(minVal, r);
                maxVal = std::max(maxVal, r);
            }
            std::cout << "[SaveTextureRGBA8] Min R: " << (int)minVal << ", Max R: " << (int)maxVal << "\n";

            // Save as PNG using stb_image_write
            if (stbi_write_png(filename.c_str(), width, height, 4, rawData.data(), width * 4)) {
                std::cout << "[SaveTextureRGBA8] Saved RGBA8 PNG successfully: " << filename << "\n";
            }
            else {
                std::cerr << "[SaveTextureRGBA8] Error saving PNG file!\n";
            }
        }
    }

}
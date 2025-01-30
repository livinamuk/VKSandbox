#include "GL_renderer.h"
#include "GL_renderer_util.h"
#include "../GL_backend.h"
#include "../GL_Util.h"
#include "../Types/GL_cubemapView.h"
#include "../Types/GL_detachedMesh.hpp"
#include "../Types/GL_frameBuffer.h"
#include "../Types/GL_indirectBuffer.hpp"
#include "../Types/GL_pbo.hpp"
#include "../Types/GL_shader.h"
#include "../Types/GL_ssbo.hpp"
#include "../AssetManagement/AssetManager.h"
#include "../BackEnd/BackEnd.h"
#include "../Core/Audio.h"
#include "../Core/Game.h"
#include "../Core/Scene.h"
#include "../Config/Config.h"
#include "../Input/Input.h"
#include "../Player/Player.h"
#include "../Util/Util.h"
#include "../UI/UIBackEnd.h"
#include "../UI/TextBlitter.h"
#include "../Types/GameObject.h"
#include "../Hardcoded.hpp"
#include "../Timer.hpp"
#include <glm/gtx/matrix_decompose.hpp>

#include "../Editor/Editor.h"
#include "../Editor/Gizmo.h"
#include "../Renderer/RenderDataManager.h"
#include "../Viewport/ViewportManager.h"

namespace OpenGLRenderer {

    std::unordered_map<std::string, OpenGLShader> g_shaders;

    struct FrameBuffers {
        GLFrameBuffer gBuffer;
        GLFrameBuffer finalImage;
        GLFrameBuffer hair;
        GLFrameBuffer ui;
    } g_frameBuffers;

    struct SSBOs {
        SSBO samplers;
        SSBO playerData;
        SSBO rendererData;
        SSBO instanceData;
    } g_ssbos;
    
    IndirectBuffer g_indirectBuffer;

    int g_peelCount = 4;
    GLint g_UIQuadVAO;

    struct Cubemaps {
        CubemapView g_skyboxView;
    } g_cubemaps;

    void LightingPass();
    void DebugPass();
    void HairPass();
    void RenderHairLayer(const DrawCommands& drawCommands, int peelCount);
    void RenderUI();
    void SkyBoxPass();
    void EditorPass();
    void MultiDrawIndirect(const std::vector<DrawIndexedIndirectCommand>& commands);

    void Init() {
        const Resolutions& resolutions = Config::GetResolutions();

        g_frameBuffers.gBuffer.Create("Main", resolutions.gBuffer);
        g_frameBuffers.gBuffer.CreateAttachment("Color", GL_RGBA16F);
        g_frameBuffers.gBuffer.CreateAttachment("MousePick", GL_RG16UI, GL_NEAREST, GL_NEAREST);
        g_frameBuffers.gBuffer.CreateDepthAttachment(GL_DEPTH32F_STENCIL8);

        g_frameBuffers.hair.Create("Hair", resolutions.hair);
        g_frameBuffers.hair.CreateDepthAttachment(GL_DEPTH32F_STENCIL8);
        g_frameBuffers.hair.CreateAttachment("Lighting", GL_RGBA16F);
        g_frameBuffers.hair.CreateAttachment("ViewspaceDepth", GL_R32F);
        g_frameBuffers.hair.CreateAttachment("ViewspaceDepthPrevious", GL_R32F);
        g_frameBuffers.hair.CreateAttachment("Composite", GL_RGBA16F);

        g_frameBuffers.finalImage.Create("Main", resolutions.finalImage);
        g_frameBuffers.finalImage.CreateAttachment("Color", GL_RGBA16F);

        g_frameBuffers.ui.Create("UI", resolutions.ui);
        g_frameBuffers.ui.CreateAttachment("Color", GL_RGBA8, GL_NEAREST, GL_NEAREST);

        g_UIQuadVAO = OpenGLRendererUtil::CreateQuadVAO();

        int framebufferHandle = g_frameBuffers.gBuffer.GetHandle();
        int attachmentSlot = g_frameBuffers.gBuffer.GetColorAttachmentSlotByName("MousePick");
        OpenGLBackEnd::SetMousePickHandles(framebufferHandle, attachmentSlot);

        // Create ssbos
        g_ssbos.samplers.PreAllocate(sizeof(glm::uvec2) * TEXTURE_ARRAY_SIZE);
        g_ssbos.playerData.PreAllocate(sizeof(ViewportData) * 4);
        g_ssbos.rendererData.PreAllocate(sizeof(RendererData));
        g_ssbos.instanceData.PreAllocate(sizeof(RenderItem) * MAX_INSTANCE_DATA_COUNT);

        // Preallocate the indirect command buffer
        g_indirectBuffer.PreAllocate(sizeof(DrawIndexedIndirectCommand) * MAX_INDIRECT_DRAW_COMMAND_COUNT);

        // Load shaders
        g_shaders["Gizmo"] = OpenGLShader({ "gl_gizmo.vert", "gl_gizmo.frag" });
        g_shaders["HairDepthPeel"] = OpenGLShader({ "gl_hair_depth_peel.vert", "gl_hair_depth_peel.frag" });
        g_shaders["HairFinalComposite"] = OpenGLShader({ "gl_hair_final_composite.comp" });
        g_shaders["HairLayerComposite"] = OpenGLShader({ "gl_hair_layer_composite.comp" });
        g_shaders["HairLighting"] = OpenGLShader({ "gl_hair_lighting.vert", "gl_hair_lighting.frag" });
        g_shaders["Lighting"] = OpenGLShader({ "gl_lighting.vert", "gl_lighting.frag" });
        g_shaders["SolidColor"] = OpenGLShader({ "gl_solid_color.vert", "gl_solid_color.frag" });
        g_shaders["Skybox"] = OpenGLShader({ "gl_skybox.vert", "gl_skybox.frag" });
        g_shaders["UI"] = OpenGLShader({ "gl_ui.vert", "gl_ui.frag" });
    }

    void InitMain() {
        std::vector<GLuint> textures = {
            AssetManager::GetTextureByName("NightSky_Right")->GetGLTexture().GetHandle(),
            AssetManager::GetTextureByName("NightSky_Left")->GetGLTexture().GetHandle(),
            AssetManager::GetTextureByName("NightSky_Top")->GetGLTexture().GetHandle(),
            AssetManager::GetTextureByName("NightSky_Bottom")->GetGLTexture().GetHandle(),
            AssetManager::GetTextureByName("NightSky_Front")->GetGLTexture().GetHandle(),
            AssetManager::GetTextureByName("NightSky_Back")->GetGLTexture().GetHandle()
        };
        g_cubemaps.g_skyboxView.CreateCubemap(textures);
    }

    void RenderLoadingScreen() {
        // Clear the swapchain
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Calculate text
        std::string text = "";
        size_t maxLinesDisplayed = 36;
        size_t endIndex = AssetManager::GetLoadLog().size();
        size_t beginIndex = std::max((size_t)0, endIndex - maxLinesDisplayed);
        for (size_t i = beginIndex; i < endIndex; i++) {
            text += AssetManager::GetLoadLog()[i] + "\n";
        }
        // Update UI
        GLFrameBuffer& frameBuffer = g_frameBuffers.ui;
        glm::ivec2 viewportSize = glm::ivec2(frameBuffer.GetWidth(), frameBuffer.GetHeight());
        UIBackEnd::BlitText(text, "StandardFont", 0, 0, 2.0f);
        UIBackEnd::Update();

        // Render UI
        RenderUI();
    }

    void UpdateSSBOS() {
        const std::vector<GLuint64>& samplers = OpenGLBackEnd::GetBindlessTextureIDs();
        g_ssbos.samplers.Update(samplers.size() * sizeof(GLuint64), (void*)&samplers[0]);
        g_ssbos.samplers.Bind(0);

        const RendererData& rendererData = RenderDataManager::GetRendererData();
        g_ssbos.rendererData.Update(sizeof(RendererData), (void*)&rendererData);
        g_ssbos.rendererData.Bind(1);

        const std::vector<ViewportData>& playerData = RenderDataManager::GetViewportData();
        g_ssbos.playerData.Update(playerData.size() * sizeof(ViewportData), (void*)&playerData[0]);
        g_ssbos.playerData.Bind(2);

        const std::vector<RenderItem>& instanceData = RenderDataManager::GetInstanceData();
        g_ssbos.instanceData.Update(instanceData.size() * sizeof(RenderItem), (void*)&instanceData[0]);
        g_ssbos.instanceData.Bind(3);

    }

    void RenderGame() {

        UpdateSSBOS();

        if (Input::KeyPressed(HELL_KEY_E) && g_peelCount < 7) {
            Audio::PlayAudio("UI_Select.wav", 1.0f);
            g_peelCount++;
            std::cout << "Depth peel layer count: " << g_peelCount << "\n";
        }
        if (Input::KeyPressed(HELL_KEY_Q) && g_peelCount > 0) {
            Audio::PlayAudio("UI_Select.wav", 1.0f);
            g_peelCount--;
            std::cout << "Depth peel layer count: " << g_peelCount << "\n";
        }

        g_frameBuffers.gBuffer.Bind();
        g_frameBuffers.gBuffer.SetViewport();
        g_frameBuffers.gBuffer.ClearAttachment("Color", 0, 0, 0, 0);
        g_frameBuffers.gBuffer.ClearAttachment("MousePick", 0, 0);
        g_frameBuffers.gBuffer.ClearDepthAttachment();

        SkyBoxPass();
        LightingPass();
        HairPass();
        EditorPass();
        DebugPass();

        GLFrameBuffer& mainFrameBuffer = g_frameBuffers.gBuffer;
        GLFrameBuffer& hairFrameBuffer = g_frameBuffers.hair;
        GLFrameBuffer& presentFrameBuffer = g_frameBuffers.finalImage;

        // Downscale blit
        OpenGLRendererUtil::BlitFrameBuffer(mainFrameBuffer, presentFrameBuffer, "Color", "Color", GL_COLOR_BUFFER_BIT, GL_LINEAR);

        // Blit to swapchain
        OpenGLRendererUtil::BlitToDefaultFrameBuffer(presentFrameBuffer, "Color", GL_COLOR_BUFFER_BIT, GL_NEAREST);

        RenderUI();
    }

    void SkyBoxPass() {
        OpenGLShader* shader = GetShader("Skybox");
        if (!shader) return;

        static Mesh* mesh = AssetManager::GetMeshByIndex(AssetManager::GetModelByIndex(AssetManager::GetModelIndexByName("Cube"))->GetMeshIndices()[0]);

        GLFrameBuffer& gBuffer = OpenGLRenderer::g_frameBuffers.gBuffer;
        gBuffer.Bind();
        gBuffer.DrawBuffer("Color");

        shader->Use();


        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, g_cubemaps.g_skyboxView.GetHandle());
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRendererUtil::SetViewport(gBuffer, *viewport);
                glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), 1, mesh->baseVertex, i);
            }
        }
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
    }


    void LightingPass() {
        OpenGLShader* shader = GetShader("Lighting");
        if (!shader) return;

        shader->Use();

        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();

        GLFrameBuffer gBuffer = g_frameBuffers.gBuffer;
        gBuffer.Bind();
        gBuffer.DrawBuffers({ "Color", "MousePick" });


        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRendererUtil::SetViewport(gBuffer, *viewport);

                // Non blended
                glEnable(GL_DEPTH_TEST);
                glDepthMask(GL_TRUE);
                glEnable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);
                MultiDrawIndirect(drawInfoSet.geometry.perPlayer[i]);

                // Blended
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDisable(GL_CULL_FACE);
                glDepthMask(GL_FALSE);
                MultiDrawIndirect(drawInfoSet.geometryBlended.perPlayer[i]);

            }
        }
        // Cleanup
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }

    void HairPass() {
        OpenGLShader* shader = GetShader("HairFinalComposite");
        if (!shader) return;

        GLFrameBuffer& mainFrameBuffer = g_frameBuffers.gBuffer;
        GLFrameBuffer& hairFrameBuffer = g_frameBuffers.hair;

        // Setup state
        g_frameBuffers.hair.Bind();
        g_frameBuffers.hair.ClearAttachment("Composite", 0, 0, 0, 0);
        g_frameBuffers.hair.SetViewport();
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();

        // Render all top then all Bottom layers
        RenderHairLayer(drawInfoSet.hairTopLayer, g_peelCount);
        RenderHairLayer(drawInfoSet.hairBottomLayer, g_peelCount);

        shader->Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hairFrameBuffer.GetColorAttachmentHandleByName("Composite"));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mainFrameBuffer.GetColorAttachmentHandleByName("Color"));
        glBindImageTexture(0, mainFrameBuffer.GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
        glDispatchCompute((g_frameBuffers.gBuffer.GetWidth() + 7) / 8, (g_frameBuffers.gBuffer.GetHeight() + 7) / 8, 1);

        // Cleanup
        g_frameBuffers.gBuffer.SetViewport();
        glDepthFunc(GL_LESS);
    }

    void RenderHairLayer(const DrawCommands& drawCommands, int peelCount) {
        OpenGLShader* depthPeelShader = GetShader("HairDepthPeel");
        OpenGLShader* hairLightingShader = GetShader("HairLighting");
        OpenGLShader* hairLayerCompositeShader = GetShader("HairLayerComposite");

        if (!depthPeelShader) return;
        if (!hairLightingShader) return;
        if (!hairLayerCompositeShader) return;

        GLFrameBuffer& gBuffer = g_frameBuffers.gBuffer;
        GLFrameBuffer& hairFBO = g_frameBuffers.hair;
        hairFBO.Bind();
        hairFBO.ClearAttachment("ViewspaceDepthPrevious", 1.0f);

        for (int j = 0; j < peelCount; j++) {

            // Viewspace depth pass
            for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
                Viewport* viewport = ViewportManager::GetViewportByIndex(i);
                if (viewport->IsVisible()) {
                    OpenGLRendererUtil::SetViewport(hairFBO, *viewport);
                    OpenGLRendererUtil::SetScissor(hairFBO, *viewport);
                    OpenGLRendererUtil::BlitFrameBufferDepth(gBuffer, hairFBO, *viewport);
                    hairFBO.Bind();
                    hairFBO.ClearAttachment("ViewspaceDepth", 0.0f);
                    hairFBO.DrawBuffer("ViewspaceDepth");
;
                    depthPeelShader->Use();

                    glDepthFunc(GL_LESS);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, g_frameBuffers.hair.GetColorAttachmentHandleByName("ViewspaceDepthPrevious"));
                    MultiDrawIndirect(drawCommands.perPlayer[i]);
                }
            }
            // Color pass
            for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
                Viewport* viewport = ViewportManager::GetViewportByIndex(i);
                if (viewport->IsVisible()) {
                    OpenGLRendererUtil::SetViewport(hairFBO, *viewport);
                    OpenGLRendererUtil::SetScissor(hairFBO, *viewport);
                    hairFBO.Bind();
                    hairFBO.ClearAttachment("Lighting", 0, 0, 0, 0);
                    hairFBO.DrawBuffers({ "Lighting", "ViewspaceDepthPrevious" });

                    hairLightingShader->Use();

                    glBindTextureUnit(0, g_frameBuffers.hair.GetColorAttachmentHandleByName("ViewspaceDepth"));
                    glDepthFunc(GL_EQUAL);
                    MultiDrawIndirect(drawCommands.perPlayer[i]);
                }
            }
            // Composite
            hairLayerCompositeShader->Use();
            glBindImageTexture(0, hairFBO.GetColorAttachmentHandleByName("Lighting"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
            glBindImageTexture(1, hairFBO.GetColorAttachmentHandleByName("Composite"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
            int workGroupsX = (g_frameBuffers.hair.GetWidth() + 7) / 8;
            int workGroupsY = (g_frameBuffers.hair.GetHeight() + 7) / 8;
            glDispatchCompute(workGroupsX, workGroupsY, 1);
        }
        glDisable(GL_SCISSOR_TEST);
    }

    void DebugPass() {
        OpenGLShader* shader = GetShader("SolidColor");
        if (!shader) return;

        Player* player = Game::GetLocalPlayerByIndex(0);
        Viewport* viewport = ViewportManager::GetViewportByIndex(0);

        GLFrameBuffer& mainFrameBuffer = g_frameBuffers.gBuffer;
        mainFrameBuffer.Bind();
        mainFrameBuffer.DrawBuffer("Color");
        mainFrameBuffer.SetViewport();

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glPointSize(8.0f);
        glDisable(GL_DEPTH_TEST);

        // AABB aabb(glm::vec3(-0.5), glm::vec3(0.5));
        // DrawAABB(aabb, WHITE);
        
        shader->Use();
        shader->SetMat4("projection", viewport->GetPerpsectiveMatrix());
        shader->SetMat4("view", player->GetViewMatrix());
        shader->SetMat4("model", glm::mat4(1));

        // Draw lines
        UpdateDebugLinesMesh();
        if (g_debugLinesMesh.GetIndexCount() > 0) {
            glBindVertexArray(g_debugLinesMesh.GetVAO());
            glDrawElements(GL_LINES, g_debugLinesMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
        }
        // Draw points
        UpdateDebugPointsMesh();
        if (g_debugPointsMesh.GetIndexCount() > 0) {
            glBindVertexArray(g_debugPointsMesh.GetVAO());
            glDrawElements(GL_POINTS, g_debugPointsMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
        }
    }

    void RenderUI() {
        OpenGLShader* shader = GetShader("UI");
        if (!shader) return;

        Mesh2D& mesh = UIBackEnd::GetUIMesh();
        OpenGLMesh2D& glMesh = mesh.GetGLMesh2D();
        GLFrameBuffer& frameBuffer = g_frameBuffers.ui;
        glm::ivec2 viewportSize = glm::ivec2(frameBuffer.GetWidth(), frameBuffer.GetHeight());

        frameBuffer.Bind();
        frameBuffer.SetViewport();
        frameBuffer.ClearAttachment("Color", 0.0f, 0.0f, 0.0f, 0.0f);
        frameBuffer.DrawBuffer("Color");
        shader->Use();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(glMesh.GetVAO());

        // Draw the UI elements
        std::vector<UIRenderItem>& renderItems = UIBackEnd::GetRenderItems();
        for (UIRenderItem& renderItem : renderItems) {
            OpenGLTexture& glTexture = AssetManager::GetTextureByIndex(renderItem.textureIndex)->GetGLTexture();
            glBindTextureUnit(0, glTexture.GetHandle());
            glDrawElementsInstancedBaseVertex(GL_TRIANGLES, renderItem.indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * renderItem.baseIndex), 1, renderItem.baseVertex);
        }

        // Draw the UI texture into the swapchain
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, BackEnd::GetCurrentWindowWidth(), BackEnd::GetCurrentWindowHeight());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameBuffer.GetColorAttachmentHandleByName("Color"));
        glBindVertexArray(g_UIQuadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Cleanup
        glDisable(GL_BLEND);
    }

    void EditorPass() {
        OpenGLShader* shader = GetShader("SolidColor");
        if (!shader) return;

        Player* player = Game::GetLocalPlayerByIndex(0);
        Viewport* viewport = ViewportManager::GetViewportByIndex(0);

        g_frameBuffers.gBuffer.Bind();
        g_frameBuffers.gBuffer.SetViewport();
        g_frameBuffers.gBuffer.DrawBuffers({ "Color", "MousePick" });


        glEnable(GL_DEPTH_TEST);
        g_frameBuffers.gBuffer.ClearDepthAttachment();

        if (!Editor::IsOpen()) {
            return;
        }

        glEnable(GL_BLEND);

        shader->Use();
        shader->SetMat4("projection", viewport->GetProjectionMatrix());
        shader->SetMat4("view", player->GetViewMatrix());
        shader->SetBool("useUniformColor", true);

        Gizmo::Update(viewport->GetProjectionMatrix(), player->GetViewMatrix());

        if (Editor::GetSelectedObjectType() != EditorObjectType::NONE) {
            for (GizmoRenderItem& renderItem : Gizmo::GetRenderItems()) {
                DetachedMesh* mesh = Gizmo::GetDetachedMeshByIndex(renderItem.meshIndex);
                if (mesh) {
                    OpenGLDetachedMesh glMesh = mesh->GetGLMesh();
                    shader->SetMat4("model", renderItem.modelMatrix);
                    shader->SetVec4("uniformColor", renderItem.color);
                    glBindVertexArray(glMesh.GetVAO());
                    glDrawElements(GL_TRIANGLES, glMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
                }
            }
        }       

        // Cleanup
        glDisable(GL_BLEND);
    }

    void MultiDrawIndirect(const std::vector<DrawIndexedIndirectCommand>& commands) {
        if (commands.size()) {
            // Feed the draw command data to the gpu
            g_indirectBuffer.Bind();
            g_indirectBuffer.Update(sizeof(DrawIndexedIndirectCommand) * commands.size(), commands.data());

            // Fire of the commands
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, (GLsizei)commands.size(), 0);
        }
    }

    void HotloadShaders() {
        bool allSucceeded = true;
        for (auto& [_, shader] : g_shaders) {
            if (!shader.Hotload()) {
                allSucceeded = false;
            }
        }
        if (allSucceeded) {
            std::cout << "Hotloaded shaders\n";
        }
    }

    OpenGLShader* GetShader(const std::string& name) {
        auto it = g_shaders.find(name);
        return (it != g_shaders.end()) ? &it->second : nullptr;
    }
}

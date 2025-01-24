#include "GL_renderer.h"
#include "GL_renderer_util.h"
#include "GL_backend.h"
#include "GL_util.hpp"
#include "Types/GL_detachedMesh.hpp"
#include "Types/GL_frameBuffer.hpp"
#include "Types/GL_pbo.hpp"
#include "Types/GL_shader.h"
#include "../AssetManagement/AssetManager.h"
#include "../BackEnd/BackEnd.h"
#include "../Core/Audio.h"
#include "../Core/Camera.h"
#include "../Core/Scene.hpp"
#include "../Config/Config.h"
#include "../Game/Game.h"
#include "../Input/Input.h"
#include "../Util.hpp"
#include "../UI/UIBackEnd.h"
#include "../UI/TextBlitter.h"
#include "../Types/GameObject.h"
#include "../Hardcoded.hpp"
#include "../Timer.hpp"
#include <glm/gtx/matrix_decompose.hpp>

namespace OpenGLRenderer {

    struct Shaders {
        Shader solidColor;
        Shader lighting;
        Shader hairDepthPeel;
        Shader ui;
        Shader hairfinalComposite;
        Shader hairLayerComposite;
        Shader gizmo;
    } g_shaders;

    struct FrameBuffers {
        GLFrameBuffer main;
        GLFrameBuffer present;
        GLFrameBuffer hair;
        GLFrameBuffer ui;
    } g_frameBuffers;
    
    int g_peelCount = 3;
    GLint g_UIQuadVAO;

    void DrawScene(Shader& shader);
    void RenderLighting();
    void RenderDebug();
    void RenderHair();
    void RenderHairLayer(std::vector<RenderItem>& renderItems, int peelCount);
    void RenderUI();
    void RenderEditor();

    void Init() {
        g_frameBuffers.main.Create("Main", 1920, 1080);
        g_frameBuffers.main.CreateAttachment("Color", GL_RGBA8);
        g_frameBuffers.main.CreateAttachment("MousePick", GL_RG16UI, GL_NEAREST, GL_NEAREST);
        g_frameBuffers.main.CreateDepthAttachment(GL_DEPTH32F_STENCIL8);

        float hairDownscaleRatio = 1.0f;
        g_frameBuffers.hair.Create("Hair", g_frameBuffers.main.GetWidth() * hairDownscaleRatio, g_frameBuffers.main.GetHeight() * hairDownscaleRatio);
        g_frameBuffers.hair.CreateDepthAttachment(GL_DEPTH32F_STENCIL8);
        g_frameBuffers.hair.CreateAttachment("Color", GL_RGBA8);
        g_frameBuffers.hair.CreateAttachment("ViewspaceDepth", GL_R32F);
        g_frameBuffers.hair.CreateAttachment("ViewspaceDepthPrevious", GL_R32F);
        g_frameBuffers.hair.CreateAttachment("Composite", GL_RGBA8);

        float presentScale = 0.5f;
        g_frameBuffers.present.Create("Main", g_frameBuffers.main.GetWidth() * presentScale, g_frameBuffers.main.GetHeight() * presentScale);
        g_frameBuffers.present.CreateAttachment("Color", GL_RGBA8);

        glm::uvec2 uiResolution = Config::GetUIResolution();
        g_frameBuffers.ui.Create("UI", uiResolution.x, uiResolution.y);
        g_frameBuffers.ui.CreateAttachment("Color", GL_RGBA8, GL_NEAREST, GL_NEAREST);

        g_UIQuadVAO = OpenGLRendererUtil::CreateQuadVAO();

        int framebufferHandle = g_frameBuffers.main.GetHandle();
        int attachmentSlot = g_frameBuffers.main.GetColorAttachmentSlotByName("MousePick");
        OpenGLBackEnd::SetMousePickHandles(framebufferHandle, attachmentSlot);

        LoadShaders();
    }

    void RenderLoadingScreen() {
        // Clear the swapchain
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Calculate text
        std::string text = "";
        int maxLinesDisplayed = 36;
        int endIndex = AssetManager::GetLoadLog().size();
        int beginIndex = std::max(0, endIndex - maxLinesDisplayed);
        for (int i = beginIndex; i < endIndex; i++) {
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

    void RenderGame() {

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

        g_frameBuffers.main.Bind();
        g_frameBuffers.main.SetViewport();
        g_frameBuffers.main.ClearAttachment("Color", 0, 0, 0, 0);
        g_frameBuffers.main.ClearAttachment("MousePick", -1, -1, 0, 0);
        g_frameBuffers.main.ClearDepthAttachment();
        g_frameBuffers.main.DrawBuffers({ "Color" });

        RenderLighting();
        RenderHair();
        RenderEditor();
        RenderDebug();

        GLFrameBuffer& mainFrameBuffer = g_frameBuffers.main;
        GLFrameBuffer& hairFrameBuffer = g_frameBuffers.hair;
        GLFrameBuffer& presentFrameBuffer = g_frameBuffers.present;

        g_shaders.hairfinalComposite.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hairFrameBuffer.GetColorAttachmentHandleByName("Composite"));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mainFrameBuffer.GetColorAttachmentHandleByName("Color"));
        glBindImageTexture(0, mainFrameBuffer.GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
        glDispatchCompute((g_frameBuffers.main.GetWidth() + 7) / 8, (g_frameBuffers.main.GetHeight() + 7) / 8, 1);

        // Downscale blit
        OpenGLRendererUtil::BlitFrameBuffer(mainFrameBuffer, presentFrameBuffer, "Color", "Color", GL_COLOR_BUFFER_BIT, GL_LINEAR);

        // Blit to swapchain
        OpenGLRendererUtil::BlitToDefaultFrameBuffer(presentFrameBuffer, "Color", GL_COLOR_BUFFER_BIT, GL_NEAREST);

        RenderUI();
    }

    void CopyDepthBuffer(GLFrameBuffer& srcFrameBuffer, GLFrameBuffer& dstFrameBuffer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFrameBuffer.GetHandle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFrameBuffer.GetHandle());
        glBlitFramebuffer(0, 0, srcFrameBuffer.GetWidth(), srcFrameBuffer.GetHeight(), 0, 0, dstFrameBuffer.GetWidth(), dstFrameBuffer.GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    void CopyColorBuffer(GLFrameBuffer& srcFrameBuffer, GLFrameBuffer& dstFrameBuffer, const char* srcAttachmentName, const char* dstAttachmentName) {
        GLenum srcAttachmentSlot = srcFrameBuffer.GetColorAttachmentSlotByName(srcAttachmentName);
        GLenum dstAttachmentSlot = dstFrameBuffer.GetColorAttachmentSlotByName(dstAttachmentName);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFrameBuffer.GetHandle());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFrameBuffer.GetHandle());
        glReadBuffer(srcAttachmentSlot);
        glDrawBuffer(dstAttachmentSlot);
        glBlitFramebuffer(0, 0, srcFrameBuffer.GetWidth(), srcFrameBuffer.GetHeight(), 0, 0, dstFrameBuffer.GetWidth(), dstFrameBuffer.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    void DrawScene(Shader& shader) {
        // Non blended
        for (RenderItem& renderItem : Scene::GetRenderItems()) {
            DetachedMesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
            if (mesh) {
                OpenGLDetachedMesh glMesh = mesh->GetGLMesh();
                shader.SetMat4("model", renderItem.modelMatrix);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());
                glBindVertexArray(glMesh.GetVAO());
                glDrawElements(GL_TRIANGLES, glMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
            }
        }
        // Blended
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        for (RenderItem& renderItem : Scene::GetRenderItemsBlended()) {
            DetachedMesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
            if (mesh) {
                OpenGLDetachedMesh glMesh = mesh->GetGLMesh();
                shader.SetMat4("model", renderItem.modelMatrix);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalTextureIndex)->GetGLTexture().GetHandle());
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());
                glBindVertexArray(glMesh.GetVAO());
                glDrawElements(GL_TRIANGLES, glMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
            }
        }
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }


    void RenderLighting() {
        const float waterHeight = Hardcoded::roomY + Hardcoded::waterHeight;
        static float time = 0;
        time += 1.0f / 60.0f;

        g_frameBuffers.main.Bind();
        g_frameBuffers.main.SetViewport();
        g_frameBuffers.main.DrawBuffers({ "Color", "MousePick"});

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        g_shaders.lighting.Use();
        g_shaders.lighting.SetMat4("projection", Camera::GetProjectionMatrix());
        g_shaders.lighting.SetMat4("view", Camera::GetViewMatrix());
        g_shaders.lighting.SetMat4("model", glm::mat4(1));
        g_shaders.lighting.SetVec3("viewPos", Camera::GetViewPos());
        g_shaders.lighting.SetFloat("time", time);
        g_shaders.lighting.SetFloat("viewportWidth", g_frameBuffers.hair.GetWidth());
        g_shaders.lighting.SetFloat("viewportHeight", g_frameBuffers.hair.GetHeight());
        DrawScene(g_shaders.lighting);
    }

    void RenderHair() {
        GLFrameBuffer& mainFrameBuffer = g_frameBuffers.main;
        GLFrameBuffer& hairFrameBuffer = g_frameBuffers.hair;

        // Setup state
        Shader* shader = &g_shaders.lighting;
        shader->Use();
        shader->SetBool("isHair", true);
        g_frameBuffers.hair.Bind();
        g_frameBuffers.hair.ClearAttachment("Composite", 0, 0, 0, 0);
        g_frameBuffers.hair.SetViewport();
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        // Render all top then all Bottom layers
        RenderHairLayer(Scene::GetRenderItemsHairTopLayer(), g_peelCount);
        RenderHairLayer(Scene::GetRenderItemsHairBottomLayer(), g_peelCount);

        g_shaders.hairfinalComposite.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hairFrameBuffer.GetColorAttachmentHandleByName("Composite"));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mainFrameBuffer.GetColorAttachmentHandleByName("Color"));
        glBindImageTexture(0, mainFrameBuffer.GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
        glDispatchCompute((g_frameBuffers.main.GetWidth() + 7) / 8, (g_frameBuffers.main.GetHeight() + 7) / 8, 1);

        // Cleanup
        shader->Use();
        shader->SetBool("isHair", false);
        g_frameBuffers.main.SetViewport();
        glDepthFunc(GL_LESS);
    }

    void RenderHairLayer(std::vector<RenderItem>& renderItems, int peelCount) {
        g_frameBuffers.hair.Bind();
        g_frameBuffers.hair.ClearAttachment("ViewspaceDepthPrevious", 1, 1, 1, 1);
        for (int i = 0; i < peelCount; i++) {
            // Viewspace depth pass
            CopyDepthBuffer(g_frameBuffers.main, g_frameBuffers.hair);
            g_frameBuffers.hair.Bind();
            g_frameBuffers.hair.ClearAttachment("ViewspaceDepth", 0, 0, 0, 0);
            g_frameBuffers.hair.DrawBuffer("ViewspaceDepth");
            glDepthFunc(GL_LESS);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, g_frameBuffers.hair.GetColorAttachmentHandleByName("ViewspaceDepthPrevious"));
            Shader* shader = &g_shaders.hairDepthPeel;
            shader->Use();
            shader->SetMat4("projection", Camera::GetProjectionMatrix());
            shader->SetMat4("view", Camera::GetViewMatrix());
            shader->SetFloat("nearPlane", NEAR_PLANE);
            shader->SetFloat("farPlane", FAR_PLANE);
            shader->SetFloat("viewportWidth", g_frameBuffers.hair.GetWidth());
            shader->SetFloat("viewportHeight", g_frameBuffers.hair.GetHeight());
            for (RenderItem& renderItem : renderItems) {
                DetachedMesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
                if (mesh) {
                    OpenGLDetachedMesh glMesh = mesh->GetGLMesh();
                    shader->SetMat4("model", renderItem.modelMatrix);
                    glBindVertexArray(glMesh.GetVAO());
                    glDrawElements(GL_TRIANGLES, glMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
                }
            }
            // Color pass
            glDepthFunc(GL_EQUAL);
            g_frameBuffers.hair.Bind();
            g_frameBuffers.hair.ClearAttachment("Color", 0, 0, 0, 0);
            g_frameBuffers.hair.DrawBuffer("Color");
            shader = &g_shaders.lighting;
            shader->Use();
            shader->SetMat4("projection", Camera::GetProjectionMatrix());
            shader->SetMat4("view", Camera::GetViewMatrix());
            for (RenderItem& renderItem : renderItems) {
                DetachedMesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
                if (mesh) {
                    OpenGLDetachedMesh glMesh = mesh->GetGLMesh();
                    shader->SetMat4("model", renderItem.modelMatrix);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.baseColorTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.normalTextureIndex)->GetGLTexture().GetHandle());
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.rmaTextureIndex)->GetGLTexture().GetHandle());
                    glBindVertexArray(glMesh.GetVAO());
                    glDrawElements(GL_TRIANGLES, glMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
                }
            }
            // TODO!: when you port this you can output previous viewspace depth in the pass above
            CopyColorBuffer(g_frameBuffers.hair, g_frameBuffers.hair, "ViewspaceDepth", "ViewspaceDepthPrevious");

            // Composite
            g_shaders.hairLayerComposite.Use();
            glBindImageTexture(0, g_frameBuffers.hair.GetColorAttachmentHandleByName("Color"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
            glBindImageTexture(1, g_frameBuffers.hair.GetColorAttachmentHandleByName("Composite"), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
            glDispatchCompute((g_frameBuffers.hair.GetWidth() + 7) / 8, (g_frameBuffers.hair.GetHeight() + 7) / 8, 1);
        }
    }

    void RenderDebug() {

        GLFrameBuffer& mainFrameBuffer = g_frameBuffers.main;
        mainFrameBuffer.Bind();
        mainFrameBuffer.DrawBuffer("Color");
        mainFrameBuffer.SetViewport();

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glPointSize(8.0f);
        glDisable(GL_DEPTH_TEST);

        // AABB aabb(glm::vec3(-0.5), glm::vec3(0.5));
        // DrawAABB(aabb, WHITE);
        
        g_shaders.solidColor.Use();
        g_shaders.solidColor.SetMat4("projection", Camera::GetProjectionMatrix());
        g_shaders.solidColor.SetMat4("view", Camera::GetViewMatrix());
        g_shaders.solidColor.SetMat4("model", glm::mat4(1));

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
        Shader& shader = g_shaders.ui;
        Mesh2D& mesh = UIBackEnd::GetUIMesh();
        OpenGLMesh2D& glMesh = mesh.GetGLMesh2D();
        GLFrameBuffer& frameBuffer = g_frameBuffers.ui;
        glm::ivec2 viewportSize = glm::ivec2(frameBuffer.GetWidth(), frameBuffer.GetHeight());

        frameBuffer.Bind();
        frameBuffer.SetViewport();
        frameBuffer.ClearAttachment("Color", 0.0f, 0.0f, 0.0f, 0.0f);
        frameBuffer.DrawBuffer("Color");
        shader.Use();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(glMesh.GetVAO());

        // Draw the UI elements
        std::vector<UIRenderItem>& renderItems = UIBackEnd::GetRenderItems();
        for (UIRenderItem& renderItem : renderItems) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(renderItem.textureIndex)->GetGLTexture().GetHandle());
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

    void RenderEditor() {

        g_frameBuffers.main.Bind();
        g_frameBuffers.main.SetViewport();
        g_frameBuffers.main.DrawBuffers({ "Color", "MousePick" });


        glEnable(GL_DEPTH_TEST);
        g_frameBuffers.main.ClearDepthAttachment();


        Shader& shader = g_shaders.gizmo;
        shader.Use();
        shader.SetMat4("projection", Camera::GetProjectionMatrix());
        shader.SetMat4("view", Camera::GetViewMatrix());
        shader.SetMat4("model", glm::mat4(1));

        static int modelIndex = AssetManager::GetModelIndexByName("Gizmo");
        static Model* model = AssetManager::GetModelByIndex(modelIndex);

        Transform transform;
        transform.position = glm::vec3(0, 0, 0);

        std::cout << "\n";

        for (uint32_t& meshIndex : model->GetMeshIndices()) {
            DetachedMesh* mesh = AssetManager::GetMeshByIndex(meshIndex);


            if (mesh->GetName() == "XAxis") {
                shader.SetVec3("color", RED);
                shader.SetInt("mousePickType", 0);
                shader.SetInt("mousePickValue", 1);
            }
            if (mesh->GetName() == "YAxis") {
                shader.SetVec3("color", GREEN);
                shader.SetInt("mousePickType", 0);
                shader.SetInt("mousePickValue", 2);
            }
            if (mesh->GetName() == "ZAxis") {
                shader.SetVec3("color", BLUE);
                shader.SetInt("mousePickType", 0);
                shader.SetInt("mousePickValue", 3);
            }
            if (mesh->GetName() == "Center") {
                shader.SetVec3("color", YELLOW);
                shader.SetInt("mousePickType", 0);
                shader.SetInt("mousePickValue", 0);
            }              

            if (mesh) {
                OpenGLDetachedMesh glMesh = mesh->GetGLMesh();
                shader.SetMat4("model", transform.to_mat4());
                glBindVertexArray(glMesh.GetVAO());
                glDrawElements(GL_TRIANGLES, glMesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
            }
        }
    }

    void LoadShaders() {
        if (g_shaders.hairfinalComposite.Load({ "gl_hair_final_composite.comp" }) &&
            g_shaders.hairLayerComposite.Load({ "gl_hair_layer_composite.comp" }) &&
            g_shaders.solidColor.Load({ "gl_solid_color.vert", "gl_solid_color.frag" }) &&
            g_shaders.hairDepthPeel.Load({ "gl_hair_depth_peel.vert", "gl_hair_depth_peel.frag" }) &&
            g_shaders.lighting.Load({ "gl_lighting.vert", "gl_lighting.frag" }) &&
            g_shaders.ui.Load({ "gl_ui.vert", "gl_ui.frag" })) {
            g_shaders.gizmo.Load({ "gl_gizmo.vert", "gl_gizmo.frag" });
            std::cout << "Hotloaded shaders\n";
        }
    }
}
#include "GL_renderer.h"
#include "GL_renderer_util.h"
#include "../GL_backend.h"
#include "../GL_Util.h"
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
#include "../Renderer/RenderDataManager.h"
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
    std::unordered_map<std::string, OpenGLFrameBuffer> g_frameBuffers;
    std::unordered_map<std::string, OpenGLCubemapView> g_cubemapViews;
    std::unordered_map<std::string, OpenGLRasterizerState> g_rasterizerStates;

    struct SSBOs {
        SSBO samplers;
        SSBO playerData;
        SSBO rendererData;
        SSBO instanceData;
    } g_ssbos;

    IndirectBuffer g_indirectBuffer;

    struct Cubemaps {
        OpenGLCubemapView g_skyboxView;
    } g_cubemaps;

    void Init() {
        const Resolutions& resolutions = Config::GetResolutions();

        g_frameBuffers["GBuffer"] = OpenGLFrameBuffer("GBuffer", resolutions.gBuffer);
        g_frameBuffers["GBuffer"].CreateAttachment("BaseColor", GL_RGBA8);
        g_frameBuffers["GBuffer"].CreateAttachment("Normal", GL_RGBA16F);
        g_frameBuffers["GBuffer"].CreateAttachment("RMA", GL_RGBA8);
        g_frameBuffers["GBuffer"].CreateAttachment("MousePick", GL_RG16UI, GL_NEAREST, GL_NEAREST);
        g_frameBuffers["GBuffer"].CreateAttachment("FinalLighting", GL_RGBA16F);
        g_frameBuffers["GBuffer"].CreateAttachment("WorldSpacePosition", GL_RGBA16F);
        g_frameBuffers["GBuffer"].CreateDepthAttachment(GL_DEPTH32F_STENCIL8);

        g_frameBuffers["Hair"] = OpenGLFrameBuffer("Hair", resolutions.hair);
        g_frameBuffers["Hair"].CreateDepthAttachment(GL_DEPTH32F_STENCIL8);
        g_frameBuffers["Hair"].CreateAttachment("Lighting", GL_RGBA16F);
        g_frameBuffers["Hair"].CreateAttachment("ViewspaceDepth", GL_R32F);
        g_frameBuffers["Hair"].CreateAttachment("ViewspaceDepthPrevious", GL_R32F);
        g_frameBuffers["Hair"].CreateAttachment("Composite", GL_RGBA16F);

        g_frameBuffers["FinalImage"] = OpenGLFrameBuffer("FinalImage", resolutions.finalImage);
        g_frameBuffers["FinalImage"].CreateAttachment("Color", GL_RGBA16F);

        g_frameBuffers["UI"] = OpenGLFrameBuffer("UI", resolutions.ui);
        g_frameBuffers["UI"].CreateAttachment("Color", GL_RGBA8, GL_NEAREST, GL_NEAREST);

        int framebufferHandle = g_frameBuffers["GBuffer"].GetHandle();
        int attachmentSlot = g_frameBuffers["GBuffer"].GetColorAttachmentSlotByName("MousePick");
        OpenGLBackEnd::SetMousePickHandles(framebufferHandle, attachmentSlot);

        // Create ssbos
        g_ssbos.samplers.PreAllocate(sizeof(glm::uvec2) * TEXTURE_ARRAY_SIZE);
        g_ssbos.playerData.PreAllocate(sizeof(ViewportData) * 4);
        g_ssbos.rendererData.PreAllocate(sizeof(RendererData));
        g_ssbos.instanceData.PreAllocate(sizeof(RenderItem) * MAX_INSTANCE_DATA_COUNT);

        // Preallocate the indirect command buffer
        g_indirectBuffer.PreAllocate(sizeof(DrawIndexedIndirectCommand) * MAX_INDIRECT_DRAW_COMMAND_COUNT);

        // Load shaders
        g_shaders["EditorMesh"] = OpenGLShader({ "gl_editor_mesh.vert", "gl_editor_mesh.frag" });
        g_shaders["Gizmo"] = OpenGLShader({ "gl_gizmo.vert", "gl_gizmo.frag" });
        g_shaders["HairDepthPeel"] = OpenGLShader({ "gl_hair_depth_peel.vert", "gl_hair_depth_peel.frag" });
        g_shaders["HairFinalComposite"] = OpenGLShader({ "gl_hair_final_composite.comp" });
        g_shaders["HairLayerComposite"] = OpenGLShader({ "gl_hair_layer_composite.comp" });
        g_shaders["HairLighting"] = OpenGLShader({ "gl_hair_lighting.vert", "gl_hair_lighting.frag" });
        g_shaders["Lighting"] = OpenGLShader({ "gl_lighting.comp" });
        g_shaders["GBuffer"] = OpenGLShader({ "gl_gBuffer.vert", "gl_gBuffer.frag" });
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
        g_cubemapViews["SkyboxNightSky"] = OpenGLCubemapView(textures);
        InitRasterizerStates();
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

        const RendererSettings& renderSettings = Config::GetRendererSettings();
        int peelCount = renderSettings.depthPeelCount;
        if (Input::KeyPressed(HELL_KEY_E) && peelCount < 7) {
            Audio::PlayAudio("UI_Select.wav", 1.0f);
            Config::SetDepthPeelCount(peelCount + 1);
            std::cout << "Depth peel layer count: " << peelCount << "\n";
        }
        if (Input::KeyPressed(HELL_KEY_Q) && peelCount > 0) {
            Audio::PlayAudio("UI_Select.wav", 1.0f);
            Config::SetDepthPeelCount(peelCount - 1);
            std::cout << "Depth peel layer count: " << peelCount << "\n";
        }

        g_frameBuffers["GBuffer"].Bind();
        g_frameBuffers["GBuffer"].SetViewport();

        // Write a framebuffer method to everything below in one go
        // Write a framebuffer method to everything below in one go
        // Write a framebuffer method to everything below in one go
        // Write a framebuffer method to everything below in one go
        g_frameBuffers["GBuffer"].ClearAttachment("BaseColor", 0, 0, 0, 0);
        g_frameBuffers["GBuffer"].ClearAttachment("Normal", 0, 0, 0, 0);
        g_frameBuffers["GBuffer"].ClearAttachment("RMA", 0, 0, 0, 0);
        g_frameBuffers["GBuffer"].ClearAttachment("MousePick", 0, 0);
        g_frameBuffers["GBuffer"].ClearAttachment("WorldSpacePosition", 0, 0);        
        g_frameBuffers["GBuffer"].ClearDepthAttachment();

        SkyBoxPass();
        GeometryPass();
        LightingPass();
        HairPass();
        EditorPass();
        DebugPass();

        OpenGLFrameBuffer& gBuffer = g_frameBuffers["GBuffer"];
        OpenGLFrameBuffer& hairFrameBuffer = g_frameBuffers["Hair"];
        OpenGLFrameBuffer& finalImageBuffer = g_frameBuffers["FinalImage"];

        // Downscale blit
        OpenGLRendererUtil::BlitFrameBuffer(gBuffer, finalImageBuffer, "FinalLighting", "Color", GL_COLOR_BUFFER_BIT, GL_LINEAR);

        // Blit to swapchain
        OpenGLRendererUtil::BlitToDefaultFrameBuffer(finalImageBuffer, "Color", GL_COLOR_BUFFER_BIT, GL_NEAREST);

        UIPass();
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

    OpenGLFrameBuffer* GetFrameBuffer(const std::string& name) {
        auto it = g_frameBuffers.find(name);
        return (it != g_frameBuffers.end()) ? &it->second : nullptr;
    }

    OpenGLCubemapView* GetCubemapView(const std::string& name) {
        auto it = g_cubemapViews.find(name);
        return (it != g_cubemapViews.end()) ? &it->second : nullptr;
    }

    OpenGLRasterizerState* GetRasterizerState(const std::string& name) {
        auto it = g_rasterizerStates.find(name);
        return (it != g_rasterizerStates.end()) ? &it->second : nullptr;
    }

    OpenGLRasterizerState* CreateRasterizerState(const std::string& name) {
        g_rasterizerStates[name] = OpenGLRasterizerState();
        return &g_rasterizerStates[name];
    }

    void SetRasterizerState(const std::string& name) {
        OpenGLRasterizerState* rasterizerState = GetRasterizerState(name);
        if (!rasterizerState) {
            std::cout << "OpenGLRenderer::SetRasterizerState(const std::string& name) failed!" << name << " does not exist!\n";
            return;
        }

        // check if this is required to set depth mask later
        // check if this is required to set depth mask later
        // check if this is required to set depth mask later
        glEnable(GL_DEPTH_TEST);

        rasterizerState->blendEnable ?      glEnable(GL_BLEND)      : glDisable(GL_BLEND);
        rasterizerState->cullfaceEnable ?   glEnable(GL_CULL_FACE)  : glDisable(GL_CULL_FACE);
        rasterizerState->depthMask ?        glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
        rasterizerState->depthTestEnabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

        glBlendFunc(rasterizerState->blendFuncSrcfactor, rasterizerState->blendFuncDstfactor);
        glPointSize(rasterizerState->pointSize);
        glDepthFunc(rasterizerState->depthFunc);
    }
}














//if (rasterizerState->depthTestEnabled) {
//    glEnable(GL_DEPTH_TEST);
//}
//else {
//    glDisable(GL_DEPTH_TEST);
//}
//
#include "../GL_renderer.h" 
#include "../../GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "BackEnd/Backend.h"
#include "Viewport/ViewportManager.h"
#include "Editor/Editor.h"
#include "Renderer/RenderDataManager.h"

namespace OpenGLRenderer {

    void GeometryPass() {
        const DrawCommandsSet& drawInfoSet = RenderDataManager::GetDrawInfoSet();

        OpenGLFrameBuffer* gBuffer = GetFrameBuffer("GBuffer");
        OpenGLShader* shader = GetShader("GBuffer");
        OpenGLShader* editorMeshShader = GetShader("EditorMesh");

        if (!gBuffer) return;
        if (!shader) return;
        if (!editorMeshShader) return;

        gBuffer->Bind();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick", "WorldSpacePosition" });

        const std::vector<ViewportData>& viewportData = RenderDataManager::GetViewportData();

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        shader->Use();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick", "WorldSpacePosition", "Emissive" });
        SetRasterizerState("GeometryPass_NonBlended");

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);
                if (BackEnd::RenderDocFound()) {
                    SplitMultiDrawIndirect(shader, drawInfoSet.geometry.perViewport[i]);
                }
                else {
                    MultiDrawIndirect(drawInfoSet.geometry.perViewport[i]);
                }
            }
        }

        gBuffer->DrawBuffers({ "BaseColor" });        
        SetRasterizerState("GeometryPass_Blended");

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);                
                if (BackEnd::RenderDocFound()) {
                    SplitMultiDrawIndirect(shader, drawInfoSet.geometryBlended.perViewport[i]);
                }
                else {
                    MultiDrawIndirect(drawInfoSet.geometryBlended.perViewport[i]);
                }
            }
        }

        shader->Use();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick", "WorldSpacePosition", "Emissive" });
        SetRasterizerState("GeometryPass_NonBlended");

        glBindVertexArray(OpenGLBackEnd::GetSkinnedVertexDataVAO());
        glBindBuffer(GL_ARRAY_BUFFER, OpenGLBackEnd::GetSkinnedVertexDataVBO());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGLBackEnd::GetWeightedVertexDataEBO());

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRenderer::SetViewport(gBuffer, viewport);
                if (BackEnd::RenderDocFound()) {
                    SplitMultiDrawIndirect(shader, drawInfoSet.skinnedGeometry.perViewport[i]);
                }
                else {
                    MultiDrawIndirect(drawInfoSet.skinnedGeometry.perViewport[i]);
                }
            }
        }
        
        glBindVertexArray(0);
    }
}


#include "../GL_renderer.h" 
#include "../GL_renderer_util.h" 
#include "../../GL_backend.h"
#include "AssetManagement/AssetManager.h"
#include "BackEnd/Backend.h"
#include "Viewport/ViewportManager.h"
#include "Editor/Editor.h"
#include "Renderer/RenderDataManager.h"

#include "Core/Scene.h"

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

        //editorMeshShader->Use();
        //
        //EditorMesh& editorMesh = Editor::GetEditorMesh();
        //editorMesh.RecalculateMesh();
        //
        //Material* material = AssetManager::GetDefaultMaterial();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_basecolor)->GetGLTexture().GetHandle());
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_normal)->GetGLTexture().GetHandle());
        //glActiveTexture(GL_TEXTURE2);
        //glBindTexture(GL_TEXTURE_2D, AssetManager::GetTextureByIndex(material->m_rma)->GetGLTexture().GetHandle());
        //
        //OpenGLDetachedMesh& mesh = editorMesh.m_glMesh;
        //glBindVertexArray(mesh.GetVAO());
        //for (int i = 0; i < 4; i++) {
        //    Viewport* viewport = ViewportManager::GetViewportByIndex(i);
        //    if (viewport->IsVisible()) {
        //        OpenGLRendererUtil::SetViewport(*gBuffer, *viewport);
        //        editorMeshShader->SetMat4("projection", viewportData[i].projection);
        //        editorMeshShader->SetMat4("view", viewportData[i].view);
        //        editorMeshShader->SetMat4("model", glm::mat4(1));
        //        Viewport* viewport = ViewportManager::GetViewportByIndex(i);
        //        glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
        //    }
        //}

        glBindVertexArray(OpenGLBackEnd::GetVertexDataVAO());

        shader->Use();
        gBuffer->DrawBuffers({ "BaseColor", "Normal", "RMA", "MousePick", "WorldSpacePosition", "Emissive" });
        SetRasterizerState("GeometryPass_NonBlended");

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                OpenGLRendererUtil::SetViewport(gBuffer, viewport);
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
                OpenGLRendererUtil::SetViewport(gBuffer, viewport);                
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
                OpenGLRendererUtil::SetViewport(gBuffer, viewport);
                if (BackEnd::RenderDocFound()) {
                    SplitMultiDrawIndirect(shader, drawInfoSet.skinnedGeometry.perViewport[i]);
                }
                else {
                    MultiDrawIndirect(drawInfoSet.skinnedGeometry.perViewport[i]);
                }
            }
        }
        
        glBindVertexArray(0);


       // glBindVertexArray(OpenGLBackEnd::GetWeightedVertexDataVAO());
       // glBindBuffer(GL_ARRAY_BUFFER, OpenGLBackEnd::GetWeightedVertexDataVBO());
       // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGLBackEnd::GetWeightedVertexDataEBO());
       // for (int i = 0; i < 4; i++) {
       //     Viewport* viewport = ViewportManager::GetViewportByIndex(i);
       //     if (viewport->IsVisible()) {
       //         OpenGLRendererUtil::SetViewport(gBuffer, viewport);
       //
       //         glm::mat4 projectionMatrix = viewportData[i].projection;
       //         glm::mat4 viewMatrix = viewportData[i].view;
       //
       //         shader2->SetMat4("projection", projectionMatrix);
       //         shader2->SetMat4("view", viewMatrix);
       //
       //         Transform transform;
       //         transform.position = glm::vec3(-8.0f, 0.0f, 5.0f);
       //         transform.scale = glm::vec3(0.01f);
       //
       //         SkinnedModel* skinnedModel = AssetManager::GetSkinnedModelByName("Glock");
       //         int meshCount = skinnedModel->GetMeshCount();
       //         for (int i = 0; i < meshCount; i++) {
       //             SkinnedMesh* mesh = AssetManager::GetSkinnedMeshByIndex(skinnedModel->GetMeshIndices()[i]);             
       //             shader2->SetMat4("model", transform.to_mat4());
       //             glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->baseIndex), 1, mesh->baseVertexGlobal);
       //         }
       //     }
       // }
       // glBindVertexArray(0);
    }
}


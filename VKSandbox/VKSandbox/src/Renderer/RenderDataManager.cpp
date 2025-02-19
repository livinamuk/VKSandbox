#include "RenderDataManager.h"
#include "HellDefines.h"
#include "../Camera/Frustum.h"
#include "../Core/Game.h"
#include "../Core/Scene.h"
#include "../Config/Config.h"
#include "../Editor/Editor.h"
#include "../Viewport/ViewportManager.h"
#include <span>
#include <unordered_map>

namespace RenderDataManager {
    DrawCommandsSet g_drawCommandsSet;
    RendererData g_rendererData;
    std::vector<ViewportData> g_viewportData;
    std::vector<RenderItem> g_instanceData;
    std::vector<GPULight> g_gpuLightData;
    uint32_t g_baseSkinnedVertex;

    void UpdateViewportFrustums();
    void UpdateViewportData();
    void UpdateRendererData();
    void UpdateGPULightData();
    void UpdateDrawCommandsSet();
    void CreateDrawCommands(DrawCommands& drawCommands, std::vector<RenderItem>& renderItems);
    void CreateDrawCommandsSkinned(DrawCommands& drawCommands, std::vector<RenderItem>& renderItems);
    void CreateMultiDrawIndirectCommands(std::vector<DrawIndexedIndirectCommand>& commands, std::span<RenderItem> renderItems, int playerIndex, int instanceOffset);
    void CreateMultiDrawIndirectCommandsSkinned(std::vector<DrawIndexedIndirectCommand>& commands, std::span<RenderItem> renderItems, int playerIndex, int instanceOffset);
    int EncodeBaseInstance(int playerIndex, int instanceOffset);
    void DecodeBaseInstance(int baseInstance, int& playerIndex, int& instanceOffset);

    void Update() {
        UpdateViewportData();
        UpdateViewportFrustums();
        UpdateGPULightData();
        UpdateRendererData();
        UpdateDrawCommandsSet();
    }

    void UpdateViewportData() {
        const Resolutions& resolutions = Config::GetResolutions();
        g_viewportData.resize(4);
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;


            glm::mat4 viewMatrix = glm::mat4(1);
            if (Editor::IsOpen()) {
                viewMatrix = Editor::GetViewportViewMatrix(i);
            }
            else {
                viewMatrix = Game::GetLocalPlayerCameraByIndex(i)->GetViewMatrix();
            }

            g_viewportData[i].projection = viewport->GetProjectionMatrix();
            g_viewportData[i].inverseProjection = glm::inverse(g_viewportData[i].projection);
            g_viewportData[i].view = viewMatrix;
            g_viewportData[i].inverseView = glm::inverse(g_viewportData[i].view);
            g_viewportData[i].projectionView = g_viewportData[i].projection * g_viewportData[i].view;
            g_viewportData[i].inverseProjectionView = glm::inverse(g_viewportData[i].projectionView);
            g_viewportData[i].skyboxProjectionView = viewport->GetPerpsectiveMatrix() * g_viewportData[i].view;
            g_viewportData[i].width = resolutions.gBuffer.x * viewport->GetSize().x;
            g_viewportData[i].height = resolutions.gBuffer.y * viewport->GetSize().y;
            g_viewportData[i].xOffset = resolutions.gBuffer.x * viewport->GetPosition().x;
            g_viewportData[i].yOffset = resolutions.gBuffer.y * viewport->GetPosition().y;
            g_viewportData[i].posX = viewport->GetPosition().x;
            g_viewportData[i].posY = viewport->GetPosition().y;
            g_viewportData[i].sizeX = viewport->GetSize().x;
            g_viewportData[i].sizeY = viewport->GetSize().y;

            viewport->GetFrustum().Update(g_viewportData[i].projectionView);

            g_viewportData[i].frustumPlane0 = viewport->GetFrustum().GetPlane(0);
            g_viewportData[i].frustumPlane1 = viewport->GetFrustum().GetPlane(1);
            g_viewportData[i].frustumPlane2 = viewport->GetFrustum().GetPlane(2);
            g_viewportData[i].frustumPlane3 = viewport->GetFrustum().GetPlane(3);
            g_viewportData[i].frustumPlane4 = viewport->GetFrustum().GetPlane(4);
            g_viewportData[i].frustumPlane5 = viewport->GetFrustum().GetPlane(5);
        }
    }

    void UpdateViewportFrustums() {
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (viewport->IsVisible()) {
                viewport->GetFrustum().Update(g_viewportData[i].projectionView);
            }
        }
    }

    void UpdateGPULightData() {
        g_gpuLightData.clear();
        for (Light& light : Scene::GetLights()) {
            GPULight& gpuLight = g_gpuLightData.emplace_back();
            gpuLight.posX = light.GetPosition().x;
            gpuLight.posY = light.GetPosition().y;
            gpuLight.posZ = light.GetPosition().z;
            gpuLight.colorR = light.GetColor().r;
            gpuLight.colorG = light.GetColor().g;
            gpuLight.colorB = light.GetColor().b;
            gpuLight.radius = light.GetRadius();
            gpuLight.strength = light.GetStrength();
        }
    }

    void UpdateRendererData() {
        const Resolutions& resolutions = Config::GetResolutions();
        g_rendererData.nearPlane = NEAR_PLANE;
        g_rendererData.farPlane = FAR_PLANE;
        g_rendererData.gBufferWidth = resolutions.gBuffer.x;
        g_rendererData.gBufferHeight = resolutions.gBuffer.y;
        g_rendererData.hairBufferWidth = resolutions.hair.x;
        g_rendererData.hairBufferHeight = resolutions.hair.y;
        g_rendererData.splitscreenMode = (int)Game::GetSplitscreenMode();
        g_rendererData.time = Game::GetTotalTime();
    }

    void SortRenderItems(std::vector<RenderItem>& renderItems) {
        std::sort(renderItems.begin(), renderItems.end(), [](const RenderItem& a, const RenderItem& b) {
            return a.meshIndex < b.meshIndex;
        });
    }

    void UpdateDrawCommandsSet() {
        g_instanceData.clear();
        auto& set = g_drawCommandsSet;
        CreateDrawCommands(set.geometry, Scene::GetRenderItems());
        CreateDrawCommands(set.geometryBlended, Scene::GetRenderItemsBlended());
        CreateDrawCommands(set.geometryAlphaDiscarded, Scene::GetRenderItemsAlphaDiscarded());
        CreateDrawCommands(set.hairTopLayer, Scene::GetRenderItemsHairTopLayer());
        CreateDrawCommands(set.hairBottomLayer, Scene::GetRenderItemsHairBottomLayer());
        CreateDrawCommandsSkinned(set.skinnedGeometry, Scene::GetSkinnedRenderItems());
    }

    void CreateDrawCommands(DrawCommands& drawCommands, std::vector<RenderItem>& renderItems) {
        SortRenderItems(renderItems);

        // Update all RenderItem aabbs (REPLACE THIS ONCE YOU HAVE PHYSX AGAIN)
        // Update all RenderItem aabbs (REPLACE THIS ONCE YOU HAVE PHYSX AGAIN)
        // Update all RenderItem aabbs (REPLACE THIS ONCE YOU HAVE PHYSX AGAIN)
        // Update all RenderItem aabbs (REPLACE THIS ONCE YOU HAVE PHYSX AGAIN)
        for (RenderItem& renderItem : renderItems) {
            Util::UpdateRenderItemAABB(renderItem);
        }

        // Clear any commands from last frame
        for (int i = 0; i < 4; i++) {
            drawCommands.perViewport[i].clear();
        }

        // Iterate the viewports and build the draw commands
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            // Store the instance offset for this player
            int instanceStart = g_instanceData.size();

            // Preallocate an estimate
            g_instanceData.reserve(g_instanceData.size() + renderItems.size());

            // Append new render items to the global instance data
            for (const RenderItem& renderItem : renderItems) {
                if (renderItem.ignoredViewportIndex != -1 && renderItem.ignoredViewportIndex == i) continue;
                if (renderItem.exclusiveViewportIndex != -1 && renderItem.exclusiveViewportIndex != i) continue;

                // Frustum cull it
                AABB aabb(renderItem.aabbMin, renderItem.aabbMax);
                if (viewport->GetFrustum().IntersectsAABB(aabb)) {
                    g_instanceData.push_back(renderItem);
                }
            }

            // Create indirect draw commands using the stored offset
            std::span<RenderItem> instanceView(g_instanceData.begin() + instanceStart, g_instanceData.end());
            CreateMultiDrawIndirectCommands(drawCommands.perViewport[i], instanceView, i, instanceStart);
        }
    }

    void CreateDrawCommandsSkinned(DrawCommands& drawCommands, std::vector<RenderItem>& renderItems) {
        SortRenderItems(renderItems);

        // Clear any commands from last frame
        for (int i = 0; i < 4; i++) {
            drawCommands.perViewport[i].clear();
        }

        // Iterate the viewports and build the draw commands
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            // Store the instance offset for this player
            int instanceStart = g_instanceData.size();

            // Preallocate an estimate
            g_instanceData.reserve(g_instanceData.size() + renderItems.size());

            // Append new render items to the global instance data
            for (const RenderItem& renderItem : renderItems) {
                if (renderItem.ignoredViewportIndex != -1 && renderItem.ignoredViewportIndex == i) continue;
                if (renderItem.exclusiveViewportIndex != -1 && renderItem.exclusiveViewportIndex != i) continue;

                g_instanceData.push_back(renderItem);
            }

            // Create indirect draw commands using the stored offset
            std::span<RenderItem> instanceView(g_instanceData.begin() + instanceStart, g_instanceData.end());
            CreateMultiDrawIndirectCommandsSkinned(drawCommands.perViewport[i], instanceView, i, instanceStart);
        }
    }

    void CreateMultiDrawIndirectCommands(std::vector<DrawIndexedIndirectCommand>& commands, std::span<RenderItem> renderItems, int playerIndex, int instanceOffset) {
        std::unordered_map<int, std::size_t> commandMap;
        commands.reserve(renderItems.size());

        for (const RenderItem& renderItem : renderItems) {
            int meshIndex = renderItem.meshIndex;
            Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);

            // If the command exists, increment its instance count
            auto it = commandMap.find(meshIndex);
            if (it != commandMap.end()) {
                commands[it->second].instanceCount++;
            }
            // Otherwise create a new command
            else {
                std::size_t index = commands.size();
                auto& cmd = commands.emplace_back();
                cmd.indexCount = mesh->indexCount;
                cmd.firstIndex = mesh->baseIndex;
                cmd.baseVertex = mesh->baseVertex;
                cmd.baseInstance = EncodeBaseInstance(playerIndex, instanceOffset);
                cmd.instanceCount = 1;

                commandMap[meshIndex] = index;
            }
            instanceOffset++;
        }
    }

    void CreateMultiDrawIndirectCommandsSkinned(std::vector<DrawIndexedIndirectCommand>& commands, std::span<RenderItem> renderItems, int playerIndex, int instanceOffset) {
        std::unordered_map<int, std::size_t> commandMap;
        commands.reserve(renderItems.size());

        for (const RenderItem& renderItem : renderItems) {
            int meshIndex = renderItem.meshIndex;
            SkinnedMesh* mesh = AssetManager::GetSkinnedMeshByIndex(meshIndex);

            // If the command exists, increment its instance count
            auto it = commandMap.find(meshIndex);
            if (it != commandMap.end()) {
                commands[it->second].instanceCount++;
            }
            // Otherwise create a new command
            else {
                std::size_t index = commands.size();
                auto& cmd = commands.emplace_back();
                cmd.indexCount = mesh->indexCount;
                cmd.firstIndex = mesh->baseIndex;
                cmd.baseVertex = renderItem.baseSkinnedVertex;
                cmd.baseInstance = EncodeBaseInstance(playerIndex, instanceOffset);
                cmd.instanceCount = 1;

                commandMap[meshIndex] = index;
            }
            instanceOffset++;
        }
    }

    int EncodeBaseInstance(int playerIndex, int instanceOffset) {
        return (playerIndex << VIEWPORT_INDEX_SHIFT) | instanceOffset;
    }

    void DecodeBaseInstance(int baseInstance, int& playerIndex, int& instanceOffset) {
        playerIndex = baseInstance >> VIEWPORT_INDEX_SHIFT;
        instanceOffset = baseInstance & ((1 << VIEWPORT_INDEX_SHIFT) - 1);
    }

    void ResetBaseSkinnedVertex() {
        g_baseSkinnedVertex = 0;
    }
    void IncrementBaseSkinnedVertex(uint32_t vertexCount) {
        g_baseSkinnedVertex += vertexCount;
    }

    uint32_t GetBaseSkinnedVertex() {
        return g_baseSkinnedVertex;
    }

    const RendererData& GetRendererData() {
        return g_rendererData;
    }

    const std::vector<ViewportData>& GetViewportData() {
        return g_viewportData;
    }

    const std::vector<RenderItem>& GetInstanceData() {
        return g_instanceData;
    }

    const DrawCommandsSet& GetDrawInfoSet() {
        return g_drawCommandsSet;
    }

    const std::vector<GPULight>& GetGPULightData() {
        return g_gpuLightData;
    }
}

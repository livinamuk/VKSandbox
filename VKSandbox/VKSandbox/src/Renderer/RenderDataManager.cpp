#include "RenderDataManager.h"
#include "../Core/Game.h"
#include "../Core/Scene.h"
#include "../Config/Config.h"
#include "../Editor/Editor.h"
#include "../Viewport/ViewportManager.h"
#include <span>

namespace RenderDataManager {
    DrawCommandsSet g_drawCommandsSet;
    RendererData g_rendererData;
    std::vector<ViewportData> g_viewportData;
    std::vector<RenderItem> g_instanceData;

    void UpdateViewportData();
    void UpdateRendererData();
    void UpdateDrawCommandsSet();
    void CreateDrawCommands(DrawCommands& drawCommands, std::vector<RenderItem>& renderItems);
    void CreateMultiDrawIndirectCommands(std::vector<DrawIndexedIndirectCommand>& commands, std::span<RenderItem> renderItems, int playerIndex, int instanceOffset);
    int EncodeBaseInstance(int playerIndex, int instanceOffset);
    void DecodeBaseInstance(int baseInstance, int& playerIndex, int& instanceOffset);

    void Update() {
        UpdateViewportData();
        UpdateRendererData();
        UpdateDrawCommandsSet();
    }

    void UpdateViewportData() {
        g_viewportData.resize(4);
        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            Camera* camera = Editor::IsOpen() ? Editor::GetCameraByIndex(i) : Game::GetLocalPlayerCameraByIndex(i);
            if (!camera) continue;

            g_viewportData[i].projection = viewport->GetProjectionMatrix();
            g_viewportData[i].inverseProjection = glm::inverse(g_viewportData[i].projection);
            g_viewportData[i].view = camera->GetViewMatrix();
            g_viewportData[i].inverseView = camera->GetInverseViewMatrix();
            g_viewportData[i].projectionView = g_viewportData[i].projection * g_viewportData[i].view;
            g_viewportData[i].inverseProjectionView = glm::inverse(g_viewportData[i].projectionView);
            g_viewportData[i].skyboxProjectionView = viewport->GetPerpsectiveMatrix() * g_viewportData[i].view;
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
    }

    void UpdateDrawCommandsSet() {
        g_instanceData.clear();
        auto& set = g_drawCommandsSet;
        CreateDrawCommands(set.geometry, Scene::GetRenderItems());
        CreateDrawCommands(set.geometryBlended, Scene::GetRenderItemsBlended());
        CreateDrawCommands(set.geometryAlphaDiscarded, Scene::GetRenderItemsAlphaDiscarded());
        CreateDrawCommands(set.hairTopLayer, Scene::GetRenderItemsHairTopLayer());
        CreateDrawCommands(set.hairBottomLayer, Scene::GetRenderItemsHairBottomLayer());
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

    void CreateDrawCommands(DrawCommands& drawCommands, std::vector<RenderItem>& renderItems) {
        // Clear any commands from last frame
        for (int i = 0; i < 4; i++) {
            drawCommands.perPlayer[i].clear();
        }
        // Iterate the local players and build the draw commands
        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            Player* player = Game::GetLocalPlayerByIndex(i);

            // Store the instance offset for this player
            int instanceStart = g_instanceData.size();

            // Preallocate an estimate
            g_instanceData.reserve(g_instanceData.size() + renderItems.size());

            // Append new render items to the global instance data
            for (const RenderItem& renderItem : renderItems) {
                g_instanceData.push_back(renderItem);
            }
            // Create indirect draw commands using the stored offset
            std::span<RenderItem> instanceView(g_instanceData.begin() + instanceStart, g_instanceData.end());
            CreateMultiDrawIndirectCommands(drawCommands.perPlayer[i], instanceView, i, instanceStart);
        }
    }

    int EncodeBaseInstance(int playerIndex, int instanceOffset) {
        return (playerIndex << PLAYER_INDEX_SHIFT) | instanceOffset;
    }

    void DecodeBaseInstance(int baseInstance, int& playerIndex, int& instanceOffset) {
        playerIndex = baseInstance >> PLAYER_INDEX_SHIFT;
        instanceOffset = baseInstance & ((1 << PLAYER_INDEX_SHIFT) - 1);
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
}

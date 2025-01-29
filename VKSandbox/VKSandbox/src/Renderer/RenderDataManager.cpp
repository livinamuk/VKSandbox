#include "RenderDataManager.h"
#include "../Core/Game.h"
#include "../Core/Scene.h"
#include "../Config/Config.h"

namespace RenderDataManager {

    RendererData g_rendererData;
    std::vector<PlayerData> g_playerData;

    // Instance data
    std::vector<RenderItem> g_renderItems;
    std::vector<RenderItem> g_renderItemsBlended;
    std::vector<RenderItem> g_renderItemsAlphaDiscarded;
    std::vector<RenderItem> g_renderItemsHairTopLayer;
    std::vector<RenderItem> g_renderItemsHairBottomLayer;

    void UpdatePlayerData();
    void UpdateRendererData();
    void UpdateHairTopLayerRenderItems();
    void UpdateHairBottomLayerRenderItems();

    void Update() {
        UpdatePlayerData();
        UpdateRendererData();
        UpdateHairTopLayerRenderItems();
        UpdateHairBottomLayerRenderItems();
    }

    void UpdatePlayerData() {
        g_playerData.clear();
        for (uint32_t playerIndex : Game::GetActiveViewportPlayerIndices()) {
            Player* player = Game::GetPlayerByIndex(playerIndex);
            Viewport* viewport = Game::GetViewportByIndex(player->GetViewportIndex());
            if (viewport) {
                PlayerData& playerData = g_playerData.emplace_back();
                playerData.projection = viewport->GetProjectionMatrix();
                playerData.inverseProjection = glm::inverse(playerData.projection);
                playerData.view = player->GetViewMatrix();
                playerData.inverseView = player->GetInverseViewMatrix();
                playerData.projectionView = playerData.projection * playerData.view;
                playerData.inverseProjectionView = glm::inverse(playerData.projectionView);
                playerData.skyboxModelMatrix = Transform(player->GetCameraPosition(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(FAR_PLANE * 0.9)).to_mat4();
            }
        }
    }

    void UpdateRendererData() {
        const Resolutions& resolutions = Config::GetResolutions();
        g_rendererData.gBufferWidth = resolutions.gBuffer.x;
        g_rendererData.gBufferHeight = resolutions.gBuffer.y;
        g_rendererData.splitscreenMode = (int)Game::GetSplitscreenMode();
    }

    void UpdateHairTopLayerRenderItems() {
        int playerDataIndex = 0;
        g_renderItemsHairTopLayer.clear();
        
        for (uint32_t playerIndex : Game::GetActiveViewportPlayerIndices()) {
            Player* player = Game::GetPlayerByIndex(playerIndex);
            Viewport* viewport = Game::GetViewportByIndex(player->GetViewportIndex());

            g_renderItemsHairTopLayer.reserve(g_renderItemsHairTopLayer.size() + Scene::GetRenderItemsHairTopLayer().size());
            for (RenderItem& renderItem : Scene::GetRenderItemsHairTopLayer()) {
                g_renderItemsHairTopLayer.emplace_back();
            }
        }
    }

    void UpdateHairBottomLayerRenderItems() {

    }

    //std::vector<RenderItem> g_renderItems;
    //std::vector<RenderItem> g_renderItemsBlended;
    //std::vector<RenderItem> g_renderItemsAlphaDiscarded;
    //std::vector<RenderItem> g_renderItemsHairTopLayer;
    //std::vector<RenderItem> g_renderItemsHairBottomLayer;

    const RendererData& GetRendererData() {
        return g_rendererData;
    }

    const std::vector<PlayerData>& GetPlayerData() {
        return g_playerData;
    }
}

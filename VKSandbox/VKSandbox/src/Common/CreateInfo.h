#pragma once
#include "HellTypes.h"
#include <map>

struct LightCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1, 0.7799999713897705, 0.5289999842643738);
    float radius = 6.0f;
    float strength = 1.0f;
    std::string type = "LAMP_POST";
};

struct PickUpCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    std::string pickUpType = "";
};

struct SpriteSheetObjectCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    bool loop = false;
    bool billboard = true;
    bool renderingEnabled = true;
    float animationSpeed = 1.0f;
    std::string textureName = "";
};

struct BulletCasingCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 force = glm::vec3(0.0f);
    uint32_t modelIndex = 0;
    uint32_t materialIndex = 0;
};

struct GameObjectCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    std::string modelName = "";
    std::vector<MeshRenderingInfo> meshRenderingInfoSet;
};

struct TreeCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    int type = 0;
};

struct SectorCreateInfo {
    std::string sectorName;
    std::string heightMapName;
    std::vector<GameObjectCreateInfo> gameObjects;
    std::vector<LightCreateInfo> lights;
    std::vector<PickUpCreateInfo> pickUps;
    std::vector<TreeCreateInfo> trees;

    // TO DO:
    //std::vector<InteractTextCreateInfo> interactTexts;
    //std::vector<HouseLocation> houseLocations;
    //glm::vec3 mermaidsLocation;
};

struct MapCreateInfo {
    std::string name;
    uint32_t width = 4;
    uint32_t depth = 4;
    ivecXZ spawnCoords = ivecXZ(0, 0);
    std::map<ivecXZ, std::string> sectorLocations;
};
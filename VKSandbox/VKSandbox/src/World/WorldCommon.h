#pragma once

struct HouseData {
    //std::vector<GameObjectCreateInfo> gameObjects;
    //std::vector<TreeCreateInfo> trees;
    //std::vector<LightCreateInfo> lights;
    //HouseType houseType; // enum:.eg small, medium, shed etc
};

struct SectorData {
    //std::vector<GameObjectCreateInfo> gameObjects;
    //std::vector<InteractTextCreateInfo> interactTexts;
    std::vector<LightCreateInfo> lights;
    //std::vector<TreeCreateInfo> trees;
    //std::vector<PickUpCreateInfo> pickUps;
    //std::vector<HouseLocation> houseLocations;
    //glm::vec3 mermaidsLocation;
    //std::string heightMapName;
};
#pragma once

struct HouseData {
    //std::vector<GameObjectCreateInfo> gameObjects;
    //std::vector<TreeCreateInfo> trees;
    //std::vector<LightCreateInfo> lights;
    //HouseType houseType; // enum:.eg small, medium, shed etc
};

struct SectorData {
    std::vector<GameObjectCreateInfo> gameObjects;
    std::vector<LightCreateInfo> lights;
    std::vector<PickUpCreateInfo> pickUps;
    std::vector<TreeCreateInfo> trees;

    // TO DO:
    //std::vector<InteractTextCreateInfo> interactTexts;
    //
    //std::vector<HouseLocation> houseLocations;
    //glm::vec3 mermaidsLocation;
    //std::string heightMapName;
};
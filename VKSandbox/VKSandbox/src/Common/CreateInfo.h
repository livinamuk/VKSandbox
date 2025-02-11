#pragma once
#include "HellTypes.h"

struct LightCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1, 0.7799999713897705, 0.5289999842643738);
    float radius = 6.0f;
    float strength = 1.0f;
    LightType type = LightType::LAMP_POST;
};

struct SpriteSheetObjectCreateInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    float animationSpeed = 1.0f;
    bool loop = false;
    bool billboard = true;
    std::string textureName = "";
};
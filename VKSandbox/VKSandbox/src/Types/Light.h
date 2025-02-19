#pragma once
#include "HellTypes.h"
#include "CreateInfo.h"
#include "Renderer/Types/Model.hpp"

struct Light {
    Light() = default;
    Light(LightCreateInfo createInfo);
    glm::vec3 GetPosition();
    glm::vec3 GetColor();
    LightCreateInfo GetCreateInfo();
    float GetStrength();
    float GetRadius();
    void SetMousePickIndex(int index);
    void SetPosition(glm::vec3 position);
    void UpdateRenderItems();
    std::vector<RenderItem>& GetRenderItems();

private:
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_color = glm::vec3(1, 0.7799999713897705, 0.5289999842643738);
    int m_mousePickIndex = 0;
    float m_radius = 6.0f;
    float m_strength = 1.0f;
    LightType m_type = LightType::LAMP_POST;
    Material* m_material = nullptr;
    Model* m_model = nullptr;
    std::vector<RenderItem> m_renderItems;
};
#pragma once
#include "HellTypes.h"

namespace Camera {
    void Init(void* glfw);
    void Update(float deltaTime);
    glm::mat4 GetProjectionMatrix();
    glm::mat4 GetViewMatrix();
    glm::mat4 GetInverseViewMatrix();
    glm::vec3 GetViewPos();
    Transform GetTransform(); 
    glm::vec3 GetViewRotation();
    glm::vec3 GetForward();
    glm::vec3 GetRight();
    glm::vec3 GetUp();
}
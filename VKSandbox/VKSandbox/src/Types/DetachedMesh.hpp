#pragma once
#include "../API/OpenGL/Types/GL_detachedMesh.hpp"
#include "../API/Vulkan/Types/vk_detachedMesh.h"
#include "../BackEnd/BackEnd.h"

struct DetachedMesh {
public:
    glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());
private:
    std::string m_name;
    OpenGLDetachedMesh openglDetachedMesh;
    VulkanDetachedMesh vulkanDetachedMesh;

public:

    void UpdateBuffers(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
        if (BackEnd::GetAPI() == API::OPENGL) {
            openglDetachedMesh.UpdateBuffers(vertices, indices);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            //lkanDetachedMesh.UpdateBuffers(vertices, indices);
        }
    }
    OpenGLDetachedMesh& GetGLMesh() {
        return openglDetachedMesh;
    }

    VulkanDetachedMesh& GetVKMesh() {
        return vulkanDetachedMesh;
    }

    void SetName(const std::string& name) {
        m_name = name;
    }

    std::string& GetName() {
        return m_name;
    }

};
#pragma once
#include "../API/OpenGL/Types/GL_detachedMesh.hpp"
#include "../API/Vulkan/Types/vk_detachedMesh.h"
#include "../BackEnd/BackEnd.h"

struct DetachedMesh {
public:
    glm::vec3 m_aabbMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 m_aabbMax = glm::vec3(-std::numeric_limits<float>::max());
private:
    std::string m_name;
    OpenGLDetachedMesh m_openglDetachedMesh;
    VulkanDetachedMesh m_vulkanDetachedMesh;
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

public:
    void UpdateBuffers(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
        m_vertices = vertices;
        m_indices = indices;
        if (BackEnd::GetAPI() == API::OPENGL) {
            m_openglDetachedMesh.UpdateBuffers(vertices, indices);
        }
        else if (BackEnd::GetAPI() == API::VULKAN) {
            //lkanDetachedMesh.UpdateBuffers(vertices, indices);
        }
    }
    OpenGLDetachedMesh& GetGLMesh() {
        return m_openglDetachedMesh;
    }

    VulkanDetachedMesh& GetVKMesh() {
        return m_vulkanDetachedMesh;
    }

    void SetName(const std::string& name) {
        m_name = name;
    }

    std::string& GetName() {
        return m_name;
    }    
    
    std::vector<Vertex>& GetVertices() {
        return m_vertices;
    }

    std::vector<uint32_t>& GetIndices() {
        return m_indices;
    }
};
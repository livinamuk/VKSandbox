#pragma once
#include <glm/glm.hpp>
#include <string>

struct Mesh {
    int32_t baseVertex = 0;
    uint32_t baseIndex = 0;
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    glm::vec3 aabbMin = glm::vec3(0);
    glm::vec3 aabbMax = glm::vec3(0);
    glm::vec3 extents = glm::vec3(0);
    float boundingSphereRadius = 0;

    // TEMP. Remove me
    // Instead access them via the main vertex/index buffer. You don't need or want these copies.
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

    void SetName(const std::string& name) {
        m_name = name;
    }

    int32_t GetIndexCount() {
        return indexCount;
    }

    const std::string& GetName() {
        return m_name;
    }

private:
    std::string m_name = "undefined";
};


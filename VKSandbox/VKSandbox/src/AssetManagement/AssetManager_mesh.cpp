#include "AssetManager.h"

namespace AssetManager {

    int g_nextVertexInsert = 0;
    int g_nextIndexInsert = 0;

    int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, glm::vec3 aabbMin, glm::vec3 aabbMax) {
        std::vector<Mesh>& meshes = GetMeshes();
        std::vector<Vertex>& allVertices = GetVertices();
        std::vector<uint32_t>& allIndices = GetIndies();

        Mesh& mesh = meshes.emplace_back();
        mesh.baseVertex = g_nextVertexInsert;
        mesh.baseIndex = g_nextIndexInsert;
        mesh.vertexCount = (uint32_t)vertices.size();
        mesh.indexCount = (uint32_t)indices.size();
        mesh.SetName(name);
        mesh.aabbMin = aabbMin;
        mesh.aabbMax = aabbMax;
        mesh.extents = aabbMax - aabbMin;
        mesh.boundingSphereRadius = std::max(mesh.extents.x, std::max(mesh.extents.y, mesh.extents.z)) * 0.5f;

        // Remove me
        mesh.m_vertices = vertices;
        mesh.m_indices = indices;

        allVertices.reserve(allVertices.size() + vertices.size());
        allVertices.insert(std::end(allVertices), std::begin(vertices), std::end(vertices));
        allIndices.reserve(allIndices.size() + indices.size());
        allIndices.insert(std::end(allIndices), std::begin(indices), std::end(indices));
        g_nextVertexInsert += mesh.vertexCount;
        g_nextIndexInsert += mesh.indexCount;
        return meshes.size() - 1;
    }

    int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
        // Initialize AABB min and max with first vertex
        glm::vec3 aabbMin = vertices[0].position;
        glm::vec3 aabbMax = vertices[0].position;

        // Calculate AABB by iterating over all vertices
        for (const Vertex& v : vertices) {
            aabbMin = glm::min(aabbMin, v.position);
            aabbMax = glm::max(aabbMax, v.position);
        }

        return CreateMesh(name, vertices, indices, aabbMin, aabbMax);
    }

    int GetMeshIndexByName(const std::string& name) {
        std::vector<Mesh>& meshes = GetMeshes();
        for (int i = 0; i < meshes.size(); i++) {
            if (meshes[i].GetName() == name)
                return i;
        }
        std::cout << "AssetManager::GetMeshIndexByName() failed because '" << name << "' does not exist\n";
        return -1;
    }

    Mesh* GetMeshByName(const std::string& name) {
        std::vector<Mesh>& meshes = GetMeshes();
        for (int i = 0; i < meshes.size(); i++) {
            if (meshes[i].GetName() == name)
                return &meshes[i];
        }
        std::cout << "AssetManager::GetMeshByName() failed because '" << name << "' does not exist\n";
        return nullptr;
    }

    Mesh* GetMeshByIndex(int index) {
        std::vector<Mesh>& meshes = GetMeshes();
        if (index >= 0 && index < meshes.size()) {
            return &meshes[index];
        }
        else {
            std::cout << "AssetManager::GetMeshByIndex() failed because index '" << index << "' is out of range. Size is " << meshes.size() << "!\n";
            return nullptr;
        }
    }
}
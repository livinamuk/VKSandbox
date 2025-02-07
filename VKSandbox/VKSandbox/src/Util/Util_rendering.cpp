#include "Util.h"
#include "AssetManagement/AssetManager.h"

namespace Util {

    void UpdateRenderItemAABB(RenderItem& renderItem) {
        Mesh* mesh = AssetManager::GetMeshByIndex(renderItem.meshIndex);
        if (!mesh) return;

        glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());

        std::vector<glm::vec3> corners = {
            renderItem.modelMatrix * glm::vec4(mesh->aabbMin.x, mesh->aabbMax.y, mesh->aabbMax.z, 1.0f),
            renderItem.modelMatrix * glm::vec4(mesh->aabbMax.x, mesh->aabbMax.y, mesh->aabbMax.z, 1.0f),
            renderItem.modelMatrix * glm::vec4(mesh->aabbMin.x, mesh->aabbMin.y, mesh->aabbMax.z, 1.0f),
            renderItem.modelMatrix * glm::vec4(mesh->aabbMax.x, mesh->aabbMin.y, mesh->aabbMax.z, 1.0f),
            renderItem.modelMatrix * glm::vec4(mesh->aabbMin.x, mesh->aabbMax.y, mesh->aabbMin.z, 1.0f),
            renderItem.modelMatrix * glm::vec4(mesh->aabbMax.x, mesh->aabbMax.y, mesh->aabbMin.z, 1.0f),
            renderItem.modelMatrix * glm::vec4(mesh->aabbMin.x, mesh->aabbMin.y, mesh->aabbMin.z, 1.0f),
            renderItem.modelMatrix * glm::vec4(mesh->aabbMax.x, mesh->aabbMin.y, mesh->aabbMin.z, 1.0f)
        };

        for (glm::vec3& corner : corners) {
            aabbMin = glm::min(aabbMin, corner);
            aabbMax = glm::max(aabbMax, corner);
        }

        renderItem.aabbMin = glm::vec4(aabbMin, 0.0f);
        renderItem.aabbMax = glm::vec4(aabbMax, 0.0f);
    }

    AABB ComputeWorldAABB(glm::vec3& localAabbMin, glm::vec3& localAabbMax, glm::mat4& modelMatrix) {
        glm::vec3 worldAabbMin = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 worldAabbMax = glm::vec3(-std::numeric_limits<float>::max());

        std::vector<glm::vec3> corners = {
            modelMatrix* glm::vec4(localAabbMin.x, localAabbMax.y, localAabbMax.z, 1.0f),
            modelMatrix* glm::vec4(localAabbMax.x, localAabbMax.y, localAabbMax.z, 1.0f),
            modelMatrix* glm::vec4(localAabbMin.x, localAabbMin.y, localAabbMax.z, 1.0f),
            modelMatrix* glm::vec4(localAabbMax.x, localAabbMin.y, localAabbMax.z, 1.0f),
            modelMatrix* glm::vec4(localAabbMin.x, localAabbMax.y, localAabbMin.z, 1.0f),
            modelMatrix* glm::vec4(localAabbMax.x, localAabbMax.y, localAabbMin.z, 1.0f),
            modelMatrix* glm::vec4(localAabbMin.x, localAabbMin.y, localAabbMin.z, 1.0f),
            modelMatrix* glm::vec4(localAabbMax.x, localAabbMin.y, localAabbMin.z, 1.0f)
        };

        for (glm::vec3& corner : corners) {
            worldAabbMin = glm::min(worldAabbMin, corner);
            worldAabbMax = glm::max(worldAabbMax, corner);
        }

        return AABB(worldAabbMin, worldAabbMax);
    }
}
#include "Physics.h"

namespace Physics {
    glm::vec3 PxVec3toGlmVec3(PxVec3 vec) {
        return { vec.x, vec.y, vec.z };
    }
    
    glm::vec3 PxVec3toGlmVec3(PxExtendedVec3 vec) {
        return { vec.x, vec.y, vec.z };
    }
    
    PxVec3 GlmVec3toPxVec3(glm::vec3 vec) {
        return { vec.x, vec.y, vec.z };
    }
    
    PxQuat GlmQuatToPxQuat(glm::quat quat) {
        return { quat.x, quat.y, quat.z, quat.w };
    }
    
    glm::quat PxQuatToGlmQuat(PxQuat quat) {
        return { quat.x, quat.y, quat.z, quat.w };
    }    

    glm::mat4 PxMat44ToGlmMat4(physx::PxMat44 pxMatrix) {
        glm::mat4 matrix;
        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                matrix[x][y] = pxMatrix[x][y];
        return matrix;
    }

    physx::PxMat44 GlmMat4ToPxMat44(glm::mat4 glmMatrix) {
        physx::PxMat44 matrix;
        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                matrix[x][y] = glmMatrix[x][y];
        return matrix;
    }

    PhysXRayResult CastPhysXRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, float rayLength, PxU32 collisionFlags, bool cullBackFacing) {
        PxScene* scene = Physics::GetPxScene();
        PxVec3 origin = PxVec3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
        PxVec3 unitDir = PxVec3(rayDirection.x, rayDirection.y, rayDirection.z);
        PxReal maxDistance = rayLength;
        PxRaycastBuffer hit;
        // const PxHitFlags outputFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | ~PxHitFlag::eMESH_BOTH_SIDES;
        PxHitFlags outputFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eMESH_BOTH_SIDES;
        if (cullBackFacing) {
            outputFlags = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;
        }

        // Only ray cast against objects with the GROUP_RAYCAST flag
        PxQueryFilterData filterData = PxQueryFilterData();
        filterData.data.word0 = collisionFlags;

        // Defaults
        PhysXRayResult result;
        result.hitObjectName = "NO_HIT";
        result.hitPosition = glm::vec3(0, 0, 0);
        result.surfaceNormal = glm::vec3(0, 0, 0);
        result.rayDirection = rayDirection;
        result.hitFound = false;
        result.hitActor = nullptr;
        //result.parent = nullptr;

        // Cast the ray
        bool status = scene->raycast(origin, unitDir, maxDistance, hit, outputFlags, filterData);

        if (status) {
            if (hit.block.actor->getName()) {
                result.hitObjectName = hit.block.actor->getName();
            }
            else
                result.hitObjectName = "HIT OBJECT HAS NO ACTOR NAME";

            result.hitPosition = glm::vec3(hit.block.position.x, hit.block.position.y, hit.block.position.z);
            result.surfaceNormal = glm::vec3(hit.block.normal.x, hit.block.normal.y, hit.block.normal.z);
            result.hitFound = true;
            result.hitActor = hit.block.actor;

            if (hit.block.actor->userData) {
                //PhysicsObjectData* physicsObjectData = (PhysicsObjectData*)hit.block.actor->userData;
                //result.objectType = physicsObjectData->type;
                //result.parent = physicsObjectData->parent;
            }
            else {
                //result.objectType = ObjectType::UNDEFINED;
                //result.hitFound = false;
                //std::cout << "no user data found on ray hit\n";
            }
        }
        return result;
    }
}


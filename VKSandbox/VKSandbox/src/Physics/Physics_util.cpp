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
}


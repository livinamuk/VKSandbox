#pragma once
#include "HellTypes.h"
#include "Physics.h"

namespace PhysicsUtil {
    glm::vec3 PxVec3toGlmVec3(PxVec3 vec);
    glm::vec3 PxVec3toGlmVec3(PxExtendedVec3 vec);
    glm::quat PxQuatToGlmQuat(PxQuat quat);
    glm::mat4 PxMat44ToGlmMat4(physx::PxMat44 pxMatrix);
    PxVec3 GlmVec3toPxVec3(glm::vec3 vec);
    PxQuat GlmQuatToPxQuat(glm::quat quat); 
    PxMat44 GlmMat4ToPxMat44(glm::mat4 glmMatrix);

    PxShape* CreateBoxShape(float width, float height, float depth, Transform shapeOffset = Transform(), PxMaterial* material = NULL);
    PxRigidDynamic* CreateRigidDynamic(Transform transform, PhysicsFilterData filterData, PxShape* shape, Transform shapeOffset = Transform());

}
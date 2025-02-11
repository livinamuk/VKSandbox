#include "Physics_util.h"

namespace PhysicsUtil {

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

    PxShape* CreateBoxShape(float width, float height, float depth, Transform shapeOffset, PxMaterial* material) {
        if (material == NULL) {
            material = Physics::GetDefaultMaterial();
        }
        PxShape* shape = Physics::GetPxPhysics()->createShape(PxBoxGeometry(width, height, depth), *material, true);
        PxMat44 localShapeMatrix = GlmMat4ToPxMat44(shapeOffset.to_mat4());
        PxTransform localShapeTransform(localShapeMatrix);
        shape->setLocalPose(localShapeTransform);
        return shape;
    }

    PxRigidDynamic* CreateRigidDynamic(Transform transform, PhysicsFilterData physicsFilterData, PxShape* shape, Transform shapeOffset) {

        PxQuat quat = GlmQuatToPxQuat(glm::quat(transform.rotation));
        PxTransform trans = PxTransform(PxVec3(transform.position.x, transform.position.y, transform.position.z), quat);
        PxRigidDynamic* body = Physics::GetPxPhysics()->createRigidDynamic(trans);

        // You are passing in a PxShape pointer and any shape offset will affects that actually object, wherever the fuck it is up the function chain.
        // Maybe look into this when you can be fucked, possibly you can just set the isExclusive bool to true, where and whenever the fuck that is and happens.
        PxFilterData filterData;
        filterData.word0 = (PxU32)physicsFilterData.raycastGroup;
        filterData.word1 = (PxU32)physicsFilterData.collisionGroup;
        filterData.word2 = (PxU32)physicsFilterData.collidesWith;
        shape->setQueryFilterData(filterData);       // ray casts
        shape->setSimulationFilterData(filterData);  // collisions
        PxMat44 localShapeMatrix = GlmMat4ToPxMat44(shapeOffset.to_mat4());
        PxTransform localShapeTransform(localShapeMatrix);
        shape->setLocalPose(localShapeTransform);

        body->attachShape(*shape);
        PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
        Physics::GetPxScene()->addActor(*body);
        return body;
    }
}


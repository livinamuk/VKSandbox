#include "Physics.h"

namespace Physics {
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

    void Physics::Destroy(PxRigidDynamic*& rigidDynamic) {
        if (rigidDynamic) {
            if (rigidDynamic->userData) {
                //delete static_cast<PhysicsObjectData*>(rigidDynamic->userData);
                rigidDynamic->userData = nullptr;
            }
            Physics::GetPxScene()->removeActor(*rigidDynamic);
            rigidDynamic->release();
            rigidDynamic = nullptr;
        }
    }

    void Physics::Destroy(PxRigidStatic*& rigidStatic) {
        if (rigidStatic) {
            if (rigidStatic->userData) {
                //delete static_cast<PhysicsObjectData*>(rigidStatic->userData);
                rigidStatic->userData = nullptr;
            }
            Physics::GetPxScene()->removeActor(*rigidStatic);
            rigidStatic->release();
            rigidStatic = nullptr;
        }
    }

    void Physics::Destroy(PxShape*& shape) {
        if (shape) {
            if (shape->userData) {
                //delete static_cast<PhysicsObjectData*>(shape->userData);
                shape->userData = nullptr;
            }
            shape->release();
            shape = nullptr;
        }
    }

    void Physics::Destroy(PxRigidBody*& rigidBody) {
        if (rigidBody) {
            if (rigidBody->userData) {
                //delete static_cast<PhysicsObjectData*>(rigidBody->userData);
                rigidBody->userData = nullptr;
            }
            Physics::GetPxScene()->removeActor(*rigidBody);
            rigidBody->release();
            rigidBody = nullptr;
        }
    }

    void Physics::Destroy(PxTriangleMesh*& triangleMesh) {
        if (triangleMesh) {
            triangleMesh = nullptr;
        }
    }

}


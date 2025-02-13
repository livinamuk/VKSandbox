#include "BulletCasing.h"
#include "Util.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Audio.h"
#include "Physics/Physics.h"

#define BULLET_CASING_LIFETIME 2.0f

BulletCasing::BulletCasing(BulletCasingCreateInfo createInfo) {
    m_materialIndex = createInfo.materialIndex;

    // Get model
    Model* model = AssetManager::GetModelByIndex(createInfo.modelIndex);
    if (!model) {
        std::cout << "BulletCasing(BulletCasingCreateInfo createInfo) failed from invalid model\n";
        return;
    }
    if (model->GetMeshCount() < 1) {
        std::cout << "BulletCasing(BulletCasingCreateInfo createInfo) failed from mesh count 0\n";
    }
    
    // Get mesh
    m_meshIndex = model->GetMeshIndices()[0];
    Mesh* mesh = AssetManager::GetMeshByIndex(m_meshIndex);
    if (!mesh) {
        std::cout << "BulletCasing(BulletCasingCreateInfo createInfo) failed from invalid mesh\n";
    }
    
    Transform transform;
    transform.position = createInfo.position;
    transform.rotation = createInfo.rotation;

    PhysicsFilterData filterData;
    filterData.raycastGroup = RaycastGroup::RAYCAST_DISABLED;
    filterData.collisionGroup = CollisionGroup::BULLET_CASING;
    filterData.collidesWith = CollisionGroup::ENVIROMENT_OBSTACLE;

    m_pxShape = Physics::CreateBoxShape(mesh->extents.x, mesh->extents.y, mesh->extents.z);
    m_pxRigidBody = Physics::CreateRigidDynamic(transform, filterData, m_pxShape);

    PxVec3 force = PxVec3(createInfo.force.x, createInfo.force.y, createInfo.force.z);
    m_pxRigidBody->addForce(force);
    m_pxRigidBody->setName("BulletCasing");
    m_pxRigidBody->userData = nullptr;

    // Angular force
    PxVec3 torque = PxVec3(Util::RandomFloat(-10.0f, 10.0f), Util::RandomFloat(-10.0f, 10.0f), Util::RandomFloat(-10.0f, 10.0f));
    m_pxRigidBody->addTorque(torque);
}

void BulletCasing::CleanUp() {
    Physics::Destroy(m_pxShape);
    Physics::Destroy(m_pxRigidBody);
}

const glm::mat4& BulletCasing::GetModelMatrix() {

    Transform localTransform;
    localTransform.scale *= glm::vec3(2.0f);
    if (m_pxRigidBody) {
        m_modelMatrix = Physics::PxMat44ToGlmMat4(m_pxRigidBody->getGlobalPose()) * localTransform.to_mat4();
    }
    return m_modelMatrix;
}

void BulletCasing::Update(float deltaTime) {
    m_lifeTime += deltaTime;
    // These don't have collision right away because it fucks with the muzzle flash
    if (m_pxRigidBody && !m_collisionsEnabled && m_lifeTime > 0.0005) {
        PxShape* shape;
        m_pxRigidBody->getShapes(&shape, 1);
        PxFilterData filterData = shape->getQueryFilterData();
        filterData.word2 = CollisionGroup::ENVIROMENT_OBSTACLE;
        shape->setQueryFilterData(filterData);
        shape->setSimulationFilterData(filterData);
        m_collisionsEnabled = true;
    }
    //if (m_lifeTime < BULLET_CASING_LIFETIME) {
    //    Transform localTransform;
    //    localTransform.scale *= glm::vec3(2.0f);
    //    m_modelMatrix = Physics::PxMat44ToGlmMat4(m_pxRigidBody->getGlobalPose()) * localTransform.to_mat4();
    //}
    else if (m_pxRigidBody) {
        CleanUp();
    }
}

void BulletCasing::CollisionResponse() {
    if (m_audioDelay == 0) {
        Audio::PlayAudio("BulletCasingBounce.wav", Util::RandomFloat(0.1f, 0.2f));
    }
    m_audioDelay = 0.1f;
}

void BulletCasing::UpdatePhysXPointer() {
    // Update collision object PhysX pointer
    //if (m_pxRigidBody) {
    //    if (m_pxRigidBody->userData) {
    //        delete static_cast<PhysicsObjectData*>(m_pxRigidBody->userData);
    //    }
    //    m_pxRigidBody->userData = new PhysicsObjectData(ObjectType::GAME_OBJECT, this);
    //}
}
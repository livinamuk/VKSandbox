#include "Physics.h"
#include "HellDefines.h"
#include "HellEnums.h"
#include <iostream>

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {
    PX_UNUSED(attributes0);
    PX_UNUSED(attributes1);
    PX_UNUSED(constantBlockSize);
    PX_UNUSED(constantBlock);
    // generate contacts for all that were not filtered above
    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

    if (filterData0.word2 == CollisionGroup::NO_COLLISION) {
        return PxFilterFlag::eKILL;
    }
    else if ((filterData0.word2 & filterData1.word1) && (filterData1.word2 & filterData0.word1)) {
        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
        return PxFilterFlag::eDEFAULT;
    }

    return PxFilterFlag::eKILL;
}

class UserErrorCallback : public physx::PxErrorCallback {
public:
    virtual void reportError(physx::PxErrorCode::Enum /*code*/, const char* message, const char* file, int line) {
        std::cout << file << " line " << line << ": " << message << "\n";
        std::cout << "\n";
    }
} gErrorCallback;

namespace Physics {

    physx::PxPhysics* g_physics = NULL;
    physx::PxScene* g_scene = NULL;
    physx::PxFoundation* g_foundation;
    physx::PxDefaultAllocator g_allocator;
    physx::PxDefaultCpuDispatcher* g_dispatcher = NULL;
    physx::PxPvd* g_pvd = NULL;
    physx::PxMaterial* g_defaultMaterial = NULL;
    physx::PxControllerManager* g_characterControllerManager;
    std::vector<CollisionReport> g_collisionReports;
    std::vector<CharacterCollisionReport> g_characterCollisionReports;
    ContactReportCallback g_contactReportCallback;
    CCTHitCallback g_cctHitCallback;

    #define PVD_HOST "127.0.0.1"

    void Init() {
        g_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_allocator, gErrorCallback);
        g_pvd = physx::PxCreatePvd(*g_foundation);
        physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
        g_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
        g_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_foundation, physx::PxTolerancesScale(), true, g_pvd);
        physx::PxSceneDesc sceneDesc(g_physics->getTolerancesScale());
        sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
        g_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher = g_dispatcher;
        sceneDesc.filterShader = contactReportFilterShader;
        sceneDesc.simulationEventCallback = &g_contactReportCallback;

        g_scene = g_physics->createScene(sceneDesc);
        g_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
        g_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);

        physx::PxPvdSceneClient* pvdClient = g_scene->getScenePvdClient();
        if (pvdClient) {
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
            pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        }
        g_defaultMaterial = g_physics->createMaterial(0.5f, 0.5f, 0.6f);

        // Character controller shit
        g_characterControllerManager = PxCreateControllerManager(*g_scene);

        // temporary ground plane
        PxRigidStatic* groundPlane = NULL;
        PxShape* groundShape = NULL;

        groundPlane = PxCreatePlane(*g_physics, PxPlane(0, 1, 0, 5.0f), *g_defaultMaterial);
        g_scene->addActor(*groundPlane);
        groundPlane->getShapes(&groundShape, 1);
        PxFilterData filterData;
        filterData.word0 = RaycastGroup::RAYCAST_ENABLED; // must be disabled or it causes crash in scene::update when it tries to retrieve rigid body flags from this actor
        filterData.word1 = CollisionGroup::ENVIROMENT_OBSTACLE;
        filterData.word2 = CollisionGroup::BULLET_CASING | CollisionGroup::GENERIC_BOUNCEABLE | CollisionGroup::PLAYER;
        groundShape->setQueryFilterData(filterData);
        groundShape->setSimulationFilterData(filterData); // sim is for ragz
    }

    void StepPhysics(float deltaTime) {
        ClearCollisionReports();
        g_scene->simulate(deltaTime);
        g_scene->fetchResults(true);
    }

    void AddCollisionReport(CollisionReport& collisionReport) {
        g_collisionReports.push_back(collisionReport);
    }

    void ClearCollisionReports() {
        g_collisionReports.clear();
    }

    void ClearCharacterControllerCollsionReports() {
        g_characterCollisionReports.clear();
    }

    std::vector<CollisionReport>& GetCollisionReports() {
        return g_collisionReports;
    }

    std::vector<CharacterCollisionReport>& GetCharacterCollisionReports() {
        return g_characterCollisionReports;
    }

    std::vector<Vertex> GetDebugLineVertices(DebugLineRenderMode debugLineRenderMode, std::vector<PxRigidActor*> ignoreList) {
        if (debugLineRenderMode == DebugLineRenderMode::SHOW_NO_LINES) return std::vector<Vertex>();

        // Prepare
        PxU32 nbActors = g_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
        if (nbActors) {
            std::vector<PxRigidActor*> actors(nbActors);
            g_scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<PxActor**>(&actors[0]), nbActors);
            for (PxRigidActor* actor : actors) {
                PxShape* shape;
                actor->getShapes(&shape, 1);
                actor->setActorFlag(PxActorFlag::eVISUALIZATION, true);
                for (PxRigidActor* ignoredActor : ignoreList) {
                    if (ignoredActor == actor) {
                        actor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
                    }
                }
                if (debugLineRenderMode == DebugLineRenderMode::PHYSX_RAYCAST) {
                    if (shape->getQueryFilterData().word0 == RaycastGroup::RAYCAST_DISABLED) {
                        actor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
                    }
                }
                else if (debugLineRenderMode == DebugLineRenderMode::PHYSX_COLLISION) {
                    if (shape->getQueryFilterData().word1 == CollisionGroup::NO_COLLISION) {
                        actor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
                    }
                }
            }
        }
        // Build vertices
        std::vector<Vertex> vertices;
        auto* renderBuffer = &g_scene->getRenderBuffer();
        for (unsigned int i = 0; i < renderBuffer->getNbLines(); i++) {
            auto pxLine = renderBuffer->getLines()[i];
            Vertex v1, v2;
            v1.position = Physics::PxVec3toGlmVec3(pxLine.pos0);
            v2.position = Physics::PxVec3toGlmVec3(pxLine.pos1);
            if (debugLineRenderMode == DebugLineRenderMode::PHYSX_ALL) {
                v1.normal = GREEN;
                v2.normal = GREEN;
            }
            else if (debugLineRenderMode == DebugLineRenderMode::PHYSX_COLLISION) {
                v1.normal = LIGHT_BLUE;
                v2.normal = LIGHT_BLUE;
            }
            else if (debugLineRenderMode == DebugLineRenderMode::PHYSX_RAYCAST) {
                v1.normal = RED;
                v2.normal = RED;
            }
            vertices.push_back(v1);
            vertices.push_back(v2);
        }
        return vertices;
    }

    PxMaterial* GetDefaultMaterial() {
        return g_defaultMaterial;
    }

    PxPhysics* GetPxPhysics() {
        return g_physics;
    }

    PxScene* GetPxScene() {
        return g_scene;
    }

    CCTHitCallback& GetCharacterControllerHitCallback() {
        return g_cctHitCallback;
    }

    PxControllerManager* GetCharacterControllerManager() {
        return g_characterControllerManager;
    }
}

void CCTHitCallback::onShapeHit(const PxControllerShapeHit& hit) {
    CharacterCollisionReport report;
    report.hitNormal = Physics::PxVec3toGlmVec3(hit.worldNormal);
    report.worldPosition = Physics::PxVec3toGlmVec3(hit.worldPos);
    report.characterController = hit.controller;
    report.hitShape = hit.shape;
    report.hitActor = hit.actor;
    Physics::GetCharacterCollisionReports().push_back(report);
}

void CCTHitCallback::onControllerHit(const PxControllersHit& hit) {
}

void CCTHitCallback::onObstacleHit(const PxControllerObstacleHit& hit) {
}



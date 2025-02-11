#pragma once
#pragma warning(push, 0)
#include "PxPhysicsAPI.h"
#include "geometry/PxGeometryHelpers.h"
#pragma warning(pop)
#include <vector>
#include "HellTypes.h"

using namespace physx;

struct CollisionReport {
    PxActor* rigidA = NULL;
    PxActor* rigidB = NULL;
};

struct CharacterCollisionReport {
    PxController* characterController;
    PxShape* hitShape;
    PxRigidActor* hitActor;
    glm::vec3 hitNormal;
    glm::vec3 worldPosition;
};

//struct OverlapReport {
//    std::vector<OverlapResult> hits;
//    bool HitsFound() {
//        return hits.size();
//    }
//};

class CCTHitCallback : public PxUserControllerHitReport {
public:
    void onShapeHit(const PxControllerShapeHit& hit);
    void onControllerHit(const PxControllersHit& hit);
    void onObstacleHit(const PxControllerObstacleHit& hit);
};

namespace Physics {
    void Init();
    void StepPhysics(float deltaTime);
    void AddCollisionReport(CollisionReport& collisionReport);
    void ClearCollisionReports();
    std::vector<CollisionReport>& GetCollisionReports();
    std::vector<CharacterCollisionReport>& GetCharacterCollisionReports();
    std::vector<Vertex> GetDebugLineVertices(DebugLineRenderMode debugLineRenderMode, std::vector<PxRigidActor*> ignoreList);
    PxMaterial* GetDefaultMaterial();
    PxPhysics* GetPxPhysics();
    PxScene* GetPxScene();
    CCTHitCallback& GetCharacterControllerHitCallback();    
    PxControllerManager* GetCharacterControllerManager();
}

class ContactReportCallback : public PxSimulationEventCallback {
public:
    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
    void onWake(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
    void onSleep(PxActor** actors, PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
    void onTrigger(PxTriggerPair* pairs, PxU32 count) { PX_UNUSED(pairs); PX_UNUSED(count); }
    void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}

    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* /*pairs*/, PxU32 /*nbPairs*/) {
        if (!pairHeader.actors[0] || !pairHeader.actors[1]) {
            return;
        }
        CollisionReport report;
        report.rigidA = pairHeader.actors[0];
        report.rigidB = pairHeader.actors[1];
        Physics::AddCollisionReport(report);
    }
};

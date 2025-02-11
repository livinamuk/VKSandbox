#include "Player.h"
#include "Physics/Physics.h"
#include "Physics/Physics_util.h"

void Player::CreateCharacterController(glm::vec3 position) {

    float height = PLAYER_CAPSULE_HEIGHT;
    height = m_viewHeightStanding - PLAYER_CAPSULE_RADIUS - PLAYER_CAPSULE_RADIUS;

    PxMaterial* material = Physics::GetDefaultMaterial();
    PxCapsuleControllerDesc* desc = new PxCapsuleControllerDesc;
    desc->setToDefault();
    desc->height = height;
    desc->radius = PLAYER_CAPSULE_RADIUS;
    desc->position = PxExtendedVec3(position.x, position.y + (height / 2) + (PLAYER_CAPSULE_RADIUS * 2), position.z);
    desc->material = material;
    desc->stepOffset = 0.05f;
    desc->contactOffset = 0.001;
    desc->scaleCoeff = .99f;
    desc->reportCallback = &Physics::GetCharacterControllerHitCallback();
    desc->slopeLimit = cosf(glm::radians(85.0f));
    m_characterController = Physics::GetCharacterControllerManager()->createController(*desc);

    PxShape* shape;
    m_characterController->getActor()->getShapes(&shape, 1);

    PxFilterData filterData;
    filterData.word1 = CollisionGroup::PLAYER;
    filterData.word2 = CollisionGroup(ITEM_PICK_UP | ENVIROMENT_OBSTACLE | SHARK);
    shape->setQueryFilterData(filterData);
}

void Player::MoveCharacterController(glm::vec3 displacement) {
    PxFilterData filterData;
    filterData.word0 = 0;
    filterData.word1 = CollisionGroup::ENVIROMENT_OBSTACLE | CollisionGroup::ENVIROMENT_OBSTACLE_NO_DOG | CollisionGroup::SHARK;	// Things to collide with
    PxControllerFilters data;
    data.mFilterData = &filterData;
    PxF32 minDist = 0.001f;
    float fixedDeltaTime = (1.0f / 60.0f);
    m_characterController->move(PxVec3(displacement.x, displacement.y, displacement.z), minDist, fixedDeltaTime, data);
    m_position = PhysicsUtil::PxVec3toGlmVec3(m_characterController->getFootPosition());
}

void Player::UpdateCharacterController() {
   //
   //m_characterController->setSlopeLimit(cosf(glm::radians(80.0f)));
   //
   //float crouchScale = 0.5f;
   //float standingHeight = m_viewHeightStanding - PLAYER_CAPSULE_RADIUS - PLAYER_CAPSULE_RADIUS;
   //float crouchingHeight = PLAYER_CAPSULE_HEIGHT * crouchScale;
   //
   //// If you pressed crouch while on the ground, lower the height of the character controller to prevent jolting the camera
   //if (PressedCrouch() && m_grounded) {
   //    PxExtendedVec3 footPosition = _characterController->getFootPosition();
   //    footPosition.y -= standingHeight * (crouchScale * 0.5f);
   //    _characterController->setFootPosition(footPosition);
   //}
   //// If released crouching and aren't in water, move the character controller up so camera doesn't jolt
   //if (m_pressingCrouchLastFrame && !PressingCrouch() && !FeetBelowWater()) {
   //    PxExtendedVec3 footPosition = _characterController->getFootPosition();
   //    footPosition.y += standingHeight * (crouchScale * 0.5f);
   //    _characterController->setFootPosition(footPosition);
   //}
   //// Change the height
   //PxCapsuleController* capsuleController = static_cast<PxCapsuleController*>(_characterController);
   //if (IsCrouching()) {
   //    capsuleController->setHeight(crouchingHeight);
   //}
   //else {
   //    capsuleController->setHeight(standingHeight);
   //}
}

PxShape* Player::GetCharacterControllerShape() {
    PxShape* shape;
    m_characterController->getActor()->getShapes(&shape, 1);
    return shape;
}

PxRigidDynamic* Player::GetCharacterControllerActor() {
    return m_characterController->getActor();
}


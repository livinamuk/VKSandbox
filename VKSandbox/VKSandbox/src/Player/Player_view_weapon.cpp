#include "Player.h"
#include "Input/InputMulti.h"
#include "Core/Debug.h"
#include "Input/Input.h"
#include "Util/Util.h"

void Player::UpdateViewWeapon(float deltaTime) {    
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    if (!viewWeapon) return;

    SkinnedModel* model = viewWeapon->m_skinnedModel;

    glm::mat4 dmMaster = glm::mat4(1);
    glm::mat4 cameraMatrix = glm::mat4(1);
    glm::mat4 cameraBindMatrix = glm::mat4(1);
    glm::mat4 root = glm::mat4(1);


    for (int i = 0; i < model->m_nodes.size(); i++) {
        if (model->m_nodes[i].name == "camera") {
            cameraBindMatrix = model->m_nodes[i].inverseBindTransform;
        }
    }

    glm::mat4 cameraAnimation = inverse(cameraBindMatrix) * inverse(dmMaster) * cameraMatrix;

    if (viewWeapon->m_skinnedModel->GetName() == "Knife" ||
        viewWeapon->m_skinnedModel->GetName() == "Shotgun" ||
        viewWeapon->m_skinnedModel->GetName() == "Smith" ||
        viewWeapon->m_skinnedModel->GetName() == "P90" ||
        viewWeapon->m_skinnedModel->GetName() == "SPAS") {
        cameraAnimation = inverse(cameraBindMatrix) * cameraMatrix;
    }

    // Weapon sway
    float xMax = 5.0;
    float SWAY_AMOUNT = 0.125f;
    float SMOOTH_AMOUNT = 4.0f;
    float SWAY_MIN_X = -xMax;
    float SWAY_MAX_X = xMax;
    float SWAY_MIN_Y = -2;
    float SWAY_MAX_Y = 0.95f;
    float xOffset = (float)InputMulti::GetMouseXOffset(m_mouseIndex);
    float yOffset = (float)InputMulti::GetMouseYOffset(m_mouseIndex);
    float movementX = xOffset * SWAY_AMOUNT;
    float movementY = -yOffset * SWAY_AMOUNT;

    movementX = std::min(movementX, SWAY_MAX_X);
    movementX = std::max(movementX, SWAY_MIN_X);
    movementY = std::min(movementY, SWAY_MAX_Y);
    movementY = std::max(movementY, SWAY_MIN_Y);

    if (HasControl()) {
        m_weaponSwayX = Util::FInterpTo(m_weaponSwayX, movementX, deltaTime, SMOOTH_AMOUNT);
        m_weaponSwayY = Util::FInterpTo(m_weaponSwayY, movementY, deltaTime, SMOOTH_AMOUNT);
    }

    if (ViewportIsVisible()) {
       //Debug::AddText("m_weaponSwayX: " + std::to_string(m_weaponSwayX));
       //Debug::AddText("m_weaponSwayY: " + std::to_string(m_weaponSwayY));
       //Debug::AddText("xOffset: " + std::to_string(xOffset));
       //Debug::AddText("yOffset: " + std::to_string(yOffset));
    }

    float weaponScale = 0.001f;
    //weaponScale = 0.01f;

    // Final transform
    Transform transform;
    transform.position = m_camera.GetPosition();
    transform.position += (m_weaponSwayX * weaponScale) * m_camera.GetRight();
    transform.position += (m_weaponSwayY * weaponScale) * m_camera.GetUp();
    transform.rotation.x = m_camera.GetEulerRotation().x;
    transform.rotation.y = m_camera.GetEulerRotation().y;
    transform.scale = glm::vec3(weaponScale);
    viewWeapon->m_cameraMatrix = transform.to_mat4() * glm::inverse(cameraBindMatrix) * glm::inverse(dmMaster);

    viewWeapon->useCameraMatrix = true;
}
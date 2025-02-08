#include "Player.h"
#include "Util/Util.h"

void Player::UpdateViewWeapon() {    
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    if (!viewWeapon) return;

    glm::mat4 dmMaster = glm::mat4(1);
    glm::mat4 cameraMatrix = glm::mat4(1);
    glm::mat4 cameraBindMatrix = glm::mat4(1);
    glm::mat4 root = glm::mat4(1);

    for (int i = 0; i < viewWeapon->m_jointWorldMatrices.size(); i++) {
        if (Util::StrCmp(viewWeapon->m_jointWorldMatrices[i].name, "camera")) {
            cameraMatrix = viewWeapon->m_jointWorldMatrices[i].worldMatrix;
        }
    }
    for (int i = 0; i < viewWeapon->m_jointWorldMatrices.size(); i++) {
        if (Util::StrCmp(viewWeapon->m_jointWorldMatrices[i].name, "Dm-Master")) {
            dmMaster = viewWeapon->m_jointWorldMatrices[i].worldMatrix;
        }
    }

    SkinnedModel* model = viewWeapon->m_skinnedModel;

    for (int i = 0; i < model->m_joints.size(); i++) {
        if (Util::StrCmp(model->m_joints[i].m_name, "camera")) {
            glm::mat4 cameraBoneTransform = viewWeapon->m_jointWorldMatrices[i].worldMatrix;
            glm::mat4 cameraBindPose = model->m_joints[i].m_inverseBindTransform;
            cameraBindMatrix = model->m_joints[i].m_inverseBindTransform;
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

    Transform transform;
    transform.position = m_camera.GetPosition();
    transform.rotation.x = m_camera.GetEulerRotation().x;
    transform.rotation.y = m_camera.GetEulerRotation().y;
    transform.scale = glm::vec3(0.001);
    viewWeapon->m_cameraMatrix = transform.to_mat4() * glm::inverse(cameraBindMatrix) * glm::inverse(dmMaster);

    viewWeapon->useCameraMatrix = true;
}
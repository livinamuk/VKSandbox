#include "Player.h"
#include "Editor/Editor.h"
#include "Input/InputMulti.h"

void Player::UpdateCamera() {
    // Mouselook
    if (!Editor::IsOpen() && m_controlEnabled) {
        float xOffset = (float)InputMulti::GetMouseXOffset(m_mouseIndex);
        float yOffset = (float)InputMulti::GetMouseYOffset(m_mouseIndex);
        m_camera.AddPitch(-yOffset * m_mouseSensitivity);
        m_camera.AddYaw(xOffset * m_mouseSensitivity);        
    }

    // Position
    m_camera.SetPosition(m_position + glm::vec3(0, m_currentViewHeight, 0));

    // Get view weapon camera matrix
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    SkinnedModel* model = viewWeapon->m_skinnedModel;

    glm::mat4 cameraMatrix = viewWeapon->GetAnimatedTransformByBoneName("camera");
    glm::mat4 dmMaster = viewWeapon->GetAnimatedTransformByBoneName("Dm-Master");
    glm::mat4 cameraBindMatrix;

    for (int i = 0; i < model->m_joints.size(); i++) {
        if (Util::StrCmp(model->m_joints[i].m_name, "camera")) {
            cameraBindMatrix = model->m_joints[i].m_inverseBindTransform;
        }
    }
    m_viewWeaponCameraMatrix = inverse(cameraBindMatrix) * cameraMatrix;
}

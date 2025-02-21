#include "Player.h"
#include "Input/InputMulti.h"

bool Player::PressingWalkForward() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyDown(m_keyboardIndex, m_mouseIndex, m_controls.WALK_FORWARD);
    }
    else {
        // return InputMulti::ButtonDown(_controllerIndex, m_controls.WALK_FORWARD);
        return false;
    }
}

bool Player::PressingWalkBackward() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyDown(m_keyboardIndex, m_mouseIndex, m_controls.WALK_BACKWARD);
    }
    else {
        //return InputMulti::ButtonDown(_controllerIndex, m_controls.WALK_BACKWARD);
        return false;
    }
}

bool Player::PressingWalkLeft() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyDown(m_keyboardIndex, m_mouseIndex, m_controls.WALK_LEFT);
    }
    else {
        //return InputMulti::ButtonDown(_controllerIndex, m_controls.WALK_LEFT);
        return false;
    }
}

bool Player::PressingWalkRight() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyDown(m_keyboardIndex, m_mouseIndex, m_controls.WALK_RIGHT);
    }
    else {
        //return InputMulti::ButtonDown(_controllerIndex, m_controls.WALK_RIGHT);
        return false;
    }
}

bool Player::PressingCrouch() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyDown(m_keyboardIndex, m_mouseIndex, m_controls.CROUCH);
    }
    else {
        //return InputMulti::ButtonDown(_controllerIndex, m_controls.CROUCH);
        return false;
    }
}

bool Player::PressedWalkForward() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.WALK_FORWARD);
    }
    else {
        //return InputMulti::ButtonPressed(_controllerIndex, m_controls.WALK_FORWARD);
        return false;
    }
}

bool Player::PressedWalkBackward() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.WALK_BACKWARD);
    }
    else {
        //return InputMulti::ButtonPressed(_controllerIndex, m_controls.WALK_BACKWARD);
        return false;
    }
}

bool Player::PressedWalkLeft() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.WALK_LEFT);
    }
    else {
        // return InputMulti::ButtonPressed(_controllerIndex, m_controls.WALK_LEFT);
        return false;
    }
}

bool Player::PressedWalkRight() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.WALK_RIGHT);
    }
    else {
        //return InputMulti::ButtonPressed(_controllerIndex, m_controls.WALK_RIGHT);
        return false;
    }
}

bool Player::PressedInteract() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.INTERACT);
    }
    else {
        //return InputMulti::ButtonPressed(_controllerIndex, m_controls.INTERACT);
        return false;
    }
}

bool Player::PressedReload() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.RELOAD);
    }
    else {
        //return InputMulti::ButtonPressed(_controllerIndex, m_controls.RELOAD);
        return false;
    }
}

bool Player::PressedFire() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.FIRE);
    }
    else {
        //return InputMulti::ButtonPressed(_controllerIndex, m_controls.FIRE);
        return false;
    }
}

bool Player::PressingFire() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyDown(m_keyboardIndex, m_mouseIndex, m_controls.FIRE);
    }
    else {
        // return InputMulti::ButtonDown(_controllerIndex, m_controls.FIRE);
        return false;
    }
}

bool Player::PresingJump() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyDown(m_keyboardIndex, m_mouseIndex, m_controls.JUMP);
    }
    else {
        //return InputMulti::ButtonPressed(_controllerIndex, m_controls.JUMP);
        return false;
    }
}
bool Player::PressedWeaponMiscFunction() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyDown(m_keyboardIndex, m_mouseIndex, m_controls.MISC_WEAPON_FUNCTION);
    }
    else {
        //return InputMulti::ButtonPressed(_controllerIndex, m_controls.JUMP);
        return false;
    }
}

bool Player::PressedCrouch() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.CROUCH);
    }
    else {
        // return InputMulti::ButtonPressed(_controllerIndex, m_controls.CROUCH);
        return false;
    }
}

bool Player::PressedNextWeapon() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.NEXT_WEAPON);
    }
    else {
        //return InputMulti::ButtonPressed(_controllerIndex, m_controls.NEXT_WEAPON);
        return false;
    }
}

bool Player::PressingADS() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyDown(m_keyboardIndex, m_mouseIndex, m_controls.ADS);
    }
    else {
        // return InputMulti::ButtonDown(_controllerIndex, m_controls.ADS);
        return false;
    }
}

bool Player::PressedADS() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.ADS);
    }
    else {
        // return InputMulti::ButtonPressed(_controllerIndex, m_controls.ADS);
        return false;
    }
}

bool Player::PressedMelee() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.MELEE);
    }
    else {
        // return InputMulti::ButtonPressed(_controllerIndex, m_controls.ADS);
        return false;
    }
}


bool Player::PressedEscape() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.ESCAPE);
    }
    else {
        // return InputMulti::ButtonPressed(_controllerIndex, m_controls.ESCAPE);
        return false;
    }
}
bool Player::PressedFullscreen() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.DEBUG_FULLSCREEN);
    }
    else {
        // return InputMulti::ButtonPressed(_controllerIndex, m_controls.ESCAPE);
        return false;
    }
}

bool Player::PressedFlashlight() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.FLASHLIGHT);
    }
    else {
        //return InputMulti::ButtonPressed(_controllerIndex, _controls.FIRE);
        return false;
    }
}

bool Player::PressedOne() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.DEBUG_ONE);
    }
    else {
        // return InputMulti::ButtonPressed(_controllerIndex, m_controls.ESCAPE);
        return false;
    }
}

bool Player::PressedTwo() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.DEBUG_TWO);
    }
    else {
        // return InputMulti::ButtonPressed(_controllerIndex, m_controls.ESCAPE);
        return false;
    }
}

bool Player::PressedThree() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.DEBUG_THREE);
    }
    else {
        // return InputMulti::ButtonPressed(_controllerIndex, m_controls.ESCAPE);
        return false;
    }
}
bool Player::PressedFour() {
    if (m_inputType == InputType::KEYBOARD_AND_MOUSE) {
        return InputMulti::KeyPressed(m_keyboardIndex, m_mouseIndex, m_controls.DEBUG_FOUR);
    }
    else {
        // return InputMulti::ButtonPressed(_controllerIndex, m_controls.ESCAPE);
        return false;
    }
}

void Player::SetKeyboardIndex(int32_t index) {
    m_keyboardIndex = index;
}

void Player::SetMouseIndex(int32_t index) {
    m_mouseIndex = index;
}
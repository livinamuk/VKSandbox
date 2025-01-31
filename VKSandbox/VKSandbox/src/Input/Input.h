#pragma once
#include "keycodes.h"

namespace Input {
	void Init(void* glfwWinodw);
	void Update();
	bool KeyPressed(unsigned int keycode);
	bool KeyDown(unsigned int keycode);
	float GetMouseOffsetX();
	float GetMouseOffsetY();
    bool LeftMouseDown();
    bool RightMouseDown();
    bool MiddleMouseDown();
    bool LeftMousePressed();
    bool MiddleMousePressed();
    bool RightMousePressed();
	bool MouseWheelUp();
	bool MouseWheelDown();
    int GetMouseWheelValue();
    int GetMouseX();
    int GetMouseY();
	void PreventRightMouseHold();
    void DisableCursor();
    void HideCursor();
    void ShowCursor();
    int GetCursorScreenX();
    int GetCursorScreenY();
    void SetCursorPosition(int x, int y);
    int GetMouseXPreviousFrame();
    int GetMouseYPreviousFrame();
}
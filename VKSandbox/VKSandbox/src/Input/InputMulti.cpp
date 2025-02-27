#include "InputMulti.h"
#include "HellDefines.h"
#include "BackEnd/BackEnd.h"
#include "BackEnd/GLFWIntegration.h"
#include <iostream>
#include "keycodes.h"

#define NOMINMAX
#include <Windows.h>
#ifdef _MSC_VER
#undef GetObject
#endif

namespace InputMulti {
    MouseState g_mouseStates[4];
    KeyboardState g_keyboardStates[4];
    std::vector<HANDLE> g_mouseHandles;
    std::vector<HANDLE> g_keyboardHandles;
    const USHORT HID_USAGE_GENERIC_MOUSE = 0x02;
    const USHORT HID_USAGE_GENERIC_KEYBOARD = 0x06;

    int GetHandleIndex(std::vector<HANDLE>* handleVector, HANDLE handle) {
        for (int i = 0; i < handleVector->size(); i++) {
            if ((*handleVector)[i] == handle) {
                return i;
            }
        }
        handleVector->push_back(handle);
        return (int)handleVector->size() - 1;
    }

    LRESULT CALLBACK targetWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (uMsg == WM_INPUT) {
            UINT dataSize = 0;
            // First call to get data size
            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

            if (dataSize > 0)
            {
                RAWINPUT raw = RAWINPUT();
                // Second call to get the actual data
                if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
                {
                    // Mice
                    if (raw.header.dwType == RIM_TYPEMOUSE)
                    {
                        int mouseID = GetHandleIndex(&g_mouseHandles, raw.header.hDevice);
                        if (mouseID >= 4) return 0;

                        switch (raw.data.mouse.ulButtons) {
                        case RI_MOUSE_LEFT_BUTTON_DOWN: g_mouseStates[mouseID].leftMouseDown = true;	break;
                        case RI_MOUSE_LEFT_BUTTON_UP: g_mouseStates[mouseID].leftMouseDown = false; break;
                        case RI_MOUSE_RIGHT_BUTTON_DOWN: g_mouseStates[mouseID].rightMouseDown = true; break;
                        case RI_MOUSE_RIGHT_BUTTON_UP: g_mouseStates[mouseID].rightMouseDown = false; break;
                        }

                        // Wheel change values are device-dependent. Check RAWMOUSE docs for details.
                        if (raw.data.mouse.usButtonData != 0) {
                            //	cout << "MOUSE " << mouseID << ": WHEEL CHANGE " << raw.data.mouse.usButtonData << endl;
                        }

                        g_mouseStates[mouseID].xoffset -= raw.data.mouse.lLastX;
                        g_mouseStates[mouseID].yoffset += raw.data.mouse.lLastY;
                    }
                    // Keyboard
                    else if (raw.header.dwType == RIM_TYPEKEYBOARD)
                    {
                        int keyboardID = GetHandleIndex(&g_keyboardHandles, raw.header.hDevice);
                        auto keycode = raw.data.keyboard.VKey;
                        if (keyboardID >= 4) return 0;

                        //std::cout << keycode << "\n";

                        switch (raw.data.keyboard.Flags) {
                        case RI_KEY_MAKE: g_keyboardStates[keyboardID].keyDown[keycode] = true; break;
                        case RI_KEY_BREAK: g_keyboardStates[keyboardID].keyDown[keycode] = false; break;
                        }
                    }
                }
            }
            return 0;
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    bool RegisterDeviceOfType(USHORT type, HWND eventWindow) {
        RAWINPUTDEVICE rid = {};
        rid.usUsagePage = 0x01;
        rid.usUsage = type;
        rid.dwFlags = 0;
        rid.hwndTarget = eventWindow;
        return RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }

    void Init() {
        HINSTANCE hInstance = GetModuleHandle(NULL);
        WNDCLASS windowClass = {};
        windowClass.lpfnWndProc = targetWindowProc;
        windowClass.hInstance = hInstance;
        windowClass.lpszClassName = TEXT("InputWindow");
        if (!RegisterClass(&windowClass)) {
            std::cout << "Failed to register window class\n";
            return;
        }

        HWND eventWindow = CreateWindowEx(0, windowClass.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
        if (!eventWindow) {
            std::cout << "Failed to register window class\n";
            return;
        }
        else {
            std::cout << "Multi mouse/keyboard support initialized\n";
        }
        RegisterDeviceOfType(HID_USAGE_GENERIC_MOUSE, eventWindow);
        RegisterDeviceOfType(HID_USAGE_GENERIC_KEYBOARD, eventWindow);
    }

    void ResetState() {
        memset(g_keyboardStates, 0, sizeof(g_keyboardStates));
        memset(g_mouseStates, 0, sizeof(g_mouseStates));
    }

    void Update() {
        MSG msg;
        //	while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        for (MouseState& state : g_mouseStates) {
            // Left mouse down/pressed
            if (state.leftMouseDown && !state.leftMouseDownLastFrame)
                state.leftMousePressed = true;
            else
                state.leftMousePressed = false;
            state.leftMouseDownLastFrame = state.leftMouseDown;

            // Right mouse down/pressed
            if (state.rightMouseDown && !state.rightMouseDownLastFrame)
                state.rightMousePressed = true;
            else
                state.rightMousePressed = false;
            state.rightMouseDownLastFrame = state.rightMouseDown;
        }

        for (KeyboardState& state : g_keyboardStates) {
            // Key press
            for (int i = 0; i < 350; i++) {
                if (state.keyDown[i] && !state.keyDownLastFrame[i])
                    state.keyPressed[i] = true;
                else
                    state.keyPressed[i] = false;
                state.keyDownLastFrame[i] = state.keyDown[i];
            }
        }

        // Out of window focus? then remove any detected input
        HWND activeWindow = GetActiveWindow();
        HWND myWindow = (HWND)GLFWIntegration::GetWin32Window();
        if ((void*)myWindow != (void*)activeWindow) {
            for (KeyboardState& state : g_keyboardStates) {
                for (int i = 0; i < 350; i++) {
                    state.keyPressed[i] = false;
                    state.keyDownLastFrame[i] = false;
                }
            }
            for (MouseState& state : g_mouseStates) {
                state.leftMousePressed = false;
                state.leftMouseDownLastFrame = false;
                state.rightMousePressed = false;
                state.rightMouseDownLastFrame = false;
            }
        }
    }

    void ResetMouseOffsets() {
        for (MouseState& state : g_mouseStates) {
            state.xoffset = 0;
            state.yoffset = 0;
        }
    }

    int GetMouseYOffset(int index) {
        if (index < 0 || index >= 4)
            return 0;
        else
            return g_mouseStates[index].yoffset;
    }

    bool LeftMouseDown(int index) {
        if (index < 0 || index >= 4)
            return false;
        else
            return g_mouseStates[index].leftMouseDown;
    }

    bool RightMouseDown(int index) {
        if (index < 0 || index >= 4)
            return false;
        else
            return g_mouseStates[index].rightMouseDown;
    }

    bool LeftMousePressed(int index) {
        if (index < 0 || index >= 4)
            return false;
        else
            return g_mouseStates[index].leftMousePressed;
    }

    bool RightMousePressed(int index) {
        if (index < 0 || index >= 4)
            return false;
        else
            return g_mouseStates[index].rightMousePressed;
    }

    int GetMouseXOffset(int index) {
        if (index < 0 || index >= 4)
            return 0;
        else
            return g_mouseStates[index].xoffset;
    }

    bool KeyDown(int keyboardIndex, int mouseIndex, unsigned int keycode) {
        // It's a mouse button
        if (keycode == HELL_MOUSE_LEFT && mouseIndex >= 0 && mouseIndex < 4) {
            return g_mouseStates[mouseIndex].leftMouseDown;
        }
        else if (keycode == HELL_MOUSE_RIGHT && mouseIndex >= 0 && mouseIndex < 4) {
            return g_mouseStates[mouseIndex].rightMouseDown;
        }

        // It's a keyboard button
        else if (keyboardIndex >= 0 && keyboardIndex < 4) {
            return g_keyboardStates[keyboardIndex].keyDown[keycode];
        }
        // Something else invalid
        else {
            return false;
        }
    }

    bool KeyPressed(int keyboardIndex, int mouseIndex, unsigned int keycode) {

        // It's a mouse button
        if (keycode == HELL_MOUSE_LEFT && mouseIndex >= 0 && mouseIndex < 4)
            return g_mouseStates[mouseIndex].leftMousePressed;
        else if (keycode == HELL_MOUSE_RIGHT && mouseIndex >= 0 && mouseIndex < 4)
            return g_mouseStates[mouseIndex].rightMousePressed;

        // It's a keyboard button
        else if (keyboardIndex >= 0 && keyboardIndex < 4)
            return g_keyboardStates[keyboardIndex].keyPressed[keycode];
        // Something else invalid
        else
            return false;
    }
}
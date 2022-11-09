#include "lh_input.h"
#include "lh_platform.h"
#include <windows.h>
#include <windowsx.h>
#include <xinput.h>

Input gInput;
Input gLastInput;
global_variable WORD XInputButtons[] = 
{
    XINPUT_GAMEPAD_DPAD_UP,
    XINPUT_GAMEPAD_DPAD_DOWN,
    XINPUT_GAMEPAD_DPAD_LEFT,
    XINPUT_GAMEPAD_DPAD_RIGHT,
    XINPUT_GAMEPAD_START,
    XINPUT_GAMEPAD_BACK,
    XINPUT_GAMEPAD_A,
    XINPUT_GAMEPAD_B,
    XINPUT_GAMEPAD_X,
    XINPUT_GAMEPAD_Y
};
extern bool gRunning;
extern Window gWindow;

bool KeyboardGetKeyDown(i32 key) {
    return gInput.keys[key].isDown;
}

bool KeyboardGetKeyJustDown(i32 key) {
    if(gInput.keys[key].isDown != gInput.keys[key].wasDown) {
        return gInput.keys[key].isDown; 
    }
    return false;
}

bool KeyboardGetKeyJustUp(i32 key) {
    if(gInput.keys[key].isDown != gInput.keys[key].wasDown) {
        return gInput.keys[key].wasDown; 
    }
    return false;
}

bool KeyboardGetKeyUp(i32 key) {
    return !gInput.keys[key].isDown;
}

bool MouseGetButtonDown(i32 button) {
    return gInput.mouseButtons[button].isDown;
}

bool MouseGetButtonJustDown(i32 button) {
    if(gInput.mouseButtons[button].isDown != gInput.mouseButtons[button].wasDown) {
        return gInput.mouseButtons[button].isDown; 
    }
    return false;
}

bool MouseGetButtonJustUp(i32 button) {
    if(gInput.mouseButtons[button].isDown != gInput.mouseButtons[button].wasDown) {
        return gInput.mouseButtons[button].wasDown; 
    }
    return false;
}

bool MouseGetButtonUp(i32 button) {
    return !gInput.mouseButtons[button].isDown;
}

i32 MouseGetCursorX() {
    return gInput.mouseX;
}
i32 MouseGetCursorY() {
    return gInput.mouseY;
}

i32 MouseGetWheel() {
    return gInput.mouseWheel;
}

i32 MouseGetLastCursorX() {
    return gLastInput.mouseX;
}

i32 MouseGetLastCursorY() {
    return gLastInput.mouseY;
}

i32 MouseGetScreenX() {
    POINT point;
    GetCursorPos(&point);
    return point.x;
}

i32 MouseGetScreenY() {
    POINT point;
    GetCursorPos(&point);
    return point.y;
}

void MouseSetCursor(i32 x, i32 y) {
    SetCursorPos(x, y);
}

void MouseShowCursor(bool value) {
    ShowCursor(value);
}

bool JoysickGetButtonDown(i32 button) {
    return gInput.joyButtons[button].isDown;
}

bool JoysickGetButtonJustDown(i32 button) {
    if(gInput.joyButtons[button].isDown != gInput.joyButtons[button].wasDown) {
        return gInput.joyButtons[button].isDown; 
    }
    return false;
}

bool JoysickGetButtonJustUp(i32 button) {
    if(gInput.joyButtons[button].isDown != gInput.joyButtons[button].wasDown) {
        return gInput.joyButtons[button].wasDown; 
    }
    return false;
}

bool JoysickGetButtonUp(i32 button) {
    return !gInput.joyButtons[button].isDown;
}

f32 JoysickGetLeftStickX() {
    return gInput.leftStickX;
}

f32 JoysickGetLeftStickY() {
    return gInput.leftStickY;
}

f32 JoysickGetRightStickX() {
    return gInput.rightStickX;
}

f32 JoysickGetRightStickY() {
    return gInput.rightStickY;
}

internal 
f32 ProcessXInputStick(SHORT value, i32 deadZoneValue)
{
    f32 result = 0;
    if(value < -deadZoneValue)
    {
        result = (f32)(value + deadZoneValue) / (32768.0f - deadZoneValue);
    }
    else if(value > deadZoneValue)
    {
        result = (f32)(value - deadZoneValue) / (32767.0f - deadZoneValue);
    }
    return result;
}

void ProcessInputAndMessages() {
    gInput.mouseWheel = 0;
    MSG msg = {};
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        switch(msg.message)
        {
            case WM_QUIT: {
                gRunning = false;      
            } break;
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP: { 
                bool wasDown = ((msg.lParam & (1 << 30)) != 0);
                bool isDown = ((msg.lParam & (1 << 31)) == 0);
                if(isDown != wasDown) {
                    DWORD vkCode = (DWORD)msg.wParam;
                    gInput.keys[vkCode].isDown = isDown;
                }
            }break;
            case WM_MOUSEMOVE: {
                gInput.mouseX = (i32)GET_X_LPARAM(msg.lParam); 
                gInput.mouseY = (i32)GET_Y_LPARAM(msg.lParam); 
            }break;
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: {
                gInput.mouseLeft.isDown = ((msg.wParam & MK_LBUTTON) != 0);
                gInput.mouseMiddle.isDown = ((msg.wParam & MK_MBUTTON) != 0);
                gInput.mouseRight.isDown = ((msg.wParam & MK_RBUTTON) != 0);
            }break;
            case WM_MOUSEWHEEL: {
                i32 zDelta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
                if (zDelta != 0) {
                    // Flatten the input to an OS-independent (-1, 1)
                    zDelta = (zDelta < 0) ? -1 : 1;
                    gInput.mouseWheel = zDelta;
                }
            } break;
            default: {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }break;
        } 
    }

    XINPUT_STATE state = {};
    if(XInputGetState(0, &state) == ERROR_SUCCESS)
    {
        XINPUT_GAMEPAD *pad = &state.Gamepad;
        for(i32 i = 0; i < ARRAY_LENGTH(gInput.joyButtons) - 2; ++i)
        {
            gInput.joyButtons[i].isDown = pad->wButtons & XInputButtons[i];
        }
        gInput.joyButtons[10].isDown = (pad->bLeftTrigger > 0);
        gInput.joyButtons[11].isDown = (pad->bRightTrigger > 0);
        gInput.leftStickX =  ProcessXInputStick(pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        gInput.leftStickY =  ProcessXInputStick(pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        gInput.rightStickX = ProcessXInputStick(pad->sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        gInput.rightStickY = ProcessXInputStick(pad->sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    }
    else
    {
        for(i32 i = 0; i < ARRAY_LENGTH(gInput.joyButtons); ++i)
        {
            gInput.joyButtons[i].isDown = false;
        }
        gInput.leftStickX = 0.0f; 
        gInput.leftStickY = 0.0f;
        gInput.rightStickX = 0.0f;
        gInput.rightStickY = 0.0f;
    }

    
    for(i32 i = 0; i < ARRAY_LENGTH(gInput.keys); ++i) {
        if(gLastInput.keys[i].isDown) {
            gInput.keys[i].wasDown = true;
        }
        else {
            gInput.keys[i].wasDown = false; 
        }
    }
    for(i32 i = 0; i < ARRAY_LENGTH(gInput.mouseButtons); ++i) {
        if(gLastInput.mouseButtons[i].isDown) {
            gInput.mouseButtons[i].wasDown = true;
        }
        else {
            gInput.mouseButtons[i].wasDown = false; 
        }
    }
    for(i32 i = 0; i < ARRAY_LENGTH(gInput.joyButtons); ++i) {
        if(gLastInput.joyButtons[i].isDown) {
            gInput.joyButtons[i].wasDown = true;
        }
        else {
            gInput.joyButtons[i].wasDown = false; 
        }
    }
}



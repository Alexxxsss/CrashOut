#include "InputSystem.h"
#include <windowsx.h> 

bool InputSystem::InputHandler::UpdateEvents(UINT aMessage, WPARAM aWParam, LPARAM aLParam)
{
    if (aMessage == WM_KEYDOWN)
    {
        myInputState[aWParam] = true;
        return true;
    }
    if (aMessage == WM_KEYUP)
    {
        myInputState[aWParam] = false;
        return true;
    }
    if (aMessage == WM_MOUSEMOVE)
    {
        myInputMousePos.x = GET_X_LPARAM(aLParam);
        myInputMousePos.y = GET_Y_LPARAM(aLParam);
        return true;
    }
    return false;
}

bool InputSystem::InputHandler::IsKeyDown(const int aKeyCode)
{
    return myCurrentState[aKeyCode];
}

bool InputSystem::InputHandler::IsKeyPressed(const int aKeyCode)
{
    return myCurrentState[aKeyCode] && !myPreviousState[aKeyCode];
}
bool InputSystem::InputHandler::IsKeyReleased(const int aKeyCode)
{
    if (myPreviousState[aKeyCode] && !myCurrentState[aKeyCode])
    {
        return true;
    }
    else
    {
        return false;
    }

}
bool InputSystem::InputHandler::UpdateMouseInput(UINT aMessage)
{
    return DidMouseMove();
}

void InputSystem::InputHandler::UpdateInput()
{
    myPreviousState = myCurrentState;
    myCurrentState = myInputState;

    myPreviousMousePos = myCurrentMousePos;
    myCurrentMousePos = myInputMousePos;

    myMouseDelta.x = myCurrentMousePos.x - myPreviousMousePos.x;
    myMouseDelta.y = myCurrentMousePos.y - myPreviousMousePos.y;
}
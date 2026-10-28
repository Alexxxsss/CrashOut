#pragma once
#include <iostream>
#include <array>
#include <windows.h>

namespace InputSystem
{
    class InputHandler
    {
    private:
        std::array<bool, 256> myCurrentState;
        std::array<bool, 256> myPreviousState;
        std::array<bool, 256> myInputState;
        POINT myInputMousePos{ 0, 0 };
        POINT myCurrentMousePos{ 0, 0 };
        POINT myPreviousMousePos{ 0, 0 };
        POINT myMouseDelta{ 0, 0 };


    public:
        InputHandler()
        {

        }

        bool UpdateEvents(UINT aMessage, WPARAM aWParam, LPARAM aLParam);

        bool IsKeyDown(const int aKeyCode);

        bool IsKeyPressed(const int aKeyCode);
        bool IsKeyReleased(const int aKeyCode);

        POINT GetMousePosition() { return myCurrentMousePos; }
        POINT GetMouseDelta() { return myMouseDelta; }
        bool DidMouseMove() { return myMouseDelta.x != 0 || myMouseDelta.y != 0; }
        bool UpdateMouseInput(UINT aMessage);
        void UpdateInput();
    };

}
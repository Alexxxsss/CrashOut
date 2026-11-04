#pragma once
#include <iostream>
#include <array>
#include <windows.h>

namespace CommonUtilities
{
	class InputHandler
	{
	private:
		std::array<bool, 256> myCurrentState{};
		std::array<bool, 256> myPreviousState{};
		std::array<bool, 256> myInputState{};

		POINT myInputMousePos{ 0, 0 };
		POINT myCurrentMousePos{ 0, 0 };
		POINT myPreviousMousePos{ 0, 0 };
		POINT myMouseDelta{ 0, 0 };

		int myInputWheelDelta = 0;
		int myWheelDelta = 0;
		HWND myHwnd;
	public:
		InputHandler(HWND& aHwnd) : myHwnd(aHwnd) {}

		bool UpdateEvents(UINT aMessage, WPARAM aWParam, LPARAM aLParam);

		bool IsKeyDown(const int aKeyCode);
		bool IsKeyPressed(const int aKeyCode);
		bool IsKeyReleased(const int aKeyCode);

		bool IsMouseButtonDown(const int aButton);
		bool IsMouseButtonPressed(const int aButton);
		bool IsMouseButtonReleased(const int aButton);

		int GetMouseWheelDelta() const { return myWheelDelta; }
		POINT GetMousePosition() const { return myCurrentMousePos; }
		POINT GetMouseDelta() const { return myMouseDelta; }
		bool DidMouseMove() const { return myMouseDelta.x != 0 || myMouseDelta.y != 0; }

		POINT GetAbsoluteMousePosition() const;
		void SetMousePosition(int aX, int aY);
		void CaptureMouse();
		void ReleaseMouse();

		void UpdateInput();
	};
}
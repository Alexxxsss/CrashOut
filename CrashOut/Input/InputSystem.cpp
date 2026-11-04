#include "InputSystem.h"
#include <windowsx.h>

bool CommonUtilities::InputHandler::UpdateEvents(UINT aMessage, WPARAM aWParam, LPARAM aLParam)
{
	switch (aMessage)
	{
	case WM_KEYDOWN:
		if (aWParam < 256)
		{
			myInputState[aWParam] = true;
			return true;
		}
		break;

	case WM_KEYUP:
		if (aWParam < 256)
		{
			myInputState[aWParam] = false;
			return true;
		}
		break;

	case WM_LBUTTONDOWN:
		myInputState[VK_LBUTTON] = true;
		return true;

	case WM_LBUTTONUP:
		myInputState[VK_LBUTTON] = false;
		return true;

	case WM_RBUTTONDOWN:
		myInputState[VK_RBUTTON] = true;
		return true;

	case WM_RBUTTONUP:
		myInputState[VK_RBUTTON] = false;
		return true;

	case WM_MBUTTONDOWN:
		myInputState[VK_MBUTTON] = true;
		return true;

	case WM_MBUTTONUP:
		myInputState[VK_MBUTTON] = false;
		return true;

	case WM_MOUSEWHEEL:
		myInputWheelDelta += GET_WHEEL_DELTA_WPARAM(aWParam);
		return true;

	case WM_MOUSEMOVE:
		myInputMousePos.x = GET_X_LPARAM(aLParam);
		myInputMousePos.y = GET_Y_LPARAM(aLParam);
		return true;
	}

	return false;
}

bool CommonUtilities::InputHandler::IsKeyDown(const int aKeyCode)
{
	return myCurrentState[aKeyCode];
}

bool CommonUtilities::InputHandler::IsKeyPressed(const int aKeyCode)
{
	return myCurrentState[aKeyCode] && !myPreviousState[aKeyCode];
}

bool CommonUtilities::InputHandler::IsKeyReleased(const int aKeyCode)
{
	return myPreviousState[aKeyCode] && !myCurrentState[aKeyCode];
}

bool CommonUtilities::InputHandler::IsMouseButtonDown(const int aButton)
{
	return IsKeyDown(aButton);
}

bool CommonUtilities::InputHandler::IsMouseButtonPressed(const int aButton)
{
	return IsKeyPressed(aButton);
}

bool CommonUtilities::InputHandler::IsMouseButtonReleased(const int aButton)
{
	return IsKeyReleased(aButton);
}

POINT CommonUtilities::InputHandler::GetAbsoluteMousePosition() const
{
	POINT pt{ 0, 0 };
	GetCursorPos(&pt);
	ScreenToClient(myHwnd, &pt);
	return pt;
}

void CommonUtilities::InputHandler::SetMousePosition(int aX, int aY)
{
	POINT pt{ aX, aY };
	ClientToScreen(myHwnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

void CommonUtilities::InputHandler::CaptureMouse()
{
	RECT rect{};
	GetClientRect(myHwnd, &rect);

	POINT topLeft{ rect.left, rect.top };
	POINT bottomRight{ rect.right, rect.bottom };

	ClientToScreen(myHwnd, &topLeft);
	ClientToScreen(myHwnd, &bottomRight);

	RECT clipRect{ topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
	ClipCursor(&clipRect);
}

void CommonUtilities::InputHandler::ReleaseMouse()
{
	ClipCursor(nullptr);
}

void CommonUtilities::InputHandler::UpdateInput()
{
	myPreviousState = myCurrentState;
	myCurrentState = myInputState;

	myPreviousMousePos = myCurrentMousePos;
	myCurrentMousePos = myInputMousePos;

	myMouseDelta.x = myCurrentMousePos.x - myPreviousMousePos.x;
	myMouseDelta.y = myCurrentMousePos.y - myPreviousMousePos.y;

	myWheelDelta = myInputWheelDelta;
	myInputWheelDelta = 0;
}
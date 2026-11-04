#pragma once
#include <windows.h>
namespace CommonUtilities {
    class InputHandler;
}


class Window
{
public:

    Window() = default;
    ~Window() = default;

    bool StartUpWindow(int width, int height, const wchar_t* title);
    bool UpdateOpenWindow(); // Returns false when WM_QUIT is received

    HWND GetHwnd() const { return myHwnd; }
    int GetWidth() const { return myWidth; }
    int GetHeight() const { return myHeight; }

    CommonUtilities::InputHandler* GetInputHandeler() { return myInputHandeler; }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    CommonUtilities::InputHandler* myInputHandeler;

    HWND myHwnd = nullptr;
    int myWidth = 800;
    int myHeight = 600;
};
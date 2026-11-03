#pragma once
#include <windows.h>

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

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND myHwnd = nullptr;
    int myWidth = 800;
    int myHeight = 600;
};
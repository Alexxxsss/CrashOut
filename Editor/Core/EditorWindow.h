#pragma once
#include <windows.h>
#include <memory>
#include <Input/InputSystem.h>

namespace CommonUtilities
{
    class InputHandler;
}


class EditorWindow
{
public:

    EditorWindow() = default;
    ~EditorWindow() = default;

    bool StartUpWindow(int width, int height, const wchar_t* title);
    bool UpdateOpenWindow(); // Returns false when WM_QUIT is received

    HWND GetHwnd() const { return myHwnd; }
    int GetWidth() const { return myWidth; }
    int GetHeight() const { return myHeight; }

    CommonUtilities::InputHandler* GetInputHandler()
    {
        return myInputHandler.get();
    }

    bool WasResized()
    {
        bool resized = myWasResized;
        myWasResized = false;

        return resized;
    }

    bool IsMinimized() const
    {
        return myIsMinimized;
    }

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    std::unique_ptr<CommonUtilities::InputHandler> myInputHandler;
    HWND myHwnd = nullptr;
    int myWidth = 800;
    int myHeight = 600;

    bool myWasResized = false;
    bool myIsMinimized = false;
};
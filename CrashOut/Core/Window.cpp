#include "Window.h"
#include <iostream>
#include <Input/InputSystem.h>

CommonUtilities::InputHandler *globalInputHandeler;

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (globalInputHandeler != nullptr)
    {
        globalInputHandeler->UpdateEvents(uMsg, wParam, lParam);
    }
    switch (uMsg)
    {
    case WM_DESTROY:
        std::cout << "Window is closing..." << std::endl;
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

bool Window::StartUpWindow(int width, int height, const wchar_t* title)
{
    myWidth = width;
    myHeight = height;
    HINSTANCE hInstance = GetModuleHandle(NULL);

    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = Window::WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    // Adjust window rect so the CLIENT area is exactly width x height
    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    myHwnd = CreateWindowExW(
        0, CLASS_NAME, title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        NULL, NULL, hInstance, NULL
    );

    if (myHwnd == NULL)
    {
        std::cerr << "Failed to create window!" << std::endl;
        return false;
    }

    ShowWindow(myHwnd, SW_SHOWDEFAULT);
    globalInputHandeler = new CommonUtilities::InputHandler(myHwnd);
    myInputHandeler = globalInputHandeler;
    return true;
}

bool Window::UpdateOpenWindow()
{
    if (globalInputHandeler != nullptr)
    {
        globalInputHandeler->UpdateInput();
    }
    MSG msg = {};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return false; // Signal that the application should exit
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}
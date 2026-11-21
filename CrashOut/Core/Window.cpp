#include "Window.h"
#include <iostream>
#include <Input/InputSystem.h>
#include <memory>

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Window* pWindow = nullptr;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));

        if (pWindow != nullptr)
        {
            pWindow->myHwnd = hwnd;
        }
    }
    else
    {
        pWindow = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (pWindow != nullptr)
    {
        return pWindow->HandleMessage(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

LRESULT Window::HandleMessage(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    if (myInputHandler != nullptr)
    {
        myInputHandler->UpdateEvents(
            uMsg, wParam, lParam
        );
    }

    switch (uMsg)
    {
    case WM_SIZE:
    {
        myWidth = LOWORD(lParam);
        myHeight = HIWORD(lParam);

        myIsMinimized = (wParam == SIZE_MINIMIZED);

        if (!myIsMinimized)
        {
            myWasResized = true;
        }

        return 0;
    }

    case WM_DESTROY:
        std::cout << "Window is closing..."
            << std::endl;

        PostQuitMessage(0);
        return 0;
    case WM_KILLFOCUS:
    {
        if (myInputHandler)
        {
            myInputHandler->ClearInput();
            myInputHandler->ReleaseMouse();
        }

        return 0;
    }
    default:
        return DefWindowProcW(
            hwnd, uMsg, wParam, lParam
        );
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
        NULL, NULL, hInstance, this 
    );

    if (myHwnd == NULL)
    {
        std::cerr << "Failed to create window!" << std::endl;
        return false;
    }

    ShowWindow(myHwnd, SW_SHOWDEFAULT);


    myInputHandler = std::make_unique<CommonUtilities::InputHandler>(myHwnd);
    return true;
}


bool Window::UpdateOpenWindow()
{
    MSG msg = {};

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (myInputHandler != nullptr)
    {
        myInputHandler->UpdateInput();
    }

    return true;
}

#include "Window.h"
#include <iostream>

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
    {
        std::cout << "Window is closing..." << std::endl;
        PostQuitMessage(0); 
        return 0;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}



void Window::StartUpWindow() {
    
    HINSTANCE hInstance = GetModuleHandle(NULL);

    std::cout << "Console is open automatically. App starting..." << std::endl;

    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = Window::WindowProc; 
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Window Title",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        std::cerr << "Failed to create window!" << std::endl;
        return;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    std::cout << "Window displayed successfully." << std::endl;

    MSG msg = {};
    bool isRunning = true;

    while (isRunning) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                isRunning = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    

}
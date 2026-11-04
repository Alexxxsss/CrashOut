#include <iostream>
#include <DirectXMath.h> // Needed for 3D math (matrices/vectors)

#include "Core/Window.h"
#include "Renderer/Renderer.h"
#include "Input/InputSystem.h"

using namespace DirectX; // Allows using XMMATRIX, XMVECTOR, etc. without typing DirectX:: every time

extern void OnStart();
extern void OnUpdate();
extern void OnCleanUp();

int main()
{
    OnStart();


    const UINT screenWidth = 1080;
    const UINT screenHeight = 720;

    Window window;
    if (!window.StartUpWindow(screenWidth, screenHeight, L"DirectX 11 Engine"))
    {
        return -1;
    }
    HWND hwnd = window.GetHwnd();

    // 2. Initialize Renderer with window handle and size
    Renderer renderer;
    if (!renderer.Initialize(window.GetHwnd(), screenWidth, screenHeight))
    {
        std::cerr << "Renderer initialization failed!" << std::endl;
        return -1;
    }


    bool isRunning = true;

    while (isRunning)
    {
        window.UpdateOpenWindow();
        OnUpdate();

        renderer.BeginFrame(0.1f, 0.2f, 0.4f, 1.0f);

        // ---> YOUR DRAW TRIANGLE CODE GOES HERE <---

        renderer.EndFrame();

        if (window.GetInputHandeler()->IsKeyPressed('W')) { std::cout << "WWW PRESSED\n"; }


    }
    renderer.CleanUp();
    OnCleanUp();
    return 0;
}
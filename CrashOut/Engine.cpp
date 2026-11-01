#include <iostream>

#include "Core/Window.h"
#include "Renderer/Renderer.h"

extern void OnStart();
extern void OnUpdate();
extern void OnCleanUp();

int main()
{
    OnStart();

    Window window;
    window.StartUpWindow();
    HWND hwnd = window.GetHwnd();

    Renderer renderer;
    if (!renderer.Initialize(hwnd, 800, 600))
    {
        std::cerr << "Failed to initialize Direct3D 11 Renderer!\n";
        return -1;
    }


    bool isRunning = true;
    while (isRunning)
    {

        window.UpdateOpenWindow();
        OnUpdate(); 
        // Render pass
        renderer.BeginFrame(0.1f, 0.1f, 0.15f);
        renderer.DrawTriangle();
        renderer.EndFrame();
    }
    renderer.CleanUp();
    OnCleanUp();
    return 0;
}
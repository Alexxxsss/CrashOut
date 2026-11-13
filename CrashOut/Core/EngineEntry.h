#pragma once
#include "Engine.h"

extern CrashOut::Application* CrashOut::CreateApplication();

int main(int argc, char* argv[])
{
    // 1. Initialize Engine Subsystems (Window, Renderer, Input, etc.)
    // CrashOut::Log::Init();
    CommonUtilities::Timer timer;
    // 2. Create the client application (Sandbox/Game)
    auto app = CrashOut::CreateApplication();

    // 3. Start game logic
    app->OnStart();
    // 4. High-resolution timer setup for deltaTime
    // (Using standard chrono or Win32 high-res counter)
    float deltaTime = 0.f; // Placeholder stub

    bool isRunning = true;
    while (isRunning)
    {
        deltaTime = timer.GetDeltaTime();
        timer.Update();

        // Poll window/OS messages & input
        // Update engine systems (Input, Physics, etc.)

        app->OnUpdate(deltaTime);
        app->WindowAndRenderLoop(); // Rendering InsideHere


        // Swap buffers / Present frame
    }

    // 5. Shutdown sequence
    app->OnShutdown();
    app->CleanUp();

    delete app;

    return 0;
}
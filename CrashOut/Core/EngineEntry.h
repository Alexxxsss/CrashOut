#pragma once
#include "Engine.h"

extern CrashOut::Application* CrashOut::CreateApplication();

int main(int argc, char* argv[])
{
    CommonUtilities::Timer timer;
    auto app = CrashOut::CreateApplication();

    app->OnStart();
    float deltaTime = 0.f; 

    bool isRunning = true;
    while (isRunning)
    {
        deltaTime = timer.GetDeltaTime();
        timer.Update();


        if (!app->WindowAndRenderLoop())
        {
            app->OnUpdate(deltaTime);

        }

    }

    // 5. Shutdown sequence
    app->OnShutdown();
    app->CleanUp();

    delete app;

    return 0;
}
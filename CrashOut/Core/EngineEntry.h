
#pragma once

#include "Engine.h"
#include <memory>
#include <stdexcept>

int main(int argc, char* argv[])
{
    CommonUtilities::Timer timer;

    std::unique_ptr<CrashOut::Application> app(
        CrashOut::CreateApplication()
    );

    if (!app)
        return -1;

    app->OnStart();

    bool isRunning = true;

    while (isRunning)
    {
        isRunning = app->ProcessEvents();

        if (!isRunning)
            break;

        timer.Update();

        float deltaTime = timer.GetDeltaTime();

        app->Update(deltaTime);

        app->RenderFrame();
    }

    // 5. Shutdown
    app->OnShutdown();
    app->CleanUp();

    return 0;
}

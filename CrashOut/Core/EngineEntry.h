
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

        if (app->GetWindow().IsMinimized()) ///
        {                                   /// This is for optimization
            Sleep(16);                      /// Makes the main loop slower fps to save cpu power when minimized
        }                                   ///

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

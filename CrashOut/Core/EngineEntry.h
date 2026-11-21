
#pragma once

#include "Engine.h"

#include <memory>
#include <exception>
#include <iostream>
#include <cstdlib>
#include <windows.h>

int main(int argc, char* argv[])
{
    try
    {
        CommonUtilities::Timer timer;

        std::unique_ptr<CrashOut::Application> app(
            CrashOut::CreateApplication()
        );

        if (!app)
            return EXIT_FAILURE;

        app->OnStart();
        app->InitializePhysics();

        bool isRunning = true;

        while (isRunning)
        {
            isRunning = app->ProcessEvents();

            if (!isRunning)
                break;

            timer.Update();


            if (app->GetWindow().IsMinimized()) ///
            {                                   /// This is for optimization
                Sleep(16);                      /// Makes the main loop slower fps to save cpu power when minimized
                continue;                       ///
            }                                   

            float deltaTime = timer.GetDeltaTime();

            app->Update(deltaTime);
            app->RenderFrame();
        }

        app->OnShutdown();
        app->CleanUp();

        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "\n[CrashOut Fatal Error]\n"
            << exception.what()
            << std::endl;

        MessageBoxW(
            nullptr,
            L"A fatal error occurred. "
            L"See the console for details.",
            L"CrashOut Engine Error",
            MB_OK | MB_ICONERROR
        );

        return EXIT_FAILURE;
    }
}

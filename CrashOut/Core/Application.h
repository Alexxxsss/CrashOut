#pragma once

namespace CrashOut
{

    class Application
    {
    public:
        Application() = default;
        virtual ~Application() = default;

        // Prevent copying/moving for engine stability
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        virtual void OnStart() = 0;
        virtual void OnUpdate(float deltaTime) = 0;
        virtual void OnRender() = 0;
        virtual void OnShutdown() = 0;
    };

    // Forward declaration for client factory function
    Application* CreateApplication();

}
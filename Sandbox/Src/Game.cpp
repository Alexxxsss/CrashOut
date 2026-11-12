#include "Game.h" 
#include "Core/EngineEntry.h" 
#include <iostream>

class SandboxApp : public CrashOut::Application
{
public:
    SandboxApp()
    {
        // Initialize game-specific resources
    }

    ~SandboxApp() override = default;

    void OnStart() override
    {
        std::cout << "Sandbox OnStart called!\n";
        // Initialize entities, camera, load scene assets
    }

    void OnUpdate(float deltaTime) override
    {
        // Update game logic, player input, transform matrices
    }

    void OnRender() override
    {
        // Submit draw calls to renderer
    }

    void OnShutdown() override
    {
        std::cout << "Sandbox OnShutdown called!\n";
        // Cleanup game resources
    }
};

// Define the factory function required by EntryPoint.h
CrashOut::Application* CrashOut::CreateApplication()
{
    return new SandboxApp();
}
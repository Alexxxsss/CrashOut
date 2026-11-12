#include "Game.h" 
#include "Core/EngineEntry.h" 
#include <iostream>
#include <DirectXMath.h> 

class SandboxApp : public CrashOut::Application
{

    Camera camera;
    Registry registry;
    Model cubeModel;
    TransformComponent* t1 = nullptr;
    TransformComponent* t2 = nullptr;

public:
    SandboxApp() = default;


    ~SandboxApp() override = default;

    void OnStart() override
    {
        std::cout << "Sandbox OnStart called!\n";

        camera.Init(GetWindowWidth(), GetWindowHeight());


        cubeModel.Init(&GetRenderer());
        cubeModel.Create();
        cubeModel.LoadTexture(); // Default texture



        // --- Entity 1 (Center-left Cube) ---
        Entity entity1 = registry.CreateEntity();
        t1 = &registry.AddComponent<TransformComponent>(entity1);
        t1->position = { -1.5f, 0.0f, 4.0f };
        t1->scale = { 1.0f, 1.0f, 1.0f };
        registry.AddComponent<MeshComponent>(entity1, &cubeModel);

        // --- Entity 2 (Center-right Smaller Cube) ---
        Entity entity2 = registry.CreateEntity();
        t2 = &registry.AddComponent<TransformComponent>(entity2);
        t2->position = { 1.5f, 0.5f, 5.0f };
        t2->scale = { 0.6f, 0.6f, 0.6f };
        registry.AddComponent<MeshComponent>(entity2, &cubeModel);
    }

    void OnUpdate(float deltaTime) override
    {
        if (GetWindow().GetInputHandeler()->IsKeyDown('R'))
        {
            t1->rotation.y += 0.5f * deltaTime;
            t2->rotation.x += 1.f * deltaTime;
            t2->rotation.z += 0.5f * deltaTime;
        }

    }

    void OnRender() override
    {
        ///
        ///
        /// FUNKAR INTE
        /// Måste fixa
        ///
        
        RenderEntities(registry, camera, GetShader(), cubeModel);

    }

    void OnShutdown() override
    {

        std::cout << "Sandbox OnShutdown called!\n";
    }
};

// Define the factory function required by EntryPoint.h
CrashOut::Application* CrashOut::CreateApplication()
{
    return new SandboxApp();
}
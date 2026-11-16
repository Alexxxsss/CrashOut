#include "Game.h" 
#include "Core/EngineEntry.h" 
#include "Core/Vector.h" 
#include <iostream>
#include <DirectXMath.h> 

class SandboxApp : public CrashOut::Application
{

    Camera camera;
    Model cubeModel;
    Entity entity1;
    Entity entity2;
    TransformComponent* t1 = nullptr;
    TransformComponent* t2 = nullptr;
    TransformComponent* t3 = nullptr;


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
        entity1 = myRegistry.CreateEntity();
        t1 = &myRegistry.AddComponent<TransformComponent>(entity1);
        t1->position = { -1.5f, 0.0f, 4.0f };
        t1->scale = { 1.0f, 1.0f, 1.0f };
        myRegistry.AddComponent<MeshComponent>(entity1, &cubeModel);
        myRegistry.AddComponent<BoxColliderComponent>(entity1);


        // --- Entity 2 (Center-right Smaller Cube) ---
        entity2 = myRegistry.CreateEntity();
        t2 = &myRegistry.AddComponent<TransformComponent>(entity2);
        t2->position = { 1.5f, 0.5f, 5.0f };
        t2->scale = { 0.6f, 0.6f, 0.6f };
        myRegistry.AddComponent<MeshComponent>(entity2, &cubeModel);
        myRegistry.AddComponent<BoxColliderComponent>(entity2);


        Entity floor = myRegistry.CreateEntity();
        t3 = &myRegistry.AddComponent<TransformComponent>(floor);
        t3->position = { 0.f, -2.f, 0.f };
        t3->scale = { 100.f, 0.2f, 100.f };
        myRegistry.AddComponent<MeshComponent>(floor, &cubeModel);
        auto& floorCollider = myRegistry.AddComponent<BoxColliderComponent>(floor);
        floorCollider.halfExtents = { 100.f, 0.2f, 100.f };
        floorCollider.isStatic = true;


    }

    void OnUpdate(float deltaTime) override
    {
        float speed = 1;
        if (GetWindow().GetInputHandeler()->IsKeyDown('R'))
        {
            auto* c1 = myRegistry.GetComponent<BoxColliderComponent>(entity1);
            auto* c2 = myRegistry.GetComponent<BoxColliderComponent>(entity2);

            if (c1 && b3Body_IsValid(c1->bodyId))
            {
                b3Body_SetAwake(c1->bodyId, true);
                b3Body_SetAngularVelocity(c1->bodyId, { 0.0f, 2.5f * speed, 0.0f });
                b3Body_SetLinearVelocity(c1->bodyId, { 0.0f, 2.5f * speed, 0.0f });

            }
            if (c2 && b3Body_IsValid(c2->bodyId))
            {
                b3Body_SetAwake(c2->bodyId, true);
                b3Body_SetAngularVelocity(c2->bodyId, { 5.0f * speed, 0.0f, 2.5f * speed });
            }

        }

    }

    void OnRender() override
    {
        ///
        ///
        /// FUNKAR INTE
        /// Måste fixa
        ///
        
        RenderEntities(myRegistry, camera, GetShader(), cubeModel);

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
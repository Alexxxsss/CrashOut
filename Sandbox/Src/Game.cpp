#include "Game.h" 
#include "Core/EngineEntry.h" 
#include "Core/Vector.h" 
#include <iostream>
#include <DirectXMath.h> 

class SandboxApp : public CrashOut::Application
{

    Camera camera;
    Registry registry;
    Model cubeModel;
    Entity entity1;
    Entity entity2;
    TransformComponent* t1 = nullptr;
    TransformComponent* t2 = nullptr;
    TransformComponent* t3 = nullptr;
    PhysicsSystem physics;


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
        entity1 = registry.CreateEntity();
        t1 = &registry.AddComponent<TransformComponent>(entity1);
        t1->position = { -1.5f, 0.0f, 4.0f };
        t1->scale = { 1.0f, 1.0f, 1.0f };
        registry.AddComponent<MeshComponent>(entity1, &cubeModel);
        registry.AddComponent<BoxColliderComponent>(entity1);


        // --- Entity 2 (Center-right Smaller Cube) ---
        entity2 = registry.CreateEntity();
        t2 = &registry.AddComponent<TransformComponent>(entity2);
        t2->position = { 1.5f, 0.5f, 5.0f };
        t2->scale = { 0.6f, 0.6f, 0.6f };
        registry.AddComponent<MeshComponent>(entity2, &cubeModel);
        registry.AddComponent<BoxColliderComponent>(entity2);


        Entity floor = registry.CreateEntity();
        t3 = &registry.AddComponent<TransformComponent>(floor);
        t3->position = { 0.f, -2.f, 0.f };
        t3->scale = { 100.f, 0.2f, 100.f };
        registry.AddComponent<MeshComponent>(floor, &cubeModel);
        auto& floorCollider = registry.AddComponent<BoxColliderComponent>(floor);
        floorCollider.halfExtents = { 100.f, 0.2f, 100.f };
        floorCollider.isStatic = true;


    }

    void OnUpdate(float deltaTime) override
    {
        if (GetWindow().GetInputHandeler()->IsKeyDown('R'))
        {
            auto* c1 = registry.GetComponent<BoxColliderComponent>(entity1);
            auto* c2 = registry.GetComponent<BoxColliderComponent>(entity2);

            if (c1 && b3Body_IsValid(c1->bodyId))
            {
                b3Body_SetAwake(c1->bodyId, true);
                b3Body_SetAngularVelocity(c1->bodyId, { 0.0f, 2.5f, 0.0f });
                b3Body_SetLinearVelocity(c1->bodyId, { 0.0f, 2.5f, 0.0f });

            }
            if (c2 && b3Body_IsValid(c2->bodyId))
            {
                b3Body_SetAwake(c2->bodyId, true);
                b3Body_SetAngularVelocity(c2->bodyId, { 5.0f, 0.0f, 2.5f });
            }

        }
        physics.UpdateTransforms(registry, 1.0f / 60.0f);

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
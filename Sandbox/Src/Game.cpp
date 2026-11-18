#include "Game.h" 
#include "Core/EngineEntry.h" 
#include "Core/Vector.h" 
#include <iostream>
#include <DirectXMath.h>
#include <memory>
#include <cmath>
#include <stdexcept>

class SandboxApp : public CrashOut::Application
{
    Camera camera;
    std::shared_ptr<Model> cubeModel;

    std::shared_ptr<Material> defaultMaterial;
    std::shared_ptr<Material> brickMaterial;

    Entity entity1;
    Entity entity2;
    Entity floor;
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

        cubeModel = std::make_shared<Model>();
        cubeModel->Init(&GetRenderer());
        cubeModel->Create();

        defaultMaterial = std::make_shared<Material>();
        defaultMaterial->Init(&GetRenderer());
        defaultMaterial->LoadTexture(L"PrototypeTexture.png");

        brickMaterial = std::make_shared<Material>();
        brickMaterial->Init(&GetRenderer());
        brickMaterial->LoadTexture(L"Brick.png");




        // --- Entity 1 (Center-left Cube) ---
        entity1 = myRegistry.CreateEntity();
        t1 = &myRegistry.AddComponent<TransformComponent>(entity1);
        t1->position = { -1.5f, 0.0f, 4.0f };
        t1->scale = { 1.0f, 1.0f, 1.0f };
        myRegistry.AddComponent<MeshComponent>(entity1, cubeModel);
        myRegistry.AddComponent<MaterialComponent>(entity1, brickMaterial);
        myRegistry.AddComponent<BoxColliderComponent>(entity1);



        // --- Entity 2 (Center-right Smaller Cube) ---

        entity2 = myRegistry.CreateEntity();
        t2 = &myRegistry.AddComponent<TransformComponent>(entity2);
        t2->position = { 1.5f, 0.5f, 5.0f };
        t2->scale = { 0.6f, 0.6f, 0.6f };
        myRegistry.AddComponent<MeshComponent>(entity2, cubeModel);
        myRegistry.AddComponent<MaterialComponent>(entity2, brickMaterial);

        myRegistry.AddComponent<BoxColliderComponent>(entity2);


        floor = myRegistry.CreateEntity();
        t3 = &myRegistry.AddComponent<TransformComponent>(floor);
        t3->position = { 0.f, -2.f, 0.f };
        t3->scale = { 100.f, 0.2f, 100.f };
        myRegistry.AddComponent<MeshComponent>(floor, cubeModel);
        myRegistry.AddComponent<MaterialComponent>(floor, defaultMaterial);
        auto& floorCollider = myRegistry.AddComponent<BoxColliderComponent>(floor);
        floorCollider.halfExtents = { 0.5f, 0.5f, 0.5f };
        floorCollider.isStatic = true;

        for (int i = 0; i < 10; i++)
        {
            for (int y = 0; y < 10; y++)
            {
                Entity entity2 = myRegistry.CreateEntity();
                t2 = &myRegistry.AddComponent<TransformComponent>(entity2);
                t2->position = { 1.5f + i, 0.5f, 5.0f+ y };
                t2->scale = { 0.6f, 0.6f, 0.6f };
                myRegistry.AddComponent<MeshComponent>(entity2, cubeModel);
                myRegistry.AddComponent<MaterialComponent>(entity2, brickMaterial);
                myRegistry.AddComponent<BoxColliderComponent>(entity2);
            }
            
        }

        std::cout << "Model references: "
            << cubeModel.use_count()
            << std::endl;
        

    }

    void OnUpdate(float deltaTime) override
    {
        float speed = 1;
        if (GetWindow().GetInputHandler()->IsKeyDown('R'))
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
        if (GetWindow().GetInputHandler()->IsKeyDown('E'))
        {
            DestroyEntity(floor);
        }
        if (GetWindow().GetInputHandler()->IsKeyPressed('T'))
        {
            auto* transform =
                myRegistry.GetComponent<TransformComponent>(floor);

            if (transform)
            {
                transform->position.y -= 1.0f;
            }
        }
        

        if (GetWindow().GetInputHandler()->IsKeyDown('Y'))
        {
            auto* transform =
                myRegistry.GetComponent<TransformComponent>(floor);

            if (transform)
            {
                transform->position.z -= 5.0f*deltaTime;
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
        
        myRenderSystem.Render(
            myRegistry,
            camera,
            GetShader(),
            *defaultMaterial
        );


    }


    void OnCollision(const CollisionEvent& event) override
    {
        switch (event.type)
        {
            case CollisionEventType::Begin:
            {
                if (event.entityA == entity1 || event.entityB == entity1)
                {
                    std::cout << "Player Enter Collider!\n";
                }
                break;
            }
            case CollisionEventType::End:
            {
                if (event.entityA == entity1 || event.entityB == entity1)
                {
                    std::cout << "Player Exit Collider!\n";
                }
                break;
            }
            case CollisionEventType::TriggerEnter:
            {
                break;
            }
            case CollisionEventType::TriggerExit:
            {
                break;
            }
        }
    }


    void OnResize(UINT width, UINT height) override
    {
        camera.UpdateAspectRatio(width, height);
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
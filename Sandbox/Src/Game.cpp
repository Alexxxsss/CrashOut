#include "Game.h" 
#include "Core/EngineEntry.h" 
#include "Core/Vector.h" 
#include <iostream>
#include <DirectXMath.h>
#include <memory>
#include <cmath>
#include <stdexcept>
#include "Renderer/DebugDraw/GizmoSystem.h"
#include "Renderer/Texture.h"

class SandboxApp : public CrashOut::Application
{
    Camera camera;
    GizmoSystem myGizmoSystem;
    std::shared_ptr<Model> cubeModel;
    std::shared_ptr<Model> monkeyModel;

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
        myGizmoSystem.Init(&GetRenderer());

        auto& resources = GetResourceManager();



        cubeModel = resources.LoadModel("Cube.obj");
        monkeyModel = resources.LoadModel("Basic.obj");

        auto defaultTexture = resources.LoadTexture(L"PrototypeTexture.png");
        auto brickTexture = resources.LoadTexture(L"Brick.png");

        defaultMaterial = resources.CreateMaterial("DefaultMaterial", defaultTexture);
        brickMaterial = resources.CreateMaterial("BrickMaterial", brickTexture);




        entity1 = myRegistry.CreateEntity();
        t1 = &myRegistry.AddComponent<TransformComponent>(entity1);
        t1->position = { -1.5f, 0.0f, 4.0f };
        t1->scale = { 1.0f, 1.0f, 1.0f };
        myRegistry.AddComponent<MeshComponent>(entity1, monkeyModel);
        myRegistry.AddComponent<MaterialComponent>(entity1, brickMaterial);
        auto& entity1Collider = myRegistry.AddComponent<BoxColliderComponent>(entity1);
        entity1Collider.halfExtents = { 1.f, 1.f, 1.f };


        entity2 = myRegistry.CreateEntity();
        t2 = &myRegistry.AddComponent<TransformComponent>(entity2);
        t2->position = { 1.5f, 0.5f, 5.0f };
        t2->scale = { 0.6f, 0.6f, 0.6f };
        myRegistry.AddComponent<MeshComponent>(entity2, cubeModel);
        myRegistry.AddComponent<MaterialComponent>(entity2, brickMaterial);
        auto& entity2Collider = myRegistry.AddComponent<BoxColliderComponent>(entity2);
        entity2Collider.halfExtents = { 1.f, 1.f, 1.f };


        floor = myRegistry.CreateEntity();
        t3 = &myRegistry.AddComponent<TransformComponent>(floor);
        t3->position = { 0.f, -2.f, 0.f };
        t3->scale = { 30.f, 0.1f, 30.f };
        myRegistry.AddComponent<MeshComponent>(floor, cubeModel);
        myRegistry.AddComponent<MaterialComponent>(floor, defaultMaterial);
        auto& floorCollider = myRegistry.AddComponent<BoxColliderComponent>(floor);
        floorCollider.halfExtents = { 1.f, 1.f, 1.1f };
        floorCollider.isStatic = true;


        for (int i = 0; i < 20; i+=2)
        {
            for (int y = 0; y < 20; y+=2)
            {
                Entity entity3 = myRegistry.CreateEntity();
                t2 = &myRegistry.AddComponent<TransformComponent>(entity3);
                t2->position = { 4.f + i, 0.5f, 5.0f+ y };
                t2->scale = { 0.6f, 0.6f, 0.6f };
                myRegistry.AddComponent<MeshComponent>(entity3, cubeModel);
                myRegistry.AddComponent<MaterialComponent>(entity3, brickMaterial);
                auto& entity3Collider = myRegistry.AddComponent<BoxColliderComponent>(entity3);
                entity3Collider.halfExtents = { 1.f, 1.f, 1.1f };
            }
        }

        std::cout << "Model references: "
            << cubeModel.use_count()
            << std::endl;
    }

    void OnUpdate(float deltaTime) override
    {
        float speed = 1;
        auto* c1 = myRegistry.GetComponent<BoxColliderComponent>(entity1);

        if (GetWindow().GetInputHandler()->IsKeyDown('R'))
        {
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

        b3Vec3 moveDir = { 0.0f, 0.0f, 0.0f };

        if (GetWindow().GetInputHandler()->IsKeyDown('W'))
            moveDir.z += 1.0f;
        if (GetWindow().GetInputHandler()->IsKeyDown('S'))
            moveDir.z -= 1.0f;
        if (GetWindow().GetInputHandler()->IsKeyDown('A'))
            moveDir.x -= 1.0f;
        if (GetWindow().GetInputHandler()->IsKeyDown('D'))
            moveDir.x += 1.0f;

        float lengthSq = moveDir.x * moveDir.x + moveDir.z * moveDir.z;
        if (lengthSq > 0.0f)
        {
            float invLength = 1.0f / std::sqrt(lengthSq);
            moveDir.x *= invLength;
            moveDir.z *= invLength;

            b3Body_SetAwake(c1->bodyId, true);
            b3Body_SetLinearVelocity(c1->bodyId, { moveDir.x * 3.0f * speed, 0.0f, moveDir.z * 3.0f * speed });
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
        myGizmoSystem.RenderColliders(
            myRegistry,
            camera
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
        std::cout << "Sandbox shutting down...\n";

        myRegistry.Clear();

        cubeModel.reset();
        monkeyModel.reset();

        defaultMaterial.reset();
        brickMaterial.reset();

        std::size_t removed = GetResourceManager().CollectUnused();

        std::cout
            << "Unused cache entries removed: "
            << removed
            << '\n';

        std::cout << "Sandbox shutdown complete.\n";
    }
};


// Define the factory function required by EntryPoint.h
CrashOut::Application* CrashOut::CreateApplication()
{
    return new SandboxApp();
}
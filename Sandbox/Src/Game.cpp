#include "Game.h" 
#include "Core/EngineEntry.h" 
#include "Core/Vector.h" 
#include "Core/AssetPath.h"
#include <iostream>
#include <DirectXMath.h>
#include <memory>
#include <cmath>
#include <stdexcept>
#include "Renderer/DebugDraw/GizmoSystem.h"
#include "Renderer/Texture.h"
#include <cassert>

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
    Entity sunLight;
    Entity pointLight;
    Entity spotLight;
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



        cubeModel = resources.LoadModel(CrashOut::AssetPath(L"Assets/Models/Cube.obj").string());
        monkeyModel = resources.LoadModel(CrashOut::AssetPath(L"Assets/Models/Basic.obj").string());

        auto defaultTexture = resources.LoadTexture(CrashOut::AssetPath(L"Assets/Textures/PrototypeTexture.png").wstring());
        auto brickTexture = resources.LoadTexture(CrashOut::AssetPath(L"Assets/Textures/brick.png").wstring());

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
        floorCollider.halfExtents = { 1.f, 1.f, 1.f };
        floorCollider.isStatic = true;


        for (int i = 0; i < 20; i += 2)
        {
            for (int y = 0; y < 20; y += 2)
            {
                Entity entity3 = myRegistry.CreateEntity();
                t2 = &myRegistry.AddComponent<TransformComponent>(entity3);
                t2->position = { 4.f + i, 0.5f, 5.0f + y };
                t2->scale = { 0.6f, 0.6f, 0.6f };
                myRegistry.AddComponent<MeshComponent>(entity3, cubeModel);
                myRegistry.AddComponent<MaterialComponent>(entity3, brickMaterial);
                auto& entity3Collider = myRegistry.AddComponent<BoxColliderComponent>(entity3);
                entity3Collider.halfExtents = { 1.f, 1.f, 1.1f };
            }
        }

        // --- ECS lights ---
        myLightingSystem.SetAmbient({ 1.0f, 1.0f, 1.0f }, 0.12f);

        // Sun: no Transform needed; direction is world space.
        sunLight = myRegistry.CreateEntity();
        auto& sun = myRegistry.AddComponent<DirectionalLightComponent>(sunLight);
        sun.direction = { -0.4f, -1.0f, 0.3f };
        sun.color = { 1.0f, 0.95f, 0.8f };
        sun.intensity = 0.65f;

        // Orange lamp: uses entity Transform.position.
        pointLight = myRegistry.CreateEntity();
        auto& pointTransform = myRegistry.AddComponent<TransformComponent>(pointLight);
        pointTransform.position = { 0.0f, 2.5f, 4.0f };
        auto& point = myRegistry.AddComponent<PointLightComponent>(pointLight);
        point.color = { 1.0f, 0.35f, 0.15f };
        point.intensity = 3.0f;
        point.range = 9.0f;

        // Blue spotlight pointing from above toward the cubes.
        spotLight = myRegistry.CreateEntity();
        auto& spotTransform = myRegistry.AddComponent<TransformComponent>(spotLight);
        spotTransform.position = { 0.0f, 4.0f, 2.0f };
        auto& spot = myRegistry.AddComponent<SpotLightComponent>(spotLight);
        spot.direction = { 0.0f, -1.0f, 0.5f }; // Local direction.
        spot.color = { 0.2f, 0.45f, 1.0f };
        spot.intensity = 4.0f;
        spot.range = 12.0f;
        spot.innerAngleDegrees = 18.0f;
        spot.outerAngleDegrees = 32.0f;

        std::cout << "Model references: "
            << cubeModel.use_count()
            << std::endl;

    }

    void OnUpdate(float deltaTime) override
    {
        auto* input = GetWindow().GetInputHandler();

        if (input->IsKeyPressed('E'))
        {
            DestroyEntity(floor);
        }

        if (input->IsKeyPressed('T'))
        {
            auto* transform =
                myRegistry.GetComponent<TransformComponent>(
                    floor
                );

            if (transform)
            {
                transform->position.y -= 1.0f;
            }
        }

        if (input->IsKeyPressed(VK_F1))
        {
            GetRasterState().ToggleWireframe();
        }
    }






    void OnFixedUpdate(float fixedDeltaTime) override
    {
        auto* input = GetWindow().GetInputHandler();

        float x = 0.0f;
        float z = 0.0f;

        if (input->IsKeyDown('W')) z += 1.0f;
        if (input->IsKeyDown('S')) z -= 1.0f;
        if (input->IsKeyDown('A')) x -= 1.0f;
        if (input->IsKeyDown('D')) x += 1.0f;

        // Normalize diagonal movement.
        float lengthSq = x * x + z * z;

        if (lengthSq > 0.0f)
        {
            float invLength = 1.0f / std::sqrt(lengthSq);

            x *= invLength;
            z *= invLength;
        }

        constexpr float movementSpeed = 3.0f;

        myPhysics.SetHorizontalVelocity(
            myRegistry,
            entity1,
            x * movementSpeed,
            z * movementSpeed
        );

        // Rotate objects when holding R.
        if (input->IsKeyDown('R'))
        {
            myPhysics.SetAngularVelocity(
                myRegistry,
                entity1,
                { 0.0f, 2.5f, 0.0f }
            );

            myPhysics.SetAngularVelocity(
                myRegistry,
                entity2,
                { 5.0f, 0.0f, 2.5f }
            );
        }

        // Move the static floor using a fixed timestep.
        if (input->IsKeyDown('Y'))
        {
            auto* transform =
                myRegistry.GetComponent<TransformComponent>(
                    floor
                );

            if (transform)
            {
                transform->position.z -=
                    5.0f * fixedDeltaTime;
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
            *defaultMaterial,
            myLightingSystem,
            myShadowSystem
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
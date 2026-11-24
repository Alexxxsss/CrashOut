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
    Camera myGameCamera;
    std::shared_ptr<Material> defaultMaterial;
    Entity entity1 = 0;
    Entity entity2 = 0;
    Entity floor = 0;

public:
    SandboxApp() = default;

    ~SandboxApp() override = default;

    void OnStart() override
    {
        camera.Init(GetWindowWidth(), GetWindowHeight());
        myGizmoSystem.Init(&GetRenderer());
        auto& resources = GetResourceManager();
        auto defaultTexture = resources.LoadTexture(CrashOut::AssetPath(L"Assets/Textures/PrototypeTexture.png").wstring());
        defaultMaterial = resources.CreateMaterial("DefaultMaterial", defaultTexture);
        SetScene(CrashOut::SceneSerializer::Load(
            CrashOut::AssetPath(L"Scenes/StartScene.json"), resources));
        entity1 = GetScene().FindEntityByName("Player");
        entity2 = GetScene().FindEntityByName("Cube");
        floor = GetScene().FindEntityByName("Floor");
       
        StartPlay();
    }

    void OnUpdate(float deltaTime) override
    {
        if (!IsPlaying()) return;
        auto& registry = GetScene().GetRegistry();
        auto* input = GetWindow().GetInputHandler();

        if (input->IsKeyPressed('E'))
        {
            DestroyEntity(floor);
        }

        if (input->IsKeyPressed('T'))
        {
            auto* transform =
                registry.GetComponent<TransformComponent>(
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
        auto& registry = GetScene().GetRegistry();
        auto& physics = GetScene().GetPhysics();
        auto* input = GetWindow().GetInputHandler();

        float x = 0.0f;
        float z = 0.0f;

        if (input->IsKeyDown('W')) z += 1.0f;
        if (input->IsKeyDown('S')) z -= 1.0f;
        if (input->IsKeyDown('A')) x -= 1.0f;
        if (input->IsKeyDown('D')) x += 1.0f;

        float lengthSq = x * x + z * z;

        if (lengthSq > 0.0f)
        {
            float invLength = 1.0f / std::sqrt(lengthSq);

            x *= invLength;
            z *= invLength;
        }

        constexpr float movementSpeed = 3.0f;

        physics.SetHorizontalVelocity(
            registry,
            entity1,
            x * movementSpeed,
            z * movementSpeed
        );

        if (input->IsKeyDown('R'))
        {
            physics.SetAngularVelocity(
                registry,
                entity1,
                { 0.0f, 2.5f, 0.0f }
            );

            physics.SetAngularVelocity(
                registry,
                entity2,
                { 5.0f, 0.0f, 2.5f }
            );
        }

        if (input->IsKeyDown('Y'))
        {

            auto* transform =
                registry.GetComponent<TransformComponent>(
                    floor
                );

            if (transform)
            {
                transform->position.z -=
                    5.0f * fixedDeltaTime;
            }
            auto next = CrashOut::SceneSerializer::Load(
                "Scenes/EmberCitadel.json", GetResourceManager());
            StopPlay();
            SetScene(std::move(next));
        }
    }


    void OnRender() override
    {
        Camera* view = &camera;
        if (IsPlaying() && GetScene().BuildPrimaryCamera(myGameCamera,
            GetWindowWidth(), GetWindowHeight()))
            view = &myGameCamera;
        RenderScene(*view, *defaultMaterial);
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
        StopPlay();
        defaultMaterial.reset();
    }
};


// Define the factory function required by EntryPoint.h
CrashOut::Application* CrashOut::CreateApplication()
{
    return new SandboxApp();
}

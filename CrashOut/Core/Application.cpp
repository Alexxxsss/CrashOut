#include "Application.h"
#include <iostream>
#include <DirectXMath.h> 
#include <windows.h>
#include "Window.h"
#include <DirectXMath.h> 
#include "AssetPath.h"
#include <algorithm>
using namespace DirectX;

namespace CrashOut
{

    Application::Application()
    {
        if (!myWindow.StartUpWindow(myScreenWidth, myScreenHeight, L"DirectX 11 Engine"))
        {
            throw std::runtime_error("Window initialization failed");
        }
        
        if (!myRenderer.Initialize(GetWindowHandle(), GetWindowWidth(), GetWindowHeight()))
        {
            throw std::runtime_error("Renderer initialization failed");
        }
        myRasterState.Init(&myRenderer);

        //myRenderer.CreateDepthBuffer(GetWindowWidth(), GetWindowHeight());
        //myRenderer.CreateDepthStencilState();

        myShader = myResourceManager.LoadShader(
            AssetPath(L"Shaders/Vertex.hlsl"),
            AssetPath(L"Shaders/Pixel.hlsl")
        );


        myRegistry.SetOnRemove<BoxColliderComponent>(
            [this](Entity entity, BoxColliderComponent& collider)
            {
                myPhysics.DestroyBoxCollider(collider);
            }
        );

        myRegistry.SetOnRemove<TransformComponent>(
            [this](Entity entity, TransformComponent&)
            {
                myRegistry.RemoveComponent<BoxColliderComponent>(
                    entity
                );
            }
        );
    }


    bool Application::ProcessEvents()
    {
        if (!myWindow.UpdateOpenWindow())
            return false;

        if (myWindow.WasResized() &&
            !myWindow.IsMinimized())
        {
            UINT width = GetWindowWidth();
            UINT height = GetWindowHeight();

            if (width > 0 && height > 0)
            {
                myRenderer.Resize(width, height);

                OnResize(width, height);
            }
        }

        return true;
    }



    void Application::Update(float deltaTime)
    {
        constexpr float maxDeltaTime = 0.25f;
        constexpr float fixedTimeStep = 1.0f / 60.0f;

        // Prevent large simulation jumps.
        deltaTime = std::clamp(
            deltaTime,
            0.0f,
            maxDeltaTime
        );

        // Initialize any new physics bodies.
        myPhysics.EnsureBodies(myRegistry);

        // Variable-time frame logic.
        OnUpdate(deltaTime);

        // Accumulate time for fixed updates.
        myPhysicsAccumulator += deltaTime;

        while (myPhysicsAccumulator >= fixedTimeStep)
        {
            // Include entities created during gameplay.
            myPhysics.EnsureBodies(myRegistry);

            // Physics-related gameplay logic.
            OnFixedUpdate(fixedTimeStep);

            // Run the Box3D simulation.
            myPhysics.UpdateTransforms(
                myRegistry,
                fixedTimeStep
            );

            // Deliver physics events.
            const auto& events = myPhysics.GetCollisionEvents();

            for (const auto& event : events)
            {
                OnCollision(event);
            }

            myPhysicsAccumulator -= fixedTimeStep;
        }
    }




    void Application::RenderFrame()
    {
        if (myWindow.IsMinimized())
            return;

        if (GetWindowWidth() == 0 ||
            GetWindowHeight() == 0)
            return;

        myRenderer.BeginFrame(
            0.1f,
            0.2f,
            0.4f,
            1.0f
        );

        myRasterState.Use();

        OnRender();

        myRenderer.EndFrame();
    }


    void Application::CleanUp()
    {
        myRenderer.CleanUp();
    }

    void Application::DestroyEntity(Entity entity)
    {
        myRegistry.DestroyEntity(entity);
    }

    void Application::InitializePhysics()
    {
        myPhysics.EnsureBodies(myRegistry);
    }
}


#include "Application.h"
#include <iostream>
#include <DirectXMath.h> 
#include <windows.h>
#include "Window.h"
#include <DirectXMath.h> 
#include "AssetPath.h"
#include "../Scene/SceneSerializer.h"
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
        myLightingSystem.Init(myRenderer);
        myShadowSystem.Init(myRenderer);
        myShadowSystem.SetFocus({ 0.0f, 0.0f, 0.0f });
        myShadowSystem.SetCoverage(90.0f, 90.0f);

        //myRenderer.CreateDepthBuffer(GetWindowWidth(), GetWindowHeight());
        //myRenderer.CreateDepthStencilState();

        myShader = myResourceManager.LoadShader(
            AssetPath(L"Shaders/Vertex.hlsl"),
            AssetPath(L"Shaders/Pixel.hlsl")
        );


        myEditScene = std::make_unique<Scene>();
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

        // Editor UI and variable-time gameplay both use this hook.
        OnUpdate(deltaTime);
        if (!IsPlaying()) return;

        auto& scene = GetScene();
        auto& registry = scene.GetRegistry();
        auto& physics = scene.GetPhysics();
        physics.EnsureBodies(registry);
        myPhysicsAccumulator += deltaTime;
        while (myPhysicsAccumulator >= fixedTimeStep)
        {
            physics.EnsureBodies(registry);
            OnFixedUpdate(fixedTimeStep);
            physics.UpdateTransforms(registry, fixedTimeStep);
            for (const auto& event : physics.GetCollisionEvents())
                OnCollision(event);
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
        myRuntimeScene.reset();
        myEditScene.reset();
        myRenderer.CleanUp();
    }

    Scene& Application::GetScene()
    {
        return IsPlaying() ? *myRuntimeScene : *myEditScene;
    }

    void Application::SetScene(std::unique_ptr<Scene> scene)
    {
        if (IsPlaying() || !scene)
            throw std::runtime_error("Stop Play before replacing the scene");
        myEditScene = std::move(scene);
        myPhysicsAccumulator = 0.0f;
    }

    void Application::StartPlay()
    {
        if (IsPlaying()) return;
        auto runtime = SceneSerializer::Clone(*myEditScene, myResourceManager);
        runtime->GetPhysics().EnsureBodies(runtime->GetRegistry());
        myRuntimeScene = std::move(runtime);
        myPhysicsAccumulator = 0.0f;
    }

    void Application::StopPlay()
    {
        myRuntimeScene.reset();
        myPhysicsAccumulator = 0.0f;
    }

    void Application::RenderScene(Camera& camera, Material& defaultMaterial)
    {
        auto& scene = GetScene();
        myLightingSystem.SetAmbient(scene.ambientColor, scene.ambientIntensity);
        myRenderSystem.Render(scene.GetRegistry(), camera, GetShader(),
            defaultMaterial, myLightingSystem, myShadowSystem);
    }

    void Application::DestroyEntity(Entity entity)
    {
        GetScene().DestroyEntity(entity);
    }
}

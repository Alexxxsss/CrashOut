#pragma once

#include <iostream>
#include <windows.h>
#include "Window.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/RasterState.h"
#include "../Renderer/Camera.h"
#include <memory>
#include "../Renderer/RenderSystem.h"
#include "../Resources/ResourceManager.h"
#include "../Renderer/Shader.h"
#include "../Renderer/LightingSystem.h"
#include "../Renderer/ShadowSystem.h"
#include "../Scene/Scene.h"

namespace CrashOut
{
    class Application
    {
    public:
        Application();
        virtual ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        bool ProcessEvents();
        void Update(float deltaTime);
        void RenderFrame();
        void CleanUp();

        // Call scene replacement / Play / Stop at a frame boundary,
        // e.g. OnStart or OnUpdate, not OnFixedUpdate/OnCollision/OnRender.
        Scene& GetScene();
        Scene& GetEditScene() { return *myEditScene; }
        void SetScene(std::unique_ptr<Scene> scene);
        void StartPlay();
        void StopPlay();
        bool IsPlaying() const { return bool(myRuntimeScene); }
        void RenderScene(Camera& camera, Material& defaultMaterial);

        void DestroyEntity(Entity entity);

        HWND GetWindowHandle() const { return myWindow.GetHwnd(); }
        UINT GetWindowWidth() const { return myWindow.GetWidth(); }

        UINT GetWindowHeight() const { return myWindow.GetHeight(); }
        Renderer& GetRenderer() { return myRenderer; }
        Shader& GetShader() { return *myShader; }
        RasterState& GetRasterState() { return myRasterState; }
        Window& GetWindow() { return myWindow; }
        ResourceManager& GetResourceManager()
        {
            return myResourceManager;
        }

        virtual void OnStart() = 0;
        virtual void OnUpdate(float deltaTime) = 0;
        virtual void OnFixedUpdate(float fixedDeltaTime) {}
        virtual void OnRender() = 0;
        virtual void OnShutdown() = 0;
        virtual void OnCollision(const CollisionEvent& event) {}

        virtual void OnResize(UINT width, UINT height) {}

    protected:
        const UINT myScreenWidth = 1280;
        const UINT myScreenHeight = 720;

        Window myWindow;

        Renderer myRenderer;
        ResourceManager myResourceManager{ myRenderer };
        std::shared_ptr<Shader> myShader;
        RasterState myRasterState;
        RenderSystem myRenderSystem;
        LightingSystem myLightingSystem;
        ShadowSystem myShadowSystem;

    private:
        std::unique_ptr<Scene> myEditScene;
        std::unique_ptr<Scene> myRuntimeScene;

        float myPhysicsAccumulator = 0.0f;
    };

    // Forward declaration for client factory function
    Application* CreateApplication();

}

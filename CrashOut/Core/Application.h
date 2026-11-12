#pragma once

#include <iostream>
#include <windows.h>
#include "Window.h"
#include "Engine.h"

namespace CrashOut
{
    class Application
    {
    public:
        Application();
        virtual ~Application() = default;

        // Prevent copying/moving for engine stability
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        void WindowAndRenderLoop();
        void CleanUp();


        HWND GetWindowHandle() const { return myWindow.GetHwnd(); }
        UINT GetWindowWidth() const { return myScreenWidth; }
        UINT GetWindowHeight() const { return myScreenHeight; }
        Renderer& GetRenderer() { return myRenderer; }
        Shader& GetShader() { return myShader; }
        RasterState& GetRasterState() { return myRasterState; }
        Window& GetWindow() { return myWindow; }

        void RenderEntities(Registry& aRegistry, Camera& aCamera, Shader& aShader, Model& aDefaultModel);

        virtual void OnStart() = 0;
        virtual void OnUpdate(float deltaTime) = 0;
        virtual void OnRender() = 0;
        virtual void OnShutdown() = 0;

    protected:
        const UINT myScreenWidth = 1280;
        const UINT myScreenHeight = 720;
        Window myWindow;
        Renderer myRenderer;
        Shader myShader;
        RasterState myRasterState;
    };

    // Forward declaration for client factory function
    Application* CreateApplication();

}
#include "Application.h"
#include <iostream>
#include <DirectXMath.h> 
#include <windows.h>
#include "Window.h"
#include <DirectXMath.h> 
using namespace DirectX;

namespace CrashOut
{

    Application::Application()
    {
        
        myWindow.StartUpWindow(myScreenWidth, myScreenHeight, L"DirectX 11 Engine");
        
        myRenderer.Initialize(GetWindowHandle(), GetWindowWidth(), GetWindowHeight());
        myShader.Init(&myRenderer);
        myRasterState.Init(&myRenderer);


        myShader.LoadPixelShader("../../CrashOut/Shaders/Pixel.hlsl");
        myShader.LoadVertexShader("../../CrashOut/Shaders/Vertex.hlsl");
        myShader.CreateWorldViewProjectionConstantBuffer();
    }

    void Application::WindowAndRenderLoop()
    {
        myWindow.UpdateOpenWindow();

        myRenderer.BeginFrame(0.1f, 0.2f, 0.4f, 1.0f);

        myShader.Use();
        myRasterState.Use();

        OnRender();

        myRenderer.EndFrame();

    }

    void Application::CleanUp()
    {
        myRenderer.CleanUp();
    }




    void Application::RenderEntities(Registry& aRegistry, Camera& aCamera, Shader& aShader, Model& aDefaultModel)
    {
        // Compute View-Projection once for the whole frame
        XMMATRIX view = aCamera.GetView();
        XMMATRIX proj = aCamera.GetProjection();
        XMMATRIX viewProj = view * proj;

        auto* transformPool = aRegistry.GetPool<TransformComponent>();
        if (!transformPool) return;

        for (auto& [entity, transform] : transformPool->data)
        {
            MeshComponent* mesh = aRegistry.GetComponent<MeshComponent>(entity);
            if (!mesh || !mesh->model)
                continue;

            // 1. Compute World * View * Projection for this specific entity
            XMMATRIX world = transform.GetWorldMatrix();
            XMMATRIX mvp = world * viewProj;

            // 2. Upload MVP to Constant Buffer register b0
            aShader.UpdateModelViewProjectionBuffer(mvp);

            // 3. Bind Entity's Texture (or default model texture if no custom material)
            MaterialComponent* mat = aRegistry.GetComponent<MaterialComponent>(entity);
            if (mat && mat->textureSRV)
            {
                // If the entity has a custom texture set up, bind it
                // (Uses Model's UseTexture() or PSSetShaderResources depending on your implementation)
                mesh->model->UseTexture();
            }
            else
            {
                mesh->model->UseTexture();
            }

            // 4. Draw mesh indices
            mesh->model->Render();
        }
    }

}


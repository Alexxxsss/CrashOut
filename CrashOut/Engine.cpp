#include <iostream>
#include <DirectXMath.h> // Needed for 3D math (matrices/vectors)

#include "Core/Window.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/RasterState.h"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"
#include "Input/InputSystem.h"


using namespace DirectX; 

extern void OnStart();
extern void OnUpdate();
extern void OnCleanUp();
void ComputeModelViewProjectionMatrix(Camera& aCamera, Shader& aShader);


int main()
{
    OnStart();


    // Initialize Window with size
    const UINT screenWidth = 1080;
    const UINT screenHeight = 720;

    Window window;
    if (!window.StartUpWindow(screenWidth, screenHeight, L"DirectX 11 Engine"))
    {
        return -1;
    }
    HWND hwnd = window.GetHwnd();



    // Initialize Renderer with window handle and size
    Renderer renderer;

    if (!renderer.Initialize(window.GetHwnd(), screenWidth, screenHeight))
    {
        std::cerr << "Renderer initialization failed!" << std::endl;
        return -1;
    }

    Shader shader(&renderer);
    Model model(&renderer);
    Camera camera(1080, 720);
    RasterState rasterState(&renderer);

    model.Create();
    model.LoadTexture();

    shader.LoadPixelShader("../../CrashOut/Shaders/Pixel.hlsl");
    shader.LoadVertexShader("../../CrashOut/Shaders/Vertex.hlsl");
    shader.CreateWorldViewProjectionConstantBuffer();



    bool isRunning = true;
    while (isRunning)
    {
        window.UpdateOpenWindow();
        OnUpdate();



        renderer.BeginFrame(0.1f, 0.2f, 0.4f, 1.0f);

        // 1. Bind pipeline states & shaders
        shader.Use();
        rasterState.Use();
        ComputeModelViewProjectionMatrix(camera, shader);


        model.UseTexture();
        model.Render();

        renderer.EndFrame();

        if (window.GetInputHandeler()->IsKeyPressed('W')) { std::cout << "WWW PRESSED\n"; }
    }
    renderer.CleanUp();
    OnCleanUp();
    return 0;
}



void ComputeModelViewProjectionMatrix(Camera& aCamera, Shader& aShader)
{
    DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
    matrix *= aCamera.GetView();
    matrix *= aCamera.GetProjection();

    aShader.UpdateModelViewProjectionBuffer(matrix);
}
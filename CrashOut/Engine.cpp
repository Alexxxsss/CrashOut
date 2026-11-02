#include <iostream>
#include <DirectXMath.h> // Needed for 3D math (matrices/vectors)

#include "Core/Window.h"
#include "Renderer/Renderer.h"

using namespace DirectX; // Allows using XMMATRIX, XMVECTOR, etc. without typing DirectX:: every time

extern void OnStart();
extern void OnUpdate();
extern void OnCleanUp();

int main()
{
    OnStart();

    const UINT screenWidth = 800;
    const UINT screenHeight = 600;

    Window window;
    window.StartUpWindow();
    HWND hwnd = window.GetHwnd();

    Renderer renderer;
    if (!renderer.Initialize(hwnd, screenWidth, screenHeight))
    {
        std::cerr << "Failed to initialize Direct3D 11 Renderer!\n";
        return -1;
    }

    float rotation = 0.0f;
    bool isRunning = true;

    while (isRunning)
    {
        window.UpdateOpenWindow();
        OnUpdate();

        // 1. Update rotation angle
        rotation += 0.005f;

        // 2. Build 3D Camera & Model Matrices:
        // World: Rotate the cube on Y and X axes
        XMMATRIX world = XMMatrixRotationY(rotation) * XMMatrixRotationX(rotation * 0.5f);

        // View: Camera placed back at Z = -5, looking at (0, 0, 0)
        XMVECTOR eyePos = XMVectorSet(0.0f, 1.5f, -5.0f, 0.0f);
        XMVECTOR target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMMATRIX view = XMMatrixLookAtLH(eyePos, target, up);

        // Projection: 45 degree Field of View, matching your window aspect ratio
        float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), aspectRatio, 0.1f, 100.0f);

        // Combined WVP matrix
        XMMATRIX wvp = world * view * proj;

        // 3. Render pass
        renderer.BeginFrame(0.1f, 0.1f, 0.15f);

        renderer.UpdateTransform(wvp); // Send matrix to the shader
        renderer.DrawCube();           // Draw the 3D cube

        renderer.EndFrame();
    }

    renderer.CleanUp();
    OnCleanUp();
    return 0;
}
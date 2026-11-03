#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h> // For Microsoft::WRL::ComPtr

// Link required DirectX libraries (or link them in your CMakeLists.txt)
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;

    bool Initialize(HWND hwnd, UINT width, UINT height);
    void BeginFrame(float r = 0.1f, float g = 0.2f, float b = 0.4f, float a = 1.0f);
    void EndFrame();
    void CleanUp();

    // Getters for when you start drawing triangles & shaders
    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_deviceContext.Get(); }

private:
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_deviceContext;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;

    D3D11_VIEWPORT m_viewport = {};
};
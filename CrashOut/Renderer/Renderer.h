#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 color;
};

class Renderer
{
public:
    bool Initialize(HWND hwnd, UINT width, UINT height);
    void BeginFrame(float r, float g, float b);
    void UpdateTransform(const DirectX::XMMATRIX& worldViewProj);
    void DrawCube();
    void EndFrame();
    void CleanUp();

private:
    bool CreateDeviceAndSwapChain(HWND hwnd, UINT width, UINT height);
    bool CreateRenderTargetAndDepth(UINT width, UINT height);
    bool CreateShadersAndInputLayout();
    bool CreateBuffers();

    Microsoft::WRL::ComPtr<ID3D11Device>           m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>     m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>      m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>      m_inputLayout;

    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_constantBuffer;

    D3D11_VIEWPORT                                 m_viewport = {};
};
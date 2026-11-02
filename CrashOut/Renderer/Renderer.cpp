#include "Renderer.h"
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Embedded Shaders
static const char g_ShaderCode[] = R"(
    cbuffer ConstantBuffer : register(b0)
    {
        matrix finalMatrix;
    };

    struct VS_INPUT
    {
        float3 position : POSITION;
        float3 color    : COLOR;
    };

    struct PS_INPUT
    {
        float4 position : SV_POSITION;
        float3 color    : COLOR;
    };

    PS_INPUT VSMain(VS_INPUT input)
    {
        PS_INPUT output;
        // Matrix comes transposed from DirectXMath
        output.position = mul(float4(input.position, 1.0f), finalMatrix);
        output.color = input.color;
        return output;
    }

    float4 PSMain(PS_INPUT input) : SV_TARGET
    {
        return float4(input.color, 1.0f);
    }
)";

bool Renderer::Initialize(HWND hwnd, UINT width, UINT height)
{
    if (!hwnd) return false;

    if (!CreateDeviceAndSwapChain(hwnd, width, height)) return false;
    if (!CreateRenderTargetAndDepth(width, height))      return false;
    if (!CreateShadersAndInputLayout())                 return false;
    if (!CreateBuffers())                               return false;

    // Viewport setup
    m_viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f };
    return true;
}

bool Renderer::CreateDeviceAndSwapChain(HWND hwnd, UINT width, UINT height)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
        featureLevels, 1, D3D11_SDK_VERSION, &scd,
        m_swapChain.GetAddressOf(), m_device.GetAddressOf(),
        nullptr, m_context.GetAddressOf()
    );

    return SUCCEEDED(hr);
}

bool Renderer::CreateRenderTargetAndDepth(UINT width, UINT height)
{
    // 1. Color Render Target
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (FAILED(hr)) return false;

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
    if (FAILED(hr)) return false;

    // 2. Depth Buffer (Critical for 3D so surfaces don't overlap wrongly)
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
    hr = m_device->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = m_device->CreateDepthStencilView(depthTexture.Get(), nullptr, m_depthStencilView.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Renderer::CreateShadersAndInputLayout()
{
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob, errorBlob;

    // Compile Vertex Shader
    HRESULT hr = D3DCompile(g_ShaderCode, sizeof(g_ShaderCode), nullptr, nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, vsBlob.GetAddressOf(), errorBlob.GetAddressOf());
    if (FAILED(hr)) return false;

    // Compile Pixel Shader
    hr = D3DCompile(g_ShaderCode, sizeof(g_ShaderCode), nullptr, nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, psBlob.GetAddressOf(), errorBlob.GetAddressOf());
    if (FAILED(hr)) return false;

    m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
    m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());

    // Input Layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    m_device->CreateInputLayout(layout, _countof(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_inputLayout.GetAddressOf());
    return true;
}

bool Renderer::CreateBuffers()
{
    // 8 Corners of a Cube
    Vertex vertices[] =
    {
        { { -1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 0.0f } }, // 0: Top Front Left  (Red)
        { {  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f } }, // 1: Top Front Right (Green)
        { {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f } }, // 2: Top Back Right  (Blue)
        { { -1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f } }, // 3: Top Back Left   (Yellow)
        { { -1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f } }, // 4: Bottom Front Left
        { {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f } }, // 5: Bottom Front Right
        { {  1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 1.0f } }, // 6: Bottom Back Right
        { { -1.0f, -1.0f,  1.0f }, { 0.2f, 0.2f, 0.2f } }  // 7: Bottom Back Left
    };

    // 12 Triangles making up the 6 faces (36 indices total)
    WORD indices[] =
    {
        // Front Face
        0, 1, 5,   0, 5, 4,
        // Back Face
        2, 3, 7,   2, 7, 6,
        // Left Face
        3, 0, 4,   3, 4, 7,
        // Right Face
        1, 2, 6,   1, 6, 5,
        // Top Face
        3, 2, 1,   3, 1, 0,
        // Bottom Face
        4, 5, 6,   4, 6, 7
    };

    // 1. Vertex Buffer
    D3D11_BUFFER_DESC vbd = { sizeof(vertices), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER };
    D3D11_SUBRESOURCE_DATA vData = { vertices };
    m_device->CreateBuffer(&vbd, &vData, m_vertexBuffer.GetAddressOf());

    // 2. Index Buffer
    D3D11_BUFFER_DESC ibd = { sizeof(indices), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER };
    D3D11_SUBRESOURCE_DATA iData = { indices };
    m_device->CreateBuffer(&ibd, &iData, m_indexBuffer.GetAddressOf());

    // 3. Constant Buffer for Transformations
    D3D11_BUFFER_DESC cbd = { sizeof(DirectX::XMMATRIX), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER };
    m_device->CreateBuffer(&cbd, nullptr, m_constantBuffer.GetAddressOf());

    return true;
}

void Renderer::UpdateTransform(const DirectX::XMMATRIX& worldViewProj)
{
    // DirectXMath uses row-major, HLSL default expects column-major: transpose it!
    DirectX::XMMATRIX transposed = DirectX::XMMatrixTranspose(worldViewProj);
    m_context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &transposed, 0, 0);
}

void Renderer::BeginFrame(float r, float g, float b)
{
    const float clearColor[4] = { r, g, b, 1.0f };
    // Clear color & depth targets
    m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
    m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Bind both color and depth
    m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    m_context->RSSetViewports(1, &m_viewport);
}

void Renderer::DrawCube()
{
    // 1. Input Layout & Geometry
    m_context->IASetInputLayout(m_inputLayout.Get());
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    m_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    // 2. Shaders & Constant Buffer
    m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

    // 3. Draw 36 indexed vertices (12 triangles)
    m_context->DrawIndexed(36, 0, 0);
}

void Renderer::EndFrame()
{
    m_swapChain->Present(1, 0);
}

void Renderer::CleanUp()
{
    if (m_context) m_context->ClearState();
}
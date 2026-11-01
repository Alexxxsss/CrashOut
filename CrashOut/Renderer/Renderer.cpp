#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

bool Renderer::Initialize(HWND hwnd, UINT width, UINT height)
{
    if (hwnd == nullptr)
    {
        std::cerr << "[Renderer Error] HWND is nullptr! Check Window::StartUpWindow().\n";
        return false;
    }

    if (!CreateDeviceAndSwapChain(hwnd, width, height))
    {
        std::cerr << "[Renderer Error] Failed at CreateDeviceAndSwapChain!\n";
        return false;
    }

    if (!CreateRenderTarget())
    {
        std::cerr << "[Renderer Error] Failed at CreateRenderTarget!\n";
        return false;
    }

    if (!CreateShadersAndInputLayout())
    {
        std::cerr << "[Renderer Error] Failed at CreateShadersAndInputLayout!\n";
        return false;
    }

    if (!CreateBuffers())
    {
        std::cerr << "[Renderer Error] Failed at CreateBuffers!\n";
        return false;
    }

    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<float>(width);
    m_viewport.Height = static_cast<float>(height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    return true;
}

bool Renderer::CreateDeviceAndSwapChain(HWND hwnd, UINT width, UINT height)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;

    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        featureLevels,
        1,
        D3D11_SDK_VERSION,
        &scd,
        m_swapChain.GetAddressOf(),
        m_device.GetAddressOf(),
        nullptr,
        m_context.GetAddressOf()
    );

    return SUCCEEDED(hr);
}

bool Renderer::CreateRenderTarget()
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Renderer::CreateShadersAndInputLayout()
{
    // Your exact shader embedded directly as a string
    const char shaderCode[] = R"(
        cbuffer ConstantBuffer : register(b0)
        {
            matrix finalMatrix;
        };

        struct VS_INPUT
        {
            float3 position : POSITION;
            float3 color : COLOR;
        };

        struct PS_INPUT
        {
            float4 position : SV_POSITION;
            float3 color : COLOR;
        };

        PS_INPUT VSMain(VS_INPUT input)
        {
            PS_INPUT output;
            output.position = mul(float4(input.position, 1.0f), finalMatrix);
            output.color = input.color;
            return output;
        }

        float4 PSMain(PS_INPUT input) : SV_TARGET
        {
            return float4(input.color, 1.0f);
        }
    )";

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    // 1. Compile Vertex Shader from memory
    HRESULT hr = D3DCompile(
        shaderCode,
        sizeof(shaderCode),
        "main.hlsl",
        nullptr,
        nullptr,
        "VSMain",
        "vs_5_0",
        0, 0,
        vsBlob.GetAddressOf(),
        errorBlob.GetAddressOf()
    );

    if (FAILED(hr))
    {
        if (errorBlob)
            std::cerr << "[Renderer Error] VS Compile:\n" << (char*)errorBlob->GetBufferPointer() << "\n";
        return false;
    }

    // 2. Compile Pixel Shader from memory
    errorBlob.Reset();
    hr = D3DCompile(
        shaderCode,
        sizeof(shaderCode),
        "main.hlsl",
        nullptr,
        nullptr,
        "PSMain",
        "ps_5_0",
        0, 0,
        psBlob.GetAddressOf(),
        errorBlob.GetAddressOf()
    );

    if (FAILED(hr))
    {
        if (errorBlob)
            std::cerr << "[Renderer Error] PS Compile:\n" << (char*)errorBlob->GetBufferPointer() << "\n";
        return false;
    }

    hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
    if (FAILED(hr)) return false;

    // 3. Input Layout
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = m_device->CreateInputLayout(
        layoutDesc,
        _countof(layoutDesc),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        m_inputLayout.GetAddressOf()
    );

    return SUCCEEDED(hr);
}

bool Renderer::CreateBuffers()
{
    // 1. Define triangle vertices (clockwise winding)
    Vertex vertices[] =
    {
        { DirectX::XMFLOAT3(0.0f,  0.5f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // Top (Red)
        { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // Bottom Right (Green)
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }  // Bottom Left (Blue)
    };

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(vertices);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vInitData = {};
    vInitData.pSysMem = vertices;

    HRESULT hr = m_device->CreateBuffer(&vbd, &vInitData, m_vertexBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    // 2. Constant buffer for "matrix finalMatrix" in your shader
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(DirectX::XMMATRIX); // Must be multiple of 16 bytes
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    // Identity matrix so the triangle stays in NDC space
    DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
    D3D11_SUBRESOURCE_DATA cbInitData = {};
    cbInitData.pSysMem = &identity;

    hr = m_device->CreateBuffer(&cbd, &cbInitData, m_constantBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}

void Renderer::BeginFrame(float r, float g, float b)
{
    const float clearColor[4] = { r, g, b, 1.0f };
    m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);

    m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
    m_context->RSSetViewports(1, &m_viewport);
}

void Renderer::DrawTriangle()
{
    // Bind pipeline stages
    m_context->IASetInputLayout(m_inputLayout.Get());
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

    // Bind Constant Buffer (slot b0)
    m_context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // Bind shaders
    m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

    // Draw the 3 vertices
    m_context->Draw(3, 0);
}

void Renderer::EndFrame()
{
    // 1 = VSync enabled (use 0 for uncapped framerate)
    m_swapChain->Present(1, 0);
}

void Renderer::CleanUp()
{
    if (m_context)
    {
        m_context->ClearState();
    }
}
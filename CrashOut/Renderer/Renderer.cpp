#include "Renderer.h"
#include <iostream>

bool Renderer::Initialize(HWND hwnd, UINT width, UINT height)
{
    // 1. Describe the Swap Chain
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;       // 1 = No multisampling
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    UINT createDeviceFlags = 0;
#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    D3D_FEATURE_LEVEL featureLevel;

    // 2. Create the Device, Device Context, and Swap Chain
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,                   // Primary graphics adapter
        D3D_DRIVER_TYPE_HARDWARE,  // Hardware acceleration
        nullptr,
        createDeviceFlags,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &scd,
        m_swapChain.GetAddressOf(),
        m_device.GetAddressOf(),
        &featureLevel,
        m_deviceContext.GetAddressOf()
    );

    if (FAILED(hr))
    {
        std::cerr << "Failed to create Direct3D Device and Swap Chain! HR: " << std::hex << hr << std::endl;
        return false;
    }

    // 3. Get the back buffer and create the Render Target View (RTV)
    ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
    {
        std::cerr << "Failed to get back buffer! HR: " << std::hex << hr << std::endl;
        return false;
    }

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
    if (FAILED(hr))
    {
        std::cerr << "Failed to create Render Target View! HR: " << std::hex << hr << std::endl;
        return false;
    }

    CreateDepthBuffer(width, height);
    CreateDepthStencilState();

    // 4. Bind the Render Target View
    m_deviceContext->OMSetRenderTargets(
        1,
        m_renderTargetView.GetAddressOf(),
        m_depthStencilView.Get()
    );

    m_deviceContext->OMSetDepthStencilState(
        m_depthStencilState.Get(),
        0
    );
    // 5. Setup the Viewport
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<float>(width);
    m_viewport.Height = static_cast<float>(height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_deviceContext->RSSetViewports(1, &m_viewport);

    std::cout << "DirectX 11 initialized successfully." << std::endl;
    return true;
}

void Renderer::BeginFrame(float r, float g, float b, float a)
{
    const float clearColor[4] = { r, g, b, a };
    m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);

    if (m_depthStencilView)
    {
        m_deviceContext->ClearDepthStencilView(
            m_depthStencilView.Get(),
            D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
            1.0f, // Clear depth to 1.0 (furthest away)
            0     // Stencil clear value
        );
    }

    // Re-bind both views
    m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
}

void Renderer::EndFrame()
{
    // 1 = VSync enabled (0 = unlocked frame rate)
    m_swapChain->Present(1, 0);
}

void Renderer::CleanUp()
{
    // ComPtr cleans up automatically, but you can explicitly reset or release state if needed
    if (m_deviceContext)
    {
        m_deviceContext->ClearState();
    }
}


void Renderer::CreateDepthBuffer(UINT width, UINT height)
{
    D3D11_TEXTURE2D_DESC depthDesc = {};

    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;

    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;

    depthDesc.Usage = D3D11_USAGE_DEFAULT;

    depthDesc.BindFlags =
        D3D11_BIND_DEPTH_STENCIL;

    HRESULT hr = m_device->CreateTexture2D(
        &depthDesc,
        nullptr,
        &m_depthBuffer
    );

    if (FAILED(hr))
        throw std::runtime_error("Failed to create depth buffer");

    hr = m_device->CreateDepthStencilView(
        m_depthBuffer.Get(),
        nullptr,
        &m_depthStencilView
    );

    if (FAILED(hr))
        throw std::runtime_error("Failed to create depth stencil view");
}



void Renderer::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC desc = {};

    desc.DepthEnable = TRUE;

    desc.DepthWriteMask =
        D3D11_DEPTH_WRITE_MASK_ALL;

    desc.DepthFunc = D3D11_COMPARISON_LESS;

    desc.StencilEnable = FALSE;

    HRESULT hr = m_device->CreateDepthStencilState(
        &desc,
        &m_depthStencilState
    );

    if (FAILED(hr))
        throw std::runtime_error("Failed to create depth state");
}
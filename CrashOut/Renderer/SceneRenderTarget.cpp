#include "SceneRenderTarget.h"

void SceneRenderTarget::Resize(
    Renderer& renderer, UINT width, UINT height)
{
    if (width == 0 || height == 0)
        return;

    if (width == myWidth && height == myHeight)
        return;

    auto* device = renderer.GetDevice();

    myRTV.Reset();
    mySRV.Reset();
    myTexture.Reset();
    myDSV.Reset();
    myDepthTexture.Reset();

    // Color texture
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags =
        D3D11_BIND_RENDER_TARGET |
        D3D11_BIND_SHADER_RESOURCE;

    DX::Check(device->CreateTexture2D(
        &desc, nullptr, myTexture.GetAddressOf()));

    DX::Check(device->CreateRenderTargetView(
        myTexture.Get(), nullptr, myRTV.GetAddressOf()));

    DX::Check(device->CreateShaderResourceView(
        myTexture.Get(), nullptr, mySRV.GetAddressOf()));

    // Depth texture
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    DX::Check(device->CreateTexture2D(
        &desc, nullptr, myDepthTexture.GetAddressOf()));

    DX::Check(device->CreateDepthStencilView(
        myDepthTexture.Get(), nullptr, myDSV.GetAddressOf()));

    myWidth = width;
    myHeight = height;
}

void SceneRenderTarget::Bind(Renderer& renderer)
{
    auto* context = renderer.GetContext();

    // Avoid binding the same texture for reading and writing
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    context->OMSetRenderTargets(
        1, myRTV.GetAddressOf(), myDSV.Get());

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(myWidth);
    viewport.Height = static_cast<float>(myHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    context->RSSetViewports(1, &viewport);

    const float clearColor[] = {
        0.12f, 0.12f, 0.15f, 1.0f
    };

    context->ClearRenderTargetView(
        myRTV.Get(), clearColor);

    context->ClearDepthStencilView(
        myDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
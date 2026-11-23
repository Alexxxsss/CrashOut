#include "ShadowSystem.h"

#include "Renderer.h"
#include "Model.h"
#include "DXError.h"
#include "../Core/AssetPath.h"
#include "../Ecs/Registry.h"
#include "../Ecs/Components.h"

#include <d3dcompiler.h>
#include <algorithm>
#include <cmath>
#include <string>
#include <stdexcept>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace
{
    static_assert(sizeof(XMMATRIX) == 64, "Expected 4x4 float matrix");

    XMFLOAT3 NormalizeOrDefault(const XMFLOAT3& direction)
    {
        XMVECTOR value = XMLoadFloat3(&direction);
        if (XMVectorGetX(XMVector3LengthSq(value)) < 0.000001f)
            return { 0.0f, -1.0f, 0.0f };
        XMFLOAT3 result;
        XMStoreFloat3(&result, XMVector3Normalize(value));
        return result;
    }
}

void ShadowSystem::SetCoverage(float width, float height)
{
    myCoverageWidth = (std::max)(1.0f, width);
    myCoverageHeight = (std::max)(1.0f, height);
}

void ShadowSystem::SetBias(float bias)
{
    myBias = (std::clamp)(bias, 0.0f, 0.02f);
}

void ShadowSystem::Init(Renderer& renderer, UINT resolution)
{
    if (resolution == 0)
        throw std::invalid_argument("Shadow resolution cannot be zero");
    myRenderer = &renderer;
    auto* device = renderer.GetDevice();

    // Same typeless texture is a D32 depth target during the shadow pass,
    // and an R32 shader-readable texture during the regular color pass.
    D3D11_TEXTURE2D_DESC texture = {};
    texture.Width = resolution;
    texture.Height = resolution;
    texture.MipLevels = 1;
    texture.ArraySize = 1;
    texture.Format = DXGI_FORMAT_R32_TYPELESS;
    texture.SampleDesc.Count = 1;
    texture.Usage = D3D11_USAGE_DEFAULT;
    texture.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    DX::Check(device->CreateTexture2D(
        &texture, nullptr, myDepthTexture.ReleaseAndGetAddressOf()),
        "Create shadow depth texture");

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};
    dsv.Format = DXGI_FORMAT_D32_FLOAT;
    dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsv.Texture2D.MipSlice = 0;
    DX::Check(device->CreateDepthStencilView(
        myDepthTexture.Get(), &dsv, myDepthView.ReleaseAndGetAddressOf()),
        "Create shadow DSV");

    D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
    srv.Format = DXGI_FORMAT_R32_FLOAT;
    srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv.Texture2D.MostDetailedMip = 0;
    srv.Texture2D.MipLevels = 1;
    DX::Check(device->CreateShaderResourceView(
        myDepthTexture.Get(), &srv, myShaderResource.ReleaseAndGetAddressOf()),
        "Create shadow SRV");

    myViewport.TopLeftX = 0.0f;
    myViewport.TopLeftY = 0.0f;
    myViewport.Width = static_cast<float>(resolution);
    myViewport.Height = static_cast<float>(resolution);
    myViewport.MinDepth = 0.0f;
    myViewport.MaxDepth = 1.0f;

    D3D11_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sampler.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampler.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampler.BorderColor[0] = 1.0f;
    sampler.BorderColor[1] = 1.0f;
    sampler.BorderColor[2] = 1.0f;
    sampler.BorderColor[3] = 1.0f;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D11_FLOAT32_MAX;
    DX::Check(device->CreateSamplerState(
        &sampler, myComparisonSampler.ReleaseAndGetAddressOf()),
        "Create comparison sampler");

    D3D11_RASTERIZER_DESC raster = {};
    raster.FillMode = D3D11_FILL_SOLID;
    raster.CullMode = D3D11_CULL_BACK;
    raster.FrontCounterClockwise = FALSE;
    raster.DepthClipEnable = TRUE;
    raster.DepthBias = 1200;
    raster.SlopeScaledDepthBias = 1.5f;
    raster.DepthBiasClamp = 0.0f;
    DX::Check(device->CreateRasterizerState(
        &raster, myShadowRasterizer.ReleaseAndGetAddressOf()),
        "Create depth-bias rasterizer");

    D3D11_DEPTH_STENCIL_DESC depth = {};
    depth.DepthEnable = TRUE;
    depth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth.DepthFunc = D3D11_COMPARISON_LESS;
    depth.StencilEnable = FALSE;
    DX::Check(device->CreateDepthStencilState(
        &depth, myShadowDepthState.ReleaseAndGetAddressOf()),
        "Create shadow depth state");

    D3D11_BUFFER_DESC objectBuffer = {};
    objectBuffer.ByteWidth = sizeof(ShadowObjectConstants);
    objectBuffer.Usage = D3D11_USAGE_DEFAULT;
    objectBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    DX::Check(device->CreateBuffer(
        &objectBuffer, nullptr, myDepthObjectBuffer.ReleaseAndGetAddressOf()),
        "Create shadow object constant buffer");

    D3D11_BUFFER_DESC pixelBuffer = {};
    pixelBuffer.ByteWidth = sizeof(ShadowPixelConstants);
    pixelBuffer.Usage = D3D11_USAGE_DEFAULT;
    pixelBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    DX::Check(device->CreateBuffer(
        &pixelBuffer, nullptr, myShadowPixelBuffer.ReleaseAndGetAddressOf()),
        "Create shadow pixel constant buffer");

    const auto shaderPath = CrashOut::AssetPath(L"Shaders/ShadowDepth.hlsl");
    ComPtr<ID3DBlob> compiled;
    ComPtr<ID3DBlob> errors;
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    HRESULT hr = D3DCompileFromFile(
        shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "vs_5_0", flags, 0,
        compiled.GetAddressOf(), errors.GetAddressOf());
    if (FAILED(hr))
    {
        const std::string detail = errors
            ? std::string(static_cast<const char*>(errors->GetBufferPointer()),
                errors->GetBufferSize())
            : "No compiler output";
        throw std::runtime_error(
            "Failed to compile Shaders/ShadowDepth.hlsl: " + detail);
    }

    DX::Check(device->CreateVertexShader(
        compiled->GetBufferPointer(), compiled->GetBufferSize(),
        nullptr, myDepthVS.ReleaseAndGetAddressOf()),
        "Create shadow depth vertex shader");

    // Model's vertex structure still has position at byte offset 0.
    D3D11_INPUT_ELEMENT_DESC positionLayout = {};
    positionLayout.SemanticName = "POSITION";
    positionLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    positionLayout.InputSlot = 0;
    positionLayout.AlignedByteOffset = 0;
    positionLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    DX::Check(device->CreateInputLayout(
        &positionLayout, 1,
        compiled->GetBufferPointer(), compiled->GetBufferSize(),
        myInputLayout.ReleaseAndGetAddressOf()),
        "Create shadow vertex layout");
}

void ShadowSystem::RenderDepth(Registry& registry)
{
    myHasShadow = false;
    if (!myEnabled || !myRenderer)
        return;

    // Match LightingSystem: only its first enabled directional light is used.
    const auto* lightPool = registry.GetPool<DirectionalLightComponent>();
    if (!lightPool)
        return;

    const DirectionalLightComponent* sun = nullptr;
    for (const auto& [entity, light] : lightPool->data)
    {
        if (light.enabled)
        {
            sun = &light;
            break;
        }
    }
    if (!sun || !sun->castsShadows)
        return;

    auto* meshPool = registry.GetPool<MeshComponent>();
    if (!meshPool)
        return;

    const XMFLOAT3 direction = NormalizeOrDefault(sun->direction);
    const XMVECTOR target = XMLoadFloat3(&myFocus);
    const XMVECTOR dir = XMLoadFloat3(&direction);
    const XMVECTOR eye = XMVectorSubtract(target, XMVectorScale(dir, 70.0f));
    const XMVECTOR up =
        (std::abs(direction.y) > 0.98f)
        ? XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)
        : XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    const XMMATRIX lightView = XMMatrixLookAtLH(eye, target, up);
    const XMMATRIX lightProjection = XMMatrixOrthographicLH(
        myCoverageWidth, myCoverageHeight, 0.1f, 180.0f);
    myLightViewProjection = lightView * lightProjection;

    auto* context = myRenderer->GetContext();

    // A depth texture cannot be used as SRV and DSV at the same time.
    // Unbind last frame's PS texture t1 before we write it again.
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(1, 1, &nullSRV);

    ComPtr<ID3D11RenderTargetView> previousRTV;
    ComPtr<ID3D11DepthStencilView> previousDSV;
    ComPtr<ID3D11RasterizerState> previousRaster;
    ComPtr<ID3D11DepthStencilState> previousDepthState;
    UINT previousStencilRef = 0;
    D3D11_VIEWPORT previousViewports[
        D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
        UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

        context->OMGetRenderTargets(
            1, previousRTV.GetAddressOf(), previousDSV.GetAddressOf());
        context->RSGetViewports(&viewportCount, previousViewports);
        context->RSGetState(previousRaster.GetAddressOf());
        context->OMGetDepthStencilState(
            previousDepthState.GetAddressOf(), &previousStencilRef);

        // Depth-only pass: no color RTV, and no pixel shader is necessary.
        context->OMSetRenderTargets(0, nullptr, myDepthView.Get());
        context->ClearDepthStencilView(
            myDepthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
        context->RSSetViewports(1, &myViewport);
        context->RSSetState(myShadowRasterizer.Get());
        context->OMSetDepthStencilState(myShadowDepthState.Get(), 0);
        context->IASetInputLayout(myInputLayout.Get());
        context->VSSetShader(myDepthVS.Get(), nullptr, 0);
        context->VSSetConstantBuffers(
            0, 1, myDepthObjectBuffer.GetAddressOf());
        context->PSSetShader(nullptr, nullptr, 0);

        for (const auto& [entity, mesh] : meshPool->data)
        {
            if (!mesh.model || !mesh.castShadows)
                continue;
            const auto* transform = registry.GetComponent<TransformComponent>(entity);
            if (!transform)
                continue;

            ShadowObjectConstants object = {};
            object.worldLightViewProjection = XMMatrixTranspose(
                transform->GetWorldMatrix() * myLightViewProjection);
            context->UpdateSubresource(
                myDepthObjectBuffer.Get(), 0, nullptr, &object, 0, 0);
            mesh.model->Render();
        }

        // Restore the viewport, screen framebuffer, depth state, and rasterizer.
        // The ordinary shader is bound again by RenderSystem immediately after.
        context->OMSetRenderTargets(
            1, previousRTV.GetAddressOf(), previousDSV.Get());
        if (viewportCount > 0)
            context->RSSetViewports(viewportCount, previousViewports);
        context->RSSetState(previousRaster.Get());
        context->OMSetDepthStencilState(previousDepthState.Get(), previousStencilRef);
        myHasShadow = true;
}

void ShadowSystem::BindForLighting()
{
    if (!myRenderer)
        return;
    auto* context = myRenderer->GetContext();

    ShadowPixelConstants constants = {};
    constants.lightViewProjection = XMMatrixTranspose(myLightViewProjection);
    constants.settings = {
        myEnabled && myHasShadow ? 1.0f : 0.0f,
        myBias, 0.0f, 0.0f
    };
    context->UpdateSubresource(
        myShadowPixelBuffer.Get(), 0, nullptr, &constants, 0, 0);
    context->PSSetConstantBuffers(
        2, 1, myShadowPixelBuffer.GetAddressOf());
    context->PSSetShaderResources(
        1, 1, myShaderResource.GetAddressOf());
    context->PSSetSamplers(
        1, 1, myComparisonSampler.GetAddressOf());
}

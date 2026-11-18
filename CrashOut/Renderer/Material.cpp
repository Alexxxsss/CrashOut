
#include "Material.h"
#include "Renderer.h"
#include "../External/WICTextureLoader11.h"

#include <stdexcept>

void Material::Init(Renderer* renderer)
{
    myRenderer = renderer;
}

void Material::LoadTexture(const std::wstring& path)
{
    ID3D11Device* device = myRenderer->GetDevice();
    ID3D11DeviceContext* context = myRenderer->GetContext();

    DX::Check(DirectX::CreateWICTextureFromFile(
        device,
        context,
        path.c_str(),
        nullptr,
        myDiffuseTexture.ReleaseAndGetAddressOf()
    ));

    D3D11_SAMPLER_DESC desc = {};

    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    DX::Check(device->CreateSamplerState(
        &desc,
        mySamplerState.ReleaseAndGetAddressOf()
    ));
}

void Material::Use()
{
    ID3D11DeviceContext* context = myRenderer->GetContext();

    context->PSSetShaderResources(
        0, 1, myDiffuseTexture.GetAddressOf()
    );

    context->PSSetSamplers(
        0, 1, mySamplerState.GetAddressOf()
    );
}

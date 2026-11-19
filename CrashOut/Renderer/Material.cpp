#include "Material.h"
#include "Renderer.h"
#include "Texture.h"

#include <utility>

void Material::Init(Renderer* renderer)
{
    myRenderer = renderer;

    D3D11_SAMPLER_DESC desc = {};

    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

    desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    DX::Check(myRenderer->GetDevice()->CreateSamplerState(
        &desc,
        mySamplerState.ReleaseAndGetAddressOf()
    ));
}

void Material::SetTexture(
    std::shared_ptr<Texture> aTexture)
{
    myDiffuseTexture = std::move(aTexture);
}

void Material::Use()
{
    ID3D11DeviceContext* context =
        myRenderer->GetContext();

    if (myDiffuseTexture)
    {
        myDiffuseTexture->Bind(*myRenderer, 0);
    }
    else
    {
        ID3D11ShaderResourceView* emptyTexture = nullptr;

        context->PSSetShaderResources(
            0, 1, &emptyTexture
        );
    }

    context->PSSetSamplers(
        0, 1, mySamplerState.GetAddressOf()
    );
}
#include "Texture.h"
#include "Renderer.h"
#include "../External/WICTextureLoader11.h"

void Texture::Load(Renderer& aRenderer, const std::wstring& aPath)
{
    DX::Check(DirectX::CreateWICTextureFromFile(
        aRenderer.GetDevice(),
        aRenderer.GetContext(),
        aPath.c_str(),
        nullptr,
        myTextureView.ReleaseAndGetAddressOf()
    ));
}

void Texture::Bind(Renderer& aRenderer, UINT aSlot) const
{
    aRenderer.GetContext()->PSSetShaderResources(
        aSlot,
        1,
        myTextureView.GetAddressOf()
    );
}
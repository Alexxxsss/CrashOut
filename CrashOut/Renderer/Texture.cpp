#include "Texture.h"
#include "Renderer.h"
#include <filesystem>
#include <stdexcept>
#include "../External/WICTextureLoader11.h"


void Texture::Load(
    Renderer& aRenderer,
    const std::wstring& aPath)
{
    const std::filesystem::path path(aPath);

    if (!std::filesystem::is_regular_file(path))
    {
        throw std::runtime_error(
            "Texture file does not exist: " +
            path.string()
        );
    }

    HRESULT hr = DirectX::CreateWICTextureFromFile(
        aRenderer.GetDevice(),
        aRenderer.GetContext(),
        aPath.c_str(),
        nullptr,
        myTextureView.ReleaseAndGetAddressOf()
    );

    DX::Check(
        hr,
        "Failed to load texture: " +
        path.string()
    );
}

void Texture::Bind(Renderer& aRenderer, UINT aSlot) const
{
    aRenderer.GetContext()->PSSetShaderResources(
        aSlot,
        1,
        myTextureView.GetAddressOf()
    );
}
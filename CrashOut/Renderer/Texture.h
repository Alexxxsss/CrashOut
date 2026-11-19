#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>

class Renderer;

class Texture
{
public:
    Texture() = default;
    ~Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void Load(
        Renderer& aRenderer,
        const std::wstring& aPath
    );

    void Bind(
        Renderer& aRenderer,
        UINT aSlot = 0
    ) const;

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
        myTextureView;
};
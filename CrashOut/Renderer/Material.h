#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>

class Renderer;

class Material
{
public:
    Material() = default;
    ~Material() = default;

    void Init(Renderer* renderer);

    void LoadTexture(const std::wstring& path);

    void Use();

    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;
private:
    Renderer* myRenderer = nullptr;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
        myDiffuseTexture;

    Microsoft::WRL::ComPtr<ID3D11SamplerState>
        mySamplerState;
};

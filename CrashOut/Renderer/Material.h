#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

class Renderer;
class Texture;

class Material
{
public:
    Material() = default;
    ~Material() = default;

    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;

    void Init(Renderer* renderer);

    void SetTexture(std::shared_ptr<Texture> aTexture);

    void Use();

private:
    Renderer* myRenderer = nullptr;

    std::shared_ptr<Texture> myDiffuseTexture;

    Microsoft::WRL::ComPtr<ID3D11SamplerState>
        mySamplerState;
};
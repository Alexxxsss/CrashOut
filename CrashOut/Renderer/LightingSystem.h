#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <cstdint>

class Renderer;
class Registry;

class LightingSystem
{
public:
    static constexpr std::uint32_t MaxDirectionalLights = 1;
    static constexpr std::uint32_t MaxPointLights = 8;
    static constexpr std::uint32_t MaxSpotLights = 4;

    void Init(Renderer& renderer);
    void UpdateAndBind(Registry& registry);
    void SetAmbient(const DirectX::XMFLOAT3& color, float intensity);

private:
    Renderer* myRenderer = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> myLightBuffer;
    DirectX::XMFLOAT3 myAmbientColor = { 1.0f, 1.0f, 1.0f };
    float myAmbientIntensity = 0.15f;
};

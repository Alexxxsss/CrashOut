#include "LightingSystem.h"
#include "Renderer.h"
#include "../Ecs/Components.h"
#include "../Ecs/Registry.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

using namespace DirectX;

namespace
{
    // float4 values keep HLSL constant-buffer packing predictable.
    struct GpuDirectionalLight
    {
        XMFLOAT4 direction;       // xyz: direction light travels
        XMFLOAT4 colorIntensity;  // rgb: color, w: intensity
    };

    struct GpuPointLight
    {
        XMFLOAT4 positionRange;   // xyz: position, w: range
        XMFLOAT4 colorIntensity;  // rgb: color, w: intensity
    };

    struct GpuSpotLight
    {
        XMFLOAT4 positionRange;   // xyz: position, w: range
        XMFLOAT4 directionInner;  // xyz: direction, w: cos(inner half-angle)
        XMFLOAT4 colorOuter;      // rgb: color, w: cos(outer half-angle)
        XMFLOAT4 intensityPad;    // x: intensity
    };

    struct GpuLightingBuffer
    {
        XMFLOAT4 ambient;  // rgb: color, w: intensity
        std::uint32_t directionalCount = 0;
        std::uint32_t pointCount = 0;
        std::uint32_t spotCount = 0;
        std::uint32_t padding = 0;
        GpuDirectionalLight directional[LightingSystem::MaxDirectionalLights] = {};
        GpuPointLight point[LightingSystem::MaxPointLights] = {};
        GpuSpotLight spot[LightingSystem::MaxSpotLights] = {};
    };

    static_assert(sizeof(GpuDirectionalLight) == 32);
    static_assert(sizeof(GpuPointLight) == 32);
    static_assert(sizeof(GpuSpotLight) == 64);
    static_assert(sizeof(GpuLightingBuffer) % 16 == 0);

    XMFLOAT3 Normalized(const XMFLOAT3& input)
    {
        const XMVECTOR v = XMLoadFloat3(&input);
        const float lengthSquared = XMVectorGetX(XMVector3LengthSq(v));
        if (lengthSquared < 0.000001f)
            return { 0.0f, -1.0f, 0.0f };

        XMFLOAT3 result;
        XMStoreFloat3(&result, XMVector3Normalize(v));
        return result;
    }
}

void LightingSystem::Init(Renderer& renderer)
{
    myRenderer = &renderer;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(GpuLightingBuffer);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    DX::Check(renderer.GetDevice()->CreateBuffer(
        &desc, nullptr, myLightBuffer.ReleaseAndGetAddressOf()
    ), "Failed to create lighting constant buffer");
}

void LightingSystem::SetAmbient(const XMFLOAT3& color, float intensity)
{
    myAmbientColor = color;
    myAmbientIntensity = (std::max)(0.0f, intensity);
}

void LightingSystem::UpdateAndBind(Registry& registry)
{
    if (!myRenderer || !myLightBuffer)
        throw std::runtime_error("LightingSystem::Init must be called first");

    GpuLightingBuffer gpu = {};
    gpu.ambient = { myAmbientColor.x, myAmbientColor.y,
                    myAmbientColor.z, myAmbientIntensity };

    if (auto* pool = registry.GetPool<DirectionalLightComponent>())
    {
        for (const auto& [entity, light] : pool->data)
        {
            if (!light.enabled || gpu.directionalCount >= MaxDirectionalLights)
                continue;

            const XMFLOAT3 dir = Normalized(light.direction);
            auto& dst = gpu.directional[gpu.directionalCount++];
            dst.direction = { dir.x, dir.y, dir.z, 0.0f };
            dst.colorIntensity = { light.color.x, light.color.y,
                                   light.color.z, (std::max)(0.0f, light.intensity) };
        }
    }

    if (auto* pool = registry.GetPool<PointLightComponent>())
    {
        for (const auto& [entity, light] : pool->data)
        {
            const auto* transform = registry.GetComponent<TransformComponent>(entity);
            if (!light.enabled || !transform || light.range <= 0.0f ||
                gpu.pointCount >= MaxPointLights)
                continue;

            auto& dst = gpu.point[gpu.pointCount++];
            dst.positionRange = { transform->position.x, transform->position.y,
                                  transform->position.z, light.range };
            dst.colorIntensity = { light.color.x, light.color.y,
                                   light.color.z, (std::max)(0.0f, light.intensity) };
        }
    }

    if (auto* pool = registry.GetPool<SpotLightComponent>())
    {
        for (const auto& [entity, light] : pool->data)
        {
            const auto* transform = registry.GetComponent<TransformComponent>(entity);
            if (!light.enabled || !transform || light.range <= 0.0f ||
                gpu.spotCount >= MaxSpotLights)
                continue;

            const XMFLOAT3 localDirection = Normalized(light.direction);
            XMFLOAT3 worldDirection;
            XMStoreFloat3(&worldDirection, XMVector3Rotate(
                XMLoadFloat3(&localDirection),
                XMQuaternionNormalize(XMLoadFloat4(&transform->rotation))));
            worldDirection = Normalized(worldDirection);

            const float inner = std::clamp(light.innerAngleDegrees, 0.1f, 89.0f);
            const float outer = std::clamp(light.outerAngleDegrees, inner + 0.1f, 89.9f);
            const float innerCos = std::cos(XMConvertToRadians(inner));
            const float outerCos = std::cos(XMConvertToRadians(outer));

            auto& dst = gpu.spot[gpu.spotCount++];
            dst.positionRange = { transform->position.x, transform->position.y,
                                  transform->position.z, light.range };
            dst.directionInner = { worldDirection.x, worldDirection.y,
                                   worldDirection.z, innerCos };
            dst.colorOuter = { light.color.x, light.color.y,
                               light.color.z, outerCos };
            dst.intensityPad = { (std::max)(0.0f, light.intensity), 0.0f, 0.0f, 0.0f };
        }
    }

    auto* context = myRenderer->GetContext();
    context->UpdateSubresource(myLightBuffer.Get(), 0, nullptr, &gpu, 0, 0);
    context->PSSetConstantBuffers(1, 1, myLightBuffer.GetAddressOf());
}

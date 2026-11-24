#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <box3d/box3d.h>
#include <memory>
#include <string>

class Material;
class Model;

struct NameComponent
{
    std::string name = "Entity";
};

struct CameraComponent
{
    float fov = 50.0f; 
    float nearPlane = 0.01f;
    float farPlane = 100.0f;
    bool primary = false;
};

struct TransformComponent
{
    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT4 rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

    DirectX::XMMATRIX GetWorldMatrix() const
    {
        using namespace DirectX;
        XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);
        XMMATRIX r = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
        XMMATRIX t = XMMatrixTranslation(position.x, position.y, position.z);
        return s * r * t;
    }
};

struct MeshComponent
{
    std::shared_ptr<Model> model = nullptr;
    bool castShadows = true; 
    std::string assetPath; 
};

struct MaterialComponent
{
    std::shared_ptr<Material> material = nullptr;
    std::string texturePath;
};

struct DirectionalLightComponent
{
    DirectX::XMFLOAT3 direction = { -0.4f, -1.0f, 0.3f };
    DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
    float intensity = 1.0f;
    bool enabled = true;
    bool castsShadows = true; 
};

struct PointLightComponent
{
    DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
    float intensity = 2.0f;
    float range = 10.0f;
    bool enabled = true;
};

struct SpotLightComponent
{
    DirectX::XMFLOAT3 direction = { 0.0f, -1.0f, 0.0f };
    DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
    float intensity = 3.0f;
    float range = 12.0f;
    float innerAngleDegrees = 18.0f; 
    float outerAngleDegrees = 30.0f; 
    bool enabled = true;
};

struct BoxColliderComponent
{
    DirectX::XMFLOAT3 halfExtents = { 0.5f, 0.5f, 0.5f };
    DirectX::XMFLOAT3 localOffset = { 0.0f, 0.0f, 0.0f };

    bool isStatic = false; 
    float friction = 0.3f;
    float restitution = 0.2f;
    float density = 1.0f;
    bool isSensor = false;

    b3BodyId bodyId = b3_nullBodyId;
    b3ShapeId shapeId = b3_nullShapeId;
    bool isInitialized = false;
};

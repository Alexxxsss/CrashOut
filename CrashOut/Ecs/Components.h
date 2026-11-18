#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <box3d/box3d.h>
#include <memory>

class Material;
class Model;

struct TransformComponent
{
    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    // Quaternion (x, y, z, w) - matches Box3D natively
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
};

struct MaterialComponent
{
    std::shared_ptr<Material> material = nullptr;
};

struct BoxColliderComponent
{
    DirectX::XMFLOAT3 halfExtents = { 0.5f, 0.5f, 0.5f };
    DirectX::XMFLOAT3 localOffset = { 0.0f, 0.0f, 0.0f };

    bool isStatic = false; // true = immovable (floors/walls), false = dynamic
    float friction = 0.3f;
    float restitution = 0.2f;
    float density = 1.0f;
    bool isSensor = false;

    b3BodyId bodyId = b3_nullBodyId;
    b3ShapeId shapeId = b3_nullShapeId;
    bool isInitialized = false;
};
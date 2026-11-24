
#include "GizmoSystem.h"
#include "DebugDraw.h"
#include <cmath>
#include "../Renderer.h"
#include "../Camera.h"

#include "../../Ecs/Registry.h"
#include "../../Ecs/Components.h"

using namespace DirectX;

void GizmoSystem::Init(Renderer* renderer)
{
    myRenderer = renderer;

    auto* device = renderer->GetDevice();
    auto* context = renderer->GetContext();

    myStates = std::make_unique<CommonStates>(device);

    myBatch = std::make_unique<
        PrimitiveBatch<VertexPositionColor>>(context);

    myEffect = std::make_unique<BasicEffect>(device);

    myEffect->SetVertexColorEnabled(true);
    myEffect->SetLightingEnabled(false);
    myEffect->SetWorld(XMMatrixIdentity());

    const void* shaderByteCode = nullptr;
    size_t byteCodeLength = 0;

    myEffect->GetVertexShaderBytecode(
        &shaderByteCode,
        &byteCodeLength
    );

    DX::Check(device->CreateInputLayout(
        VertexPositionColor::InputElements,
        VertexPositionColor::InputElementCount,
        shaderByteCode,
        byteCodeLength,
        myLayout.ReleaseAndGetAddressOf()
    ));
}

void GizmoSystem::RenderColliders(
    Registry& registry,
    Camera& camera)
{
    if (!enabled)
        return;

    auto* pool = registry.GetPool<BoxColliderComponent>();

    if (!pool)
        return;

    auto* context = myRenderer->GetContext();

    // Save existing render states.
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> oldDepth;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> oldRaster;
    Microsoft::WRL::ComPtr<ID3D11BlendState> oldBlend;

    UINT stencilRef;
    UINT sampleMask;
    float blendFactor[4];

    context->OMGetDepthStencilState(
        oldDepth.GetAddressOf(), &stencilRef
    );

    context->RSGetState(oldRaster.GetAddressOf());

    context->OMGetBlendState(
        oldBlend.GetAddressOf(), blendFactor, &sampleMask
    );

    // Setup gizmo rendering.
    context->OMSetBlendState(
        myStates->Opaque(), nullptr, 0xFFFFFFFF
    );

    context->OMSetDepthStencilState(
        showThroughWalls
        ? myStates->DepthNone()
        : myStates->DepthRead(),
        0
    );

    context->RSSetState(myStates->CullNone());

    myEffect->SetView(camera.GetView());
    myEffect->SetProjection(camera.GetProjection());
    myEffect->Apply(context);

    context->IASetInputLayout(myLayout.Get());

    myBatch->Begin();

    for (auto& [entity, collider] : pool->data)
    {
        auto* transform = registry.GetComponent<TransformComponent>(entity);
        if (!transform) continue;

        // Same scaled dimensions and offset as PhysicsSystem::CreateBoxCollider.
        const auto& scale = transform->scale;
        BoundingOrientedBox box;
        box.Extents = {
            std::abs(collider.halfExtents.x * scale.x),
            std::abs(collider.halfExtents.y * scale.y),
            std::abs(collider.halfExtents.z * scale.z)
        };
        XMVECTOR q = XMLoadFloat4(&transform->rotation);
        float lengthSq = XMVectorGetX(XMVector4LengthSq(q));
        if (!std::isfinite(lengthSq) || lengthSq < 1e-8f) continue;
        q = XMQuaternionNormalize(q);
        XMVECTOR offset = XMVectorSet(collider.localOffset.x * scale.x,
            collider.localOffset.y * scale.y, collider.localOffset.z * scale.z, 0);
        XMStoreFloat3(&box.Center, XMVectorAdd(XMLoadFloat3(&transform->position), XMVector3Rotate(offset, q)));
        XMStoreFloat4(&box.Orientation, q);

        XMVECTOR color = Colors::Lime;

        if (collider.isStatic)
            color = Colors::Cyan;

        if (collider.isSensor)
            color = Colors::Yellow;

        Draw(myBatch.get(), box, color);
    }

    myBatch->End();

    // Restore previous render states.
    context->OMSetDepthStencilState(
        oldDepth.Get(), stencilRef
    );

    context->RSSetState(oldRaster.Get());

    context->OMSetBlendState(
        oldBlend.Get(), blendFactor, sampleMask
    );
}

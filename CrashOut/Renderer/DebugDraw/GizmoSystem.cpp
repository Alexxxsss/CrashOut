
#include "GizmoSystem.h"
#include "DebugDraw.h"
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
        if (!b3Body_IsValid(collider.bodyId))
            continue;

        if (!b3Shape_IsValid(collider.shapeId))
            continue;

        if (b3Shape_GetType(collider.shapeId) != b3_hullShape)
            continue;

        const b3HullData* hull =
            b3Shape_GetHull(collider.shapeId);

        if (!hull)
            continue;

        const auto& min = hull->aabb.lowerBound;
        const auto& max = hull->aabb.upperBound;

        b3Vec3 localCenter = {
            (min.x + max.x) * 0.5f,
            (min.y + max.y) * 0.5f,
            (min.z + max.z) * 0.5f
        };

        b3Pos center = b3Body_GetWorldPoint(
            collider.bodyId,
            localCenter
        );

        b3Quat rotation =
            b3Body_GetRotation(collider.bodyId);

        BoundingOrientedBox box;

        box.Center = {
            static_cast<float>(center.x),
            static_cast<float>(center.y),
            static_cast<float>(center.z)
        };

        box.Extents = {
            (max.x - min.x) * 0.5f,
            (max.y - min.y) * 0.5f,
            (max.z - min.z) * 0.5f
        };

        box.Orientation = {
            rotation.v.x,
            rotation.v.y,
            rotation.v.z,
            rotation.s
        };

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

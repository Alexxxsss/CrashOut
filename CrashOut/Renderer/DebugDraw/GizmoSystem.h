
#pragma once

#include <memory>
#include <wrl/client.h>

#include "../../External/DirectXTK/Inc/PrimitiveBatch.h"
#include "../../External/DirectXTK/Inc/VertexTypes.h"
#include "../../External/DirectXTK/Inc/Effects.h"
#include "../../External/DirectXTK/Inc/CommonStates.h"

#include <DirectXCollision.h>
#include <DirectXColors.h>


class Renderer;
class Camera;
class Registry;

class GizmoSystem
{
public:
    void Init(Renderer* renderer);

    void RenderColliders(
        Registry& registry,
        Camera& camera
    );

    bool enabled = true;
    bool showThroughWalls = true;

private:
    Renderer* myRenderer = nullptr;

    std::unique_ptr<DirectX::CommonStates> myStates;

    std::unique_ptr<DirectX::PrimitiveBatch<
        DirectX::VertexPositionColor>> myBatch;

    std::unique_ptr<DirectX::BasicEffect> myEffect;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> myLayout;
};

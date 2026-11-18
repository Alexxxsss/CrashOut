#include "RenderSystem.h"

#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "Material.h"

#include "../Ecs/Registry.h"
#include "../Ecs/Components.h"

#include <DirectXMath.h>

using namespace DirectX;

void RenderSystem::Render(
    Registry& aRegistry,
    Camera& aCamera,
    Shader& aShader,
    Material& aDefaultMaterial)
{
    XMMATRIX view = aCamera.GetView();
    XMMATRIX projection = aCamera.GetProjection();

    XMMATRIX viewProjection = view * projection;

    auto* meshPool = aRegistry.GetPool<MeshComponent>();

    if (!meshPool)
        return;

    aShader.Use();

    for (auto& [entity, mesh] : meshPool->data)
    {
        if (!mesh.model)
            continue;

        auto* transform =
            aRegistry.GetComponent<TransformComponent>(entity);

        if (!transform)
            continue;

        XMMATRIX world = transform->GetWorldMatrix();
        XMMATRIX mvp = world * viewProjection;

        aShader.UpdateModelViewProjectionBuffer(mvp);

        auto* material =
            aRegistry.GetComponent<MaterialComponent>(entity);

        if (material && material->material)
        {
            material->material->Use();
        }
        else
        {
            aDefaultMaterial.Use();
        }

        mesh.model->Render();
    }
}

// Renderer/RenderSystem.h
#pragma once
#include "Ecs/Registry.h"
#include "Ecs/Components.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/RasterState.h"

class RenderSystem
{
public:
    static void Render(Registry& registry, ID3D11DeviceContext* context, Camera& camera, Shader& shader, RasterState& rasterState)
    {
        // 1. Bind global pipeline states once per frame
        shader.Use();
        rasterState.Use();

        DirectX::XMMATRIX view = camera.GetView();
        DirectX::XMMATRIX proj = camera.GetProjection();
        DirectX::XMMATRIX viewProj = view * proj;

        // 2. Iterate through all entities having a TransformComponent
        auto* transformPool = registry.GetPool<TransformComponent>();
        if (!transformPool) return;

        for (auto& [entity, transform] : transformPool->data)
        {
            MeshComponent* mesh = registry.GetComponent<MeshComponent>(entity);
            if (!mesh || !mesh->model) continue;

            MaterialComponent* mat = registry.GetComponent<MaterialComponent>(entity);

            // 3. Compute Per-Entity MVP Matrix
            DirectX::XMMATRIX world = transform.GetWorldMatrix();
            DirectX::XMMATRIX mvp = world * viewProj;

            // Upload to register b0
            shader.UpdateModelViewProjectionBuffer(mvp);

            // 4. Bind Per-Entity Texture & Sampler to t0 and s0
            if (mat && mat->textureSRV)
            {
                context->PSSetShaderResources(0, 1, &mat->textureSRV);
                if (mat->samplerState)
                {
                    context->PSSetSamplers(0, 1, &mat->samplerState);
                }
            }
            else
            {
                // Fallback to model's default texture if entity has no custom material
                mesh->model->UseTexture();
            }

            // 5. Draw mesh geometry
            mesh->model->Render();
        }
    }
};
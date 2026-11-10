#include <iostream>
#include <DirectXMath.h> 
#include "Engine.h"


using namespace DirectX;

extern void OnStart();
extern void OnUpdate(float& aDeltaTime, CommonUtilities::InputHandler* aInputHandler);
extern void OnCleanUp();

void RenderEntities(Registry& aRegistry, Camera& aCamera, Shader& aShader, Model& aDefaultModel);

int main()
{
    OnStart();

    // Initialize Window
    const UINT screenWidth = 1080;
    const UINT screenHeight = 720;
    Window window;
    if (!window.StartUpWindow(screenWidth, screenHeight, L"DirectX 11 Engine")) { return -1; }

    Renderer renderer;
    if (!renderer.Initialize(window.GetHwnd(), screenWidth, screenHeight))
    {
        std::cerr << "Renderer initialization failed!" << std::endl;
        return -1;
    }

    CommonUtilities::Timer timer;
    float deltaTime = 0;

    Shader shader(&renderer);
    RasterState rasterState(&renderer);
    Camera camera(screenWidth, screenHeight);

    shader.LoadPixelShader("../../CrashOut/Shaders/Pixel.hlsl");
    shader.LoadVertexShader("../../CrashOut/Shaders/Vertex.hlsl");
    shader.CreateWorldViewProjectionConstantBuffer();

    // 4. Create Shared Geometry (allocates hardware vertex & index buffer once)
    Model cubeModel(&renderer);
    cubeModel.Create();
    cubeModel.LoadTexture(); // Default texture

    // (Optional) Load custom textures if you want unique textures per entity:
    // ID3D11ShaderResourceView* customTex = TextureSampler::LoadTextureFromFile(renderer.GetDevice(), L"wood.png");

    // 5. ECS SETUP: Spawn Entities
    Registry registry;

    // --- Entity 1 (Center-left Cube) ---
    Entity entity1 = registry.CreateEntity();
    auto& t1 = registry.AddComponent<TransformComponent>(entity1);
    t1.position = { -1.5f, 0.0f, 4.0f };
    t1.scale = { 1.0f, 1.0f, 1.0f };
    registry.AddComponent<MeshComponent>(entity1, &cubeModel);

    // --- Entity 2 (Center-right Smaller Cube) ---
    Entity entity2 = registry.CreateEntity();
    auto& t2 = registry.AddComponent<TransformComponent>(entity2);
    t2.position = { 1.5f, 0.5f, 5.0f };
    t2.scale = { 0.6f, 0.6f, 0.6f };
    registry.AddComponent<MeshComponent>(entity2, &cubeModel);

    // If entity2 had a custom texture:
    //registry.AddComponent<MaterialComponent>(entity2, customTex, cubeModel.GetSamplerState());


    bool isRunning = true;
    while (isRunning)
    {
        window.UpdateOpenWindow();
        OnUpdate(deltaTime, window.GetInputHandeler());

        if (window.GetInputHandeler()->IsKeyDown('R'))
        {
            t1.rotation.y += 0.5f * deltaTime;
            t2.rotation.x += 1.f * deltaTime;
            t2.rotation.z += 0.5f * deltaTime;
        }

        

        renderer.BeginFrame(0.1f, 0.2f, 0.4f, 1.0f);
        deltaTime = timer.GetDeltaTime();

        timer.Update();
        shader.Use();
        rasterState.Use();

        // Render all entities through the ECS
        RenderEntities(registry, camera, shader, cubeModel);

        renderer.EndFrame();

        
    }

    renderer.CleanUp();
    OnCleanUp();
    return 0;
}


void RenderEntities(Registry& aRegistry, Camera& aCamera, Shader& aShader, Model& aDefaultModel)
{
    // Compute View-Projection once for the whole frame
    XMMATRIX view = aCamera.GetView();
    XMMATRIX proj = aCamera.GetProjection();
    XMMATRIX viewProj = view * proj;

    auto* transformPool = aRegistry.GetPool<TransformComponent>();
    if (!transformPool) return;

    for (auto& [entity, transform] : transformPool->data)
    {
        MeshComponent* mesh = aRegistry.GetComponent<MeshComponent>(entity);
        if (!mesh || !mesh->model)
            continue;

        // 1. Compute World * View * Projection for this specific entity
        XMMATRIX world = transform.GetWorldMatrix();
        XMMATRIX mvp = world * viewProj;

        // 2. Upload MVP to Constant Buffer register b0
        aShader.UpdateModelViewProjectionBuffer(mvp);

        // 3. Bind Entity's Texture (or default model texture if no custom material)
        MaterialComponent* mat = aRegistry.GetComponent<MaterialComponent>(entity);
        if (mat && mat->textureSRV)
        {
            // If the entity has a custom texture set up, bind it
            // (Uses Model's UseTexture() or PSSetShaderResources depending on your implementation)
            mesh->model->UseTexture();
        }
        else
        {
            mesh->model->UseTexture();
        }

        // 4. Draw mesh indices
        mesh->model->Render();
    }
}
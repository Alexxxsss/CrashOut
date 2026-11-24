#include "TestSupport.h"
#include "Scene/SceneSerializer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Resources/ResourceManager.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cmath>

int main()
{
    try
    {
        Renderer renderer; // No device needed for this asset-free test.
        ResourceManager resources(renderer);
        CrashOut::Scene edit("Test");
        Entity id = edit.RestoreEntity(42, "Box");
        auto& registry = edit.GetRegistry();
        registry.GetComponent<TransformComponent>(id)->position.y = 4;
        registry.AddComponent<BoxColliderComponent>(id);
        auto cameraId = edit.CreateEntity("Camera");
        CHECK(cameraId == 43);
        registry.AddComponent<CameraComponent>(cameraId);
        edit.SetPrimaryCamera(cameraId);
        CHECK(edit.GetEntities().size() == 2);
        CHECK(edit.GetPrimaryCamera() == cameraId);
        Camera camera;
        CHECK(edit.BuildPrimaryCamera(camera, 1280, 720));

        auto runtime = CrashOut::SceneSerializer::Clone(edit, resources);
        auto& rr = runtime->GetRegistry();
        CHECK(runtime->GetPrimaryCamera() == cameraId);
        CHECK(!rr.GetComponent<BoxColliderComponent>(id)->isInitialized);
        runtime->GetPhysics().EnsureBodies(rr);
        auto body = rr.GetComponent<BoxColliderComponent>(id)->bodyId;
        CHECK(b3Body_IsValid(body));
        CHECK(!registry.GetComponent<BoxColliderComponent>(id)->isInitialized);
        for (int i = 0; i < 30; ++i)
            runtime->GetPhysics().UpdateTransforms(rr, 1.0f / 60.0f);
        CHECK(rr.GetComponent<TransformComponent>(id)->position.y < 4);
        CHECK(registry.GetComponent<TransformComponent>(id)->position.y == 4);
        runtime.reset();
        CHECK(!b3Body_IsValid(body));
        CHECK(registry.HasEntity(id));

        const auto path = std::filesystem::current_path() / "CrashOutSceneTest.json";
        CrashOut::SceneSerializer::Save(edit, path);
        auto loaded = CrashOut::SceneSerializer::Load(path, resources);
        CHECK(loaded->GetName() == "Test");
        CHECK(loaded->FindEntityByName("Box") == id);
        CHECK(loaded->GetPrimaryCamera() == cameraId);
        CHECK(loaded->CreateEntity("New") == 44);
        std::filesystem::remove(path);
        std::cout << "PASS: scene round trip and runtime isolation\n";
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "FAIL: " << ex.what() << '\n';
        return 1;
    }
}

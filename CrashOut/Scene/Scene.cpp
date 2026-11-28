#include "Scene.h"
#include "Core/AssetPath.h"
#include "Resources/ResourceManager.h"
#include "Renderer/Camera.h"
#include <filesystem>
#include <stdexcept>
#include <utility>

namespace CrashOut
{
    namespace
    {
        std::string NormalizeAssetPath(const std::string& path)
        {
            auto p = std::filesystem::u8path(path);
            if (p.empty() || p.has_root_path())
                throw std::runtime_error("Expected a relative asset path");
            p = p.lexically_normal();
            for (const auto& part : p)
                if (part == "..") throw std::runtime_error("Asset path escapes root");
            auto utf8 = p.generic_u8string();
            return std::string(utf8.begin(), utf8.end());
        }
    }

    Scene::Scene(std::string name) : myName(std::move(name))
    {
        myRegistry.SetOnRemove<BoxColliderComponent>(
            [this](Entity, BoxColliderComponent& collider)
            {
                myPhysics.DestroyBoxCollider(collider);
            });
        myRegistry.SetOnRemove<TransformComponent>(
            [this](Entity entity, TransformComponent&)
            {
                myRegistry.RemoveComponent<BoxColliderComponent>(entity);
            });
    }

    Scene::~Scene()
    {
        myRegistry.Clear();
    }

    Entity Scene::AddDefaults(Entity entity, const std::string& name)
    {
        try
        {
            myRegistry.AddComponent<NameComponent>(entity, name);
            myRegistry.AddComponent<TransformComponent>(entity);
        }
        catch (...)
        {
            myRegistry.DestroyEntity(entity);
            throw;
        }
        return entity;
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        return AddDefaults(myRegistry.CreateEntity(), name);
    }

    Entity Scene::RestoreEntity(Entity id, const std::string& name)
    {
        return AddDefaults(myRegistry.CreateEntity(id), name);
    }

    void Scene::DestroyEntity(Entity entity)
    {
        myRegistry.DestroyEntity(entity);
    }

    std::vector<Entity> Scene::GetEntities() const
    {
        return myRegistry.GetEntities();
    }

    Entity Scene::FindEntityByName(const std::string& name) const
    {
        for (Entity entity : GetEntities())
        {
            auto* tag = myRegistry.GetComponent<NameComponent>(entity);
            if (tag && tag->name == name) return entity;
        }
        return 0;
    }

    void Scene::SetMesh(Entity entity, ResourceManager& resources,
        const std::string& path, bool castShadows)
    {
        if (!myRegistry.HasEntity(entity)) throw std::runtime_error("Invalid entity");
        auto relative = NormalizeAssetPath(path);
        MeshComponent mesh;
        mesh.assetPath = relative;
        mesh.castShadows = castShadows;
        mesh.model = resources.LoadModel(AssetPath(std::filesystem::u8path(relative)).string());
        if (auto* old = myRegistry.GetComponent<MeshComponent>(entity))
            *old = std::move(mesh);
        else myRegistry.AddComponent<MeshComponent>(entity, std::move(mesh));
    }

    void Scene::SetMaterial(Entity entity, ResourceManager& resources,
        const std::string& texturePath)
    {
        if (!myRegistry.HasEntity(entity)) throw std::runtime_error("Invalid entity");
        auto relative = NormalizeAssetPath(texturePath);
        MaterialComponent material;
        material.texturePath = relative;
        auto texture = resources.LoadTexture(AssetPath(std::filesystem::u8path(relative)).wstring());
        material.material = resources.CreateMaterial("SceneTexture:" + relative, texture);
        if (auto* old = myRegistry.GetComponent<MaterialComponent>(entity))
            *old = std::move(material);
        else myRegistry.AddComponent<MaterialComponent>(entity, std::move(material));
    }

    void Scene::SetPrimaryCamera(Entity entity)
    {
        if (entity != 0 && !myRegistry.HasComponent<CameraComponent>(entity))
            throw std::runtime_error("Entity has no camera");
        if (auto* pool = myRegistry.GetPool<CameraComponent>())
            for (auto& [id, camera] : pool->data)
                camera.primary = (id == entity);
    }

    Entity Scene::GetPrimaryCamera() const
    {
        for (Entity entity : GetEntities())
        {
            auto* camera = myRegistry.GetComponent<CameraComponent>(entity);
            if (camera && camera->primary) return entity;
        }
        return 0;
    }

    bool Scene::BuildPrimaryCamera(Camera& camera, int width, int height) const
    {
        Entity entity = GetPrimaryCamera();
        auto* lens = myRegistry.GetComponent<CameraComponent>(entity);
        auto* transform = myRegistry.GetComponent<TransformComponent>(entity);
        if (!lens || !transform || width <= 0 || height <= 0) return false;
        camera.SetFromTransform(transform->position, transform->rotation,
            lens->fov, lens->nearPlane, lens->farPlane, width, height);
        return true;
    }
}

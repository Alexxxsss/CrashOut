#pragma once
#include "Ecs/Registry.h"
#include "Ecs/Components.h"
#include "Physics/PhysicsSystem.h"
#include <string>
#include <vector>
#include <utility>

class Camera;
class ResourceManager;

namespace CrashOut
{
    class Scene
    {
    public:
        explicit Scene(std::string name = "Untitled");
        ~Scene();
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        Entity CreateEntity(const std::string& name = "Entity");
        Entity RestoreEntity(Entity id, const std::string& name);
        void DestroyEntity(Entity entity);
        Entity FindEntityByName(const std::string& name) const;
        std::vector<Entity> GetEntities() const;

        Registry& GetRegistry() { return myRegistry; }
        const Registry& GetRegistry() const { return myRegistry; }
        PhysicsSystem& GetPhysics() { return myPhysics; }
        const std::string& GetName() const { return myName; }
        void SetName(std::string name) { myName = std::move(name); }

        void SetMesh(Entity entity, ResourceManager& resources,
            const std::string& path, bool castShadows = true);
        void SetMaterial(Entity entity, ResourceManager& resources,
            const std::string& texturePath);

        void SetPrimaryCamera(Entity entity); // 0 clears primary camera
        Entity GetPrimaryCamera() const;
        bool BuildPrimaryCamera(Camera& camera, int width, int height) const;

        DirectX::XMFLOAT3 ambientColor = {1, 1, 1};
        float ambientIntensity = 0.15f;

    private:
        Entity AddDefaults(Entity entity, const std::string& name);
        std::string myName;
        PhysicsSystem myPhysics; // Must outlive registry cleanup.
        Registry myRegistry;
    };
}

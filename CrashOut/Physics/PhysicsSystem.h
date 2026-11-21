#pragma once
#include <box3d/box3d.h>
#include "../Ecs/Registry.h"
#include <vector>
#include <unordered_map>
#include <cstdint>

class Registry;
struct TransformComponent;
struct BoxColliderComponent;

enum class CollisionEventType
{
    Begin,
    End,
    TriggerEnter,
    TriggerExit
};

struct CollisionEvent
{
    Entity entityA;
    Entity entityB;

    CollisionEventType type;
};

class PhysicsSystem
{
public:
    PhysicsSystem();
    ~PhysicsSystem();

    void CreateBoxCollider(TransformComponent& transform, BoxColliderComponent& collider);
    void UpdateTransforms(Registry& ecsRegistry, float timeStep = 1.0f / 60.0f);
    void DestroyBoxCollider(BoxColliderComponent& collider);

    PhysicsSystem(const PhysicsSystem&) = delete;
    PhysicsSystem& operator =(const PhysicsSystem&) = delete;

    const std::vector<CollisionEvent>& GetCollisionEvents() const
    {
        return m_CollisionEvents;
    }

    void EnsureBodies(Registry& registry);

    bool SetHorizontalVelocity(
        Registry& registry,
        Entity entity,
        float x,
        float z
    );

    bool SetAngularVelocity(
        Registry& registry,
        Entity entity,
        b3Vec3 velocity
    );

private:
    void SyncPhysicsToECS(Registry& registry);
    void SyncECSToPhysics(Registry& registry);
    b3WorldId m_worldId = b3_nullWorldId;

    std::vector<CollisionEvent> m_CollisionEvents;
    std::unordered_map<uint64_t, Entity> m_ShapeOwners;
    std::vector<uint64_t> m_RetiredShapes;
    void CollectCollisionEvents();
};
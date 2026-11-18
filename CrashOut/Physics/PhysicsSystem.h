#pragma once
#include <box3d/box3d.h>

class Registry;
struct TransformComponent;
struct BoxColliderComponent;

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
private:
    b3WorldId m_worldId = b3_nullWorldId;
};
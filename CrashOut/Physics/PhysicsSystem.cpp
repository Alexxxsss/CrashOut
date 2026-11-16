#include "PhysicsSystem.h"
#include "../Ecs/Components.h"
#include "../Ecs/Registry.h"

PhysicsSystem::PhysicsSystem()
{
    b3WorldDef worldDef = b3DefaultWorldDef();
    worldDef.gravity = { 0.0f, -9.81f, 0.0f };
    m_worldId = b3CreateWorld(&worldDef);
}

PhysicsSystem::~PhysicsSystem()
{
    if (b3World_IsValid(m_worldId))
    {
        b3DestroyWorld(m_worldId);
        m_worldId = b3_nullWorldId;
    }
}

void PhysicsSystem::CreateBoxCollider(TransformComponent& transform, BoxColliderComponent& collider)
{
    if (collider.isInitialized || !b3World_IsValid(m_worldId))
        return;

    // 1. Create the body
    b3BodyDef bodyDef = b3DefaultBodyDef();
    bodyDef.type = collider.isStatic ? b3_staticBody : b3_dynamicBody;
    bodyDef.position = { transform.position.x, transform.position.y, transform.position.z };

    // Box3D quaternions store xyz in .v and w in .s
    bodyDef.rotation.v = { transform.rotation.x, transform.rotation.y, transform.rotation.z };
    bodyDef.rotation.s = transform.rotation.w;

    collider.bodyId = b3CreateBody(m_worldId, &bodyDef);
    if (!b3Body_IsValid(collider.bodyId))
        return;

    // 2. Create the box shape
    b3Vec3 offset = { collider.localOffset.x, collider.localOffset.y, collider.localOffset.z };
    b3BoxHull boxHull = b3MakeOffsetBoxHull(
        collider.halfExtents.x,
        collider.halfExtents.y,
        collider.halfExtents.z,
        offset
    );

    b3ShapeDef shapeDef = b3DefaultShapeDef();
    shapeDef.baseMaterial.friction = collider.friction;
    shapeDef.baseMaterial.restitution = collider.restitution;
    shapeDef.density = collider.isStatic ? 0.0f : collider.density;
    shapeDef.isSensor = collider.isSensor;

    // Note: pass &boxHull.base (not &boxHull)
    collider.shapeId = b3CreateHullShape(collider.bodyId, &shapeDef, &boxHull.base);
    collider.isInitialized = true;
}

void PhysicsSystem::UpdateTransforms(Registry& ecsRegistry, float timeStep)
{
    if (!b3World_IsValid(m_worldId))
        return;

    // 4 sub-steps is Box3D's recommended default
    b3World_Step(m_worldId, timeStep, 4);

    // Loop over every entity that has a BoxColliderComponent
    auto* colliderPool = ecsRegistry.GetPool<BoxColliderComponent>();
    for (auto& [entity, collider] : colliderPool->data)
    {
        TransformComponent* transform = ecsRegistry.GetComponent<TransformComponent>(entity);
        if (!transform)
            continue;

        // Auto-initialize newly added colliders
        if (!collider.isInitialized)
            CreateBoxCollider(*transform, collider);

        if (!b3Body_IsValid(collider.bodyId) || b3Body_GetType(collider.bodyId) == b3_staticBody)
            continue;

        // Sync Box3D transform back to your ECS TransformComponent
        b3Pos pos = b3Body_GetPosition(collider.bodyId);
        b3Quat rot = b3Body_GetRotation(collider.bodyId);

        transform->position = { pos.x, pos.y, pos.z };
        transform->rotation = { rot.v.x, rot.v.y, rot.v.z, rot.s };
    }
}

void PhysicsSystem::DestroyBoxCollider(BoxColliderComponent& collider)
{
    if (collider.isInitialized && b3Body_IsValid(collider.bodyId))
        b3DestroyBody(collider.bodyId);

    collider.bodyId = b3_nullBodyId;
    collider.shapeId = b3_nullShapeId;
    collider.isInitialized = false;
}
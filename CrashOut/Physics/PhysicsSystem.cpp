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
    if (collider.isInitialized ||
        !b3World_IsValid(m_worldId))
    {
        return;
    }

    // Calculate scaled collider dimensions
    float hx = collider.halfExtents.x *
        std::abs(transform.scale.x);

    float hy = collider.halfExtents.y *
        std::abs(transform.scale.y);

    float hz = collider.halfExtents.z *
        std::abs(transform.scale.z);

    if (!std::isfinite(hx) || hx <= 0.0f ||
        !std::isfinite(hy) || hy <= 0.0f ||
        !std::isfinite(hz) || hz <= 0.0f)
    {
        throw std::runtime_error("Invalid collider size");
    }

    // Scale the collider's local position as well
    b3Vec3 offset = {
        collider.localOffset.x * transform.scale.x,
        collider.localOffset.y * transform.scale.y,
        collider.localOffset.z * transform.scale.z
    };

    // Create the body
    b3BodyDef bodyDef = b3DefaultBodyDef();

    bodyDef.type = collider.isStatic
        ? b3_staticBody
        : b3_dynamicBody;

    bodyDef.position = {
        transform.position.x,
        transform.position.y,
        transform.position.z
    };

    bodyDef.rotation.v = {
        transform.rotation.x,
        transform.rotation.y,
        transform.rotation.z
    };

    bodyDef.rotation.s = transform.rotation.w;

    collider.bodyId = b3CreateBody(m_worldId, &bodyDef);

    if (!b3Body_IsValid(collider.bodyId))
        return;

    // Create the scaled box hull
    b3BoxHull boxHull =
        b3MakeOffsetBoxHull(hx, hy, hz, offset);

    b3ShapeDef shapeDef = b3DefaultShapeDef();

    shapeDef.baseMaterial.friction = collider.friction;
    shapeDef.baseMaterial.restitution = collider.restitution;

    shapeDef.density = collider.isStatic
        ? 0.0f
        : collider.density;

    shapeDef.isSensor = collider.isSensor;
    shapeDef.enableContactEvents = true;
    shapeDef.enableSensorEvents = true;

    collider.shapeId = b3CreateHullShape(
        collider.bodyId,
        &shapeDef,
        &boxHull.base
    );

    if (!b3Shape_IsValid(collider.shapeId))
    {
        b3DestroyBody(collider.bodyId);

        collider.bodyId = b3_nullBodyId;
        collider.shapeId = b3_nullShapeId;

        return;
    }

    collider.isInitialized = true;
}



void PhysicsSystem::UpdateTransforms(Registry& ecsRegistry, float timeStep)
{
    m_CollisionEvents.clear();

    if (!b3World_IsValid(m_worldId))
        return;

    PrepareBodies(ecsRegistry);

    SyncECSToPhysics(ecsRegistry);

    b3World_Step(m_worldId, timeStep, 4);

    SyncPhysicsToECS(ecsRegistry);

    CollectCollisionEvents();
}



void PhysicsSystem::DestroyBoxCollider(BoxColliderComponent& collider)
{
    if (B3_IS_NON_NULL(collider.shapeId))
    {
        m_RetiredShapes.push_back(
            b3StoreShapeId(collider.shapeId)
        );
    }

    if (b3Body_IsValid(collider.bodyId))
    {
        b3DestroyBody(collider.bodyId);
    }

    collider.bodyId = b3_nullBodyId;
    collider.shapeId = b3_nullShapeId;
    collider.isInitialized = false;
}


void PhysicsSystem::PrepareBodies(Registry& registry)
{
    auto* pool = registry.GetPool<BoxColliderComponent>();

    if (!pool)
        return;

    for (auto& [entity, collider] : pool->data)
    {
        auto* transform =
            registry.GetComponent<TransformComponent>(entity);

        if (!transform)
            continue;

        if (!collider.isInitialized)
        {
            CreateBoxCollider(*transform, collider);
        }

        if (b3Shape_IsValid(collider.shapeId))
        {
            m_ShapeOwners[b3StoreShapeId(collider.shapeId)] = entity;
        }
    }
}


void PhysicsSystem::SyncPhysicsToECS(Registry& registry)
{
    auto* pool = registry.GetPool<BoxColliderComponent>();

    if (!pool)
        return;

    for (auto& [entity, collider] : pool->data)
    {
        if (!b3Body_IsValid(collider.bodyId))
            continue;

        if (b3Body_GetType(collider.bodyId) != b3_dynamicBody)
            continue;

        auto* transform =
            registry.GetComponent<TransformComponent>(entity);

        if (!transform)
            continue;

        b3Pos position =
            b3Body_GetPosition(collider.bodyId);

        b3Quat rotation =
            b3Body_GetRotation(collider.bodyId);

        transform->position = {
            static_cast<float>(position.x),
            static_cast<float>(position.y),
            static_cast<float>(position.z)
        };

        transform->rotation = {
            rotation.v.x,
            rotation.v.y,
            rotation.v.z,
            rotation.s
        };
    }
}


void PhysicsSystem::SyncECSToPhysics(Registry& registry)
{
    auto* pool = registry.GetPool<BoxColliderComponent>();

    if (!pool)
        return;

    for (auto& [entity, collider] : pool->data)
    {
        if (!b3Body_IsValid(collider.bodyId))
            continue;

        if (b3Body_GetType(collider.bodyId) != b3_staticBody)
            continue;

        auto* transform =
            registry.GetComponent<TransformComponent>(entity);

        if (!transform)
            continue;

        b3Pos oldPosition =
            b3Body_GetPosition(collider.bodyId);

        b3Quat oldRotation =
            b3Body_GetRotation(collider.bodyId);

        b3Pos newPosition = {
            transform->position.x,
            transform->position.y,
            transform->position.z
        };

        b3Quat newRotation;

        newRotation.v = {
            transform->rotation.x,
            transform->rotation.y,
            transform->rotation.z
        };

        newRotation.s = transform->rotation.w;

        constexpr float epsilon = 0.00001f;

        bool positionChanged =
            std::abs(oldPosition.x - newPosition.x) > epsilon ||
            std::abs(oldPosition.y - newPosition.y) > epsilon ||
            std::abs(oldPosition.z - newPosition.z) > epsilon;

        bool rotationChanged =
            std::abs(oldRotation.v.x - newRotation.v.x) > epsilon ||
            std::abs(oldRotation.v.y - newRotation.v.y) > epsilon ||
            std::abs(oldRotation.v.z - newRotation.v.z) > epsilon ||
            std::abs(oldRotation.s - newRotation.s) > epsilon;

        if (positionChanged || rotationChanged)
        {
            b3Body_SetTransform(
                collider.bodyId,
                newPosition,
                newRotation
            );
        }
    }
}

void PhysicsSystem::CollectCollisionEvents()
{
    auto addEvent = [this](
        b3ShapeId shapeA,
        b3ShapeId shapeB,
        CollisionEventType type)
        {
            auto a = m_ShapeOwners.find(
                b3StoreShapeId(shapeA)
            );

            auto b = m_ShapeOwners.find(
                b3StoreShapeId(shapeB)
            );

            if (a == m_ShapeOwners.end() ||
                b == m_ShapeOwners.end())
            {
                return;
            }

            m_CollisionEvents.push_back({
                a->second,
                b->second,
                type
                });
        };

    // Regular collision events
    b3ContactEvents contacts =
        b3World_GetContactEvents(m_worldId);

    for (int i = 0; i < contacts.beginCount; i++)
    {
        const auto& event = contacts.beginEvents[i];

        addEvent(
            event.shapeIdA,
            event.shapeIdB,
            CollisionEventType::Begin
        );
    }

    for (int i = 0; i < contacts.endCount; i++)
    {
        const auto& event = contacts.endEvents[i];

        addEvent(
            event.shapeIdA,
            event.shapeIdB,
            CollisionEventType::End
        );
    }

    // Sensor events
    b3SensorEvents sensors =
        b3World_GetSensorEvents(m_worldId);

    for (int i = 0; i < sensors.beginCount; i++)
    {
        const auto& event = sensors.beginEvents[i];

        addEvent(
            event.sensorShapeId,
            event.visitorShapeId,
            CollisionEventType::TriggerEnter
        );
    }

    for (int i = 0; i < sensors.endCount; i++)
    {
        const auto& event = sensors.endEvents[i];

        addEvent(
            event.sensorShapeId,
            event.visitorShapeId,
            CollisionEventType::TriggerExit
        );
    }

    // Remove retired shape mappings
    for (uint64_t shape : m_RetiredShapes)
    {
        m_ShapeOwners.erase(shape);
    }

    m_RetiredShapes.clear();
}
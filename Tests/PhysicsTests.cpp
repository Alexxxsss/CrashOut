#include "TestSupport.h"
#include "Physics/PhysicsSystem.h"
#include "Ecs/Components.h"
#include <iostream>
#include <string>

// No Application or DirectX window needed.
struct PhysicsFixture
{
    PhysicsSystem physics; // Declare before registry so it outlives it.
    Registry registry;

    PhysicsFixture()
    {
        // Destroy physics body when a collider is removed.
        registry.SetOnRemove<BoxColliderComponent>(
            [this](Entity entity, BoxColliderComponent& collider)
            {
                physics.DestroyBoxCollider(collider);
            }
        );

        // If a transform is removed, remove its collider too.
        registry.SetOnRemove<TransformComponent>(
            [this](Entity entity, TransformComponent&)
            {
                registry.RemoveComponent<BoxColliderComponent>(
                    entity
                );
            }
        );
    }

    Entity CreateBox()
    {
        Entity e = registry.CreateEntity();
        auto& transform = registry.AddComponent<TransformComponent>(e);
        transform.position = { 0.0f, 4.0f, 0.0f };
        registry.AddComponent<BoxColliderComponent>(e);

        // Works with either PrepareBodies (original) or EnsureBodies (Part 2).
        physics.UpdateTransforms(registry, 1.0f / 60.0f);
        auto* collider = registry.GetComponent<BoxColliderComponent>(e);
        CHECK(collider != nullptr);
        CHECK(collider->isInitialized);
        CHECK(b3Body_IsValid(collider->bodyId));
        CHECK(b3Shape_IsValid(collider->shapeId));
        return e;
    }
};

void TestDestroyEntity()
{
    PhysicsFixture fixture;
    Entity e = fixture.CreateBox();
    auto* collider = fixture.registry.GetComponent<BoxColliderComponent>(e);
    b3BodyId oldBody = collider->bodyId;
    b3ShapeId oldShape = collider->shapeId;

    fixture.registry.DestroyEntity(e);
    CHECK(!fixture.registry.HasEntity(e));
    CHECK(!b3Body_IsValid(oldBody));
    CHECK(!b3Shape_IsValid(oldShape));
    fixture.registry.DestroyEntity(e); // Idempotent.
}

void TestRemoveComponents()
{
    PhysicsFixture fixture;
    Entity a = fixture.CreateBox();
    b3BodyId bodyA = fixture.registry.GetComponent<BoxColliderComponent>(a)->bodyId;

    CHECK(fixture.registry.RemoveComponent<BoxColliderComponent>(a));
    CHECK(!b3Body_IsValid(bodyA));
    CHECK(fixture.registry.HasComponent<TransformComponent>(a));
    CHECK(!fixture.registry.HasComponent<BoxColliderComponent>(a));

    Entity b = fixture.CreateBox();
    b3BodyId bodyB = fixture.registry.GetComponent<BoxColliderComponent>(b)->bodyId;
    CHECK(fixture.registry.RemoveComponent<TransformComponent>(b));
    CHECK(!fixture.registry.HasComponent<BoxColliderComponent>(b));
    CHECK(!b3Body_IsValid(bodyB));
}

void TestStress()
{
    PhysicsFixture fixture;
    for (int i = 0; i < 200; ++i)
    {
        Entity e = fixture.CreateBox();
        b3BodyId oldBody = fixture.registry.GetComponent<BoxColliderComponent>(e)->bodyId;
        fixture.registry.DestroyEntity(e);
        CHECK(!fixture.registry.HasEntity(e));
        CHECK(!b3Body_IsValid(oldBody));
    }
}

int main(int argc, char** argv)
{
    try
    {
        CHECK(argc == 2);
        std::string which = argv[1];
        if (which == "destroy-entity") TestDestroyEntity();
        else if (which == "remove-components") TestRemoveComponents();
        else if (which == "stress") TestStress();
        else throw std::runtime_error("Unknown physics test: " + which);
        std::cout << "PASS: " << which << '\n';
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "FAIL: " << ex.what() << '\n';
        return 1;
    }
}

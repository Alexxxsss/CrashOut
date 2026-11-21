#include "TestSupport.h"
#include "Ecs/Registry.h"
#include <iostream>
#include <string>

struct Health { int points = 0; };

void TestEntities()
{
    Registry registry;
    Entity a = registry.CreateEntity();
    Entity b = registry.CreateEntity();

    CHECK(a != b);
    CHECK(registry.HasEntity(a));
    CHECK(registry.HasEntity(b));

    registry.DestroyEntity(a);
    CHECK(!registry.HasEntity(a));
    CHECK(registry.HasEntity(b));
    registry.DestroyEntity(a); // Double destruction must be harmless.

    bool rejected = false;
    try { registry.AddComponent<Health>(a, 10); }
    catch (const std::runtime_error&) { rejected = true; }
    CHECK(rejected);
}

void TestComponents()
{
    Registry registry;
    Entity e = registry.CreateEntity();
    CHECK(registry.GetComponent<Health>(e) == nullptr);

    registry.AddComponent<Health>(e, 42);
    CHECK(registry.HasComponent<Health>(e));
    CHECK(registry.GetComponent<Health>(e)->points == 42);

    bool duplicateRejected = false;
    try { registry.AddComponent<Health>(e, 99); }
    catch (const std::runtime_error&) { duplicateRejected = true; }
    CHECK(duplicateRejected);
    CHECK(registry.GetComponent<Health>(e)->points == 42);

    CHECK(registry.RemoveComponent<Health>(e));
    CHECK(!registry.RemoveComponent<Health>(e));
    CHECK(registry.GetComponent<Health>(e) == nullptr);
}

void TestCallbacks()
{
    Registry registry;
    int calls = 0;
    int sum = 0;

    registry.SetOnRemove<Health>(
        [&](Entity entity, Health& health)
        {
            CHECK(registry.HasEntity(entity));
            ++calls;
            sum += health.points;
        });

    Entity a = registry.CreateEntity();
    registry.AddComponent<Health>(a, 10);
    CHECK(registry.RemoveComponent<Health>(a));
    CHECK(!registry.RemoveComponent<Health>(a));
    CHECK(calls == 1 && sum == 10);

    Entity b = registry.CreateEntity();
    registry.AddComponent<Health>(b, 20);
    registry.DestroyEntity(b);
    CHECK(calls == 2 && sum == 30);
    registry.DestroyEntity(b);
    CHECK(calls == 2);

    Entity c = registry.CreateEntity();
    registry.AddComponent<Health>(c, 30);
    registry.Clear();
    CHECK(calls == 3 && sum == 60);
    CHECK(!registry.HasEntity(c));
}

int main(int argc, char** argv)
{
    try
    {
        CHECK(argc == 2);
        std::string which = argv[1];
        if (which == "entities") TestEntities();
        else if (which == "components") TestComponents();
        else if (which == "callbacks") TestCallbacks();
        else throw std::runtime_error("Unknown ECS test: " + which);
        std::cout << "PASS: " << which << '\n';
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "FAIL: " << ex.what() << '\n';
        return 1;
    }
}

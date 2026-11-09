// Ecs/Registry.h
#pragma once
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cstdint>

using Entity = uint32_t;

class IComponentPool
{
public:
    virtual ~IComponentPool() = default;
};

template<typename T>
class ComponentPool : public IComponentPool
{
public:
    std::unordered_map<Entity, T> data;
};

class Registry
{
public:
    Entity CreateEntity()
    {
        return m_NextEntityId++;
    }

    template<typename T, typename... Args>
    T& AddComponent(Entity entity, Args&&... args)
    {
        auto pool = GetOrCreatePool<T>();
        pool->data[entity] = T{ std::forward<Args>(args)... };
        return pool->data[entity];
    }

    template<typename T>
    T* GetComponent(Entity entity)
    {
        auto pool = GetOrCreatePool<T>();
        auto it = pool->data.find(entity);
        return (it != pool->data.end()) ? &it->second : nullptr;
    }

    template<typename T>
    bool HasComponent(Entity entity)
    {
        auto pool = GetOrCreatePool<T>();
        return pool->data.find(entity) != pool->data.end();
    }

    template<typename T>
    ComponentPool<T>* GetPool()
    {
        return GetOrCreatePool<T>();
    }

private:
    Entity m_NextEntityId = 1;
    std::unordered_map<std::type_index, std::shared_ptr<IComponentPool>> m_Pools;

    template<typename T>
    ComponentPool<T>* GetOrCreatePool()
    {
        std::type_index type = typeid(T);
        if (m_Pools.find(type) == m_Pools.end())
        {
            m_Pools[type] = std::make_shared<ComponentPool<T>>();
        }
        return static_cast<ComponentPool<T>*>(m_Pools[type].get());
    }
};
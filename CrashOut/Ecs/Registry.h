// Ecs/Registry.h
#pragma once
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cstdint>
#include <unordered_set>
#include <stdexcept>
#include <utility>
#include <functional>
#include <limits>
#include <algorithm>

using Entity = uint32_t;

class IComponentPool
{
public:
    virtual ~IComponentPool() = default;

    virtual void RemoveEntity(Entity entity) = 0;
    virtual bool HasEntity(Entity entity) const = 0;

};


template<typename T>
class ComponentPool : public IComponentPool
{
public:
    std::unordered_map<Entity, T> data;

    std::function<void(Entity, T&)> onRemove;

    void RemoveEntity(Entity entity) override
    {
        auto it = data.find(entity);

        if (it == data.end())
        {
            return;
        }

        if (onRemove)
        {
            onRemove(entity, it->second);
        }

        data.erase(it);
    }

    bool HasEntity(Entity entity) const override
    {
        return data.find(entity) != data.end();
    }
};

class Registry
{
public:
    Registry() = default;

    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;


    std::vector<std::type_index> GetComponentTypes(Entity entity) const
    {
        std::vector<std::type_index> types;

        for (const auto& [type, pool] : m_Pools)
        {
            if (pool->HasEntity(entity))
            {
                types.push_back(type);
            }
        }

        return types;
    }

    Entity CreateEntity()
    {
        if (m_NextEntityId > (std::numeric_limits<Entity>::max)())
            throw std::overflow_error("Entity ID space exhausted");
        return CreateEntity(static_cast<Entity>(m_NextEntityId));
    }

    Entity CreateEntity(Entity id)
    {
        if (id == 0 || !m_AliveEntities.insert(id).second)
            throw std::runtime_error("Invalid or duplicate entity ID");
        m_NextEntityId = (std::max)(m_NextEntityId,
            static_cast<uint64_t>(id) + 1);
        return id;
    }

    std::vector<Entity> GetEntities() const
    {
        std::vector<Entity> entities(m_AliveEntities.begin(), m_AliveEntities.end());
        std::sort(entities.begin(), entities.end());
        return entities;
    }

    bool HasEntity(Entity entity) const
    {
        return m_AliveEntities.find(entity) != m_AliveEntities.end();
    }

    void DestroyEntity(Entity entity)
    {
        if (!HasEntity(entity))
        {
            return;
        }

        for (auto& [type, pool] : m_Pools)
        {
            pool->RemoveEntity(entity);
        }

        m_AliveEntities.erase(entity);
    }

    template<typename T, typename... Args>
    T& AddComponent(Entity entity, Args&&... args)
    {
        if (!HasEntity(entity))
        {
            throw std::runtime_error("Invalid entity");
        }

        auto pool = GetOrCreatePool<T>();

        auto [it, inserted] = pool->data.try_emplace(entity, T{ std::forward<Args>(args)... });

        if (!inserted)
        {
            throw std::runtime_error(
                "Entity already has this component"
            );
        }

        return it->second;
    }

    template<typename T>
    T* GetComponent(Entity entity)
    {
        if (!HasEntity(entity))
        {
            return nullptr;
        }

        auto* pool = GetPool<T>();

        if (!pool)
        {
            return nullptr;
        }

        auto it = pool->data.find(entity);

        if (it == pool->data.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    template<typename T>
    const T* GetComponent(Entity entity) const
    {
        if (!HasEntity(entity)) return nullptr;
        auto* pool = GetPool<T>();
        if (!pool) return nullptr;
        auto it = pool->data.find(entity);
        return it == pool->data.end() ? nullptr : &it->second;
    }

    template<typename T>
    bool HasComponent(Entity entity) const
    {
        return GetComponent<T>(entity) != nullptr;
    }

    template<typename T>
    bool RemoveComponent(Entity entity)
    {
        if (!HasEntity(entity))
        {
            return false;
        }

        auto* pool = GetPool<T>();

        if (!pool)
        {
            return false;
        }

        if (pool->data.find(entity) == pool->data.end())
        {
            return false;
        }

        pool->RemoveEntity(entity);

        return true;
    }


    bool RemoveComponent(Entity entity, std::type_index type)
    {
        if (!HasEntity(entity))
            return false;

        auto it = m_Pools.find(type);

        if (it == m_Pools.end())
            return false;

        if (!it->second->HasEntity(entity))
            return false;

        it->second->RemoveEntity(entity);

        return true;
    }


    template<typename T>
    ComponentPool<T>* GetPool()
    {
        auto it = m_Pools.find(std::type_index(typeid(T)));

        if (it == m_Pools.end())
        {
            return nullptr;
        }

        return static_cast<ComponentPool<T>*>(it->second.get());
    }

    template<typename T>
    const ComponentPool<T>* GetPool() const
    {
        auto it = m_Pools.find(std::type_index(typeid(T)));
        return it == m_Pools.end() ? nullptr :
            static_cast<const ComponentPool<T>*>(it->second.get());
    }

    template<typename T>
    void SetOnRemove(std::function<void(Entity, T&)> callback)
    {
        auto* pool = GetOrCreatePool<T>();

        pool->onRemove = std::move(callback);
    }

    void Clear()
    {
        std::vector<Entity> entities(m_AliveEntities.begin(), m_AliveEntities.end());

        for (Entity entity : entities)
        {
            DestroyEntity(entity);
        }
    }

private:
    uint64_t m_NextEntityId = 1;
    std::unordered_set<Entity> m_AliveEntities;

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

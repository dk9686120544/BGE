#pragma once

#include "core/Assert.h"
#include "core/Types.h"
#include "ecs/Entity.h"
#include "ecs/EntityManager.h"

#include <vector>

namespace bge
{
    template <typename T>
    class ComponentStorage
    {
    public:
        ComponentStorage() = default;

        void EnsureCapacity(u32 capacity)
        {
            const usize requiredSize = static_cast<usize>(capacity);

            if (requiredSize <= m_Data.size())
            {
                return;
            }

            m_Data.resize(requiredSize);
            m_HasComponent.resize(requiredSize, false);
        }

        bool Has(Entity entity, const EntityManager& entityManager) const noexcept
        {
            if (!entityManager.IsAlive(entity))
            {
                return false;
            }

            if (entity.index >= m_HasComponent.size())
            {
                return false;
            }

            return m_HasComponent[entity.index];
        }

        T* TryGet(Entity entity, const EntityManager& entityManager) noexcept
        {
            if (!Has(entity, entityManager))
            {
                return nullptr;
            }

            return &m_Data[entity.index];
        }

        const T* TryGet(Entity entity, const EntityManager& entityManager) const noexcept
        {
            if (!Has(entity, entityManager))
            {
                return nullptr;
            }

            return &m_Data[entity.index];
        }

        T& Add(Entity entity, const EntityManager& entityManager)
        {
            BGE_ASSERT(entityManager.IsAlive(entity));

            EnsureCapacity(entity.index + 1);

            m_HasComponent[entity.index] = true;
            return m_Data[entity.index];
        }

        void Remove(Entity entity) noexcept
        {
            if (!entity.IsValid())
            {
                return;
            }

            if (entity.index >= m_HasComponent.size())
            {
                return;
            }

            m_HasComponent[entity.index] = false;
        }

        void Clear() noexcept
        {
            m_Data.clear();
            m_HasComponent.clear();
        }

        void RemoveDestroyed(const EntityManager& entityManager) noexcept
        {
            const u32 capacity = static_cast<u32>(m_HasComponent.size());

            for (u32 index = 0; index < capacity; ++index)
            {
                if (!m_HasComponent[index])
                {
                    continue;
                }

                if (!entityManager.Exists(index))
                {
                    m_HasComponent[index] = false;
                    continue;
                }

                if (!entityManager.AliveMask()[index])
                {
                    m_HasComponent[index] = false;
                }
            }
        }

        u32 Capacity() const noexcept
        {
            return static_cast<u32>(m_Data.size());
        }

        bool HasIndex(u32 index) const noexcept
        {
            if (index >= m_HasComponent.size())
            {
                return false;
            }

            return m_HasComponent[index];
        }

        T& GetByIndex(u32 index) noexcept
        {
            BGE_ASSERT(index < m_Data.size());
            BGE_ASSERT(index < m_HasComponent.size());
            BGE_ASSERT(m_HasComponent[index]);

            return m_Data[index];
        }

        const T& GetByIndex(u32 index) const noexcept
        {
            BGE_ASSERT(index < m_Data.size());
            BGE_ASSERT(index < m_HasComponent.size());
            BGE_ASSERT(m_HasComponent[index]);

            return m_Data[index];
        }

    private:
        std::vector<T> m_Data;
        std::vector<bool> m_HasComponent;
    };
}
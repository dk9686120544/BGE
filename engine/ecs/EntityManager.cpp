#include "ecs/EntityManager.h"

#include "core/Assert.h"

namespace bge
{
    Entity EntityManager::Create()
    {
        const u32 index = AllocateIndex();

        EnsureCapacity(index);

        BGE_ASSERT(index < m_Generations.size());
        BGE_ASSERT(index < m_Alive.size());
        BGE_ASSERT(index < m_PendingDestroy.size());
        BGE_ASSERT(!m_Alive[index]);
        BGE_ASSERT(!m_PendingDestroy[index]);

        m_Alive[index] = true;
        ++m_AliveCount;

        return Entity(index, m_Generations[index]);
    }

    void EntityManager::MarkForDestroy(Entity entity)
    {
        if (!IsAlive(entity))
        {
            return;
        }

        BGE_ASSERT(entity.index < m_PendingDestroy.size());

        if (m_PendingDestroy[entity.index])
        {
            return;
        }

        m_PendingDestroy[entity.index] = true;
        m_DeferredDestroyQueue.push_back(entity);
    }

    void EntityManager::ProcessDeferredDestroy()
    {
        for (const Entity entity : m_DeferredDestroyQueue)
        {
            if (entity.index >= m_Generations.size())
            {
                continue;
            }

            if (!m_Alive[entity.index])
            {
                continue;
            }

            if (m_Generations[entity.index] != entity.generation)
            {
                continue;
            }

            m_Alive[entity.index] = false;
            m_PendingDestroy[entity.index] = false;
            ++m_Generations[entity.index];
            m_FreeList.push_back(entity.index);

            BGE_ASSERT(m_AliveCount > 0);
            --m_AliveCount;
        }

        m_DeferredDestroyQueue.clear();
    }

    bool EntityManager::IsAlive(Entity entity) const noexcept
    {
        if (!entity.IsValid())
        {
            return false;
        }

        if (!Exists(entity.index))
        {
            return false;
        }

        if (!m_Alive[entity.index])
        {
            return false;
        }

        return m_Generations[entity.index] == entity.generation;
    }

    bool EntityManager::Exists(u32 index) const noexcept
    {
        return index < m_Generations.size();
    }

    u32 EntityManager::Capacity() const noexcept
    {
        return static_cast<u32>(m_Generations.size());
    }

    u32 EntityManager::AliveCount() const noexcept
    {
        return m_AliveCount;
    }

    const std::vector<bool>& EntityManager::AliveMask() const noexcept
    {
        return m_Alive;
    }

    const std::vector<Entity>& EntityManager::DeferredDestroyQueue() const noexcept
    {
        return m_DeferredDestroyQueue;
    }

    u32 EntityManager::AllocateIndex()
    {
        if (m_FreeList.empty())
        {
            return static_cast<u32>(m_Generations.size());
        }

        usize bestPosition = 0;
        u32 bestIndex = m_FreeList[0];

        for (usize i = 1; i < m_FreeList.size(); ++i)
        {
            if (m_FreeList[i] < bestIndex)
            {
                bestIndex = m_FreeList[i];
                bestPosition = i;
            }
        }

        m_FreeList.erase(m_FreeList.begin() + static_cast<std::ptrdiff_t>(bestPosition));
        return bestIndex;
    }

    void EntityManager::EnsureCapacity(u32 index)
    {
        const usize requiredSize = static_cast<usize>(index) + 1;

        if (requiredSize <= m_Generations.size())
        {
            return;
        }

        m_Generations.resize(requiredSize, 0);
        m_Alive.resize(requiredSize, false);
        m_PendingDestroy.resize(requiredSize, false);
    }
}
#pragma once

#include "core/Types.h"
#include "ecs/Entity.h"

#include <vector>

namespace bge
{
    class EntityManager
    {
    public:
        EntityManager() = default;

        Entity Create();
        void MarkForDestroy(Entity entity);
        void ProcessDeferredDestroy();

        bool IsAlive(Entity entity) const noexcept;
        bool Exists(u32 index) const noexcept;

        u32 Capacity() const noexcept;
        u32 AliveCount() const noexcept;

        const std::vector<bool>& AliveMask() const noexcept;
        const std::vector<Entity>& DeferredDestroyQueue() const noexcept;

    private:
        u32 AllocateIndex();
        void EnsureCapacity(u32 index);

    private:
        std::vector<u32> m_Generations;
        std::vector<bool> m_Alive;
        std::vector<bool> m_PendingDestroy;
        std::vector<u32> m_FreeList;
        std::vector<Entity> m_DeferredDestroyQueue;

        u32 m_AliveCount = 0;
    };
}
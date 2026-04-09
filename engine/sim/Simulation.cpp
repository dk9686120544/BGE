#include "sim/Simulation.h"

namespace bge
{
    void Simulation::EnqueueCommand(std::unique_ptr<Command> command)
    {
        m_NextCommands.Push(std::move(command));
    }

    void Simulation::Tick()
    {
        m_CurrentCommands.Clear();

        std::vector<std::unique_ptr<Command>>& nextItems = m_NextCommands.Items();
        std::vector<std::unique_ptr<Command>>& currentItems = m_CurrentCommands.Items();

        currentItems.swap(nextItems);
        nextItems.clear();

        m_EntityManager.ProcessDeferredDestroy();

        UpdateStateHash();
        m_TickClock.Advance();
    }

    u32 Simulation::CurrentTick() const noexcept
    {
        return m_TickClock.CurrentTick();
    }

    u64 Simulation::CurrentStateHash() const noexcept
    {
        return m_CurrentStateHash;
    }

    EntityManager& Simulation::Entities() noexcept
    {
        return m_EntityManager;
    }

    const EntityManager& Simulation::Entities() const noexcept
    {
        return m_EntityManager;
    }

    CommandQueue& Simulation::CurrentCommands() noexcept
    {
        return m_CurrentCommands;
    }

    const CommandQueue& Simulation::CurrentCommands() const noexcept
    {
        return m_CurrentCommands;
    }

    CommandQueue& Simulation::NextCommands() noexcept
    {
        return m_NextCommands;
    }

    const CommandQueue& Simulation::NextCommands() const noexcept
    {
        return m_NextCommands;
    }

    void Simulation::UpdateStateHash()
    {
        m_StateHash.Reset();
        m_StateHash.AddU32(m_TickClock.CurrentTick());

        const std::vector<bool>& aliveMask = m_EntityManager.AliveMask();
        const u32 capacity = m_EntityManager.Capacity();

        for (u32 index = 0; index < capacity; ++index)
        {
            const bool alive = (index < aliveMask.size()) ? aliveMask[index] : false;

            m_StateHash.AddU32(index);
            m_StateHash.AddBool(alive);
        }

        m_CurrentStateHash = m_StateHash.Value();
    }
}
#pragma once

#include "ecs/EntityManager.h"
#include "sim/CommandQueue.h"
#include "sim/StateHash.h"
#include "sim/TickClock.h"

namespace bge
{
    class Simulation
    {
    public:
        Simulation() = default;

        void EnqueueCommand(std::unique_ptr<Command> command);
        void Tick();

        u32 CurrentTick() const noexcept;
        u64 CurrentStateHash() const noexcept;

        EntityManager& Entities() noexcept;
        const EntityManager& Entities() const noexcept;

        CommandQueue& CurrentCommands() noexcept;
        const CommandQueue& CurrentCommands() const noexcept;

        CommandQueue& NextCommands() noexcept;
        const CommandQueue& NextCommands() const noexcept;

    private:
        void UpdateStateHash();

    private:
        TickClock m_TickClock;
        EntityManager m_EntityManager;
        CommandQueue m_CurrentCommands;
        CommandQueue m_NextCommands;
        StateHash m_StateHash;
        u64 m_CurrentStateHash = 0;
    };
}
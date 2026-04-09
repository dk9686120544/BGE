#pragma once

#include "core/Types.h"

namespace bge
{
    class TickClock
    {
    public:
        static constexpr u32 TicksPerSecond = 20;
        static constexpr u32 MillisecondsPerTick = 50;

    public:
        TickClock() noexcept = default;

        u32 CurrentTick() const noexcept
        {
            return m_CurrentTick;
        }

        void Advance() noexcept
        {
            ++m_CurrentTick;
        }

        void Reset() noexcept
        {
            m_CurrentTick = 0;
        }

    private:
        u32 m_CurrentTick = 0;
    };
}
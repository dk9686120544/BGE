#pragma once

#include "core/Types.h"

namespace bge
{
    class Random
    {
    public:
        Random() noexcept;
        Random(u64 seed, u64 sequence) noexcept;

        void Seed(u64 seed, u64 sequence) noexcept;

        u32 NextU32() noexcept;
        u32 NextU32(u32 bound) noexcept;

        u64 State() const noexcept;
        u64 Increment() const noexcept;

    private:
        u64 m_State;
        u64 m_Increment;
    };
}
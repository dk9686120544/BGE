#include "core/Random.h"

namespace bge
{
    namespace
    {
        constexpr u64 kMultiplier = 6364136223846793005ULL;
    }

    Random::Random() noexcept
        : m_State(0)
        , m_Increment(0)
    {
    }

    Random::Random(u64 seed, u64 sequence) noexcept
        : m_State(0)
        , m_Increment(0)
    {
        Seed(seed, sequence);
    }

    void Random::Seed(u64 seed, u64 sequence) noexcept
    {
        m_State = 0;
        m_Increment = (sequence << 1u) | 1u;

        NextU32();
        m_State += seed;
        NextU32();
    }

    u32 Random::NextU32() noexcept
    {
        const u64 oldState = m_State;
        m_State = oldState * kMultiplier + m_Increment;

        const u32 xorshifted =
            static_cast<u32>(((oldState >> 18u) ^ oldState) >> 27u);

        const u32 rotation = static_cast<u32>(oldState >> 59u);
        const u32 right = rotation & 31u;
        const u32 left = (32u - right) & 31u;

        return (xorshifted >> right) | (xorshifted << left);
    }

    u32 Random::NextU32(u32 bound) noexcept
    {
        if (bound == 0u)
        {
            return 0u;
        }

        const u32 threshold = static_cast<u32>(0u - bound) % bound;

        for (;;)
        {
            const u32 value = NextU32();
            if (value >= threshold)
            {
                return value % bound;
            }
        }
    }

    u64 Random::State() const noexcept
    {
        return m_State;
    }

    u64 Random::Increment() const noexcept
    {
        return m_Increment;
    }
}
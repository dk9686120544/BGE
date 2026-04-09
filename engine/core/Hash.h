#pragma once

#include "core/Types.h"

namespace bge
{
    class Hasher64
    {
    public:
        static constexpr u64 kOffsetBasis = 14695981039346656037ull;
        static constexpr u64 kPrime = 1099511628211ull;

    public:
        constexpr Hasher64() noexcept
            : m_Value(kOffsetBasis)
        {
        }

        constexpr void Reset() noexcept
        {
            m_Value = kOffsetBasis;
        }

        constexpr void AddBytes(const void* data, usize size) noexcept
        {
            const u8* bytes = static_cast<const u8*>(data);

            for (usize i = 0; i < size; ++i)
            {
                m_Value ^= static_cast<u64>(bytes[i]);
                m_Value *= kPrime;
            }
        }

        constexpr void AddU8(u8 value) noexcept
        {
            AddBytes(&value, sizeof(value));
        }

        constexpr void AddU16(u16 value) noexcept
        {
            AddBytes(&value, sizeof(value));
        }

        constexpr void AddU32(u32 value) noexcept
        {
            AddBytes(&value, sizeof(value));
        }

        constexpr void AddU64(u64 value) noexcept
        {
            AddBytes(&value, sizeof(value));
        }

        constexpr void AddI8(i8 value) noexcept
        {
            AddBytes(&value, sizeof(value));
        }

        constexpr void AddI16(i16 value) noexcept
        {
            AddBytes(&value, sizeof(value));
        }

        constexpr void AddI32(i32 value) noexcept
        {
            AddBytes(&value, sizeof(value));
        }

        constexpr void AddI64(i64 value) noexcept
        {
            AddBytes(&value, sizeof(value));
        }

        constexpr u64 Value() const noexcept
        {
            return m_Value;
        }

    private:
        u64 m_Value;
    };
}
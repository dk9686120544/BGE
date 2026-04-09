#pragma once

#include "core/Assert.h"
#include "core/Types.h"

#include <limits>
#include <intrin.h>

namespace bge
{
    class Scalar
    {
    public:
        static constexpr i32 FractionBits = 16;
        static constexpr i64 OneRaw = 1LL << FractionBits;
        static constexpr i64 HalfRaw = 1LL << (FractionBits - 1);

    public:
        constexpr Scalar() noexcept
            : m_Raw(0)
        {
        }

        explicit constexpr Scalar(i64 rawValue) noexcept
            : m_Raw(rawValue)
        {
        }

        static constexpr Scalar FromRaw(i64 rawValue) noexcept
        {
            return Scalar(rawValue);
        }

        static constexpr Scalar FromInt(i32 value) noexcept
        {
            return Scalar(static_cast<i64>(value) << FractionBits);
        }

        static constexpr Scalar Zero() noexcept
        {
            return Scalar(0);
        }

        static constexpr Scalar One() noexcept
        {
            return Scalar(OneRaw);
        }

        static constexpr Scalar Half() noexcept
        {
            return Scalar(HalfRaw);
        }

        constexpr i64 Raw() const noexcept
        {
            return m_Raw;
        }

        constexpr bool IsZero() const noexcept
        {
            return m_Raw == 0;
        }

        constexpr Scalar operator+() const noexcept
        {
            return *this;
        }

        constexpr Scalar operator-() const noexcept
        {
            BGE_ASSERT(m_Raw != std::numeric_limits<i64>::min());
            return Scalar(-m_Raw);
        }

        constexpr Scalar operator+(Scalar other) const noexcept
        {
            return Scalar(m_Raw + other.m_Raw);
        }

        constexpr Scalar operator-(Scalar other) const noexcept
        {
            return Scalar(m_Raw - other.m_Raw);
        }

        Scalar operator*(Scalar other) const noexcept;
        Scalar operator/(Scalar other) const noexcept;

        Scalar& operator+=(Scalar other) noexcept
        {
            m_Raw += other.m_Raw;
            return *this;
        }

        Scalar& operator-=(Scalar other) noexcept
        {
            m_Raw -= other.m_Raw;
            return *this;
        }

        Scalar& operator*=(Scalar other) noexcept
        {
            *this = (*this * other);
            return *this;
        }

        Scalar& operator/=(Scalar other) noexcept
        {
            *this = (*this / other);
            return *this;
        }

        constexpr bool operator==(Scalar other) const noexcept
        {
            return m_Raw == other.m_Raw;
        }

        constexpr bool operator!=(Scalar other) const noexcept
        {
            return m_Raw != other.m_Raw;
        }

        constexpr bool operator<(Scalar other) const noexcept
        {
            return m_Raw < other.m_Raw;
        }

        constexpr bool operator<=(Scalar other) const noexcept
        {
            return m_Raw <= other.m_Raw;
        }

        constexpr bool operator>(Scalar other) const noexcept
        {
            return m_Raw > other.m_Raw;
        }

        constexpr bool operator>=(Scalar other) const noexcept
        {
            return m_Raw >= other.m_Raw;
        }

        static constexpr Scalar Min(Scalar a, Scalar b) noexcept
        {
            return (a.m_Raw < b.m_Raw) ? a : b;
        }

        static constexpr Scalar Max(Scalar a, Scalar b) noexcept
        {
            return (a.m_Raw > b.m_Raw) ? a : b;
        }

        static Scalar Abs(Scalar value) noexcept;

        constexpr i32 TruncToInt() const noexcept
        {
            return static_cast<i32>(m_Raw / OneRaw);
        }

        float ToFloat() const noexcept
        {
            return static_cast<float>(m_Raw) / static_cast<float>(OneRaw);
        }

    private:
        static bool IsInt64Min(i64 value) noexcept;
        static u64 AbsToUnsigned(i64 value) noexcept;

    private:
        i64 m_Raw;
    };
}
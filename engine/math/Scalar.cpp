#include "math/Scalar.h"

namespace bge
{
    bool Scalar::IsInt64Min(i64 value) noexcept
    {
        return value == std::numeric_limits<i64>::min();
    }

    u64 Scalar::AbsToUnsigned(i64 value) noexcept
    {
        if (value >= 0)
        {
            return static_cast<u64>(value);
        }

        // Avoid UB for INT64_MIN:
        // -(value + 1) is safe, then +1 in unsigned domain.
        return static_cast<u64>(-(value + 1)) + 1ULL;
    }

    Scalar Scalar::Abs(Scalar value) noexcept
    {
        if (value.m_Raw >= 0)
        {
            return value;
        }

        BGE_ASSERT(!IsInt64Min(value.m_Raw));
        return Scalar(-value.m_Raw);
    }

    Scalar Scalar::operator*(Scalar other) const noexcept
    {
        const bool isNegative = (m_Raw < 0) ^ (other.m_Raw < 0);

        const u64 a = AbsToUnsigned(m_Raw);
        const u64 b = AbsToUnsigned(other.m_Raw);

        u64 high = 0;
        const u64 low = _umul128(a, b, &high);

        // Logical 128-bit right shift by FractionBits.
        const u64 shifted =
            (high << (64 - FractionBits)) |
            (low >> FractionBits);

        // If any bits remain above signed 63-bit positive range after scaling,
        // the result would not fit safely into i64 in this implementation path.
        BGE_ASSERT((high >> FractionBits) == 0);

        const i64 result = static_cast<i64>(shifted);
        return Scalar(isNegative ? -result : result);
    }

    Scalar Scalar::operator/(Scalar other) const noexcept
    {
        BGE_ASSERT(other.m_Raw != 0);

        const bool isNegative = (m_Raw < 0) ^ (other.m_Raw < 0);

        const u64 dividendAbs = AbsToUnsigned(m_Raw);
        const u64 divisorAbs = AbsToUnsigned(other.m_Raw);

        // Build unsigned 128-bit dividend: (dividendAbs << FractionBits)
        const u64 high = (FractionBits == 0) ? 0ULL : (dividendAbs >> (64 - FractionBits));
        const u64 low = (dividendAbs << FractionBits);

        u64 remainder = 0;
        const u64 quotient = _udiv128(high, low, divisorAbs, &remainder);

        // Must fit into signed i64 positive range before sign is applied.
        BGE_ASSERT((quotient >> 63) == 0);

        const i64 result = static_cast<i64>(quotient);
        return Scalar(isNegative ? -result : result);
    }
}
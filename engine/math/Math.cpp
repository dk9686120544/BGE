#include "math/Math.h"

namespace bge
{
    namespace
    {
        static u64 ISqrtU64(u64 value) noexcept
        {
            u64 result = 0;
            u64 bit = 1ULL << 62;

            while (bit > value)
            {
                bit >>= 2;
            }

            while (bit != 0)
            {
                if (value >= result + bit)
                {
                    value -= result + bit;
                    result = (result >> 1) + bit;
                }
                else
                {
                    result >>= 1;
                }

                bit >>= 2;
            }

            return result;
        }
    }

    Scalar Sqrt(Scalar value) noexcept
    {
        if (value.Raw() <= 0)
        {
            return Scalar::Zero();
        }

        // Для fixed с 16 дробными битами:
        // resultRaw = sqrt(valueRaw << 16)
        //
        // В "жестко production" варианте нам нужен безопасный путь без
        // скрытых трюков. Для текущего проекта v1 мы фиксируем рабочий
        // диапазон таким, чтобы (raw << 16) помещался в u64.
        //
        // То есть raw не должен использовать старшие 16 бит.
        const u64 raw = static_cast<u64>(value.Raw());

        BGE_ASSERT((raw >> (64 - Scalar::FractionBits)) == 0);

        const u64 shifted = raw << Scalar::FractionBits;
        const u64 root = ISqrtU64(shifted);

        return Scalar::FromRaw(static_cast<i64>(root));
    }

    Scalar Length(const Vec2& value) noexcept
    {
        return Sqrt(Vec2::LengthSq(value));
    }

    Scalar Distance(const Vec2& a, const Vec2& b) noexcept
    {
        return Sqrt(Vec2::DistanceSq(a, b));
    }

    Vec2 NormalizeApprox(const Vec2& value) noexcept
    {
        const Scalar length = Length(value);

        if (length.IsZero())
        {
            return Vec2::Zero();
        }

        return value / length;
    }
}
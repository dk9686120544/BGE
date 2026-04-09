#include "math/Vec2.h"

namespace bge
{
    Scalar Vec2::Dot(const Vec2& a, const Vec2& b) noexcept
    {
        return (a.x * b.x) + (a.y * b.y);
    }

    Scalar Vec2::LengthSq(const Vec2& value) noexcept
    {
        return Dot(value, value);
    }

    Scalar Vec2::DistanceSq(const Vec2& a, const Vec2& b) noexcept
    {
        return LengthSq(a - b);
    }
}
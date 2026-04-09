#pragma once

#include "math/Scalar.h"
#include "math/Vec2.h"

namespace bge
{
    Scalar Sqrt(Scalar value) noexcept;
    Scalar Length(const Vec2& value) noexcept;
    Scalar Distance(const Vec2& a, const Vec2& b) noexcept;
    Vec2 NormalizeApprox(const Vec2& value) noexcept;
}
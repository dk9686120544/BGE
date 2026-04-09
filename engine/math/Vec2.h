#pragma once

#include "math/Scalar.h"

namespace bge
{
    struct Vec2
    {
        Scalar x;
        Scalar y;

        constexpr Vec2() noexcept
            : x(Scalar::Zero())
            , y(Scalar::Zero())
        {
        }

        constexpr Vec2(Scalar inX, Scalar inY) noexcept
            : x(inX)
            , y(inY)
        {
        }

        static constexpr Vec2 Zero() noexcept
        {
            return Vec2(Scalar::Zero(), Scalar::Zero());
        }

        constexpr Vec2 operator+() const noexcept
        {
            return *this;
        }

        constexpr Vec2 operator-() const noexcept
        {
            return Vec2(-x, -y);
        }

        constexpr Vec2 operator+(const Vec2& other) const noexcept
        {
            return Vec2(x + other.x, y + other.y);
        }

        constexpr Vec2 operator-(const Vec2& other) const noexcept
        {
            return Vec2(x - other.x, y - other.y);
        }

        Vec2 operator*(Scalar scalar) const noexcept
        {
            return Vec2(x * scalar, y * scalar);
        }

        Vec2 operator/(Scalar scalar) const noexcept
        {
            return Vec2(x / scalar, y / scalar);
        }

        Vec2& operator+=(const Vec2& other) noexcept
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vec2& operator-=(const Vec2& other) noexcept
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vec2& operator*=(Scalar scalar) noexcept
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        Vec2& operator/=(Scalar scalar) noexcept
        {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        constexpr bool operator==(const Vec2& other) const noexcept
        {
            return x == other.x && y == other.y;
        }

        constexpr bool operator!=(const Vec2& other) const noexcept
        {
            return !(*this == other);
        }

        static Scalar Dot(const Vec2& a, const Vec2& b) noexcept;
        static Scalar LengthSq(const Vec2& value) noexcept;
        static Scalar DistanceSq(const Vec2& a, const Vec2& b) noexcept;
    };

    inline Vec2 operator*(Scalar scalar, const Vec2& value) noexcept
    {
        return value * scalar;
    }
}
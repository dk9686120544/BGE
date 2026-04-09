#pragma once

#include "core/Types.h"
#include "math/Scalar.h"
#include "math/Vec2.h"

namespace bge
{
    struct MovementTarget
    {
        Vec2 position;

        constexpr MovementTarget() noexcept
            : position(Vec2::Zero())
        {
        }

        explicit constexpr MovementTarget(const Vec2& inPosition) noexcept
            : position(inPosition)
        {
        }

        constexpr bool operator==(const MovementTarget& other) const noexcept
        {
            return position == other.position;
        }

        constexpr bool operator!=(const MovementTarget& other) const noexcept
        {
            return !(*this == other);
        }
    };

    struct MovementState
    {
        Vec2 position;
        Vec2 velocity;
        Vec2 desiredVelocity;
        MovementTarget target;

        Scalar maxSpeed;
        Scalar radius;

        bool hasTarget;

        constexpr MovementState() noexcept
            : position(Vec2::Zero())
            , velocity(Vec2::Zero())
            , desiredVelocity(Vec2::Zero())
            , target()
            , maxSpeed(Scalar::Zero())
            , radius(Scalar::Zero())
            , hasTarget(false)
        {
        }
    };

    struct ArrivalSettings
    {
        Scalar slowDownDistance;
        Scalar stopDistance;

        constexpr ArrivalSettings() noexcept
            : slowDownDistance(Scalar::FromInt(2))
            , stopDistance(Scalar::FromRaw(Scalar::OneRaw / 10))
        {
        }
    };

    struct SeparationSettings
    {
        Scalar weight;
        Scalar maxDistance;

        constexpr SeparationSettings() noexcept
            : weight(Scalar::One())
            , maxDistance(Scalar::FromInt(1))
        {
        }
    };
}
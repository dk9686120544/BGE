#pragma once

#include "math/Scalar.h"
#include "math/Vec2.h"
#include "movement/MovementTypes.h"

namespace bge
{
    class Arrival
    {
    public:
        static bool HasArrived(
            const Vec2& position,
            const MovementTarget& target,
            const ArrivalSettings& settings) noexcept;

        static Vec2 ComputeDesiredVelocity(
            const Vec2& position,
            const MovementTarget& target,
            Scalar maxSpeed,
            const ArrivalSettings& settings) noexcept;
    };
}
#pragma once

#include "movement/MovementTypes.h"
#include "math/Math.h"

namespace bge
{
    class MovementSystem
    {
    public:
        static void Tick(
            MovementState& state,
            const ArrivalSettings& arrivalSettings,
            const Vec2& separationOffset,
            Scalar deltaTime) noexcept;

    private:
        static Vec2 ClampToMaxSpeed(const Vec2& velocity, Scalar maxSpeed) noexcept;
    };
}
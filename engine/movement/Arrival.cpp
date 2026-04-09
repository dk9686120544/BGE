#include "movement/Arrival.h"

#include "math/Math.h"

namespace bge
{
    bool Arrival::HasArrived(
        const Vec2& position,
        const MovementTarget& target,
        const ArrivalSettings& settings) noexcept
    {
        const Scalar distance = Distance(position, target.position);
        return distance <= settings.stopDistance;
    }

    Vec2 Arrival::ComputeDesiredVelocity(
        const Vec2& position,
        const MovementTarget& target,
        Scalar maxSpeed,
        const ArrivalSettings& settings) noexcept
    {
        const Vec2 toTarget = target.position - position;
        const Scalar distance = Length(toTarget);

        if (distance <= settings.stopDistance)
        {
            return Vec2::Zero();
        }

        const Vec2 direction = NormalizeApprox(toTarget);

        if (distance >= settings.slowDownDistance)
        {
            return direction * maxSpeed;
        }

        const Scalar scaledSpeed =
            (maxSpeed * distance) / settings.slowDownDistance;

        return direction * scaledSpeed;
    }
}
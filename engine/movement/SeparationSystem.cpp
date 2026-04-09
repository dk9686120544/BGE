#include "movement/SeparationSystem.h"

#include "math/Math.h"

namespace bge
{
    Vec2 SeparationSystem::ComputeSeparationOffset(
        const MovementState& self,
        const MovementState& other,
        const SeparationSettings& settings) noexcept
    {
        const Vec2 delta = self.position - other.position;
        const Scalar distance = Length(delta);

        const Scalar desiredDistance = self.radius + other.radius;
        const Scalar maxDistance = Scalar::Max(desiredDistance, settings.maxDistance);

        if (distance >= maxDistance)
        {
            return Vec2::Zero();
        }

        Vec2 direction = Vec2::Zero();

        if (distance.IsZero())
        {
            // Deterministic fallback direction when both units are at the same point.
            direction = Vec2(Scalar::One(), Scalar::Zero());
        }
        else
        {
            direction = NormalizeApprox(delta);
        }

        Scalar strength = Scalar::Zero();

        if (distance < desiredDistance)
        {
            strength = settings.weight;
        }
        else
        {
            const Scalar remaining = maxDistance - distance;
            const Scalar range = maxDistance - desiredDistance;

            if (!range.IsZero())
            {
                strength = (settings.weight * remaining) / range;
            }
        }

        return direction * strength;
    }

    void SeparationSystem::ApplyPairwiseSeparation(
        MovementState& a,
        MovementState& b,
        const SeparationSettings& settings) noexcept
    {
        const Vec2 offsetAB = ComputeSeparationOffset(a, b, settings);
        const Vec2 offsetBA = ComputeSeparationOffset(b, a, settings);

        a.desiredVelocity += offsetAB;
        b.desiredVelocity += offsetBA;
    }
}
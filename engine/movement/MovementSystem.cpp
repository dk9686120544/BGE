#include "movement/MovementSystem.h"

#include "math/Math.h"
#include "movement/Arrival.h"

namespace bge
{
    void MovementSystem::Tick(
        MovementState& state,
        const ArrivalSettings& arrivalSettings,
        const Vec2& separationOffset,
        Scalar deltaTime) noexcept
    {
        if (!state.hasTarget)
        {
            state.desiredVelocity = Vec2::Zero();
            state.velocity = Vec2::Zero();
            return;
        }

        if (Arrival::HasArrived(state.position, state.target, arrivalSettings))
        {
            state.position = state.target.position;
            state.desiredVelocity = Vec2::Zero();
            state.velocity = Vec2::Zero();
            state.hasTarget = false;
            return;
        }

        state.desiredVelocity =
            Arrival::ComputeDesiredVelocity(
                state.position,
                state.target,
                state.maxSpeed,
                arrivalSettings);

        state.desiredVelocity += separationOffset;
        state.desiredVelocity = ClampToMaxSpeed(state.desiredVelocity, state.maxSpeed);

        state.velocity = state.desiredVelocity;
        state.position += state.velocity * deltaTime;

        if (Arrival::HasArrived(state.position, state.target, arrivalSettings))
        {
            state.position = state.target.position;
            state.desiredVelocity = Vec2::Zero();
            state.velocity = Vec2::Zero();
            state.hasTarget = false;
        }
    }

    Vec2 MovementSystem::ClampToMaxSpeed(const Vec2& velocity, Scalar maxSpeed) noexcept
    {
        const Scalar length = Length(velocity);

        if (length.IsZero())
        {
            return Vec2::Zero();
        }

        if (length <= maxSpeed)
        {
            return velocity;
        }

        const Vec2 direction = NormalizeApprox(velocity);
        return direction * maxSpeed;
    }
}
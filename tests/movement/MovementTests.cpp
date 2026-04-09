#include "movement/MovementSystem.h"
#include "tests/TestFramework.h"

using namespace bge;

namespace
{
    static MovementState MakeState(
        const Vec2& position,
        const Vec2& targetPosition,
        Scalar maxSpeed = Scalar::FromInt(4),
        Scalar radius = Scalar::FromRaw(Scalar::OneRaw / 2)) noexcept
    {
        MovementState state;
        state.position = position;
        state.velocity = Vec2::Zero();
        state.desiredVelocity = Vec2::Zero();
        state.target = MovementTarget(targetPosition);
        state.maxSpeed = maxSpeed;
        state.radius = radius;
        state.hasTarget = true;
        return state;
    }
}

BGE_TEST(MovementTests, NoTargetKeepsStateStopped)
{
    MovementState state;
    state.position = Vec2(Scalar::FromInt(1), Scalar::FromInt(2));
    state.velocity = Vec2(Scalar::FromInt(3), Scalar::FromInt(4));
    state.desiredVelocity = Vec2(Scalar::FromInt(5), Scalar::FromInt(6));
    state.maxSpeed = Scalar::FromInt(4);
    state.radius = Scalar::FromRaw(Scalar::OneRaw / 2);
    state.hasTarget = false;

    const ArrivalSettings arrivalSettings;
    MovementSystem::Tick(state, arrivalSettings, Vec2::Zero(), Scalar::One());

    BGE_TEST_EQ(state.position, Vec2(Scalar::FromInt(1), Scalar::FromInt(2)));
    BGE_TEST_EQ(state.velocity, Vec2::Zero());
    BGE_TEST_EQ(state.desiredVelocity, Vec2::Zero());
    BGE_TEST_FALSE(state.hasTarget);
}

BGE_TEST(MovementTests, TickMovesTowardTarget)
{
    MovementState state = MakeState(
        Vec2(Scalar::Zero(), Scalar::Zero()),
        Vec2(Scalar::FromInt(10), Scalar::Zero()),
        Scalar::FromInt(2));

    const ArrivalSettings arrivalSettings;

    MovementSystem::Tick(state, arrivalSettings, Vec2::Zero(), Scalar::One());

    BGE_TEST_TRUE(state.position.x > Scalar::Zero());
    BGE_TEST_EQ(state.position.y, Scalar::Zero());
    BGE_TEST_TRUE(state.velocity.x > Scalar::Zero());
    BGE_TEST_EQ(state.velocity.y, Scalar::Zero());
    BGE_TEST_TRUE(state.hasTarget);
}

BGE_TEST(MovementTests, TickStopsAtTargetWhenAlreadyArrived)
{
    MovementState state = MakeState(
        Vec2(Scalar::Zero(), Scalar::Zero()),
        Vec2(Scalar::FromRaw(Scalar::OneRaw / 20), Scalar::Zero()),
        Scalar::FromInt(2));

    const ArrivalSettings arrivalSettings;

    MovementSystem::Tick(state, arrivalSettings, Vec2::Zero(), Scalar::One());

    BGE_TEST_EQ(state.position, state.target.position);
    BGE_TEST_EQ(state.velocity, Vec2::Zero());
    BGE_TEST_EQ(state.desiredVelocity, Vec2::Zero());
    BGE_TEST_FALSE(state.hasTarget);
}

BGE_TEST(MovementTests, TickAppliesSeparationOffset)
{
    MovementState state = MakeState(
        Vec2(Scalar::Zero(), Scalar::Zero()),
        Vec2(Scalar::FromInt(10), Scalar::Zero()),
        Scalar::FromInt(4));

    const ArrivalSettings arrivalSettings;
    const Vec2 separationOffset(Scalar::Zero(), Scalar::FromInt(1));

    MovementSystem::Tick(state, arrivalSettings, separationOffset, Scalar::One());

    BGE_TEST_TRUE(state.position.x > Scalar::Zero());
    BGE_TEST_TRUE(state.position.y > Scalar::Zero());
    BGE_TEST_TRUE(state.velocity.y > Scalar::Zero());
}

BGE_TEST(MovementTests, TickClampsVelocityToMaxSpeed)
{
    MovementState state = MakeState(
        Vec2(Scalar::Zero(), Scalar::Zero()),
        Vec2(Scalar::FromInt(10), Scalar::Zero()),
        Scalar::FromInt(2));

    const ArrivalSettings arrivalSettings;
    const Vec2 separationOffset(Scalar::FromInt(10), Scalar::FromInt(0));

    MovementSystem::Tick(state, arrivalSettings, separationOffset, Scalar::One());

    const Scalar speed = Length(state.velocity);
    BGE_TEST_TRUE(speed <= state.maxSpeed);
}

BGE_TEST(MovementTests, MultipleTicksEventuallyReachTarget)
{
    MovementState state = MakeState(
        Vec2(Scalar::Zero(), Scalar::Zero()),
        Vec2(Scalar::FromInt(5), Scalar::Zero()),
        Scalar::FromInt(1));

    const ArrivalSettings arrivalSettings;

    for (int i = 0; i < 20 && state.hasTarget; ++i)
    {
        MovementSystem::Tick(state, arrivalSettings, Vec2::Zero(), Scalar::One());
    }

    BGE_TEST_FALSE(state.hasTarget);
    BGE_TEST_EQ(state.position, Vec2(Scalar::FromInt(5), Scalar::Zero()));
    BGE_TEST_EQ(state.velocity, Vec2::Zero());
    BGE_TEST_EQ(state.desiredVelocity, Vec2::Zero());
}

BGE_TEST(MovementTests, TickUsesDeltaTimeForIntegration)
{
    MovementState stateA = MakeState(
        Vec2(Scalar::Zero(), Scalar::Zero()),
        Vec2(Scalar::FromInt(10), Scalar::Zero()),
        Scalar::FromInt(2));

    MovementState stateB = stateA;

    const ArrivalSettings arrivalSettings;

    MovementSystem::Tick(stateA, arrivalSettings, Vec2::Zero(), Scalar::One());
    MovementSystem::Tick(stateB, arrivalSettings, Vec2::Zero(), Scalar::Half());

    BGE_TEST_TRUE(stateA.position.x > stateB.position.x);
}

BGE_TEST(MovementTests, TickCanMoveInNegativeDirection)
{
    MovementState state = MakeState(
        Vec2(Scalar::FromInt(5), Scalar::Zero()),
        Vec2(Scalar::Zero(), Scalar::Zero()),
        Scalar::FromInt(2));

    const ArrivalSettings arrivalSettings;

    MovementSystem::Tick(state, arrivalSettings, Vec2::Zero(), Scalar::One());

    BGE_TEST_TRUE(state.position.x < Scalar::FromInt(5));
    BGE_TEST_TRUE(state.velocity.x < Scalar::Zero());
}
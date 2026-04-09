#include "movement/SeparationSystem.h"
#include "tests/TestFramework.h"

using namespace bge;

namespace
{
    static MovementState MakeState(
        const Vec2& position,
        Scalar radius = Scalar::FromRaw(Scalar::OneRaw / 2)) noexcept
    {
        MovementState state;
        state.position = position;
        state.velocity = Vec2::Zero();
        state.desiredVelocity = Vec2::Zero();
        state.maxSpeed = Scalar::FromInt(4);
        state.radius = radius;
        state.hasTarget = false;
        return state;
    }
}

BGE_TEST(SeparationTests, NoOffsetWhenUnitsAreFarAway)
{
    const MovementState a = MakeState(Vec2(Scalar::Zero(), Scalar::Zero()));
    const MovementState b = MakeState(Vec2(Scalar::FromInt(10), Scalar::Zero()));

    const SeparationSettings settings;
    const Vec2 offset = SeparationSystem::ComputeSeparationOffset(a, b, settings);

    BGE_TEST_EQ(offset, Vec2::Zero());
}

BGE_TEST(SeparationTests, OffsetPushesAwayFromOtherUnit)
{
    const MovementState a = MakeState(Vec2(Scalar::Zero(), Scalar::Zero()));
    const MovementState b = MakeState(Vec2(Scalar::FromRaw(Scalar::OneRaw / 2), Scalar::Zero()));

    const SeparationSettings settings;
    const Vec2 offset = SeparationSystem::ComputeSeparationOffset(a, b, settings);

    BGE_TEST_TRUE(offset.x < Scalar::Zero());
    BGE_TEST_EQ(offset.y, Scalar::Zero());
}

BGE_TEST(SeparationTests, ZeroDistanceUsesDeterministicFallbackDirection)
{
    const MovementState a = MakeState(Vec2::Zero());
    const MovementState b = MakeState(Vec2::Zero());

    const SeparationSettings settings;
    const Vec2 offset = SeparationSystem::ComputeSeparationOffset(a, b, settings);

    BGE_TEST_TRUE(offset.x > Scalar::Zero());
    BGE_TEST_EQ(offset.y, Scalar::Zero());
}

BGE_TEST(SeparationTests, PairwiseSeparationChangesBothDesiredVelocities)
{
    MovementState a = MakeState(Vec2(Scalar::Zero(), Scalar::Zero()));
    MovementState b = MakeState(Vec2(Scalar::FromRaw(Scalar::OneRaw / 2), Scalar::Zero()));

    const SeparationSettings settings;

    SeparationSystem::ApplyPairwiseSeparation(a, b, settings);

    BGE_TEST_TRUE(a.desiredVelocity.x < Scalar::Zero());
    BGE_TEST_TRUE(b.desiredVelocity.x > Scalar::Zero());
    BGE_TEST_EQ(a.desiredVelocity.y, Scalar::Zero());
    BGE_TEST_EQ(b.desiredVelocity.y, Scalar::Zero());
}

BGE_TEST(SeparationTests, PairwiseSeparationIsSymmetricInMagnitudeForSimpleCase)
{
    MovementState a = MakeState(Vec2(Scalar::Zero(), Scalar::Zero()));
    MovementState b = MakeState(Vec2(Scalar::FromRaw(Scalar::OneRaw / 2), Scalar::Zero()));

    const SeparationSettings settings;

    SeparationSystem::ApplyPairwiseSeparation(a, b, settings);

    BGE_TEST_EQ(Scalar::Abs(a.desiredVelocity.x), Scalar::Abs(b.desiredVelocity.x));
}

BGE_TEST(SeparationTests, MaxDistanceControlsInfluenceRange)
{
    MovementState a = MakeState(Vec2(Scalar::Zero(), Scalar::Zero()));
    MovementState b = MakeState(Vec2(Scalar::FromInt(2), Scalar::Zero()));

    SeparationSettings settings;
    settings.maxDistance = Scalar::FromInt(1);

    const Vec2 offset = SeparationSystem::ComputeSeparationOffset(a, b, settings);

    BGE_TEST_EQ(offset, Vec2::Zero());
}

BGE_TEST(SeparationTests, WeightScalesOffset)
{
    const MovementState a = MakeState(Vec2(Scalar::Zero(), Scalar::Zero()));
    const MovementState b = MakeState(Vec2(Scalar::FromRaw(Scalar::OneRaw / 2), Scalar::Zero()));

    SeparationSettings weakSettings;
    weakSettings.weight = Scalar::FromInt(1);

    SeparationSettings strongSettings;
    strongSettings.weight = Scalar::FromInt(3);

    const Vec2 weakOffset =
        SeparationSystem::ComputeSeparationOffset(a, b, weakSettings);
    const Vec2 strongOffset =
        SeparationSystem::ComputeSeparationOffset(a, b, strongSettings);

    BGE_TEST_TRUE(Scalar::Abs(strongOffset.x) > Scalar::Abs(weakOffset.x));
}
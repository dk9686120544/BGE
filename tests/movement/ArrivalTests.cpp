#include "movement/Arrival.h"
#include "tests/TestFramework.h"

using namespace bge;

BGE_TEST(ArrivalTests, HasArrivedReturnsTrueInsideStopDistance)
{
    const Vec2 position(Scalar::Zero(), Scalar::Zero());
    const MovementTarget target(Vec2(Scalar::FromRaw(Scalar::OneRaw / 20), Scalar::Zero())); // 0.05
    const ArrivalSettings settings;

    BGE_TEST_TRUE(Arrival::HasArrived(position, target, settings));
}

BGE_TEST(ArrivalTests, HasArrivedReturnsFalseOutsideStopDistance)
{
    const Vec2 position(Scalar::Zero(), Scalar::Zero());
    const MovementTarget target(Vec2(Scalar::FromInt(1), Scalar::Zero()));
    const ArrivalSettings settings;

    BGE_TEST_FALSE(Arrival::HasArrived(position, target, settings));
}

BGE_TEST(ArrivalTests, ComputeDesiredVelocityReturnsZeroWhenAlreadyArrived)
{
    const Vec2 position(Scalar::Zero(), Scalar::Zero());
    const MovementTarget target(Vec2(Scalar::FromRaw(Scalar::OneRaw / 20), Scalar::Zero()));
    const ArrivalSettings settings;

    const Vec2 velocity =
        Arrival::ComputeDesiredVelocity(position, target, Scalar::FromInt(5), settings);

    BGE_TEST_EQ(velocity, Vec2::Zero());
}

BGE_TEST(ArrivalTests, ComputeDesiredVelocityReturnsMaxSpeedWhenFarFromTarget)
{
    const Vec2 position(Scalar::Zero(), Scalar::Zero());
    const MovementTarget target(Vec2(Scalar::FromInt(10), Scalar::Zero()));
    const ArrivalSettings settings;
    const Scalar maxSpeed = Scalar::FromInt(4);

    const Vec2 velocity =
        Arrival::ComputeDesiredVelocity(position, target, maxSpeed, settings);

    BGE_TEST_EQ(velocity.x, Scalar::FromInt(4));
    BGE_TEST_EQ(velocity.y, Scalar::Zero());
}

BGE_TEST(ArrivalTests, ComputeDesiredVelocitySlowsDownInsideSlowDownDistance)
{
    const Vec2 position(Scalar::Zero(), Scalar::Zero());
    const MovementTarget target(Vec2(Scalar::FromInt(1), Scalar::Zero()));
    const ArrivalSettings settings;
    const Scalar maxSpeed = Scalar::FromInt(4);

    const Vec2 velocity =
        Arrival::ComputeDesiredVelocity(position, target, maxSpeed, settings);

    BGE_TEST_TRUE(velocity.x > Scalar::Zero());
    BGE_TEST_TRUE(velocity.x < maxSpeed);
    BGE_TEST_EQ(velocity.y, Scalar::Zero());
}

BGE_TEST(ArrivalTests, ComputeDesiredVelocityUsesDirectionToTarget)
{
    const Vec2 position(Scalar::Zero(), Scalar::Zero());
    const MovementTarget target(Vec2(Scalar::FromInt(3), Scalar::FromInt(4)));
    const ArrivalSettings settings;
    const Scalar maxSpeed = Scalar::FromInt(5);

    const Vec2 velocity =
        Arrival::ComputeDesiredVelocity(position, target, maxSpeed, settings);

    BGE_TEST_NEAR(velocity.x.ToFloat(), 3.0f, 0.05f);
    BGE_TEST_NEAR(velocity.y.ToFloat(), 4.0f, 0.05f);
}

BGE_TEST(ArrivalTests, ComputeDesiredVelocityForNegativeDirectionWorks)
{
    const Vec2 position(Scalar::FromInt(5), Scalar::FromInt(0));
    const MovementTarget target(Vec2(Scalar::FromInt(0), Scalar::FromInt(0)));
    const ArrivalSettings settings;
    const Scalar maxSpeed = Scalar::FromInt(2);

    const Vec2 velocity =
        Arrival::ComputeDesiredVelocity(position, target, maxSpeed, settings);

    BGE_TEST_TRUE(velocity.x < Scalar::Zero());
    BGE_TEST_EQ(velocity.y, Scalar::Zero());
}
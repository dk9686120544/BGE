#include "math/Math.h"
#include "tests/TestFramework.h"

using namespace bge;

BGE_TEST(MathTests, SqrtOfZeroIsZero)
{
    const Scalar value = Scalar::Zero();
    const Scalar result = Sqrt(value);

    BGE_TEST_EQ(result, Scalar::Zero());
}

BGE_TEST(MathTests, SqrtOfOneIsOne)
{
    const Scalar value = Scalar::One();
    const Scalar result = Sqrt(value);

    BGE_TEST_EQ(result, Scalar::One());
}

BGE_TEST(MathTests, SqrtOfTwentyFiveIsFive)
{
    const Scalar value = Scalar::FromInt(25);
    const Scalar result = Sqrt(value);

    BGE_TEST_EQ(result, Scalar::FromInt(5));
}

BGE_TEST(MathTests, LengthWorks)
{
    const Vec2 value(Scalar::FromInt(3), Scalar::FromInt(4));
    const Scalar result = Length(value);

    BGE_TEST_EQ(result, Scalar::FromInt(5));
}

BGE_TEST(MathTests, DistanceWorks)
{
    const Vec2 a(Scalar::FromInt(1), Scalar::FromInt(1));
    const Vec2 b(Scalar::FromInt(4), Scalar::FromInt(5));
    const Scalar result = Distance(a, b);

    BGE_TEST_EQ(result, Scalar::FromInt(5));
}

BGE_TEST(MathTests, NormalizeApproxOfZeroIsZero)
{
    const Vec2 value = Vec2::Zero();
    const Vec2 result = NormalizeApprox(value);

    BGE_TEST_EQ(result, Vec2::Zero());
}

BGE_TEST(MathTests, NormalizeApproxOf345Triangle)
{
    const Vec2 value(Scalar::FromInt(3), Scalar::FromInt(4));
    const Vec2 result = NormalizeApprox(value);

    BGE_TEST_NEAR(result.x.ToFloat(), 0.6f, 0.01f);
    BGE_TEST_NEAR(result.y.ToFloat(), 0.8f, 0.01f);
}

BGE_TEST(MathTests, DistanceIsSymmetric)
{
    const Vec2 a(Scalar::FromInt(2), Scalar::FromInt(7));
    const Vec2 b(Scalar::FromInt(-3), Scalar::FromInt(1));

    const Scalar ab = Distance(a, b);
    const Scalar ba = Distance(b, a);

    BGE_TEST_EQ(ab, ba);
}
#include "math/Vec2.h"
#include "tests/TestFramework.h"

using namespace bge;

BGE_TEST(Vec2Tests, DefaultConstructorIsZero)
{
    const Vec2 v;

    BGE_TEST_EQ(v.x, Scalar::Zero());
    BGE_TEST_EQ(v.y, Scalar::Zero());
}

BGE_TEST(Vec2Tests, ConstructorWorks)
{
    const Vec2 v(Scalar::FromInt(2), Scalar::FromInt(3));

    BGE_TEST_EQ(v.x, Scalar::FromInt(2));
    BGE_TEST_EQ(v.y, Scalar::FromInt(3));
}

BGE_TEST(Vec2Tests, ZeroFactoryWorks)
{
    const Vec2 v = Vec2::Zero();

    BGE_TEST_EQ(v.x, Scalar::Zero());
    BGE_TEST_EQ(v.y, Scalar::Zero());
}

BGE_TEST(Vec2Tests, AddWorks)
{
    const Vec2 a(Scalar::FromInt(1), Scalar::FromInt(2));
    const Vec2 b(Scalar::FromInt(3), Scalar::FromInt(4));
    const Vec2 c = a + b;

    BGE_TEST_EQ(c.x, Scalar::FromInt(4));
    BGE_TEST_EQ(c.y, Scalar::FromInt(6));
}

BGE_TEST(Vec2Tests, SubtractWorks)
{
    const Vec2 a(Scalar::FromInt(5), Scalar::FromInt(7));
    const Vec2 b(Scalar::FromInt(2), Scalar::FromInt(3));
    const Vec2 c = a - b;

    BGE_TEST_EQ(c.x, Scalar::FromInt(3));
    BGE_TEST_EQ(c.y, Scalar::FromInt(4));
}

BGE_TEST(Vec2Tests, UnaryMinusWorks)
{
    const Vec2 a(Scalar::FromInt(2), Scalar::FromInt(-3));
    const Vec2 b = -a;

    BGE_TEST_EQ(b.x, Scalar::FromInt(-2));
    BGE_TEST_EQ(b.y, Scalar::FromInt(3));
}

BGE_TEST(Vec2Tests, ScalarMultiplyWorks)
{
    const Vec2 a(Scalar::FromInt(2), Scalar::FromInt(3));
    const Scalar s = Scalar::FromInt(4);
    const Vec2 b = a * s;

    BGE_TEST_EQ(b.x, Scalar::FromInt(8));
    BGE_TEST_EQ(b.y, Scalar::FromInt(12));
}

BGE_TEST(Vec2Tests, ScalarDivideWorks)
{
    const Vec2 a(Scalar::FromInt(8), Scalar::FromInt(12));
    const Scalar s = Scalar::FromInt(4);
    const Vec2 b = a / s;

    BGE_TEST_EQ(b.x, Scalar::FromInt(2));
    BGE_TEST_EQ(b.y, Scalar::FromInt(3));
}

BGE_TEST(Vec2Tests, DotWorks)
{
    const Vec2 a(Scalar::FromInt(2), Scalar::FromInt(3));
    const Vec2 b(Scalar::FromInt(4), Scalar::FromInt(5));
    const Scalar result = Vec2::Dot(a, b);

    BGE_TEST_EQ(result, Scalar::FromInt(23));
}

BGE_TEST(Vec2Tests, LengthSqWorks)
{
    const Vec2 v(Scalar::FromInt(3), Scalar::FromInt(4));
    const Scalar result = Vec2::LengthSq(v);

    BGE_TEST_EQ(result, Scalar::FromInt(25));
}

BGE_TEST(Vec2Tests, DistanceSqWorks)
{
    const Vec2 a(Scalar::FromInt(1), Scalar::FromInt(1));
    const Vec2 b(Scalar::FromInt(4), Scalar::FromInt(5));
    const Scalar result = Vec2::DistanceSq(a, b);

    BGE_TEST_EQ(result, Scalar::FromInt(25));
}

BGE_TEST(Vec2Tests, EqualityWorks)
{
    const Vec2 a(Scalar::FromInt(2), Scalar::FromInt(3));
    const Vec2 b(Scalar::FromInt(2), Scalar::FromInt(3));
    const Vec2 c(Scalar::FromInt(3), Scalar::FromInt(2));

    BGE_TEST_TRUE(a == b);
    BGE_TEST_TRUE(a != c);
}
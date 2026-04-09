#include "math/Scalar.h"
#include "tests/TestFramework.h"

using namespace bge;

BGE_TEST(ScalarTests, DefaultConstructorIsZero)
{
    const Scalar value;
    BGE_TEST_EQ(value.Raw(), 0);
}

BGE_TEST(ScalarTests, FromIntWorks)
{
    const Scalar zero = Scalar::FromInt(0);
    const Scalar one = Scalar::FromInt(1);
    const Scalar minusOne = Scalar::FromInt(-1);

    BGE_TEST_EQ(zero.Raw(), 0);
    BGE_TEST_EQ(one.Raw(), Scalar::OneRaw);
    BGE_TEST_EQ(minusOne.Raw(), -Scalar::OneRaw);
}

BGE_TEST(ScalarTests, FromRawWorks)
{
    const Scalar half = Scalar::FromRaw(Scalar::HalfRaw);
    BGE_TEST_EQ(half.Raw(), Scalar::HalfRaw);
}

BGE_TEST(ScalarTests, AddWorks)
{
    const Scalar a = Scalar::FromInt(2);
    const Scalar b = Scalar::FromInt(3);
    const Scalar c = a + b;

    BGE_TEST_EQ(c, Scalar::FromInt(5));
}

BGE_TEST(ScalarTests, SubtractWorks)
{
    const Scalar a = Scalar::FromInt(7);
    const Scalar b = Scalar::FromInt(4);
    const Scalar c = a - b;

    BGE_TEST_EQ(c, Scalar::FromInt(3));
}

BGE_TEST(ScalarTests, UnaryMinusWorks)
{
    const Scalar a = Scalar::FromInt(5);
    const Scalar b = -a;

    BGE_TEST_EQ(b, Scalar::FromInt(-5));
}

BGE_TEST(ScalarTests, MultiplyIntegerValues)
{
    const Scalar a = Scalar::FromInt(2);
    const Scalar b = Scalar::FromInt(3);

    BGE_TEST_EQ(a * b, Scalar::FromInt(6));
}

BGE_TEST(ScalarTests, MultiplyFractionalValues)
{
    const Scalar half = Scalar::Half();
    const Scalar three = Scalar::FromInt(3);
    const Scalar result = half * three;

    BGE_TEST_EQ(result.Raw(), Scalar::FromRaw(98304).Raw());
}

BGE_TEST(ScalarTests, MultiplyNegativeValues)
{
    const Scalar a = Scalar::FromInt(-2);
    const Scalar b = Scalar::FromInt(3);

    BGE_TEST_EQ(a * b, Scalar::FromInt(-6));
}

BGE_TEST(ScalarTests, MultiplyByZero)
{
    const Scalar zero = Scalar::Zero();
    const Scalar value = Scalar::FromInt(123);

    BGE_TEST_EQ(zero * value, Scalar::Zero());
    BGE_TEST_EQ(value * zero, Scalar::Zero());
}

BGE_TEST(ScalarTests, DivideIntegerValues)
{
    const Scalar a = Scalar::FromInt(6);
    const Scalar b = Scalar::FromInt(3);

    BGE_TEST_EQ(a / b, Scalar::FromInt(2));
}

BGE_TEST(ScalarTests, DivideFractionalValues)
{
    const Scalar a = Scalar::FromInt(3);
    const Scalar b = Scalar::FromInt(2);
    const Scalar result = a / b;

    BGE_TEST_EQ(result.Raw(), Scalar::FromRaw(98304).Raw());
}

BGE_TEST(ScalarTests, DivideNegativeValues)
{
    const Scalar a = Scalar::FromInt(-3);
    const Scalar b = Scalar::FromInt(2);
    const Scalar result = a / b;

    BGE_TEST_EQ(result.Raw(), Scalar::FromRaw(-98304).Raw());
}

BGE_TEST(ScalarTests, AbsWorks)
{
    const Scalar a = Scalar::FromInt(-7);
    const Scalar b = Scalar::Abs(a);

    BGE_TEST_EQ(b, Scalar::FromInt(7));
}

BGE_TEST(ScalarTests, MinWorks)
{
    const Scalar a = Scalar::FromInt(2);
    const Scalar b = Scalar::FromInt(5);

    BGE_TEST_EQ(Scalar::Min(a, b), a);
    BGE_TEST_EQ(Scalar::Min(b, a), a);
}

BGE_TEST(ScalarTests, MaxWorks)
{
    const Scalar a = Scalar::FromInt(2);
    const Scalar b = Scalar::FromInt(5);

    BGE_TEST_EQ(Scalar::Max(a, b), b);
    BGE_TEST_EQ(Scalar::Max(b, a), b);
}

BGE_TEST(ScalarTests, ComparisonWorks)
{
    const Scalar a = Scalar::FromInt(2);
    const Scalar b = Scalar::FromInt(3);

    BGE_TEST_TRUE(a < b);
    BGE_TEST_TRUE(a <= b);
    BGE_TEST_TRUE(b > a);
    BGE_TEST_TRUE(b >= a);
    BGE_TEST_TRUE(a != b);
    BGE_TEST_TRUE(a == Scalar::FromInt(2));
}

BGE_TEST(ScalarTests, TruncToIntWorks)
{
    const Scalar a = Scalar::FromInt(5);
    const Scalar b = Scalar::Half();
    const Scalar c = a + b;

    BGE_TEST_EQ(c.TruncToInt(), 5);
}

BGE_TEST(ScalarTests, StabilityLoop)
{
    Scalar value = Scalar::FromInt(1);

    for (int i = 0; i < 1000000; ++i)
    {
        value = (value * Scalar::FromInt(3)) / Scalar::FromInt(3);
    }

    BGE_TEST_EQ(value, Scalar::FromInt(1));
}
#include "sim/StateHash.h"
#include "tests/TestFramework.h"

using namespace bge;

BGE_TEST(StateHashTests, DefaultHashHasDeterministicInitialValue)
{
    StateHash hashA;
    StateHash hashB;

    BGE_TEST_EQ(hashA.Value(), hashB.Value());
}

BGE_TEST(StateHashTests, ResetRestoresInitialState)
{
    StateHash hash;

    const u64 initial = hash.Value();

    hash.AddU32(123U);
    hash.AddI64(-456LL);
    hash.AddBool(true);

    BGE_TEST_TRUE(hash.Value() != initial);

    hash.Reset();

    BGE_TEST_EQ(hash.Value(), initial);
}

BGE_TEST(StateHashTests, AddU32IsDeterministic)
{
    StateHash a;
    StateHash b;

    a.AddU32(42U);
    a.AddU32(77U);

    b.AddU32(42U);
    b.AddU32(77U);

    BGE_TEST_EQ(a.Value(), b.Value());
}

BGE_TEST(StateHashTests, AddDifferentValuesChangesHash)
{
    StateHash a;
    StateHash b;

    a.AddU32(1U);
    b.AddU32(2U);

    BGE_TEST_TRUE(a.Value() != b.Value());
}

BGE_TEST(StateHashTests, AddBoolProducesDeterministicResult)
{
    StateHash a;
    StateHash b;

    a.AddBool(true);
    a.AddBool(false);

    b.AddBool(true);
    b.AddBool(false);

    BGE_TEST_EQ(a.Value(), b.Value());
}

BGE_TEST(StateHashTests, AddScalarUsesRawValueDeterministically)
{
    StateHash a;
    StateHash b;

    a.AddScalar(Scalar::FromInt(10));
    a.AddScalar(Scalar::Half());

    b.AddScalar(Scalar::FromInt(10));
    b.AddScalar(Scalar::Half());

    BGE_TEST_EQ(a.Value(), b.Value());
}

BGE_TEST(StateHashTests, AddVec2UsesBothComponents)
{
    StateHash a;
    StateHash b;
    StateHash c;

    const Vec2 valueA(Scalar::FromInt(3), Scalar::FromInt(4));
    const Vec2 valueB(Scalar::FromInt(3), Scalar::FromInt(4));
    const Vec2 valueC(Scalar::FromInt(4), Scalar::FromInt(3));

    a.AddVec2(valueA);
    b.AddVec2(valueB);
    c.AddVec2(valueC);

    BGE_TEST_EQ(a.Value(), b.Value());
    BGE_TEST_TRUE(a.Value() != c.Value());
}

BGE_TEST(StateHashTests, OperationOrderMatters)
{
    StateHash a;
    StateHash b;

    a.AddU32(1U);
    a.AddU32(2U);

    b.AddU32(2U);
    b.AddU32(1U);

    BGE_TEST_TRUE(a.Value() != b.Value());
}

BGE_TEST(StateHashTests, SameSequenceProducesSameHash)
{
    StateHash a;
    StateHash b;

    a.AddU32(10U);
    a.AddI32(-20);
    a.AddU64(30ULL);
    a.AddI64(-40LL);
    a.AddBool(true);
    a.AddScalar(Scalar::FromInt(5));
    a.AddVec2(Vec2(Scalar::FromInt(7), Scalar::FromInt(8)));

    b.AddU32(10U);
    b.AddI32(-20);
    b.AddU64(30ULL);
    b.AddI64(-40LL);
    b.AddBool(true);
    b.AddScalar(Scalar::FromInt(5));
    b.AddVec2(Vec2(Scalar::FromInt(7), Scalar::FromInt(8)));

    BGE_TEST_EQ(a.Value(), b.Value());
}
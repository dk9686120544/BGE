#include "core/Random.h"
#include "tests/TestFramework.h"

using namespace bge;

BGE_TEST(RandomTests, DefaultConstructorCreatesDeterministicZeroState)
{
    Random rng;

    BGE_TEST_EQ(rng.State(), 0ULL);
    BGE_TEST_EQ(rng.Increment(), 0ULL);
}

BGE_TEST(RandomTests, SeedInitializesStateAndIncrement)
{
    Random rng;
    rng.Seed(42ULL, 7ULL);

    BGE_TEST_TRUE(rng.State() != 0ULL);
    BGE_TEST_EQ(rng.Increment(), (7ULL << 1ULL) | 1ULL);
}

BGE_TEST(RandomTests, ConstructorWithSeedMatchesExplicitSeed)
{
    Random a(123ULL, 456ULL);
    Random b;
    b.Seed(123ULL, 456ULL);

    BGE_TEST_EQ(a.State(), b.State());
    BGE_TEST_EQ(a.Increment(), b.Increment());

    for (int i = 0; i < 32; ++i)
    {
        BGE_TEST_EQ(a.NextU32(), b.NextU32());
    }
}

BGE_TEST(RandomTests, SameSeedProducesSameSequence)
{
    Random a(111ULL, 222ULL);
    Random b(111ULL, 222ULL);

    for (int i = 0; i < 128; ++i)
    {
        BGE_TEST_EQ(a.NextU32(), b.NextU32());
    }
}

BGE_TEST(RandomTests, DifferentSeedsProduceDifferentSequences)
{
    Random a(111ULL, 222ULL);
    Random b(333ULL, 444ULL);

    bool foundDifference = false;

    for (int i = 0; i < 64; ++i)
    {
        if (a.NextU32() != b.NextU32())
        {
            foundDifference = true;
            break;
        }
    }

    BGE_TEST_TRUE(foundDifference);
}

BGE_TEST(RandomTests, NextU32AdvancesState)
{
    Random rng(123ULL, 456ULL);

    const u64 beforeState = rng.State();
    const u32 value = rng.NextU32();
    const u64 afterState = rng.State();

    (void)value;

    BGE_TEST_TRUE(afterState != beforeState);
}

BGE_TEST(RandomTests, NextU32BoundZeroReturnsZero)
{
    Random rng(123ULL, 456ULL);

    BGE_TEST_EQ(rng.NextU32(0), 0U);
}

BGE_TEST(RandomTests, NextU32BoundOneAlwaysReturnsZero)
{
    Random rng(123ULL, 456ULL);

    for (int i = 0; i < 128; ++i)
    {
        BGE_TEST_EQ(rng.NextU32(1), 0U);
    }
}

BGE_TEST(RandomTests, NextU32BoundStaysWithinRange)
{
    Random rng(987654321ULL, 123456789ULL);
    const u32 bound = 10U;

    for (int i = 0; i < 10000; ++i)
    {
        const u32 value = rng.NextU32(bound);
        BGE_TEST_TRUE(value < bound);
    }
}

BGE_TEST(RandomTests, NextU32BoundDeterministicSequence)
{
    Random a(999ULL, 777ULL);
    Random b(999ULL, 777ULL);

    for (int i = 0; i < 256; ++i)
    {
        BGE_TEST_EQ(a.NextU32(37U), b.NextU32(37U));
    }
}

BGE_TEST(RandomTests, ReSeedingResetsSequence)
{
    Random rng(12ULL, 34ULL);

    const u32 firstA = rng.NextU32();
    const u32 secondA = rng.NextU32();
    const u32 thirdA = rng.NextU32();

    rng.Seed(12ULL, 34ULL);

    const u32 firstB = rng.NextU32();
    const u32 secondB = rng.NextU32();
    const u32 thirdB = rng.NextU32();

    BGE_TEST_EQ(firstA, firstB);
    BGE_TEST_EQ(secondA, secondB);
    BGE_TEST_EQ(thirdA, thirdB);
}

BGE_TEST(RandomTests, LongRunDeterminism)
{
    Random a(0xDEADBEEFCAFEBABEULL, 0x123456789ABCDEF0ULL);
    Random b(0xDEADBEEFCAFEBABEULL, 0x123456789ABCDEF0ULL);

    for (int i = 0; i < 100000; ++i)
    {
        BGE_TEST_EQ(a.NextU32(), b.NextU32());
    }

    BGE_TEST_EQ(a.State(), b.State());
    BGE_TEST_EQ(a.Increment(), b.Increment());
}
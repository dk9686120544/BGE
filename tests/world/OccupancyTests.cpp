#include "tests/TestFramework.h"
#include "world/Occupancy.h"

using namespace bge;

BGE_TEST(OccupancyTests, DefaultConstructorCreatesEmptyOccupancy)
{
    Occupancy occupancy;

    BGE_TEST_TRUE(occupancy.IsEmpty());
    BGE_TEST_EQ(occupancy.Size(), GridSize(0U, 0U));
}

BGE_TEST(OccupancyTests, SizedConstructorInitializesGrid)
{
    Occupancy occupancy(GridSize(8U, 6U));

    BGE_TEST_FALSE(occupancy.IsEmpty());
    BGE_TEST_EQ(occupancy.Size(), GridSize(8U, 6U));
}

BGE_TEST(OccupancyTests, ResizeChangesDimensionsAndClearsCells)
{
    Occupancy occupancy(GridSize(2U, 2U));
    occupancy.SetStaticBlocked(GridCoord(1, 1), true);
    occupancy.SetDynamicOccupied(GridCoord(0, 1), Entity(3U, 0U));

    occupancy.Resize(GridSize(4U, 3U));

    BGE_TEST_EQ(occupancy.Size(), GridSize(4U, 3U));
    BGE_TEST_FALSE(occupancy.IsStaticBlocked(GridCoord(1, 1)));
    BGE_TEST_FALSE(occupancy.HasDynamicOccupant(GridCoord(0, 1)));
}

BGE_TEST(OccupancyTests, IsInsideWorksForValidAndInvalidCoords)
{
    Occupancy occupancy(GridSize(4U, 3U));

    BGE_TEST_TRUE(occupancy.IsInside(GridCoord(0, 0)));
    BGE_TEST_TRUE(occupancy.IsInside(GridCoord(3, 2)));
    BGE_TEST_FALSE(occupancy.IsInside(GridCoord(-1, 0)));
    BGE_TEST_FALSE(occupancy.IsInside(GridCoord(0, -1)));
    BGE_TEST_FALSE(occupancy.IsInside(GridCoord(4, 0)));
    BGE_TEST_FALSE(occupancy.IsInside(GridCoord(0, 3)));
}

BGE_TEST(OccupancyTests, StaticBlockedCanBeSetAndQueried)
{
    Occupancy occupancy(GridSize(5U, 5U));

    BGE_TEST_FALSE(occupancy.IsStaticBlocked(GridCoord(2, 2)));

    occupancy.SetStaticBlocked(GridCoord(2, 2), true);
    BGE_TEST_TRUE(occupancy.IsStaticBlocked(GridCoord(2, 2)));

    occupancy.SetStaticBlocked(GridCoord(2, 2), false);
    BGE_TEST_FALSE(occupancy.IsStaticBlocked(GridCoord(2, 2)));
}

BGE_TEST(OccupancyTests, DynamicOccupantCanBeSetAndCleared)
{
    Occupancy occupancy(GridSize(5U, 5U));
    const Entity entity(7U, 1U);

    BGE_TEST_FALSE(occupancy.HasDynamicOccupant(GridCoord(1, 3)));
    BGE_TEST_EQ(occupancy.GetDynamicOccupant(GridCoord(1, 3)), Entity::Invalid());

    occupancy.SetDynamicOccupied(GridCoord(1, 3), entity);

    BGE_TEST_TRUE(occupancy.HasDynamicOccupant(GridCoord(1, 3)));
    BGE_TEST_EQ(occupancy.GetDynamicOccupant(GridCoord(1, 3)), entity);
    BGE_TEST_TRUE(occupancy.IsOccupied(GridCoord(1, 3)));

    occupancy.ClearDynamicOccupied(GridCoord(1, 3));

    BGE_TEST_FALSE(occupancy.HasDynamicOccupant(GridCoord(1, 3)));
    BGE_TEST_EQ(occupancy.GetDynamicOccupant(GridCoord(1, 3)), Entity::Invalid());
    BGE_TEST_FALSE(occupancy.IsOccupied(GridCoord(1, 3)));
}

BGE_TEST(OccupancyTests, IsBlockedCombinesStaticAndDynamicState)
{
    Occupancy occupancy(GridSize(5U, 5U));

    BGE_TEST_FALSE(occupancy.IsBlocked(GridCoord(2, 2)));

    occupancy.SetStaticBlocked(GridCoord(2, 2), true);
    BGE_TEST_TRUE(occupancy.IsBlocked(GridCoord(2, 2)));

    occupancy.SetStaticBlocked(GridCoord(2, 2), false);
    BGE_TEST_FALSE(occupancy.IsBlocked(GridCoord(2, 2)));

    occupancy.SetDynamicOccupied(GridCoord(2, 2), Entity(5U, 0U));
    BGE_TEST_TRUE(occupancy.IsBlocked(GridCoord(2, 2)));
}

BGE_TEST(OccupancyTests, ClearDynamicRemovesOnlyDynamicOccupants)
{
    Occupancy occupancy(GridSize(4U, 4U));

    occupancy.SetStaticBlocked(GridCoord(0, 0), true);
    occupancy.SetStaticBlocked(GridCoord(3, 3), true);
    occupancy.SetDynamicOccupied(GridCoord(1, 1), Entity(1U, 0U));
    occupancy.SetDynamicOccupied(GridCoord(2, 2), Entity(2U, 0U));

    occupancy.ClearDynamic();

    BGE_TEST_TRUE(occupancy.IsStaticBlocked(GridCoord(0, 0)));
    BGE_TEST_TRUE(occupancy.IsStaticBlocked(GridCoord(3, 3)));
    BGE_TEST_FALSE(occupancy.HasDynamicOccupant(GridCoord(1, 1)));
    BGE_TEST_FALSE(occupancy.HasDynamicOccupant(GridCoord(2, 2)));
}

BGE_TEST(OccupancyTests, ClearResetsEverything)
{
    Occupancy occupancy(GridSize(4U, 4U));

    occupancy.SetStaticBlocked(GridCoord(0, 0), true);
    occupancy.SetDynamicOccupied(GridCoord(1, 1), Entity(9U, 0U));

    occupancy.Clear();

    BGE_TEST_FALSE(occupancy.IsStaticBlocked(GridCoord(0, 0)));
    BGE_TEST_FALSE(occupancy.HasDynamicOccupant(GridCoord(1, 1)));
    BGE_TEST_FALSE(occupancy.IsBlocked(GridCoord(0, 0)));
    BGE_TEST_FALSE(occupancy.IsBlocked(GridCoord(1, 1)));
}

BGE_TEST(OccupancyTests, OutsideCoordsBehaveSafely)
{
    Occupancy occupancy(GridSize(3U, 3U));

    BGE_TEST_TRUE(occupancy.IsStaticBlocked(GridCoord(-1, 0)));
    BGE_TEST_TRUE(occupancy.IsStaticBlocked(GridCoord(3, 0)));
    BGE_TEST_TRUE(occupancy.IsOccupied(GridCoord(0, 3)));
    BGE_TEST_TRUE(occupancy.IsBlocked(GridCoord(0, 3)));
    BGE_TEST_FALSE(occupancy.HasDynamicOccupant(GridCoord(-1, 0)));
    BGE_TEST_EQ(occupancy.GetDynamicOccupant(GridCoord(-1, 0)), Entity::Invalid());

    occupancy.SetStaticBlocked(GridCoord(-1, 0), true);
    occupancy.SetDynamicOccupied(GridCoord(10, 10), Entity(1U, 0U));
    occupancy.ClearDynamicOccupied(GridCoord(10, 10));

    BGE_TEST_FALSE(occupancy.HasDynamicOccupant(GridCoord(1, 1)));
}
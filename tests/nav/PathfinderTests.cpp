#include "nav/Pathfinder.h"
#include "tests/TestFramework.h"

using namespace bge;

namespace
{
    static bool PathStartsAt(const PathQueryResult& result, GridCoord coord)
    {
        if (!result.Succeeded() || result.path.steps.empty())
        {
            return false;
        }

        return result.path.steps.front().coord == coord;
    }

    static bool PathEndsAt(const PathQueryResult& result, GridCoord coord)
    {
        if (!result.Succeeded() || result.path.steps.empty())
        {
            return false;
        }

        return result.path.steps.back().coord == coord;
    }

    static bool PathContains(const PathQueryResult& result, GridCoord coord)
    {
        for (const PathStep& step : result.path.steps)
        {
            if (step.coord == coord)
            {
                return true;
            }
        }

        return false;
    }
}

BGE_TEST(PathfinderTests, StartOutOfBoundsReturnsCorrectStatus)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, GridCoord(-1, 0), GridCoord(2, 2));

    BGE_TEST_EQ(result.status, PathQueryStatus::StartOutOfBounds);
    BGE_TEST_TRUE(result.path.IsEmpty());
}

BGE_TEST(PathfinderTests, GoalOutOfBoundsReturnsCorrectStatus)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, GridCoord(0, 0), GridCoord(5, 5));

    BGE_TEST_EQ(result.status, PathQueryStatus::GoalOutOfBounds);
    BGE_TEST_TRUE(result.path.IsEmpty());
}

BGE_TEST(PathfinderTests, BlockedStartReturnsCorrectStatus)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    map.SetBlocked(GridCoord(1, 1), true);

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, GridCoord(1, 1), GridCoord(3, 3));

    BGE_TEST_EQ(result.status, PathQueryStatus::StartBlocked);
    BGE_TEST_TRUE(result.path.IsEmpty());
}

BGE_TEST(PathfinderTests, OccupiedStartReturnsCorrectStatus)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    occupancy.Set(GridCoord(1, 1), 42U);

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, GridCoord(1, 1), GridCoord(3, 3));

    BGE_TEST_EQ(result.status, PathQueryStatus::StartBlocked);
    BGE_TEST_TRUE(result.path.IsEmpty());
}

BGE_TEST(PathfinderTests, BlockedGoalReturnsCorrectStatus)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    map.SetBlocked(GridCoord(3, 3), true);

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, GridCoord(0, 0), GridCoord(3, 3));

    BGE_TEST_EQ(result.status, PathQueryStatus::GoalBlocked);
    BGE_TEST_TRUE(result.path.IsEmpty());
}

BGE_TEST(PathfinderTests, StartEqualsGoalReturnsSingleStepPath)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    const GridCoord cell(2, 2);

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, cell, cell);

    BGE_TEST_TRUE(result.Succeeded());
    BGE_TEST_EQ(result.status, PathQueryStatus::Success);
    BGE_TEST_EQ(result.path.StepCount(), 1U);
    BGE_TEST_TRUE(PathStartsAt(result, cell));
    BGE_TEST_TRUE(PathEndsAt(result, cell));
}

BGE_TEST(PathfinderTests, StraightLinePathWorks)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    const GridCoord start(0, 0);
    const GridCoord goal(3, 0);

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, start, goal);

    BGE_TEST_TRUE(result.Succeeded());
    BGE_TEST_EQ(result.status, PathQueryStatus::Success);
    BGE_TEST_TRUE(PathStartsAt(result, start));
    BGE_TEST_TRUE(PathEndsAt(result, goal));
    BGE_TEST_EQ(result.path.StepCount(), 4U);
}

BGE_TEST(PathfinderTests, PathAroundSingleBlockWorks)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    const GridCoord start(0, 1);
    const GridCoord goal(4, 1);

    map.SetBlocked(GridCoord(2, 1), true);

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, start, goal);

    BGE_TEST_TRUE(result.Succeeded());
    BGE_TEST_TRUE(PathStartsAt(result, start));
    BGE_TEST_TRUE(PathEndsAt(result, goal));
    BGE_TEST_FALSE(PathContains(result, GridCoord(2, 1)));
}

BGE_TEST(PathfinderTests, OccupancyBlocksIntermediateCell)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    const GridCoord start(0, 1);
    const GridCoord goal(4, 1);

    occupancy.Set(GridCoord(2, 1), 99U);

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, start, goal);

    BGE_TEST_TRUE(result.Succeeded());
    BGE_TEST_TRUE(PathStartsAt(result, start));
    BGE_TEST_TRUE(PathEndsAt(result, goal));
    BGE_TEST_FALSE(PathContains(result, GridCoord(2, 1)));
}

BGE_TEST(PathfinderTests, OccupiedGoalIsStillAllowed)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    const GridCoord start(0, 0);
    const GridCoord goal(2, 0);

    occupancy.Set(goal, 123U);

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, start, goal);

    BGE_TEST_TRUE(result.Succeeded());
    BGE_TEST_TRUE(PathStartsAt(result, start));
    BGE_TEST_TRUE(PathEndsAt(result, goal));
}

BGE_TEST(PathfinderTests, NoPathReturnsCorrectStatus)
{
    Map map(GridSize(5, 5));
    Occupancy occupancy(map.Size());
    Pathfinder pathfinder;

    const GridCoord start(2, 2);
    const GridCoord goal(4, 4);

    map.SetBlocked(GridCoord(2, 1), true);
    map.SetBlocked(GridCoord(1, 2), true);
    map.SetBlocked(GridCoord(3, 2), true);
    map.SetBlocked(GridCoord(2, 3), true);

    const PathQueryResult result =
        pathfinder.FindPath(map, occupancy, start, goal);

    BGE_TEST_EQ(result.status, PathQueryStatus::NoPath);
    BGE_TEST_TRUE(result.path.IsEmpty());
}

BGE_TEST(PathfinderTests, SameScenarioProducesDeterministicPath)
{
    Map mapA(GridSize(7, 7));
    Map mapB(GridSize(7, 7));
    Occupancy occupancyA(mapA.Size());
    Occupancy occupancyB(mapB.Size());
    Pathfinder pathfinder;

    mapA.SetBlocked(GridCoord(3, 1), true);
    mapA.SetBlocked(GridCoord(3, 2), true);
    mapA.SetBlocked(GridCoord(3, 3), true);

    mapB.SetBlocked(GridCoord(3, 1), true);
    mapB.SetBlocked(GridCoord(3, 2), true);
    mapB.SetBlocked(GridCoord(3, 3), true);

    occupancyA.Set(GridCoord(1, 3), 7U);
    occupancyB.Set(GridCoord(1, 3), 7U);

    const PathQueryResult a =
        pathfinder.FindPath(mapA, occupancyA, GridCoord(0, 3), GridCoord(6, 3));
    const PathQueryResult b =
        pathfinder.FindPath(mapB, occupancyB, GridCoord(0, 3), GridCoord(6, 3));

    BGE_TEST_EQ(a.status, b.status);
    BGE_TEST_EQ(a.path.StepCount(), b.path.StepCount());

    for (u32 i = 0; i < a.path.StepCount(); ++i)
    {
        BGE_TEST_EQ(a.path.steps[i].coord, b.path.steps[i].coord);
    }
}
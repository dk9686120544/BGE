#include "nav/Pathfinder.h"

#include "core/Assert.h"

namespace bge
{
    namespace
    {
        constexpr u32 kStepCost = 10U;
    }

    PathQueryResult Pathfinder::FindPath(
        const Map& map,
        const Occupancy& occupancy,
        GridCoord start,
        GridCoord goal) const
    {
        PathQueryResult result;

        if (!IsInBounds(map, start))
        {
            result.status = PathQueryStatus::StartOutOfBounds;
            return result;
        }

        if (!IsInBounds(map, goal))
        {
            result.status = PathQueryStatus::GoalOutOfBounds;
            return result;
        }

        if (map.IsBlocked(start) || occupancy.IsOccupied(start))
        {
            result.status = PathQueryStatus::StartBlocked;
            return result;
        }

        if (map.IsBlocked(goal))
        {
            result.status = PathQueryStatus::GoalBlocked;
            return result;
        }

        if (start == goal)
        {
            result.status = PathQueryStatus::Success;
            result.path.steps.push_back(PathStep(start));
            return result;
        }

        const u32 cellCount = map.Size().CellCount();
        std::vector<NodeRecord> records(static_cast<usize>(cellCount));
        std::vector<u32> openSet;
        openSet.reserve(static_cast<usize>(cellCount));

        const u32 startIndex = CoordToIndex(map, start);

        records[startIndex].parentIndex = -1;
        records[startIndex].gCost = 0;
        records[startIndex].hCost = Heuristic(start, goal);
        records[startIndex].inOpenSet = true;

        openSet.push_back(startIndex);

        while (!openSet.empty())
        {
            usize bestPosition = 0;
            u32 bestIndex = openSet[0];

            for (usize i = 1; i < openSet.size(); ++i)
            {
                const u32 candidateIndex = openSet[i];

                const u32 candidateF = records[candidateIndex].FCost();
                const u32 bestF = records[bestIndex].FCost();

                if (candidateF < bestF)
                {
                    bestIndex = candidateIndex;
                    bestPosition = i;
                }
                else if (candidateF == bestF)
                {
                    if (records[candidateIndex].hCost < records[bestIndex].hCost)
                    {
                        bestIndex = candidateIndex;
                        bestPosition = i;
                    }
                    else if (records[candidateIndex].hCost == records[bestIndex].hCost &&
                        candidateIndex < bestIndex)
                    {
                        bestIndex = candidateIndex;
                        bestPosition = i;
                    }
                }
            }

            openSet.erase(openSet.begin() + static_cast<std::ptrdiff_t>(bestPosition));
            records[bestIndex].inOpenSet = false;
            records[bestIndex].inClosedSet = true;

            const GridCoord current = IndexToCoord(map, bestIndex);

            if (current == goal)
            {
                BuildPath(map, start, goal, records, result);
                result.status = PathQueryStatus::Success;
                return result;
            }

            const GridCoord neighbors[4] =
            {
                GridCoord(current.x,     current.y - 1),
                GridCoord(current.x - 1, current.y),
                GridCoord(current.x + 1, current.y),
                GridCoord(current.x,     current.y + 1)
            };

            for (const GridCoord neighbor : neighbors)
            {
                if (!IsInBounds(map, neighbor))
                {
                    continue;
                }

                if (IsCoordBlocked(map, occupancy, neighbor, goal))
                {
                    continue;
                }

                const u32 neighborIndex = CoordToIndex(map, neighbor);

                if (records[neighborIndex].inClosedSet)
                {
                    continue;
                }

                AddNeighborIfBetter(map, current, neighbor, goal, records, openSet);
            }
        }

        result.status = PathQueryStatus::NoPath;
        return result;
    }

    u32 Pathfinder::Heuristic(GridCoord a, GridCoord b) noexcept
    {
        const i32 dx = (a.x > b.x) ? (a.x - b.x) : (b.x - a.x);
        const i32 dy = (a.y > b.y) ? (a.y - b.y) : (b.y - a.y);

        return static_cast<u32>(dx + dy) * kStepCost;
    }

    bool Pathfinder::IsCoordBlocked(
        const Map& map,
        const Occupancy& occupancy,
        GridCoord coord,
        GridCoord goal) noexcept
    {
        if (map.IsBlocked(coord))
        {
            return true;
        }

        if (coord == goal)
        {
            return false;
        }

        return occupancy.IsOccupied(coord);
    }

    void Pathfinder::BuildPath(
        const Map& map,
        GridCoord start,
        GridCoord goal,
        const std::vector<NodeRecord>& records,
        PathQueryResult& result)
    {
        result.path.Clear();

        u32 currentIndex = CoordToIndex(map, goal);

        for (;;)
        {
            const GridCoord coord = IndexToCoord(map, currentIndex);
            result.path.steps.push_back(PathStep(coord));

            if (coord == start)
            {
                break;
            }

            const i32 parentIndex = records[currentIndex].parentIndex;
            BGE_ASSERT(parentIndex >= 0);

            currentIndex = static_cast<u32>(parentIndex);
        }

        std::vector<PathStep>& steps = result.path.steps;

        const usize count = steps.size();
        for (usize i = 0; i < count / 2; ++i)
        {
            const usize j = count - 1 - i;
            const PathStep temp = steps[i];
            steps[i] = steps[j];
            steps[j] = temp;
        }
    }

    void Pathfinder::AddNeighborIfBetter(
        const Map& map,
        GridCoord current,
        GridCoord neighbor,
        GridCoord goal,
        std::vector<NodeRecord>& records,
        std::vector<u32>& openSet)
    {
        const u32 currentIndex = CoordToIndex(map, current);
        const u32 neighborIndex = CoordToIndex(map, neighbor);

        const u32 tentativeG = records[currentIndex].gCost + kStepCost;

        if (!records[neighborIndex].inOpenSet || tentativeG < records[neighborIndex].gCost)
        {
            records[neighborIndex].parentIndex = static_cast<i32>(currentIndex);
            records[neighborIndex].gCost = tentativeG;
            records[neighborIndex].hCost = Heuristic(neighbor, goal);

            if (!records[neighborIndex].inOpenSet)
            {
                records[neighborIndex].inOpenSet = true;
                openSet.push_back(neighborIndex);
            }
        }
    }

    u32 Pathfinder::CoordToIndex(const Map& map, GridCoord coord) noexcept
    {
        BGE_ASSERT(IsInBounds(map, coord));

        const u32 x = static_cast<u32>(coord.x);
        const u32 y = static_cast<u32>(coord.y);

        return y * map.Width() + x;
    }

    GridCoord Pathfinder::IndexToCoord(const Map& map, u32 index) noexcept
    {
        BGE_ASSERT(map.Width() > 0);
        BGE_ASSERT(index < map.Size().CellCount());

        const u32 x = index % map.Width();
        const u32 y = index / map.Width();

        return GridCoord(static_cast<i32>(x), static_cast<i32>(y));
    }

    bool Pathfinder::IsInBounds(const Map& map, GridCoord coord) noexcept
    {
        return map.IsInBounds(coord);
    }
}
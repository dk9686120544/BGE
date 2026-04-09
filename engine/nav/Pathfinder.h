#pragma once

#include "nav/PathTypes.h"
#include "world/Map.h"
#include "world/Occupancy.h"

#include <vector>

namespace bge
{
    class Pathfinder
    {
    public:
        Pathfinder() = default;

        PathQueryResult FindPath(
            const Map& map,
            const Occupancy& occupancy,
            GridCoord start,
            GridCoord goal) const;

    private:
        struct NodeRecord
        {
            i32 parentIndex;
            u32 gCost;
            u32 hCost;
            bool inOpenSet;
            bool inClosedSet;

            NodeRecord() noexcept
                : parentIndex(-1)
                , gCost(0)
                , hCost(0)
                , inOpenSet(false)
                , inClosedSet(false)
            {
            }

            u32 FCost() const noexcept
            {
                return gCost + hCost;
            }
        };

    private:
        static u32 Heuristic(GridCoord a, GridCoord b) noexcept;
        static bool IsCoordBlocked(
            const Map& map,
            const Occupancy& occupancy,
            GridCoord coord,
            GridCoord goal) noexcept;

        static void BuildPath(
            const Map& map,
            GridCoord start,
            GridCoord goal,
            const std::vector<NodeRecord>& records,
            PathQueryResult& result);

        static void AddNeighborIfBetter(
            const Map& map,
            GridCoord current,
            GridCoord neighbor,
            GridCoord goal,
            std::vector<NodeRecord>& records,
            std::vector<u32>& openSet);

        static u32 CoordToIndex(const Map& map, GridCoord coord) noexcept;
        static GridCoord IndexToCoord(const Map& map, u32 index) noexcept;
        static bool IsInBounds(const Map& map, GridCoord coord) noexcept;
    };
}
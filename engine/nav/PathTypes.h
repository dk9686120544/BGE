#pragma once

#include "core/Types.h"
#include "world/WorldTypes.h"

#include <vector>

namespace bge
{
    struct PathStep
    {
        GridCoord coord;

        constexpr PathStep() noexcept
            : coord()
        {
        }

        explicit constexpr PathStep(GridCoord inCoord) noexcept
            : coord(inCoord)
        {
        }

        constexpr bool operator==(const PathStep& other) const noexcept
        {
            return coord == other.coord;
        }

        constexpr bool operator!=(const PathStep& other) const noexcept
        {
            return !(*this == other);
        }
    };

    struct Path
    {
        std::vector<PathStep> steps;

        bool IsEmpty() const noexcept
        {
            return steps.empty();
        }

        u32 StepCount() const noexcept
        {
            return static_cast<u32>(steps.size());
        }

        void Clear() noexcept
        {
            steps.clear();
        }
    };

    enum class PathQueryStatus : u8
    {
        Success = 0,
        StartOutOfBounds,
        GoalOutOfBounds,
        StartBlocked,
        GoalBlocked,
        NoPath
    };

    struct PathQueryResult
    {
        PathQueryStatus status;
        Path path;

        constexpr PathQueryResult() noexcept
            : status(PathQueryStatus::NoPath)
            , path()
        {
        }

        bool Succeeded() const noexcept
        {
            return status == PathQueryStatus::Success;
        }

        void Clear() noexcept
        {
            status = PathQueryStatus::NoPath;
            path.Clear();
        }
    };
}
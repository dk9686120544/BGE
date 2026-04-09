#pragma once

#include "core/Types.h"
#include "ecs/Entity.h"
#include "world/WorldTypes.h"

#include <vector>

namespace bge
{
    struct OccupancyCell
    {
        bool blockedStatic;
        bool occupiedDynamic;
        Entity dynamicOccupant;

        constexpr OccupancyCell() noexcept
            : blockedStatic(false)
            , occupiedDynamic(false)
            , dynamicOccupant(Entity::Invalid())
        {
        }
    };

    class Occupancy
    {
    public:
        Occupancy() noexcept;
        explicit Occupancy(const GridSize& size);

        void Resize(const GridSize& size);
        void Clear() noexcept;
        void ClearDynamic() noexcept;

        const GridSize& Size() const noexcept;
        bool IsEmpty() const noexcept;

        bool IsInside(const GridCoord& coord) const noexcept;

        void SetStaticBlocked(const GridCoord& coord, bool blocked) noexcept;
        bool IsStaticBlocked(const GridCoord& coord) const noexcept;

        void SetDynamicOccupied(const GridCoord& coord, Entity occupant) noexcept;
        void ClearDynamicOccupied(const GridCoord& coord) noexcept;

        bool HasDynamicOccupant(const GridCoord& coord) const noexcept;
        Entity GetDynamicOccupant(const GridCoord& coord) const noexcept;

        bool IsOccupied(const GridCoord& coord) const noexcept;
        bool IsBlocked(const GridCoord& coord) const noexcept;

        // compatibility API for older pathfinder/grid tests and callers
        void Set(const GridCoord& coord, CellType type) noexcept;
        void Set(const GridCoord& coord, u32 value) noexcept;
        void Set(const GridCoord& coord, bool blocked) noexcept;
        CellType Get(const GridCoord& coord) const noexcept;

    private:
        OccupancyCell* TryGetCell(const GridCoord& coord) noexcept;
        const OccupancyCell* TryGetCell(const GridCoord& coord) const noexcept;

    private:
        GridSize m_Size;
        std::vector<OccupancyCell> m_Cells;
    };
}
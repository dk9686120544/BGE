#pragma once

#include "world/Grid.h"

namespace bge
{
    class Map
    {
    public:
        Map() noexcept = default;
        explicit Map(GridSize size);

        void Resize(GridSize size);
        void Clear(CellType value = CellType::Empty) noexcept;

        GridSize Size() const noexcept;
        u32 Width() const noexcept;
        u32 Height() const noexcept;
        bool IsEmpty() const noexcept;

        bool IsInBounds(GridCoord coord) const noexcept;

        bool IsWalkable(GridCoord coord) const noexcept;
        bool IsBlocked(GridCoord coord) const noexcept;

        void SetWalkable(GridCoord coord, bool walkable) noexcept;
        void SetBlocked(GridCoord coord, bool blocked) noexcept;
        void SetCell(GridCoord coord, CellType type) noexcept;

        CellType GetCell(GridCoord coord) const noexcept;

        const Grid& Cells() const noexcept;
        Grid& Cells() noexcept;

    private:
        Grid m_Cells;
    };
}
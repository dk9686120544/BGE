#pragma once

#include "core/Assert.h"
#include "core/Types.h"
#include "world/WorldTypes.h"

#include <vector>

namespace bge
{
    class Grid
    {
    public:
        Grid() noexcept = default;
        explicit Grid(GridSize size);

        void Resize(GridSize size);
        void Clear(CellType value = CellType::Empty) noexcept;

        GridSize Size() const noexcept;
        u32 Width() const noexcept;
        u32 Height() const noexcept;
        bool IsEmpty() const noexcept;

        bool IsInBounds(GridCoord coord) const noexcept;
        CellIndex ToIndex(GridCoord coord) const noexcept;
        GridCoord ToCoord(CellIndex index) const noexcept;

        CellType Get(GridCoord coord) const noexcept;
        void Set(GridCoord coord, CellType value) noexcept;

        const std::vector<CellType>& Cells() const noexcept;
        std::vector<CellType>& Cells() noexcept;

    private:
        GridSize m_Size;
        std::vector<CellType> m_Cells;
    };
}
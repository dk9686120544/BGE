#include "world/Grid.h"

namespace bge
{
    Grid::Grid(GridSize size)
        : m_Size()
        , m_Cells()
    {
        Resize(size);
    }

    void Grid::Resize(GridSize size)
    {
        m_Size = size;
        m_Cells.resize(static_cast<usize>(m_Size.CellCount()), CellType::Empty);
    }

    void Grid::Clear(CellType value) noexcept
    {
        const usize count = m_Cells.size();

        for (usize i = 0; i < count; ++i)
        {
            m_Cells[i] = value;
        }
    }

    GridSize Grid::Size() const noexcept
    {
        return m_Size;
    }

    u32 Grid::Width() const noexcept
    {
        return m_Size.width;
    }

    u32 Grid::Height() const noexcept
    {
        return m_Size.height;
    }

    bool Grid::IsEmpty() const noexcept
    {
        return m_Size.IsEmpty();
    }

    bool Grid::IsInBounds(GridCoord coord) const noexcept
    {
        if (coord.x < 0 || coord.y < 0)
        {
            return false;
        }

        const u32 x = static_cast<u32>(coord.x);
        const u32 y = static_cast<u32>(coord.y);

        return x < m_Size.width && y < m_Size.height;
    }

    CellIndex Grid::ToIndex(GridCoord coord) const noexcept
    {
        BGE_ASSERT(IsInBounds(coord));

        const u32 x = static_cast<u32>(coord.x);
        const u32 y = static_cast<u32>(coord.y);

        return CellIndex(y * m_Size.width + x);
    }

    GridCoord Grid::ToCoord(CellIndex index) const noexcept
    {
        BGE_ASSERT(m_Size.width > 0);
        BGE_ASSERT(index.value < m_Size.CellCount());

        const u32 x = index.value % m_Size.width;
        const u32 y = index.value / m_Size.width;

        return GridCoord(static_cast<i32>(x), static_cast<i32>(y));
    }

    CellType Grid::Get(GridCoord coord) const noexcept
    {
        BGE_ASSERT(IsInBounds(coord));
        return m_Cells[ToIndex(coord).value];
    }

    void Grid::Set(GridCoord coord, CellType value) noexcept
    {
        BGE_ASSERT(IsInBounds(coord));
        m_Cells[ToIndex(coord).value] = value;
    }

    const std::vector<CellType>& Grid::Cells() const noexcept
    {
        return m_Cells;
    }

    std::vector<CellType>& Grid::Cells() noexcept
    {
        return m_Cells;
    }
}
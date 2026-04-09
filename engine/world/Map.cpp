#include "world/Map.h"

namespace bge
{
    Map::Map(GridSize size)
        : m_Cells(size)
    {
    }

    void Map::Resize(GridSize size)
    {
        m_Cells.Resize(size);
    }

    void Map::Clear(CellType value) noexcept
    {
        m_Cells.Clear(value);
    }

    GridSize Map::Size() const noexcept
    {
        return m_Cells.Size();
    }

    u32 Map::Width() const noexcept
    {
        return m_Cells.Width();
    }

    u32 Map::Height() const noexcept
    {
        return m_Cells.Height();
    }

    bool Map::IsEmpty() const noexcept
    {
        return m_Cells.IsEmpty();
    }

    bool Map::IsInBounds(GridCoord coord) const noexcept
    {
        return m_Cells.IsInBounds(coord);
    }

    bool Map::IsWalkable(GridCoord coord) const noexcept
    {
        return IsInBounds(coord) && GetCell(coord) == CellType::Empty;
    }

    bool Map::IsBlocked(GridCoord coord) const noexcept
    {
        return !IsWalkable(coord);
    }

    void Map::SetWalkable(GridCoord coord, bool walkable) noexcept
    {
        SetCell(coord, walkable ? CellType::Empty : CellType::Blocked);
    }

    void Map::SetBlocked(GridCoord coord, bool blocked) noexcept
    {
        SetCell(coord, blocked ? CellType::Blocked : CellType::Empty);
    }

    void Map::SetCell(GridCoord coord, CellType type) noexcept
    {
        m_Cells.Set(coord, type);
    }

    CellType Map::GetCell(GridCoord coord) const noexcept
    {
        return m_Cells.Get(coord);
    }

    const Grid& Map::Cells() const noexcept
    {
        return m_Cells;
    }

    Grid& Map::Cells() noexcept
    {
        return m_Cells;
    }
}
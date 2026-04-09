#include "world/Occupancy.h"

namespace bge
{
    Occupancy::Occupancy() noexcept
        : m_Size()
        , m_Cells()
    {
    }

    Occupancy::Occupancy(const GridSize& size)
        : m_Size()
        , m_Cells()
    {
        Resize(size);
    }

    void Occupancy::Resize(const GridSize& size)
    {
        m_Size = size;
        m_Cells.clear();
        m_Cells.resize(static_cast<usize>(size.width) * static_cast<usize>(size.height));
    }

    void Occupancy::Clear() noexcept
    {
        for (OccupancyCell& cell : m_Cells)
        {
            cell.blockedStatic = false;
            cell.occupiedDynamic = false;
            cell.dynamicOccupant = Entity::Invalid();
        }
    }

    void Occupancy::ClearDynamic() noexcept
    {
        for (OccupancyCell& cell : m_Cells)
        {
            cell.occupiedDynamic = false;
            cell.dynamicOccupant = Entity::Invalid();
        }
    }

    const GridSize& Occupancy::Size() const noexcept
    {
        return m_Size;
    }

    bool Occupancy::IsEmpty() const noexcept
    {
        return m_Cells.empty();
    }

    bool Occupancy::IsInside(const GridCoord& coord) const noexcept
    {
        return WorldTypes::IsInside(coord, m_Size);
    }

    void Occupancy::SetStaticBlocked(const GridCoord& coord, bool blocked) noexcept
    {
        OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return;
        }

        cell->blockedStatic = blocked;
    }

    bool Occupancy::IsStaticBlocked(const GridCoord& coord) const noexcept
    {
        const OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return true;
        }

        return cell->blockedStatic;
    }

    void Occupancy::SetDynamicOccupied(const GridCoord& coord, Entity occupant) noexcept
    {
        OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return;
        }

        cell->occupiedDynamic = true;
        cell->dynamicOccupant = occupant;
    }

    void Occupancy::ClearDynamicOccupied(const GridCoord& coord) noexcept
    {
        OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return;
        }

        cell->occupiedDynamic = false;
        cell->dynamicOccupant = Entity::Invalid();
    }

    bool Occupancy::HasDynamicOccupant(const GridCoord& coord) const noexcept
    {
        const OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return false;
        }

        return cell->occupiedDynamic;
    }

    Entity Occupancy::GetDynamicOccupant(const GridCoord& coord) const noexcept
    {
        const OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return Entity::Invalid();
        }

        return cell->dynamicOccupant;
    }

    bool Occupancy::IsOccupied(const GridCoord& coord) const noexcept
    {
        const OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return true;
        }

        return cell->occupiedDynamic;
    }

    bool Occupancy::IsBlocked(const GridCoord& coord) const noexcept
    {
        const OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return true;
        }

        return cell->blockedStatic || cell->occupiedDynamic;
    }

    void Occupancy::Set(const GridCoord& coord, CellType type) noexcept
    {
        OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return;
        }

        const bool occupied = (type == CellType::Blocked);
        cell->occupiedDynamic = occupied;
        cell->dynamicOccupant = occupied ? Entity(0U, 0U) : Entity::Invalid();
    }

    void Occupancy::Set(const GridCoord& coord, u32 value) noexcept
    {
        OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return;
        }

        const bool occupied = (value != 0U);
        cell->occupiedDynamic = occupied;
        cell->dynamicOccupant = occupied ? Entity(0U, 0U) : Entity::Invalid();
    }

    void Occupancy::Set(const GridCoord& coord, bool blocked) noexcept
    {
        OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return;
        }

        cell->occupiedDynamic = blocked;
        cell->dynamicOccupant = blocked ? Entity(0U, 0U) : Entity::Invalid();
    }

    CellType Occupancy::Get(const GridCoord& coord) const noexcept
    {
        const OccupancyCell* cell = TryGetCell(coord);
        if (cell == nullptr)
        {
            return CellType::Blocked;
        }

        return (cell->blockedStatic || cell->occupiedDynamic)
            ? CellType::Blocked
            : CellType::Empty;
    }

    OccupancyCell* Occupancy::TryGetCell(const GridCoord& coord) noexcept
    {
        if (!IsInside(coord))
        {
            return nullptr;
        }

        const CellIndex index = WorldTypes::Flatten(coord, m_Size);
        return &m_Cells[static_cast<usize>(index.value)];
    }

    const OccupancyCell* Occupancy::TryGetCell(const GridCoord& coord) const noexcept
    {
        if (!IsInside(coord))
        {
            return nullptr;
        }

        const CellIndex index = WorldTypes::Flatten(coord, m_Size);
        return &m_Cells[static_cast<usize>(index.value)];
    }
}
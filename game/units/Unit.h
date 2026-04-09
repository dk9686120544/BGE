#pragma once

#include "ecs/Entity.h"
#include "movement/MovementTypes.h"
#include "world/WorldTypes.h"

namespace bge
{
    struct Unit
    {
        Entity entity;
        MovementState movement;

        GridCoord occupiedCell;
        bool hasOccupiedCell;

        constexpr Unit() noexcept
            : entity(Entity::Invalid())
            , movement()
            , occupiedCell(GridCoord::Invalid())
            , hasOccupiedCell(false)
        {
        }

        explicit constexpr Unit(Entity inEntity) noexcept
            : entity(inEntity)
            , movement()
            , occupiedCell(GridCoord::Invalid())
            , hasOccupiedCell(false)
        {
        }

        constexpr bool IsValid() const noexcept
        {
            return entity.IsValid();
        }

        constexpr void ClearOccupiedCell() noexcept
        {
            occupiedCell = GridCoord::Invalid();
            hasOccupiedCell = false;
        }

        constexpr void SetOccupiedCell(const GridCoord& cell) noexcept
        {
            occupiedCell = cell;
            hasOccupiedCell = true;
        }
    };
}
#pragma once

#include "core/Types.h"

namespace bge
{
    struct GridCoord
    {
        i32 x;
        i32 y;

        constexpr GridCoord() noexcept
            : x(0)
            , y(0)
        {
        }

        constexpr GridCoord(i32 inX, i32 inY) noexcept
            : x(inX)
            , y(inY)
        {
        }

        constexpr bool operator==(const GridCoord& other) const noexcept
        {
            return x == other.x && y == other.y;
        }

        constexpr bool operator!=(const GridCoord& other) const noexcept
        {
            return !(*this == other);
        }
    };

    struct GridSize
    {
        u32 width;
        u32 height;

        constexpr GridSize() noexcept
            : width(0)
            , height(0)
        {
        }

        constexpr GridSize(u32 inWidth, u32 inHeight) noexcept
            : width(inWidth)
            , height(inHeight)
        {
        }

        constexpr bool IsEmpty() const noexcept
        {
            return width == 0 || height == 0;
        }

        constexpr u32 CellCount() const noexcept
        {
            return width * height;
        }

        constexpr bool operator==(const GridSize& other) const noexcept
        {
            return width == other.width && height == other.height;
        }

        constexpr bool operator!=(const GridSize& other) const noexcept
        {
            return !(*this == other);
        }
    };

    enum class CellType : u8
    {
        Empty = 0,
        Blocked = 1
    };

    struct CellIndex
    {
        u32 value;

        constexpr CellIndex() noexcept
            : value(0)
        {
        }

        explicit constexpr CellIndex(u32 inValue) noexcept
            : value(inValue)
        {
        }

        constexpr bool operator==(const CellIndex& other) const noexcept
        {
            return value == other.value;
        }

        constexpr bool operator!=(const CellIndex& other) const noexcept
        {
            return !(*this == other);
        }
    };
}
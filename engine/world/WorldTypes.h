#pragma once

#include "core/Types.h"
#include "math/Scalar.h"
#include "math/Vec2.h"

namespace bge
{
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

    enum class CellType : u8
    {
        Empty = 0,
        Blocked = 1
    };

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

        static constexpr GridCoord Invalid() noexcept
        {
            return GridCoord(-1, -1);
        }

        constexpr bool IsValid() const noexcept
        {
            return x >= 0 && y >= 0;
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

    struct WorldRect
    {
        GridCoord min;
        GridCoord max;

        constexpr WorldRect() noexcept
            : min()
            , max()
        {
        }

        constexpr WorldRect(const GridCoord& inMin, const GridCoord& inMax) noexcept
            : min(inMin)
            , max(inMax)
        {
        }
    };

    namespace WorldTypes
    {
        constexpr bool IsInside(const GridCoord& coord, const GridSize& size) noexcept
        {
            return coord.x >= 0 &&
                coord.y >= 0 &&
                static_cast<u32>(coord.x) < size.width &&
                static_cast<u32>(coord.y) < size.height;
        }

        constexpr CellIndex Flatten(const GridCoord& coord, const GridSize& size) noexcept
        {
            return CellIndex(
                static_cast<u32>(coord.y) * size.width + static_cast<u32>(coord.x));
        }

        inline GridCoord WorldToCell(const Vec2& position) noexcept
        {
            return GridCoord(
                position.x.TruncToInt(),
                position.y.TruncToInt());
        }

        inline Vec2 CellToWorldCenter(const GridCoord& coord) noexcept
        {
            return Vec2(
                Scalar::FromInt(coord.x) + Scalar::Half(),
                Scalar::FromInt(coord.y) + Scalar::Half());
        }

        inline Vec2 CellToWorldMin(const GridCoord& coord) noexcept
        {
            return Vec2(
                Scalar::FromInt(coord.x),
                Scalar::FromInt(coord.y));
        }

        inline Vec2 CellToWorldMax(const GridCoord& coord) noexcept
        {
            return Vec2(
                Scalar::FromInt(coord.x + 1),
                Scalar::FromInt(coord.y + 1));
        }
    }
}
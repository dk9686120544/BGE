#pragma once

#include "core/Types.h"
#include "math/Scalar.h"
#include "math/Vec2.h"

#include <vector>

namespace bge
{
    struct FormationSlot
    {
        Vec2 position;

        constexpr FormationSlot() noexcept
            : position(Vec2::Zero())
        {
        }

        explicit constexpr FormationSlot(const Vec2& inPosition) noexcept
            : position(inPosition)
        {
        }

        constexpr bool operator==(const FormationSlot& other) const noexcept
        {
            return position == other.position;
        }

        constexpr bool operator!=(const FormationSlot& other) const noexcept
        {
            return !(*this == other);
        }
    };

    struct FormationLayout
    {
        Vec2 center;
        Scalar spacing;
        u32 rows;
        u32 columns;
        std::vector<FormationSlot> slots;

        FormationLayout() noexcept
            : center(Vec2::Zero())
            , spacing(Scalar::One())
            , rows(0)
            , columns(0)
            , slots()
        {
        }

        void Clear() noexcept
        {
            center = Vec2::Zero();
            spacing = Scalar::One();
            rows = 0;
            columns = 0;
            slots.clear();
        }

        bool IsEmpty() const noexcept
        {
            return slots.empty();
        }

        u32 SlotCount() const noexcept
        {
            return static_cast<u32>(slots.size());
        }
    };

    class Formation
    {
    public:
        static FormationLayout BuildRectangular(
            const Vec2& center,
            u32 unitCount,
            Scalar spacing);
    };
}
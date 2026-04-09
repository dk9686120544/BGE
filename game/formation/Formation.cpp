#include "game/formation/Formation.h"

namespace bge
{
    FormationLayout Formation::BuildRectangular(
        const Vec2& center,
        u32 unitCount,
        Scalar spacing)
    {
        FormationLayout layout;
        layout.Clear();

        layout.center = center;
        layout.spacing = spacing;

        if (unitCount == 0)
        {
            return layout;
        }

        u32 columns = 1;
        while (columns * columns < unitCount)
        {
            ++columns;
        }

        const u32 rows = (unitCount + columns - 1) / columns;

        layout.rows = rows;
        layout.columns = columns;
        layout.slots.reserve(static_cast<usize>(unitCount));

        const Scalar half = Scalar::Half();

        const Scalar totalWidth =
            Scalar::FromInt(static_cast<i32>(columns - 1)) * spacing;

        const Scalar totalHeight =
            Scalar::FromInt(static_cast<i32>(rows - 1)) * spacing;

        const Scalar startX = center.x - (totalWidth * half);
        const Scalar startY = center.y - (totalHeight * half);

        u32 created = 0;

        for (u32 row = 0; row < rows && created < unitCount; ++row)
        {
            for (u32 column = 0; column < columns && created < unitCount; ++column)
            {
                const Vec2 slotPosition(
                    startX + Scalar::FromInt(static_cast<i32>(column)) * spacing,
                    startY + Scalar::FromInt(static_cast<i32>(row)) * spacing);

                layout.slots.push_back(FormationSlot(slotPosition));
                ++created;
            }
        }

        return layout;
    }
}
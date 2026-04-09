#include "game/formation/FormationPlanner.h"

namespace bge
{
    namespace
    {
        struct SlotCandidate
        {
            u32 slotIndex;
            Scalar distanceSq;

            SlotCandidate() noexcept
                : slotIndex(0)
                , distanceSq(Scalar::Zero())
            {
            }

            SlotCandidate(u32 inSlotIndex, Scalar inDistanceSq) noexcept
                : slotIndex(inSlotIndex)
                , distanceSq(inDistanceSq)
            {
            }
        };
    }

    FormationPlan FormationPlanner::BuildMoveFormation(
        const std::vector<Unit*>& units,
        const Vec2& center,
        Scalar spacing)
    {
        FormationPlan plan;
        plan.Clear();

        if (units.empty())
        {
            return plan;
        }

        plan.layout = Formation::BuildRectangular(
            center,
            static_cast<u32>(units.size()),
            spacing);

        const u32 slotCount = plan.layout.SlotCount();
        if (slotCount == 0)
        {
            return plan;
        }

        std::vector<bool> slotUsed(static_cast<usize>(slotCount), false);
        plan.assignments.reserve(units.size());

        for (const Unit* unit : units)
        {
            if (unit == nullptr || !unit->IsValid())
            {
                continue;
            }

            bool found = false;
            SlotCandidate bestCandidate;

            for (u32 slotIndex = 0; slotIndex < slotCount; ++slotIndex)
            {
                if (slotUsed[slotIndex])
                {
                    continue;
                }

                const Vec2 delta =
                    plan.layout.slots[slotIndex].position - unit->movement.position;

                const Scalar distanceSq = Vec2::LengthSq(delta);

                if (!found)
                {
                    bestCandidate = SlotCandidate(slotIndex, distanceSq);
                    found = true;
                    continue;
                }

                if (distanceSq < bestCandidate.distanceSq)
                {
                    bestCandidate = SlotCandidate(slotIndex, distanceSq);
                }
                else if (distanceSq == bestCandidate.distanceSq &&
                    slotIndex < bestCandidate.slotIndex)
                {
                    bestCandidate = SlotCandidate(slotIndex, distanceSq);
                }
            }

            if (!found)
            {
                continue;
            }

            slotUsed[bestCandidate.slotIndex] = true;

            plan.assignments.push_back(
                FormationAssignment(
                    unit->entity,
                    plan.layout.slots[bestCandidate.slotIndex].position));
        }

        return plan;
    }
}
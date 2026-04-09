#pragma once

#include "ecs/Entity.h"
#include "game/formation/Formation.h"
#include "game/units/Unit.h"
#include "math/Vec2.h"

#include <vector>

namespace bge
{
    struct FormationAssignment
    {
        Entity entity;
        Vec2 targetPosition;

        constexpr FormationAssignment() noexcept
            : entity(Entity::Invalid())
            , targetPosition(Vec2::Zero())
        {
        }

        constexpr FormationAssignment(Entity inEntity, const Vec2& inTargetPosition) noexcept
            : entity(inEntity)
            , targetPosition(inTargetPosition)
        {
        }
    };

    struct FormationPlan
    {
        FormationLayout layout;
        std::vector<FormationAssignment> assignments;

        void Clear() noexcept
        {
            layout.Clear();
            assignments.clear();
        }

        bool IsEmpty() const noexcept
        {
            return assignments.empty();
        }

        u32 AssignmentCount() const noexcept
        {
            return static_cast<u32>(assignments.size());
        }
    };

    class FormationPlanner
    {
    public:
        static FormationPlan BuildMoveFormation(
            const std::vector<Unit*>& units,
            const Vec2& center,
            Scalar spacing);
    };
}
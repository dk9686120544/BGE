#pragma once

#include "ecs/Entity.h"
#include "math/Vec2.h"
#include "sim/Command.h"

namespace bge
{
    class MoveCommand final : public Command
    {
    public:
        MoveCommand(Entity entity, const Vec2& targetPosition) noexcept
            : m_Entity(entity)
            , m_TargetPosition(targetPosition)
        {
        }

        Entity GetEntity() const noexcept
        {
            return m_Entity;
        }

        const Vec2& GetTargetPosition() const noexcept
        {
            return m_TargetPosition;
        }

    private:
        Entity m_Entity;
        Vec2 m_TargetPosition;
    };
}
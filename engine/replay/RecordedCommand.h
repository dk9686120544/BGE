#pragma once

#include "core/Types.h"
#include "ecs/Entity.h"
#include "math/Vec2.h"

namespace bge
{
    enum class RecordedCommandType : u8
    {
        None = 0,
        Move = 1
    };

    struct RecordedMoveCommand
    {
        Entity entity;
        Vec2 targetPosition;

        constexpr RecordedMoveCommand() noexcept
            : entity(Entity::Invalid())
            , targetPosition(Vec2::Zero())
        {
        }

        constexpr RecordedMoveCommand(
            Entity inEntity,
            const Vec2& inTargetPosition) noexcept
            : entity(inEntity)
            , targetPosition(inTargetPosition)
        {
        }

        constexpr bool operator==(const RecordedMoveCommand& other) const noexcept
        {
            return entity == other.entity &&
                   targetPosition == other.targetPosition;
        }

        constexpr bool operator!=(const RecordedMoveCommand& other) const noexcept
        {
            return !(*this == other);
        }
    };

    struct RecordedCommand
    {
        u32 tick;
        RecordedCommandType type;
        RecordedMoveCommand move;

        constexpr RecordedCommand() noexcept
            : tick(0)
            , type(RecordedCommandType::None)
            , move()
        {
        }

        static constexpr RecordedCommand MakeMove(
            u32 tickValue,
            Entity entity,
            const Vec2& targetPosition) noexcept
        {
            RecordedCommand command;
            command.tick = tickValue;
            command.type = RecordedCommandType::Move;
            command.move = RecordedMoveCommand(entity, targetPosition);
            return command;
        }

        constexpr bool IsValid() const noexcept
        {
            switch (type)
            {
            case RecordedCommandType::Move:
                return move.entity.IsValid();

            case RecordedCommandType::None:
            default:
                return false;
            }
        }

        constexpr bool operator==(const RecordedCommand& other) const noexcept
        {
            return tick == other.tick &&
                   type == other.type &&
                   move == other.move;
        }

        constexpr bool operator!=(const RecordedCommand& other) const noexcept
        {
            return !(*this == other);
        }
    };
}
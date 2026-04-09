#pragma once

#include "replay/RecordedCommand.h"

#include <vector>

namespace bge
{
    class CommandRecorder
    {
    public:
        CommandRecorder() noexcept = default;

        void Clear() noexcept;

        void RecordMove(
            u32 tick,
            Entity entity,
            const Vec2& targetPosition);

        const std::vector<RecordedCommand>& Commands() const noexcept;
        std::vector<RecordedCommand>& Commands() noexcept;

        bool IsEmpty() const noexcept;
        u32 Count() const noexcept;

    private:
        std::vector<RecordedCommand> m_Commands;
    };
}
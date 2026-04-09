#pragma once

#include "replay/RecordedCommand.h"

#include <vector>

namespace bge
{
    class GameSession;

    class CommandPlayback
    {
    public:
        CommandPlayback() noexcept = default;

        void SetCommands(const std::vector<RecordedCommand>& commands);
        void Clear() noexcept;
        void Reset() noexcept;

        void PlaybackTick(
            u32 tick,
            GameSession& session);

        bool IsFinished() const noexcept;
        u32 NextCommandIndex() const noexcept;
        u32 CommandCount() const noexcept;

        const std::vector<RecordedCommand>& Commands() const noexcept;

    private:
        std::vector<RecordedCommand> m_Commands;
        u32 m_NextCommandIndex = 0;
    };
}
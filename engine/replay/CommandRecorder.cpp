#include "replay/CommandRecorder.h"

namespace bge
{
    void CommandRecorder::Clear() noexcept
    {
        m_Commands.clear();
    }

    void CommandRecorder::RecordMove(
        u32 tick,
        Entity entity,
        const Vec2& targetPosition)
    {
        m_Commands.push_back(
            RecordedCommand::MakeMove(
                tick,
                entity,
                targetPosition));
    }

    const std::vector<RecordedCommand>& CommandRecorder::Commands() const noexcept
    {
        return m_Commands;
    }

    std::vector<RecordedCommand>& CommandRecorder::Commands() noexcept
    {
        return m_Commands;
    }

    bool CommandRecorder::IsEmpty() const noexcept
    {
        return m_Commands.empty();
    }

    u32 CommandRecorder::Count() const noexcept
    {
        return static_cast<u32>(m_Commands.size());
    }
}
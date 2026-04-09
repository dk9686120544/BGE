#include "replay/CommandPlayback.h"

#include "game/session/GameSession.h"

namespace bge
{
    void CommandPlayback::SetCommands(const std::vector<RecordedCommand>& commands)
    {
        m_Commands = commands;
        m_NextCommandIndex = 0;
    }

    void CommandPlayback::Clear() noexcept
    {
        m_Commands.clear();
        m_NextCommandIndex = 0;
    }

    void CommandPlayback::Reset() noexcept
    {
        m_NextCommandIndex = 0;
    }

    void CommandPlayback::PlaybackTick(
        u32 tick,
        GameSession& session)
    {
        while (m_NextCommandIndex < static_cast<u32>(m_Commands.size()))
        {
            const RecordedCommand& command = m_Commands[m_NextCommandIndex];

            if (command.tick > tick)
            {
                break;
            }

            if (command.tick == tick)
            {
                switch (command.type)
                {
                case RecordedCommandType::Move:
                    if (command.move.entity.IsValid())
                    {
                        session.QueueMoveCommand(
                            command.move.entity,
                            command.move.targetPosition);
                    }
                    break;

                case RecordedCommandType::None:
                default:
                    break;
                }
            }

            ++m_NextCommandIndex;
        }
    }

    bool CommandPlayback::IsFinished() const noexcept
    {
        return m_NextCommandIndex >= static_cast<u32>(m_Commands.size());
    }

    u32 CommandPlayback::NextCommandIndex() const noexcept
    {
        return m_NextCommandIndex;
    }

    u32 CommandPlayback::CommandCount() const noexcept
    {
        return static_cast<u32>(m_Commands.size());
    }

    const std::vector<RecordedCommand>& CommandPlayback::Commands() const noexcept
    {
        return m_Commands;
    }
}
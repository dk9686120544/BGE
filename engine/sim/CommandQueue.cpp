#include "sim/CommandQueue.h"

#include "core/Assert.h"

namespace bge
{
    void CommandQueue::Push(std::unique_ptr<Command> command)
    {
        BGE_ASSERT(command != nullptr);
        m_Items.push_back(std::move(command));
    }

    void CommandQueue::Clear() noexcept
    {
        m_Items.clear();
    }

    bool CommandQueue::IsEmpty() const noexcept
    {
        return m_Items.empty();
    }

    std::size_t CommandQueue::Size() const noexcept
    {
        return m_Items.size();
    }

    const std::vector<std::unique_ptr<Command>>& CommandQueue::Items() const noexcept
    {
        return m_Items;
    }

    std::vector<std::unique_ptr<Command>>& CommandQueue::Items() noexcept
    {
        return m_Items;
    }
}
#pragma once

#include "sim/Command.h"

#include <memory>
#include <vector>

namespace bge
{
    class CommandQueue
    {
    public:
        CommandQueue() = default;

        void Push(std::unique_ptr<Command> command);
        void Clear() noexcept;

        bool IsEmpty() const noexcept;
        std::size_t Size() const noexcept;

        const std::vector<std::unique_ptr<Command>>& Items() const noexcept;
        std::vector<std::unique_ptr<Command>>& Items() noexcept;

    private:
        std::vector<std::unique_ptr<Command>> m_Items;
    };
}
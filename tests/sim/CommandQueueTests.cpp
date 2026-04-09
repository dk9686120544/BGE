#include "sim/CommandQueue.h"
#include "tests/TestFramework.h"

#include <memory>

using namespace bge;

namespace
{
    class DummyCommand final : public Command
    {
    public:
        explicit DummyCommand(int value) noexcept
            : m_Value(value)
        {
        }

        int Value() const noexcept
        {
            return m_Value;
        }

    private:
        int m_Value;
    };
}

BGE_TEST(CommandQueueTests, DefaultStateIsEmpty)
{
    CommandQueue queue;

    BGE_TEST_TRUE(queue.IsEmpty());
    BGE_TEST_EQ(queue.Size(), static_cast<std::size_t>(0));
    BGE_TEST_TRUE(queue.Items().empty());
}

BGE_TEST(CommandQueueTests, PushAddsCommand)
{
    CommandQueue queue;

    queue.Push(std::make_unique<DummyCommand>(123));

    BGE_TEST_FALSE(queue.IsEmpty());
    BGE_TEST_EQ(queue.Size(), static_cast<std::size_t>(1));
    BGE_TEST_EQ(queue.Items().size(), static_cast<std::size_t>(1));
}

BGE_TEST(CommandQueueTests, PushPreservesOrder)
{
    CommandQueue queue;

    queue.Push(std::make_unique<DummyCommand>(10));
    queue.Push(std::make_unique<DummyCommand>(20));
    queue.Push(std::make_unique<DummyCommand>(30));

    BGE_TEST_EQ(queue.Size(), static_cast<std::size_t>(3));

    const DummyCommand* first =
        static_cast<const DummyCommand*>(queue.Items()[0].get());
    const DummyCommand* second =
        static_cast<const DummyCommand*>(queue.Items()[1].get());
    const DummyCommand* third =
        static_cast<const DummyCommand*>(queue.Items()[2].get());

    BGE_TEST_TRUE(first != nullptr);
    BGE_TEST_TRUE(second != nullptr);
    BGE_TEST_TRUE(third != nullptr);

    BGE_TEST_EQ(first->Value(), 10);
    BGE_TEST_EQ(second->Value(), 20);
    BGE_TEST_EQ(third->Value(), 30);
}

BGE_TEST(CommandQueueTests, ClearRemovesAllCommands)
{
    CommandQueue queue;

    queue.Push(std::make_unique<DummyCommand>(1));
    queue.Push(std::make_unique<DummyCommand>(2));

    BGE_TEST_FALSE(queue.IsEmpty());

    queue.Clear();

    BGE_TEST_TRUE(queue.IsEmpty());
    BGE_TEST_EQ(queue.Size(), static_cast<std::size_t>(0));
    BGE_TEST_TRUE(queue.Items().empty());
}

BGE_TEST(CommandQueueTests, MutableItemsAccessWorks)
{
    CommandQueue queue;

    queue.Push(std::make_unique<DummyCommand>(77));

    BGE_TEST_EQ(queue.Items().size(), static_cast<std::size_t>(1));

    std::vector<std::unique_ptr<Command>>& items = queue.Items();
    BGE_TEST_EQ(items.size(), static_cast<std::size_t>(1));

    const DummyCommand* command =
        static_cast<const DummyCommand*>(items[0].get());

    BGE_TEST_TRUE(command != nullptr);
    BGE_TEST_EQ(command->Value(), 77);
}
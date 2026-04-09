#include "sim/Simulation.h"
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

BGE_TEST(SimulationTests, DefaultStateIsClean)
{
    Simulation simulation;

    BGE_TEST_EQ(simulation.CurrentTick(), 0U);
    BGE_TEST_EQ(simulation.CurrentCommands().Size(), static_cast<std::size_t>(0));
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(0));
    BGE_TEST_EQ(simulation.Entities().AliveCount(), 0U);
}

BGE_TEST(SimulationTests, EnqueueCommandAddsToNextQueue)
{
    Simulation simulation;

    simulation.EnqueueCommand(std::make_unique<DummyCommand>(10));

    BGE_TEST_EQ(simulation.CurrentCommands().Size(), static_cast<std::size_t>(0));
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(1));
}

BGE_TEST(SimulationTests, TickMovesNextCommandsToCurrentCommands)
{
    Simulation simulation;

    simulation.EnqueueCommand(std::make_unique<DummyCommand>(10));
    simulation.EnqueueCommand(std::make_unique<DummyCommand>(20));

    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(2));
    BGE_TEST_EQ(simulation.CurrentCommands().Size(), static_cast<std::size_t>(0));

    simulation.Tick();

    BGE_TEST_EQ(simulation.CurrentTick(), 1U);
    BGE_TEST_EQ(simulation.CurrentCommands().Size(), static_cast<std::size_t>(2));
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(0));

    const DummyCommand* first =
        static_cast<const DummyCommand*>(simulation.CurrentCommands().Items()[0].get());
    const DummyCommand* second =
        static_cast<const DummyCommand*>(simulation.CurrentCommands().Items()[1].get());

    BGE_TEST_TRUE(first != nullptr);
    BGE_TEST_TRUE(second != nullptr);

    BGE_TEST_EQ(first->Value(), 10);
    BGE_TEST_EQ(second->Value(), 20);
}

BGE_TEST(SimulationTests, TickClearsPreviousCurrentCommandsBeforeSwap)
{
    Simulation simulation;

    simulation.EnqueueCommand(std::make_unique<DummyCommand>(1));
    simulation.Tick();

    BGE_TEST_EQ(simulation.CurrentCommands().Size(), static_cast<std::size_t>(1));

    simulation.EnqueueCommand(std::make_unique<DummyCommand>(2));
    simulation.EnqueueCommand(std::make_unique<DummyCommand>(3));
    simulation.Tick();

    BGE_TEST_EQ(simulation.CurrentTick(), 2U);
    BGE_TEST_EQ(simulation.CurrentCommands().Size(), static_cast<std::size_t>(2));

    const DummyCommand* first =
        static_cast<const DummyCommand*>(simulation.CurrentCommands().Items()[0].get());
    const DummyCommand* second =
        static_cast<const DummyCommand*>(simulation.CurrentCommands().Items()[1].get());

    BGE_TEST_TRUE(first != nullptr);
    BGE_TEST_TRUE(second != nullptr);

    BGE_TEST_EQ(first->Value(), 2);
    BGE_TEST_EQ(second->Value(), 3);
}

BGE_TEST(SimulationTests, TickProcessesDeferredDestroy)
{
    Simulation simulation;

    const Entity entity = simulation.Entities().Create();

    BGE_TEST_TRUE(simulation.Entities().IsAlive(entity));
    BGE_TEST_EQ(simulation.Entities().AliveCount(), 1U);

    simulation.Entities().MarkForDestroy(entity);

    BGE_TEST_TRUE(simulation.Entities().IsAlive(entity));

    simulation.Tick();

    BGE_TEST_FALSE(simulation.Entities().IsAlive(entity));
    BGE_TEST_EQ(simulation.Entities().AliveCount(), 0U);
}

BGE_TEST(SimulationTests, StateHashChangesAfterTickAdvance)
{
    Simulation simulation;

    const u64 before = simulation.CurrentStateHash();

    simulation.Tick();

    const u64 after = simulation.CurrentStateHash();

    BGE_TEST_TRUE(before != after);
    BGE_TEST_EQ(simulation.CurrentTick(), 1U);
}

BGE_TEST(SimulationTests, StateHashIsDeterministicForSameScenario)
{
    Simulation a;
    Simulation b;

    a.EnqueueCommand(std::make_unique<DummyCommand>(100));
    a.EnqueueCommand(std::make_unique<DummyCommand>(200));

    b.EnqueueCommand(std::make_unique<DummyCommand>(100));
    b.EnqueueCommand(std::make_unique<DummyCommand>(200));

    const Entity entityA = a.Entities().Create();
    const Entity entityB = b.Entities().Create();

    BGE_TEST_EQ(entityA.index, entityB.index);
    BGE_TEST_EQ(entityA.generation, entityB.generation);

    a.Tick();
    b.Tick();

    BGE_TEST_EQ(a.CurrentTick(), b.CurrentTick());
    BGE_TEST_EQ(a.CurrentStateHash(), b.CurrentStateHash());
}

BGE_TEST(SimulationTests, SameEntityLifecycleProducesSameHash)
{
    Simulation a;
    Simulation b;

    const Entity a0 = a.Entities().Create();
    const Entity b0 = b.Entities().Create();

    BGE_TEST_EQ(a0.index, b0.index);
    BGE_TEST_EQ(a0.generation, b0.generation);

    a.Entities().MarkForDestroy(a0);
    b.Entities().MarkForDestroy(b0);

    a.Tick();
    b.Tick();

    BGE_TEST_EQ(a.CurrentStateHash(), b.CurrentStateHash());
    BGE_TEST_EQ(a.CurrentTick(), b.CurrentTick());
}

BGE_TEST(SimulationTests, CurrentAndNextQueuesRemainIndependent)
{
    Simulation simulation;

    simulation.EnqueueCommand(std::make_unique<DummyCommand>(1));
    simulation.Tick();

    BGE_TEST_EQ(simulation.CurrentCommands().Size(), static_cast<std::size_t>(1));
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(0));

    simulation.EnqueueCommand(std::make_unique<DummyCommand>(2));

    BGE_TEST_EQ(simulation.CurrentCommands().Size(), static_cast<std::size_t>(1));
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(1));
}

BGE_TEST(SimulationTests, MultipleTicksAdvanceClockMonotonically)
{
    Simulation simulation;

    BGE_TEST_EQ(simulation.CurrentTick(), 0U);

    simulation.Tick();
    BGE_TEST_EQ(simulation.CurrentTick(), 1U);

    simulation.Tick();
    BGE_TEST_EQ(simulation.CurrentTick(), 2U);

    simulation.Tick();
    BGE_TEST_EQ(simulation.CurrentTick(), 3U);
}
#include "replay/CommandRecorder.h"
#include "tests/TestFramework.h"

using namespace bge;

BGE_TEST(CommandRecorderTests, DefaultStateIsEmpty)
{
    CommandRecorder recorder;

    BGE_TEST_TRUE(recorder.IsEmpty());
    BGE_TEST_EQ(recorder.Count(), 0U);
    BGE_TEST_TRUE(recorder.Commands().empty());
}

BGE_TEST(CommandRecorderTests, RecordMoveAddsCommand)
{
    CommandRecorder recorder;

    recorder.RecordMove(
        5U,
        Entity(10U, 2U),
        Vec2(Scalar::FromInt(7), Scalar::FromInt(9)));

    BGE_TEST_FALSE(recorder.IsEmpty());
    BGE_TEST_EQ(recorder.Count(), 1U);
    BGE_TEST_EQ(recorder.Commands().size(), static_cast<usize>(1));

    const RecordedCommand& command = recorder.Commands()[0];
    BGE_TEST_EQ(command.tick, 5U);
    BGE_TEST_EQ(command.type, RecordedCommandType::Move);
    BGE_TEST_EQ(command.move.entity, Entity(10U, 2U));
    BGE_TEST_EQ(command.move.targetPosition, Vec2(Scalar::FromInt(7), Scalar::FromInt(9)));
}

BGE_TEST(CommandRecorderTests, RecordMovePreservesOrder)
{
    CommandRecorder recorder;

    recorder.RecordMove(
        1U,
        Entity(1U, 0U),
        Vec2(Scalar::FromInt(1), Scalar::FromInt(1)));

    recorder.RecordMove(
        2U,
        Entity(2U, 0U),
        Vec2(Scalar::FromInt(2), Scalar::FromInt(2)));

    recorder.RecordMove(
        3U,
        Entity(3U, 0U),
        Vec2(Scalar::FromInt(3), Scalar::FromInt(3)));

    BGE_TEST_EQ(recorder.Count(), 3U);

    BGE_TEST_EQ(recorder.Commands()[0].tick, 1U);
    BGE_TEST_EQ(recorder.Commands()[1].tick, 2U);
    BGE_TEST_EQ(recorder.Commands()[2].tick, 3U);

    BGE_TEST_EQ(recorder.Commands()[0].move.entity, Entity(1U, 0U));
    BGE_TEST_EQ(recorder.Commands()[1].move.entity, Entity(2U, 0U));
    BGE_TEST_EQ(recorder.Commands()[2].move.entity, Entity(3U, 0U));
}

BGE_TEST(CommandRecorderTests, ClearRemovesAllCommands)
{
    CommandRecorder recorder;

    recorder.RecordMove(
        1U,
        Entity(1U, 0U),
        Vec2(Scalar::FromInt(1), Scalar::FromInt(1)));

    recorder.RecordMove(
        2U,
        Entity(2U, 0U),
        Vec2(Scalar::FromInt(2), Scalar::FromInt(2)));

    BGE_TEST_FALSE(recorder.IsEmpty());

    recorder.Clear();

    BGE_TEST_TRUE(recorder.IsEmpty());
    BGE_TEST_EQ(recorder.Count(), 0U);
    BGE_TEST_TRUE(recorder.Commands().empty());
}

BGE_TEST(CommandRecorderTests, MutableCommandsAccessWorks)
{
    CommandRecorder recorder;

    recorder.RecordMove(
        10U,
        Entity(5U, 1U),
        Vec2(Scalar::FromInt(4), Scalar::FromInt(6)));

    std::vector<RecordedCommand>& commands = recorder.Commands();

    BGE_TEST_EQ(commands.size(), static_cast<usize>(1));
    BGE_TEST_EQ(commands[0].tick, 10U);
    BGE_TEST_EQ(commands[0].type, RecordedCommandType::Move);
}

BGE_TEST(CommandRecorderTests, MultipleCommandsCanHaveSameTick)
{
    CommandRecorder recorder;

    recorder.RecordMove(
        12U,
        Entity(1U, 0U),
        Vec2(Scalar::FromInt(1), Scalar::FromInt(0)));

    recorder.RecordMove(
        12U,
        Entity(2U, 0U),
        Vec2(Scalar::FromInt(2), Scalar::FromInt(0)));

    recorder.RecordMove(
        12U,
        Entity(3U, 0U),
        Vec2(Scalar::FromInt(3), Scalar::FromInt(0)));

    BGE_TEST_EQ(recorder.Count(), 3U);
    BGE_TEST_EQ(recorder.Commands()[0].tick, 12U);
    BGE_TEST_EQ(recorder.Commands()[1].tick, 12U);
    BGE_TEST_EQ(recorder.Commands()[2].tick, 12U);
}

BGE_TEST(CommandRecorderTests, RecordedCommandDataIsExact)
{
    CommandRecorder recorder;

    const Entity entity(42U, 7U);
    const Vec2 target(
        Scalar::FromRaw(12345),
        Scalar::FromRaw(-6789));

    recorder.RecordMove(99U, entity, target);

    const RecordedCommand& command = recorder.Commands()[0];

    BGE_TEST_EQ(command.tick, 99U);
    BGE_TEST_EQ(command.type, RecordedCommandType::Move);
    BGE_TEST_EQ(command.move.entity, entity);
    BGE_TEST_EQ(command.move.targetPosition, target);
}
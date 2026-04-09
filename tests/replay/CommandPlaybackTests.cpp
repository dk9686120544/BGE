#include "game/session/GameSession.h"
#include "replay/CommandPlayback.h"
#include "tests/TestFramework.h"

using namespace bge;

BGE_TEST(CommandPlaybackTests, DefaultStateIsEmptyAndFinished)
{
    CommandPlayback playback;

    BGE_TEST_TRUE(playback.IsFinished());
    BGE_TEST_EQ(playback.CommandCount(), 0U);
    BGE_TEST_EQ(playback.NextCommandIndex(), 0U);
    BGE_TEST_TRUE(playback.Commands().empty());
}

BGE_TEST(CommandPlaybackTests, SetCommandsStoresSequenceAndResetsCursor)
{
    CommandPlayback playback;

    std::vector<RecordedCommand> commands;
    commands.push_back(
        RecordedCommand::MakeMove(
            3U,
            Entity(1U, 0U),
            Vec2(Scalar::FromInt(10), Scalar::FromInt(20))));

    playback.SetCommands(commands);

    BGE_TEST_FALSE(playback.IsFinished());
    BGE_TEST_EQ(playback.CommandCount(), 1U);
    BGE_TEST_EQ(playback.NextCommandIndex(), 0U);
    BGE_TEST_EQ(playback.Commands().size(), static_cast<usize>(1));
    BGE_TEST_EQ(playback.Commands()[0], commands[0]);
}

BGE_TEST(CommandPlaybackTests, ClearRemovesCommandsAndResetsCursor)
{
    CommandPlayback playback;

    std::vector<RecordedCommand> commands;
    commands.push_back(
        RecordedCommand::MakeMove(
            1U,
            Entity(1U, 0U),
            Vec2(Scalar::FromInt(1), Scalar::FromInt(2))));

    playback.SetCommands(commands);
    playback.Clear();

    BGE_TEST_TRUE(playback.IsFinished());
    BGE_TEST_EQ(playback.CommandCount(), 0U);
    BGE_TEST_EQ(playback.NextCommandIndex(), 0U);
    BGE_TEST_TRUE(playback.Commands().empty());
}

BGE_TEST(CommandPlaybackTests, ResetRestartsPlayback)
{
    Simulation simulation;
    GameSession session(simulation);
    CommandPlayback playback;

    const Entity entity = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    std::vector<RecordedCommand> commands;
    commands.push_back(
        RecordedCommand::MakeMove(
            0U,
            entity,
            Vec2(Scalar::FromInt(5), Scalar::FromInt(0))));

    playback.SetCommands(commands);

    playback.PlaybackTick(0U, session);

    BGE_TEST_TRUE(playback.IsFinished());
    BGE_TEST_EQ(playback.NextCommandIndex(), 1U);

    playback.Reset();

    BGE_TEST_FALSE(playback.IsFinished());
    BGE_TEST_EQ(playback.NextCommandIndex(), 0U);
}

BGE_TEST(CommandPlaybackTests, PlaybackTickDoesNothingBeforeCommandTick)
{
    Simulation simulation;
    GameSession session(simulation);
    CommandPlayback playback;

    const Entity entity = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    std::vector<RecordedCommand> commands;
    commands.push_back(
        RecordedCommand::MakeMove(
            5U,
            entity,
            Vec2(Scalar::FromInt(9), Scalar::FromInt(3))));

    playback.SetCommands(commands);

    playback.PlaybackTick(4U, session);

    BGE_TEST_EQ(playback.NextCommandIndex(), 0U);
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(0));
}

BGE_TEST(CommandPlaybackTests, PlaybackTickQueuesCommandAtMatchingTick)
{
    Simulation simulation;
    GameSession session(simulation);
    CommandPlayback playback;

    const Entity entity = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Vec2 target(Scalar::FromInt(9), Scalar::FromInt(3));

    std::vector<RecordedCommand> commands;
    commands.push_back(
        RecordedCommand::MakeMove(
            5U,
            entity,
            target));

    playback.SetCommands(commands);

    playback.PlaybackTick(5U, session);

    BGE_TEST_EQ(playback.NextCommandIndex(), 1U);
    BGE_TEST_TRUE(playback.IsFinished());
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(1));

    simulation.Tick();
    session.ApplyQueuedCommands();

    const Unit* unit = session.TryGetUnit(entity);
    BGE_TEST_TRUE(unit != nullptr);
    BGE_TEST_TRUE(unit->movement.hasTarget);
    BGE_TEST_EQ(unit->movement.target.position, target);
}

BGE_TEST(CommandPlaybackTests, PlaybackTickQueuesMultipleCommandsForSameTick)
{
    Simulation simulation;
    GameSession session(simulation);
    CommandPlayback playback;

    const Entity a = session.CreateUnit(
        Vec2(Scalar::FromInt(0), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity b = session.CreateUnit(
        Vec2(Scalar::FromInt(2), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    std::vector<RecordedCommand> commands;
    commands.push_back(
        RecordedCommand::MakeMove(
            7U,
            a,
            Vec2(Scalar::FromInt(10), Scalar::FromInt(0))));
    commands.push_back(
        RecordedCommand::MakeMove(
            7U,
            b,
            Vec2(Scalar::FromInt(12), Scalar::FromInt(0))));

    playback.SetCommands(commands);
    playback.PlaybackTick(7U, session);

    BGE_TEST_EQ(playback.NextCommandIndex(), 2U);
    BGE_TEST_TRUE(playback.IsFinished());
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(2));
}

BGE_TEST(CommandPlaybackTests, PlaybackTickSkipsInvalidMoveEntityButAdvancesCursor)
{
    Simulation simulation;
    GameSession session(simulation);
    CommandPlayback playback;

    std::vector<RecordedCommand> commands;
    commands.push_back(
        RecordedCommand::MakeMove(
            2U,
            Entity::Invalid(),
            Vec2(Scalar::FromInt(5), Scalar::FromInt(5))));

    playback.SetCommands(commands);
    playback.PlaybackTick(2U, session);

    BGE_TEST_EQ(playback.NextCommandIndex(), 1U);
    BGE_TEST_TRUE(playback.IsFinished());
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(0));
}

BGE_TEST(CommandPlaybackTests, PlaybackTickProcessesCommandsAcrossMultipleTicks)
{
    Simulation simulation;
    GameSession session(simulation);
    CommandPlayback playback;

    const Entity a = session.CreateUnit(
        Vec2(Scalar::FromInt(0), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity b = session.CreateUnit(
        Vec2(Scalar::FromInt(2), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    std::vector<RecordedCommand> commands;
    commands.push_back(
        RecordedCommand::MakeMove(
            1U,
            a,
            Vec2(Scalar::FromInt(10), Scalar::FromInt(0))));
    commands.push_back(
        RecordedCommand::MakeMove(
            3U,
            b,
            Vec2(Scalar::FromInt(20), Scalar::FromInt(0))));

    playback.SetCommands(commands);

    playback.PlaybackTick(0U, session);
    BGE_TEST_EQ(playback.NextCommandIndex(), 0U);
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(0));

    playback.PlaybackTick(1U, session);
    BGE_TEST_EQ(playback.NextCommandIndex(), 1U);
    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(1));

    simulation.Tick();
    session.ApplyQueuedCommands();

    playback.PlaybackTick(2U, session);
    BGE_TEST_EQ(playback.NextCommandIndex(), 1U);

    playback.PlaybackTick(3U, session);
    BGE_TEST_EQ(playback.NextCommandIndex(), 2U);
    BGE_TEST_TRUE(playback.IsFinished());
}

BGE_TEST(CommandPlaybackTests, PlaybackProducesDeterministicTargetAssignment)
{
    Simulation simA;
    Simulation simB;

    GameSession sessionA(simA);
    GameSession sessionB(simB);

    CommandPlayback playbackA;
    CommandPlayback playbackB;

    const Entity entityA1 = sessionA.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity entityB1 = sessionB.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    std::vector<RecordedCommand> commands;
    commands.push_back(
        RecordedCommand::MakeMove(
            4U,
            entityA1,
            Vec2(Scalar::FromInt(15), Scalar::FromInt(6))));

    std::vector<RecordedCommand> commandsB;
    commandsB.push_back(
        RecordedCommand::MakeMove(
            4U,
            entityB1,
            Vec2(Scalar::FromInt(15), Scalar::FromInt(6))));

    playbackA.SetCommands(commands);
    playbackB.SetCommands(commandsB);

    playbackA.PlaybackTick(4U, sessionA);
    playbackB.PlaybackTick(4U, sessionB);

    simA.Tick();
    sessionA.ApplyQueuedCommands();

    simB.Tick();
    sessionB.ApplyQueuedCommands();

    const Unit* unitA = sessionA.TryGetUnit(entityA1);
    const Unit* unitB = sessionB.TryGetUnit(entityB1);

    BGE_TEST_TRUE(unitA != nullptr);
    BGE_TEST_TRUE(unitB != nullptr);
    BGE_TEST_EQ(unitA->movement.target.position, unitB->movement.target.position);
}
#include "game/session/GameSession.h"
#include "tests/TestFramework.h"
#include "world/Occupancy.h"

using namespace bge;

BGE_TEST(GameSessionTests, CreateUnitReturnsValidEntity)
{
    Simulation simulation;
    GameSession session(simulation);

    const Entity entity = session.CreateUnit(
        Vec2(Scalar::FromInt(1), Scalar::FromInt(2)),
        Scalar::FromInt(3),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    BGE_TEST_TRUE(entity.IsValid());
    BGE_TEST_TRUE(simulation.Entities().IsAlive(entity));
    BGE_TEST_EQ(session.Units().size(), static_cast<usize>(1));
}

BGE_TEST(GameSessionTests, CreatedUnitHasExpectedInitialMovementState)
{
    Simulation simulation;
    GameSession session(simulation);

    const Vec2 position(Scalar::FromInt(5), Scalar::FromInt(7));
    const Scalar maxSpeed = Scalar::FromInt(4);
    const Scalar radius = Scalar::FromRaw(Scalar::OneRaw / 2);

    const Entity entity = session.CreateUnit(position, maxSpeed, radius);
    const Unit* unit = session.TryGetUnit(entity);

    BGE_TEST_TRUE(unit != nullptr);
    BGE_TEST_EQ(unit->entity, entity);
    BGE_TEST_EQ(unit->movement.position, position);
    BGE_TEST_EQ(unit->movement.velocity, Vec2::Zero());
    BGE_TEST_EQ(unit->movement.desiredVelocity, Vec2::Zero());
    BGE_TEST_EQ(unit->movement.maxSpeed, maxSpeed);
    BGE_TEST_EQ(unit->movement.radius, radius);
    BGE_TEST_FALSE(unit->movement.hasTarget);
}

BGE_TEST(GameSessionTests, TryGetUnitReturnsNullForInvalidEntity)
{
    Simulation simulation;
    GameSession session(simulation);

    Unit* unit = session.TryGetUnit(Entity::Invalid());
    BGE_TEST_TRUE(unit == nullptr);
}

BGE_TEST(GameSessionTests, QueueMoveCommandAddsCommandToSimulationNextQueue)
{
    Simulation simulation;
    GameSession session(simulation);

    const Entity entity = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(3),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    session.QueueMoveCommand(entity, Vec2(Scalar::FromInt(10), Scalar::Zero()));

    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(1));
    BGE_TEST_EQ(simulation.CurrentCommands().Size(), static_cast<std::size_t>(0));
}

BGE_TEST(GameSessionTests, ApplyQueuedCommandsSetsMovementTarget)
{
    Simulation simulation;
    GameSession session(simulation);

    const Entity entity = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(3),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Vec2 target(Scalar::FromInt(8), Scalar::FromInt(2));

    session.QueueMoveCommand(entity, target);

    simulation.Tick();
    session.ApplyQueuedCommands();

    const Unit* unit = session.TryGetUnit(entity);
    BGE_TEST_TRUE(unit != nullptr);
    BGE_TEST_TRUE(unit->movement.hasTarget);
    BGE_TEST_EQ(unit->movement.target.position, target);
}

BGE_TEST(GameSessionTests, TickMovesUnitAfterMoveCommandApplied)
{
    Simulation simulation;
    GameSession session(simulation);

    const Entity entity = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    session.QueueMoveCommand(entity, Vec2(Scalar::FromInt(10), Scalar::Zero()));

    simulation.Tick();
    session.ApplyQueuedCommands();
    session.Tick(Scalar::One());

    const Unit* unit = session.TryGetUnit(entity);
    BGE_TEST_TRUE(unit != nullptr);
    BGE_TEST_TRUE(unit->movement.position.x > Scalar::Zero());
    BGE_TEST_EQ(unit->movement.position.y, Scalar::Zero());
    BGE_TEST_TRUE(unit->movement.velocity.x > Scalar::Zero());
}

BGE_TEST(GameSessionTests, TickDoesNothingToUnitWithoutTarget)
{
    Simulation simulation;
    GameSession session(simulation);

    const Vec2 initialPosition(Scalar::FromInt(3), Scalar::FromInt(4));

    const Entity entity = session.CreateUnit(
        initialPosition,
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    session.Tick(Scalar::One());

    const Unit* unit = session.TryGetUnit(entity);
    BGE_TEST_TRUE(unit != nullptr);
    BGE_TEST_EQ(unit->movement.position, initialPosition);
    BGE_TEST_EQ(unit->movement.velocity, Vec2::Zero());
    BGE_TEST_EQ(unit->movement.desiredVelocity, Vec2::Zero());
}

BGE_TEST(GameSessionTests, ApplyQueuedCommandsIgnoresUnknownCommandTypes)
{
    class DummyCommand final : public Command
    {
    };

    Simulation simulation;
    GameSession session(simulation);

    const Entity entity = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    simulation.EnqueueCommand(std::make_unique<DummyCommand>());
    simulation.Tick();

    session.ApplyQueuedCommands();

    const Unit* unit = session.TryGetUnit(entity);
    BGE_TEST_TRUE(unit != nullptr);
    BGE_TEST_FALSE(unit->movement.hasTarget);
}

BGE_TEST(GameSessionTests, RemoveDestroyedUnitsCleansRuntimeList)
{
    Simulation simulation;
    GameSession session(simulation);

    const Entity a = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity b = session.CreateUnit(
        Vec2(Scalar::FromInt(1), Scalar::Zero()),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    BGE_TEST_EQ(session.Units().size(), static_cast<usize>(2));

    simulation.Entities().MarkForDestroy(a);
    simulation.Tick();
    session.Tick(Scalar::One());

    BGE_TEST_FALSE(simulation.Entities().IsAlive(a));
    BGE_TEST_TRUE(simulation.Entities().IsAlive(b));
    BGE_TEST_EQ(session.Units().size(), static_cast<usize>(1));

    const Unit* remaining = session.TryGetUnit(b);
    BGE_TEST_TRUE(remaining != nullptr);
    BGE_TEST_EQ(remaining->entity, b);
}

BGE_TEST(GameSessionTests, MultipleUnitsCanReceiveDifferentTargets)
{
    Simulation simulation;
    GameSession session(simulation);

    const Entity a = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity b = session.CreateUnit(
        Vec2(Scalar::FromInt(10), Scalar::Zero()),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Vec2 targetA(Scalar::FromInt(5), Scalar::Zero());
    const Vec2 targetB(Scalar::FromInt(8), Scalar::FromInt(3));

    session.QueueMoveCommand(a, targetA);
    session.QueueMoveCommand(b, targetB);

    simulation.Tick();
    session.ApplyQueuedCommands();

    const Unit* unitA = session.TryGetUnit(a);
    const Unit* unitB = session.TryGetUnit(b);

    BGE_TEST_TRUE(unitA != nullptr);
    BGE_TEST_TRUE(unitB != nullptr);

    BGE_TEST_TRUE(unitA->movement.hasTarget);
    BGE_TEST_TRUE(unitB->movement.hasTarget);

    BGE_TEST_EQ(unitA->movement.target.position, targetA);
    BGE_TEST_EQ(unitB->movement.target.position, targetB);
}

BGE_TEST(GameSessionTests, QueueGroupMoveCommandWithEmptyInputDoesNothing)
{
    Simulation simulation;
    GameSession session(simulation);

    const std::vector<Entity> entities;
    session.QueueGroupMoveCommand(
        entities,
        Vec2(Scalar::FromInt(10), Scalar::FromInt(10)),
        Scalar::FromInt(2));

    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(0));
}

BGE_TEST(GameSessionTests, QueueGroupMoveCommandWithSingleEntityFallsBackToSingleMove)
{
    Simulation simulation;
    GameSession session(simulation);

    const Entity entity = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    std::vector<Entity> entities;
    entities.push_back(entity);

    const Vec2 target(Scalar::FromInt(12), Scalar::FromInt(4));

    session.QueueGroupMoveCommand(entities, target, Scalar::FromInt(2));

    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(1));

    simulation.Tick();
    session.ApplyQueuedCommands();

    const Unit* unit = session.TryGetUnit(entity);
    BGE_TEST_TRUE(unit != nullptr);
    BGE_TEST_TRUE(unit->movement.hasTarget);
    BGE_TEST_EQ(unit->movement.target.position, target);
}

BGE_TEST(GameSessionTests, QueueGroupMoveCommandBuildsMultipleAssignments)
{
    Simulation simulation;
    GameSession session(simulation);

    const Entity a = session.CreateUnit(
        Vec2(Scalar::FromInt(0), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity b = session.CreateUnit(
        Vec2(Scalar::FromInt(4), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity c = session.CreateUnit(
        Vec2(Scalar::FromInt(8), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    std::vector<Entity> entities;
    entities.push_back(a);
    entities.push_back(b);
    entities.push_back(c);

    session.QueueGroupMoveCommand(
        entities,
        Vec2(Scalar::FromInt(20), Scalar::FromInt(20)),
        Scalar::FromInt(2));

    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(3));

    simulation.Tick();
    session.ApplyQueuedCommands();

    const Unit* unitA = session.TryGetUnit(a);
    const Unit* unitB = session.TryGetUnit(b);
    const Unit* unitC = session.TryGetUnit(c);

    BGE_TEST_TRUE(unitA != nullptr);
    BGE_TEST_TRUE(unitB != nullptr);
    BGE_TEST_TRUE(unitC != nullptr);

    BGE_TEST_TRUE(unitA->movement.hasTarget);
    BGE_TEST_TRUE(unitB->movement.hasTarget);
    BGE_TEST_TRUE(unitC->movement.hasTarget);

    BGE_TEST_TRUE(unitA->movement.target.position != unitB->movement.target.position);
    BGE_TEST_TRUE(unitA->movement.target.position != unitC->movement.target.position);
    BGE_TEST_TRUE(unitB->movement.target.position != unitC->movement.target.position);
}

BGE_TEST(GameSessionTests, QueueGroupMoveCommandIgnoresUnknownEntities)
{
    Simulation simulation;
    GameSession session(simulation);

    const Entity valid = session.CreateUnit(
        Vec2::Zero(),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity invalid(999U, 0U);

    std::vector<Entity> entities;
    entities.push_back(valid);
    entities.push_back(invalid);

    session.QueueGroupMoveCommand(
        entities,
        Vec2(Scalar::FromInt(10), Scalar::FromInt(10)),
        Scalar::FromInt(2));

    BGE_TEST_EQ(simulation.NextCommands().Size(), static_cast<std::size_t>(1));

    simulation.Tick();
    session.ApplyQueuedCommands();

    const Unit* unit = session.TryGetUnit(valid);
    BGE_TEST_TRUE(unit != nullptr);
    BGE_TEST_TRUE(unit->movement.hasTarget);
}

BGE_TEST(GameSessionTests, QueueGroupMoveCommandProducesDeterministicResults)
{
    Simulation simA;
    Simulation simB;

    GameSession sessionA(simA);
    GameSession sessionB(simB);

    const Entity a1 = sessionA.CreateUnit(
        Vec2(Scalar::FromInt(0), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity b1 = sessionA.CreateUnit(
        Vec2(Scalar::FromInt(4), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity c1 = sessionA.CreateUnit(
        Vec2(Scalar::FromInt(8), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity a2 = sessionB.CreateUnit(
        Vec2(Scalar::FromInt(0), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity b2 = sessionB.CreateUnit(
        Vec2(Scalar::FromInt(4), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const Entity c2 = sessionB.CreateUnit(
        Vec2(Scalar::FromInt(8), Scalar::FromInt(0)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    std::vector<Entity> entitiesA;
    entitiesA.push_back(a1);
    entitiesA.push_back(b1);
    entitiesA.push_back(c1);

    std::vector<Entity> entitiesB;
    entitiesB.push_back(a2);
    entitiesB.push_back(b2);
    entitiesB.push_back(c2);

    const Vec2 center(Scalar::FromInt(20), Scalar::FromInt(20));
    const Scalar spacing = Scalar::FromInt(2);

    sessionA.QueueGroupMoveCommand(entitiesA, center, spacing);
    sessionB.QueueGroupMoveCommand(entitiesB, center, spacing);

    simA.Tick();
    simB.Tick();

    sessionA.ApplyQueuedCommands();
    sessionB.ApplyQueuedCommands();

    const Unit* ua1 = sessionA.TryGetUnit(a1);
    const Unit* ub1 = sessionA.TryGetUnit(b1);
    const Unit* uc1 = sessionA.TryGetUnit(c1);

    const Unit* ua2 = sessionB.TryGetUnit(a2);
    const Unit* ub2 = sessionB.TryGetUnit(b2);
    const Unit* uc2 = sessionB.TryGetUnit(c2);

    BGE_TEST_TRUE(ua1 != nullptr);
    BGE_TEST_TRUE(ub1 != nullptr);
    BGE_TEST_TRUE(uc1 != nullptr);
    BGE_TEST_TRUE(ua2 != nullptr);
    BGE_TEST_TRUE(ub2 != nullptr);
    BGE_TEST_TRUE(uc2 != nullptr);

    BGE_TEST_EQ(ua1->movement.target.position, ua2->movement.target.position);
    BGE_TEST_EQ(ub1->movement.target.position, ub2->movement.target.position);
    BGE_TEST_EQ(uc1->movement.target.position, uc2->movement.target.position);
}

BGE_TEST(GameSessionTests, CreateUnitRegistersOccupancyWhenConnected)
{
    Simulation simulation;
    GameSession session(simulation);
    Occupancy occupancy(GridSize(16U, 16U));

    session.SetOccupancy(&occupancy);

    const Vec2 position(Scalar::FromInt(3), Scalar::FromInt(4));
    const Entity entity = session.CreateUnit(
        position,
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const GridCoord cell = WorldTypes::WorldToCell(position);

    BGE_TEST_TRUE(occupancy.HasDynamicOccupant(cell));
    BGE_TEST_EQ(occupancy.GetDynamicOccupant(cell), entity);

    const Unit* unit = session.TryGetUnit(entity);
    BGE_TEST_TRUE(unit != nullptr);
    BGE_TEST_TRUE(unit->hasOccupiedCell);
    BGE_TEST_EQ(unit->occupiedCell, cell);
}

BGE_TEST(GameSessionTests, TickUpdatesOccupancyWhenUnitMovesToNewCell)
{
    Simulation simulation;
    GameSession session(simulation);
    Occupancy occupancy(GridSize(32U, 32U));

    session.SetOccupancy(&occupancy);

    const Entity entity = session.CreateUnit(
        Vec2(Scalar::Zero(), Scalar::Zero()),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const GridCoord startCell = WorldTypes::WorldToCell(Vec2(Scalar::Zero(), Scalar::Zero()));
    BGE_TEST_EQ(startCell, GridCoord(0, 0));
    BGE_TEST_EQ(occupancy.GetDynamicOccupant(startCell), entity);

    session.QueueMoveCommand(entity, Vec2(Scalar::FromInt(5), Scalar::Zero()));
    simulation.Tick();
    session.ApplyQueuedCommands();

    for (int i = 0; i < 3; ++i)
    {
        session.Tick(Scalar::One());
    }

    const Unit* unit = session.TryGetUnit(entity);
    BGE_TEST_TRUE(unit != nullptr);
    BGE_TEST_TRUE(unit->movement.position.x >= Scalar::FromInt(2));

    const GridCoord updatedCell = WorldTypes::WorldToCell(unit->movement.position);
    BGE_TEST_TRUE(updatedCell != startCell);
    BGE_TEST_FALSE(occupancy.HasDynamicOccupant(startCell));
    BGE_TEST_TRUE(occupancy.HasDynamicOccupant(updatedCell));
    BGE_TEST_EQ(occupancy.GetDynamicOccupant(updatedCell), entity);
}

BGE_TEST(GameSessionTests, RemovingDestroyedUnitFreesOccupancy)
{
    Simulation simulation;
    GameSession session(simulation);
    Occupancy occupancy(GridSize(16U, 16U));

    session.SetOccupancy(&occupancy);

    const Entity entity = session.CreateUnit(
        Vec2(Scalar::FromInt(2), Scalar::FromInt(2)),
        Scalar::FromInt(2),
        Scalar::FromRaw(Scalar::OneRaw / 2));

    const GridCoord cell = WorldTypes::WorldToCell(Vec2(Scalar::FromInt(2), Scalar::FromInt(2)));

    BGE_TEST_TRUE(occupancy.HasDynamicOccupant(cell));

    simulation.Entities().MarkForDestroy(entity);
    simulation.Tick();
    session.Tick(Scalar::One());

    BGE_TEST_FALSE(occupancy.HasDynamicOccupant(cell));
    BGE_TEST_EQ(occupancy.GetDynamicOccupant(cell), Entity::Invalid());
}

BGE_TEST(GameSessionTests, SetOccupancyAndGetOccupancyWork)
{
    Simulation simulation;
    GameSession session(simulation);
    Occupancy occupancy(GridSize(8U, 8U));

    BGE_TEST_TRUE(session.GetOccupancy() == nullptr);

    session.SetOccupancy(&occupancy);

    BGE_TEST_TRUE(session.GetOccupancy() == &occupancy);
    BGE_TEST_TRUE(static_cast<const GameSession&>(session).GetOccupancy() == &occupancy);
}
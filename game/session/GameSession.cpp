#include "game/session/GameSession.h"

#include "world/WorldTypes.h"

#include <memory>

namespace bge
{
    GameSession::GameSession(Simulation& simulation) noexcept
        : m_Simulation(simulation)
        , m_Units()
        , m_ArrivalSettings()
        , m_CommandRecorder(nullptr)
        , m_CommandRecordingBaseTick(0)
        , m_Occupancy(nullptr)
    {
    }

    Entity GameSession::CreateUnit(
        const Vec2& position,
        Scalar maxSpeed,
        Scalar radius)
    {
        Entity entity = m_Simulation.Entities().Create();

        Unit unit(entity);
        unit.movement.position = position;
        unit.movement.velocity = Vec2::Zero();
        unit.movement.desiredVelocity = Vec2::Zero();
        unit.movement.target = MovementTarget(position);
        unit.movement.maxSpeed = maxSpeed;
        unit.movement.radius = radius;
        unit.movement.hasTarget = false;
        unit.ClearOccupiedCell();

        m_Units.push_back(unit);

        Unit& createdUnit = m_Units.back();
        RegisterUnitOccupancy(createdUnit);

        return entity;
    }

    void GameSession::SetOccupancy(Occupancy* occupancy) noexcept
    {
        m_Occupancy = occupancy;
    }

    Occupancy* GameSession::GetOccupancy() noexcept
    {
        return m_Occupancy;
    }

    const Occupancy* GameSession::GetOccupancy() const noexcept
    {
        return m_Occupancy;
    }

    void GameSession::SetCommandRecorder(CommandRecorder* recorder) noexcept
    {
        m_CommandRecorder = recorder;
    }

    CommandRecorder* GameSession::GetCommandRecorder() noexcept
    {
        return m_CommandRecorder;
    }

    const CommandRecorder* GameSession::GetCommandRecorder() const noexcept
    {
        return m_CommandRecorder;
    }

    void GameSession::SetCommandRecordingBaseTick(u32 baseTick) noexcept
    {
        m_CommandRecordingBaseTick = baseTick;
    }

    u32 GameSession::GetCommandRecordingBaseTick() const noexcept
    {
        return m_CommandRecordingBaseTick;
    }

    void GameSession::QueueMoveCommand(Entity entity, const Vec2& targetPosition)
    {
        if (m_CommandRecorder != nullptr)
        {
            const u32 currentTick = m_Simulation.CurrentTick();
            const u32 relativeTick = currentTick - m_CommandRecordingBaseTick;

            m_CommandRecorder->RecordMove(
                relativeTick,
                entity,
                targetPosition);
        }

        m_Simulation.EnqueueCommand(
            std::make_unique<MoveCommand>(entity, targetPosition));
    }

    void GameSession::QueueGroupMoveCommand(
        const std::vector<Entity>& entities,
        const Vec2& targetCenter,
        Scalar spacing)
    {
        if (entities.empty())
        {
            return;
        }

        std::vector<Unit*> selectedUnits;
        selectedUnits.reserve(entities.size());

        for (const Entity entity : entities)
        {
            Unit* unit = TryGetUnit(entity);
            if (unit != nullptr)
            {
                selectedUnits.push_back(unit);
            }
        }

        if (selectedUnits.empty())
        {
            return;
        }

        if (selectedUnits.size() == 1)
        {
            QueueMoveCommand(selectedUnits[0]->entity, targetCenter);
            return;
        }

        const FormationPlan plan =
            FormationPlanner::BuildMoveFormation(
                selectedUnits,
                targetCenter,
                spacing);

        for (const FormationAssignment& assignment : plan.assignments)
        {
            QueueMoveCommand(assignment.entity, assignment.targetPosition);
        }
    }

    void GameSession::ApplyQueuedCommands()
    {
        const auto& commands = m_Simulation.CurrentCommands().Items();

        for (const auto& commandPtr : commands)
        {
            const MoveCommand* moveCommand =
                dynamic_cast<const MoveCommand*>(commandPtr.get());

            if (moveCommand == nullptr)
            {
                continue;
            }

            ApplyMoveCommand(*moveCommand);
        }
    }

    void GameSession::Tick(Scalar deltaTime)
    {
        RemoveDestroyedUnits();

        for (Unit& unit : m_Units)
        {
            if (!m_Simulation.Entities().IsAlive(unit.entity))
            {
                continue;
            }

            MovementSystem::Tick(
                unit.movement,
                m_ArrivalSettings,
                Vec2::Zero(),
                deltaTime);

            UpdateUnitOccupancy(unit);
        }
    }

    Unit* GameSession::TryGetUnit(Entity entity) noexcept
    {
        if (!m_Simulation.Entities().IsAlive(entity))
        {
            return nullptr;
        }

        for (Unit& unit : m_Units)
        {
            if (unit.entity == entity)
            {
                return &unit;
            }
        }

        return nullptr;
    }

    const Unit* GameSession::TryGetUnit(Entity entity) const noexcept
    {
        if (!m_Simulation.Entities().IsAlive(entity))
        {
            return nullptr;
        }

        for (const Unit& unit : m_Units)
        {
            if (unit.entity == entity)
            {
                return &unit;
            }
        }

        return nullptr;
    }

    const std::vector<Unit>& GameSession::Units() const noexcept
    {
        return m_Units;
    }

    std::vector<Unit>& GameSession::Units() noexcept
    {
        return m_Units;
    }

    Simulation& GameSession::GetSimulation() noexcept
    {
        return m_Simulation;
    }

    const Simulation& GameSession::GetSimulation() const noexcept
    {
        return m_Simulation;
    }

    void GameSession::RegisterUnitOccupancy(Unit& unit) noexcept
    {
        if (m_Occupancy == nullptr)
        {
            return;
        }

        const GridCoord cell = WorldTypes::WorldToCell(unit.movement.position);
        if (!m_Occupancy->IsInside(cell))
        {
            unit.ClearOccupiedCell();
            return;
        }

        m_Occupancy->SetDynamicOccupied(cell, unit.entity);
        unit.SetOccupiedCell(cell);
    }

    void GameSession::UpdateUnitOccupancy(Unit& unit) noexcept
    {
        if (m_Occupancy == nullptr)
        {
            return;
        }

        const GridCoord newCell = WorldTypes::WorldToCell(unit.movement.position);

        if (!m_Occupancy->IsInside(newCell))
        {
            if (unit.hasOccupiedCell)
            {
                m_Occupancy->ClearDynamicOccupied(unit.occupiedCell);
                unit.ClearOccupiedCell();
            }

            return;
        }

        if (unit.hasOccupiedCell && unit.occupiedCell == newCell)
        {
            return;
        }

        if (unit.hasOccupiedCell)
        {
            m_Occupancy->ClearDynamicOccupied(unit.occupiedCell);
        }

        m_Occupancy->SetDynamicOccupied(newCell, unit.entity);
        unit.SetOccupiedCell(newCell);
    }

    void GameSession::UnregisterUnitOccupancy(Unit& unit) noexcept
    {
        if (m_Occupancy == nullptr)
        {
            unit.ClearOccupiedCell();
            return;
        }

        if (!unit.hasOccupiedCell)
        {
            return;
        }

        m_Occupancy->ClearDynamicOccupied(unit.occupiedCell);
        unit.ClearOccupiedCell();
    }

    void GameSession::RemoveDestroyedUnits()
    {
        std::vector<Unit> aliveUnits;
        aliveUnits.reserve(m_Units.size());

        for (Unit& unit : m_Units)
        {
            if (m_Simulation.Entities().IsAlive(unit.entity))
            {
                aliveUnits.push_back(unit);
            }
            else
            {
                UnregisterUnitOccupancy(unit);
            }
        }

        m_Units.swap(aliveUnits);
    }

    void GameSession::ApplyMoveCommand(const MoveCommand& command) noexcept
    {
        Unit* unit = TryGetUnit(command.GetEntity());
        if (unit == nullptr)
        {
            return;
        }

        unit->movement.target = MovementTarget(command.GetTargetPosition());
        unit->movement.hasTarget = true;
    }
}
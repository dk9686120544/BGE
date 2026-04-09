#pragma once

#include "game/commands/MoveCommand.h"
#include "game/formation/FormationPlanner.h"
#include "game/units/Unit.h"
#include "movement/MovementSystem.h"
#include "replay/CommandRecorder.h"
#include "sim/Simulation.h"
#include "world/Occupancy.h"

#include <vector>

namespace bge
{
    class GameSession
    {
    public:
        explicit GameSession(Simulation& simulation) noexcept;

        Entity CreateUnit(
            const Vec2& position,
            Scalar maxSpeed,
            Scalar radius);

        void SetOccupancy(Occupancy* occupancy) noexcept;
        Occupancy* GetOccupancy() noexcept;
        const Occupancy* GetOccupancy() const noexcept;

        void SetCommandRecorder(CommandRecorder* recorder) noexcept;
        CommandRecorder* GetCommandRecorder() noexcept;
        const CommandRecorder* GetCommandRecorder() const noexcept;

        void SetCommandRecordingBaseTick(u32 baseTick) noexcept;
        u32 GetCommandRecordingBaseTick() const noexcept;

        void QueueMoveCommand(Entity entity, const Vec2& targetPosition);
        void QueueGroupMoveCommand(
            const std::vector<Entity>& entities,
            const Vec2& targetCenter,
            Scalar spacing);

        void ApplyQueuedCommands();
        void Tick(Scalar deltaTime);

        Unit* TryGetUnit(Entity entity) noexcept;
        const Unit* TryGetUnit(Entity entity) const noexcept;

        const std::vector<Unit>& Units() const noexcept;
        std::vector<Unit>& Units() noexcept;

        Simulation& GetSimulation() noexcept;
        const Simulation& GetSimulation() const noexcept;

    private:
        void RegisterUnitOccupancy(Unit& unit) noexcept;
        void UpdateUnitOccupancy(Unit& unit) noexcept;
        void UnregisterUnitOccupancy(Unit& unit) noexcept;

        void RemoveDestroyedUnits();
        void ApplyMoveCommand(const MoveCommand& command) noexcept;

    private:
        Simulation& m_Simulation;
        std::vector<Unit> m_Units;
        ArrivalSettings m_ArrivalSettings;
        CommandRecorder* m_CommandRecorder = nullptr;
        u32 m_CommandRecordingBaseTick = 0;
        Occupancy* m_Occupancy = nullptr;
    };
}
#pragma once

#include "core/Types.h"
#include "math/Vec2.h"

#include <vector>

namespace bge
{
    enum class DebugInputMode : u8
    {
        Idle = 0,
        DragSelecting = 1
    };

    struct SnapshotUnit
    {
        u32 entityIndex;
        Vec2 position;
        Vec2 velocity;
        Vec2 targetPosition;
        bool hasTarget;
        bool selected;

        constexpr SnapshotUnit() noexcept
            : entityIndex(0)
            , position(Vec2::Zero())
            , velocity(Vec2::Zero())
            , targetPosition(Vec2::Zero())
            , hasTarget(false)
            , selected(false)
        {
        }
    };

    struct SnapshotSelectionBox
    {
        bool active;
        int startScreenX;
        int startScreenY;
        int currentScreenX;
        int currentScreenY;

        constexpr SnapshotSelectionBox() noexcept
            : active(false)
            , startScreenX(0)
            , startScreenY(0)
            , currentScreenX(0)
            , currentScreenY(0)
        {
        }
    };

    struct Snapshot
    {
        u32 currentTick;
        u64 stateHash;

        u32 primarySelectedEntityIndex;
        u32 selectedCount;
        u32 totalUnitCount;
        u32 tickRateHz;

        DebugInputMode inputMode;

        bool recording;
        bool playingBack;
        bool parityKnown;
        bool parityMatch;

        u32 recordedCommandCount;
        u32 playbackCommandCount;
        u32 playbackNextIndex;

        u32 recordStartTick;
        u32 recordEndTick;
        u64 recordEndHash;

        u32 playbackStartTick;
        u32 playbackEndTick;
        u64 playbackEndHash;

        const wchar_t* statusMessage;

        std::vector<SnapshotUnit> units;
        SnapshotSelectionBox selectionBox;

        constexpr Snapshot() noexcept
            : currentTick(0)
            , stateHash(0)
            , primarySelectedEntityIndex(0xFFFFFFFFu)
            , selectedCount(0)
            , totalUnitCount(0)
            , tickRateHz(0)
            , inputMode(DebugInputMode::Idle)
            , recording(false)
            , playingBack(false)
            , parityKnown(false)
            , parityMatch(false)
            , recordedCommandCount(0)
            , playbackCommandCount(0)
            , playbackNextIndex(0)
            , recordStartTick(0)
            , recordEndTick(0)
            , recordEndHash(0)
            , playbackStartTick(0)
            , playbackEndTick(0)
            , playbackEndHash(0)
            , statusMessage(L"Ready")
            , units()
            , selectionBox()
        {
        }

        void Clear() noexcept
        {
            currentTick = 0;
            stateHash = 0;

            primarySelectedEntityIndex = 0xFFFFFFFFu;
            selectedCount = 0;
            totalUnitCount = 0;
            tickRateHz = 0;

            inputMode = DebugInputMode::Idle;

            recording = false;
            playingBack = false;
            parityKnown = false;
            parityMatch = false;

            recordedCommandCount = 0;
            playbackCommandCount = 0;
            playbackNextIndex = 0;

            recordStartTick = 0;
            recordEndTick = 0;
            recordEndHash = 0;

            playbackStartTick = 0;
            playbackEndTick = 0;
            playbackEndHash = 0;

            statusMessage = L"Ready";

            units.clear();
            selectionBox = SnapshotSelectionBox();
        }
    };
}
#include "bridge/Snapshot.h"
#include "debug/DebugDraw.h"
#include "game/session/GameSession.h"
#include "replay/CommandPlayback.h"
#include "replay/CommandRecorder.h"
#include "world/Map.h"

#include <windows.h>
#include <windowsx.h>

#include <new>
#include <vector>

namespace
{
    struct SelectionDragState
    {
        bool mouseDown;
        bool dragging;
        int startScreenX;
        int startScreenY;
        int currentScreenX;
        int currentScreenY;

        SelectionDragState() noexcept
            : mouseDown(false)
            , dragging(false)
            , startScreenX(0)
            , startScreenY(0)
            , currentScreenX(0)
            , currentScreenY(0)
        {
        }
    };

    struct AppState
    {
        bge::Simulation simulation;
        bge::GameSession session;
        bge::Map map;
        bge::Snapshot snapshot;

        std::vector<bge::Entity> selectedEntities;
        SelectionDragState selectionDrag;

        bge::CommandRecorder recorder;
        bge::CommandPlayback playback;

        bool recording;
        bool playingBack;
        bool parityKnown;
        bool parityMatch;

        bge::u32 recordStartTick;
        bge::u32 recordEndTick;
        bge::u64 recordEndHash;

        bge::u32 playbackEndTick;
        bge::u64 playbackEndHash;

        const wchar_t* statusMessage;

        AppState()
            : simulation()
            , session(simulation)
            , map(bge::GridSize(100, 60))
            , snapshot()
            , selectedEntities()
            , selectionDrag()
            , recorder()
            , playback()
            , recording(false)
            , playingBack(false)
            , parityKnown(false)
            , parityMatch(false)
            , recordStartTick(0)
            , recordEndTick(0)
            , recordEndHash(0)
            , playbackEndTick(0)
            , playbackEndHash(0)
            , statusMessage(L"Ready")
        {
        }
    };

    AppState* g_AppState = nullptr;

    constexpr double kTickRateHz = 25.0;
    constexpr double kFixedDtSeconds = 1.0 / kTickRateHz;
    constexpr int kDragThresholdPixels = 4;
    constexpr bge::u32 kInvalidEntityIndex = 0xFFFFFFFFu;

    bge::Vec2 ScreenToWorld(int screenX, int screenY)
    {
        const float worldX =
            static_cast<float>(screenX - bge::DebugDraw::OriginX) /
            static_cast<float>(bge::DebugDraw::CellSize);

        const float worldY =
            static_cast<float>(screenY - bge::DebugDraw::OriginY) /
            static_cast<float>(bge::DebugDraw::CellSize);

        return bge::Vec2(
            bge::Scalar::FromRaw(
                static_cast<bge::i64>(worldX * static_cast<float>(bge::Scalar::OneRaw))),
            bge::Scalar::FromRaw(
                static_cast<bge::i64>(worldY * static_cast<float>(bge::Scalar::OneRaw))));
    }

    bge::Scalar FixedDeltaTime()
    {
        return bge::Scalar::FromRaw(
            static_cast<bge::i64>(kFixedDtSeconds * static_cast<double>(bge::Scalar::OneRaw)));
    }

    void ClearSelection(AppState& app)
    {
        app.selectedEntities.clear();
    }

    bool IsSelected(const AppState& app, bge::Entity entity)
    {
        for (const bge::Entity selected : app.selectedEntities)
        {
            if (selected == entity)
            {
                return true;
            }
        }

        return false;
    }

    void SelectSingle(AppState& app, bge::Entity entity)
    {
        app.selectedEntities.clear();

        if (entity.IsValid())
        {
            app.selectedEntities.push_back(entity);
        }
    }

    bool PickUnit(AppState& app, const bge::Vec2& worldPosition, bge::Entity& outEntity)
    {
        const bge::Scalar pickRadius = bge::Scalar::FromInt(1);
        const bge::Scalar pickRadiusSq = pickRadius * pickRadius;

        for (const bge::Unit& unit : app.session.Units())
        {
            const bge::Vec2 delta = unit.movement.position - worldPosition;
            const bge::Scalar distSq = bge::Vec2::LengthSq(delta);

            if (distSq <= pickRadiusSq)
            {
                outEntity = unit.entity;
                return true;
            }
        }

        return false;
    }

    void SelectUnitsInBox(AppState& app)
    {
        const int left = app.selectionDrag.startScreenX < app.selectionDrag.currentScreenX
            ? app.selectionDrag.startScreenX
            : app.selectionDrag.currentScreenX;

        const int right = app.selectionDrag.startScreenX > app.selectionDrag.currentScreenX
            ? app.selectionDrag.startScreenX
            : app.selectionDrag.currentScreenX;

        const int top = app.selectionDrag.startScreenY < app.selectionDrag.currentScreenY
            ? app.selectionDrag.startScreenY
            : app.selectionDrag.currentScreenY;

        const int bottom = app.selectionDrag.startScreenY > app.selectionDrag.currentScreenY
            ? app.selectionDrag.startScreenY
            : app.selectionDrag.currentScreenY;

        app.selectedEntities.clear();

        for (const bge::Unit& unit : app.session.Units())
        {
            const int unitX =
                bge::DebugDraw::OriginX +
                static_cast<int>(unit.movement.position.x.ToFloat() * static_cast<float>(bge::DebugDraw::CellSize));

            const int unitY =
                bge::DebugDraw::OriginY +
                static_cast<int>(unit.movement.position.y.ToFloat() * static_cast<float>(bge::DebugDraw::CellSize));

            if (unitX >= left && unitX <= right &&
                unitY >= top && unitY <= bottom)
            {
                app.selectedEntities.push_back(unit.entity);
            }
        }
    }

    void PopulateScenario(AppState& app)
    {
        app.session.CreateUnit(
            bge::Vec2(bge::Scalar::FromInt(2), bge::Scalar::FromInt(2)),
            bge::Scalar::FromInt(1),
            bge::Scalar::FromRaw(bge::Scalar::OneRaw / 2));

        app.session.CreateUnit(
            bge::Vec2(bge::Scalar::FromInt(5), bge::Scalar::FromInt(3)),
            bge::Scalar::FromInt(1),
            bge::Scalar::FromRaw(bge::Scalar::OneRaw / 2));

        app.session.CreateUnit(
            bge::Vec2(bge::Scalar::FromInt(8), bge::Scalar::FromInt(5)),
            bge::Scalar::FromInt(1),
            bge::Scalar::FromRaw(bge::Scalar::OneRaw / 2));
    }

    void RebuildAppState(AppState& app)
    {
        const std::vector<bge::RecordedCommand> recordedCommands = app.recorder.Commands();
        const bge::u32 savedRecordStartTick = app.recordStartTick;
        const bge::u32 savedRecordEndTick = app.recordEndTick;
        const bge::u64 savedRecordEndHash = app.recordEndHash;
        const bge::u32 savedPlaybackEndTick = app.playbackEndTick;
        const bge::u64 savedPlaybackEndHash = app.playbackEndHash;
        const bool savedParityKnown = app.parityKnown;
        const bool savedParityMatch = app.parityMatch;

        app.~AppState();
        new (&app) AppState();

        if (!recordedCommands.empty())
        {
            app.recorder.Commands() = recordedCommands;
        }

        app.recordStartTick = savedRecordStartTick;
        app.recordEndTick = savedRecordEndTick;
        app.recordEndHash = savedRecordEndHash;
        app.playbackEndTick = savedPlaybackEndTick;
        app.playbackEndHash = savedPlaybackEndHash;
        app.parityKnown = savedParityKnown;
        app.parityMatch = savedParityMatch;

        app.session.SetCommandRecorder(nullptr);
        app.session.SetCommandRecordingBaseTick(0);

        PopulateScenario(app);
    }

    void StartRecording(AppState& app)
    {
        app.playingBack = false;
        app.recording = true;
        app.parityKnown = false;
        app.parityMatch = false;

        app.recorder.Clear();
        app.playback.Clear();
        app.selectedEntities.clear();

        app.recordStartTick = app.simulation.CurrentTick();
        app.recordEndTick = 0;
        app.recordEndHash = 0;
        app.playbackEndTick = 0;
        app.playbackEndHash = 0;

        app.session.SetCommandRecorder(&app.recorder);
        app.session.SetCommandRecordingBaseTick(app.simulation.CurrentTick());

        app.statusMessage = L"Replay started";
    }

    void StopRecording(AppState& app)
    {
        app.recording = false;
        app.recordEndTick = app.simulation.CurrentTick();
        app.recordEndHash = app.simulation.CurrentStateHash();
        app.session.SetCommandRecorder(nullptr);
        app.session.SetCommandRecordingBaseTick(0);
        app.statusMessage = L"Recording stopped";
    }

    void ResetScene(AppState& app)
    {
        app.recording = false;
        app.playingBack = false;
        app.session.SetCommandRecorder(nullptr);
        app.session.SetCommandRecordingBaseTick(0);
        RebuildAppState(app);
        app.statusMessage = L"Scene reset";
    }

    void StartPlayback(AppState& app)
    {
        if (app.recorder.IsEmpty())
        {
            app.statusMessage = L"No recorded commands";
            return;
        }

        const std::vector<bge::RecordedCommand> commands = app.recorder.Commands();

        app.recording = false;
        app.playingBack = false;
        app.session.SetCommandRecorder(nullptr);
        app.session.SetCommandRecordingBaseTick(0);

        RebuildAppState(app);

        app.playback.SetCommands(commands);
        app.playingBack = true;
        app.playbackEndTick = 0;
        app.playbackEndHash = 0;
        app.statusMessage = L"Playback replay";
    }

    void BuildSnapshot(AppState& app)
    {
        app.snapshot.Clear();
        app.snapshot.currentTick = app.simulation.CurrentTick();
        app.snapshot.stateHash = app.simulation.CurrentStateHash();
        app.snapshot.primarySelectedEntityIndex =
            app.selectedEntities.empty()
            ? kInvalidEntityIndex
            : app.selectedEntities.front().index;
        app.snapshot.selectedCount = static_cast<bge::u32>(app.selectedEntities.size());
        app.snapshot.totalUnitCount = static_cast<bge::u32>(app.session.Units().size());
        app.snapshot.tickRateHz = static_cast<bge::u32>(kTickRateHz);
        app.snapshot.inputMode =
            app.selectionDrag.dragging
            ? bge::DebugInputMode::DragSelecting
            : bge::DebugInputMode::Idle;

        app.snapshot.recording = app.recording;
        app.snapshot.playingBack = app.playingBack;
        app.snapshot.parityKnown = app.parityKnown;
        app.snapshot.parityMatch = app.parityMatch;
        app.snapshot.recordedCommandCount = app.recorder.Count();
        app.snapshot.playbackCommandCount = app.playback.CommandCount();
        app.snapshot.playbackNextIndex = app.playback.NextCommandIndex();
        app.snapshot.recordStartTick = app.recordStartTick;
        app.snapshot.recordEndTick = app.recordEndTick;
        app.snapshot.recordEndHash = app.recordEndHash;
        app.snapshot.playbackStartTick = 0;
        app.snapshot.playbackEndTick = app.playbackEndTick;
        app.snapshot.playbackEndHash = app.playbackEndHash;
        app.snapshot.statusMessage = app.statusMessage;

        if (app.selectionDrag.dragging)
        {
            app.snapshot.selectionBox.active = true;
            app.snapshot.selectionBox.startScreenX = app.selectionDrag.startScreenX;
            app.snapshot.selectionBox.startScreenY = app.selectionDrag.startScreenY;
            app.snapshot.selectionBox.currentScreenX = app.selectionDrag.currentScreenX;
            app.snapshot.selectionBox.currentScreenY = app.selectionDrag.currentScreenY;
        }

        for (const bge::Unit& unit : app.session.Units())
        {
            bge::SnapshotUnit snapshotUnit;
            snapshotUnit.entityIndex = unit.entity.index;
            snapshotUnit.position = unit.movement.position;
            snapshotUnit.velocity = unit.movement.velocity;
            snapshotUnit.targetPosition = unit.movement.target.position;
            snapshotUnit.hasTarget = unit.movement.hasTarget;
            snapshotUnit.selected = IsSelected(app, unit.entity);
            app.snapshot.units.push_back(snapshotUnit);
        }
    }

    void DrawFrame(HWND hwnd, HDC windowHdc)
    {
        RECT clientRect{};
        GetClientRect(hwnd, &clientRect);

        const int width = clientRect.right - clientRect.left;
        const int height = clientRect.bottom - clientRect.top;

        HDC memoryDc = CreateCompatibleDC(windowHdc);
        HBITMAP backBuffer = CreateCompatibleBitmap(windowHdc, width, height);
        HBITMAP oldBitmap = static_cast<HBITMAP>(SelectObject(memoryDc, backBuffer));

        if (g_AppState != nullptr)
        {
            BuildSnapshot(*g_AppState);
            bge::DebugDraw::Draw(memoryDc, clientRect, g_AppState->map, g_AppState->snapshot);
        }

        BitBlt(windowHdc, 0, 0, width, height, memoryDc, 0, 0, SRCCOPY);

        SelectObject(memoryDc, oldBitmap);
        DeleteObject(backBuffer);
        DeleteDC(memoryDc);
    }

    void RunSimulationStep(AppState& app, bge::Scalar fixedDt)
    {
        if (app.playingBack)
        {
            app.playback.PlaybackTick(app.simulation.CurrentTick(), app.session);
        }

        app.simulation.Tick();
        app.session.ApplyQueuedCommands();
        app.session.Tick(fixedDt);

        if (app.playingBack && app.playback.IsFinished())
        {
            app.playingBack = false;
            app.playbackEndTick = app.simulation.CurrentTick();
            app.playbackEndHash = app.simulation.CurrentStateHash();
            app.parityKnown = true;
            app.parityMatch = (app.recordEndHash == app.playbackEndHash);
            app.statusMessage = L"Replay finished";
        }
    }

    LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_ERASEBKGND:
            return 1;

        case WM_KEYDOWN:
        {
            if (g_AppState != nullptr)
            {
                switch (wParam)
                {
                case VK_F5:
                    StartRecording(*g_AppState);
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                case VK_F6:
                    StopRecording(*g_AppState);
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                case VK_F7:
                    ResetScene(*g_AppState);
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                case VK_F8:
                    StartPlayback(*g_AppState);
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                default:
                    break;
                }
            }
            break;
        }

        case WM_LBUTTONDOWN:
        {
            if (g_AppState != nullptr && !g_AppState->playingBack)
            {
                SetCapture(hwnd);
                g_AppState->selectionDrag.mouseDown = true;
                g_AppState->selectionDrag.dragging = false;
                g_AppState->selectionDrag.startScreenX = GET_X_LPARAM(lParam);
                g_AppState->selectionDrag.startScreenY = GET_Y_LPARAM(lParam);
                g_AppState->selectionDrag.currentScreenX = g_AppState->selectionDrag.startScreenX;
                g_AppState->selectionDrag.currentScreenY = g_AppState->selectionDrag.startScreenY;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (g_AppState != nullptr &&
                !g_AppState->playingBack &&
                g_AppState->selectionDrag.mouseDown)
            {
                const int mouseX = GET_X_LPARAM(lParam);
                const int mouseY = GET_Y_LPARAM(lParam);

                g_AppState->selectionDrag.currentScreenX = mouseX;
                g_AppState->selectionDrag.currentScreenY = mouseY;

                const int dx = mouseX - g_AppState->selectionDrag.startScreenX;
                const int dy = mouseY - g_AppState->selectionDrag.startScreenY;

                if (!g_AppState->selectionDrag.dragging)
                {
                    const int absDx = dx < 0 ? -dx : dx;
                    const int absDy = dy < 0 ? -dy : dy;

                    if (absDx >= kDragThresholdPixels || absDy >= kDragThresholdPixels)
                    {
                        g_AppState->selectionDrag.dragging = true;
                    }
                }

                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONUP:
        {
            if (g_AppState != nullptr &&
                !g_AppState->playingBack &&
                g_AppState->selectionDrag.mouseDown)
            {
                ReleaseCapture();

                const int mouseX = GET_X_LPARAM(lParam);
                const int mouseY = GET_Y_LPARAM(lParam);

                g_AppState->selectionDrag.currentScreenX = mouseX;
                g_AppState->selectionDrag.currentScreenY = mouseY;

                if (g_AppState->selectionDrag.dragging)
                {
                    SelectUnitsInBox(*g_AppState);
                }
                else
                {
                    const bge::Vec2 worldPosition = ScreenToWorld(mouseX, mouseY);
                    bge::Entity pickedEntity = bge::Entity::Invalid();

                    if (PickUnit(*g_AppState, worldPosition, pickedEntity))
                    {
                        SelectSingle(*g_AppState, pickedEntity);
                    }
                    else
                    {
                        ClearSelection(*g_AppState);
                    }
                }

                g_AppState->selectionDrag.mouseDown = false;
                g_AppState->selectionDrag.dragging = false;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            if (g_AppState != nullptr &&
                !g_AppState->playingBack &&
                !g_AppState->selectedEntities.empty())
            {
                const int mouseX = GET_X_LPARAM(lParam);
                const int mouseY = GET_Y_LPARAM(lParam);
                const bge::Vec2 target = ScreenToWorld(mouseX, mouseY);

                if (g_AppState->selectedEntities.size() == 1)
                {
                    g_AppState->session.QueueMoveCommand(g_AppState->selectedEntities[0], target);
                }
                else
                {
                    g_AppState->session.QueueGroupMoveCommand(
                        g_AppState->selectedEntities,
                        target,
                        bge::Scalar::FromInt(2));
                }

                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_SIZE:
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            DrawFrame(hwnd, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand)
{
    alignas(AppState) unsigned char appStorage[sizeof(AppState)];
    AppState* app = new (appStorage) AppState();
    g_AppState = app;

    PopulateScenario(*app);

    const wchar_t kClassName[] = L"BGEWindowClass";

    WNDCLASS wc{};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        kClassName,
        L"BGE Debug Window",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1400,
        900,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (hwnd == nullptr)
    {
        app->~AppState();
        return 1;
    }

    ShowWindow(hwnd, showCommand);
    UpdateWindow(hwnd);

    LARGE_INTEGER frequency{};
    LARGE_INTEGER previousCounter{};
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&previousCounter);

    double accumulatorSeconds = 0.0;
    const bge::Scalar fixedDt = FixedDeltaTime();

    MSG msg{};
    bool running = true;

    while (running)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running)
        {
            break;
        }

        LARGE_INTEGER currentCounter{};
        QueryPerformanceCounter(&currentCounter);

        const double frameSeconds =
            static_cast<double>(currentCounter.QuadPart - previousCounter.QuadPart) /
            static_cast<double>(frequency.QuadPart);

        previousCounter = currentCounter;

        double clampedFrameSeconds = frameSeconds;
        if (clampedFrameSeconds > 0.25)
        {
            clampedFrameSeconds = 0.25;
        }

        accumulatorSeconds += clampedFrameSeconds;

        while (accumulatorSeconds >= kFixedDtSeconds)
        {
            RunSimulationStep(*app, fixedDt);
            accumulatorSeconds -= kFixedDtSeconds;
        }

        InvalidateRect(hwnd, nullptr, FALSE);
        Sleep(1);
    }

    app->~AppState();
    return 0;
}
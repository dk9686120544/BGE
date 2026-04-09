\# BGE Roadmap v1.0



\## 1. Назначение



Этот документ фиксирует практический roadmap первой фазы разработки BGE.



Его задача:

\- перевести Architecture Freeze в реальные шаги;

\- определить, что именно должно быть сделано в каком порядке;

\- не допустить преждевременного расширения проекта;

\- удержать команду разработки на траектории "правильный фундамент сначала".



Текущий roadmap описывает путь от пустого репозитория до первого рабочего deterministic vertical slice.



\---



\## 2. Главный принцип roadmap



Разработка идет по слоям снизу вверх:



1\. Documents

2\. Core

3\. Math

4\. ECS

5\. Simulation loop

6\. World representation

7\. Navigation

8\. Movement

9\. Game layer

10\. App / visualization



Ни один следующий слой не считается разрешенным к активной разработке, если предыдущий слой не доведен до рабочего и тестируемого состояния.



\---



\## 3. Обязательное правило прогресса



Переход к следующему этапу разрешен только если:

\- код компилируется;

\- тесты текущего слоя проходят;

\- нет известных deterministic violations;

\- базовый debug/proof механизм уже есть.



Если текущий слой нестабилен, расширение вверх запрещено.



\---



\## 4. Фаза 0 — Freeze и инфраструктура



\## Цель

Зафиксировать правила проекта и создать технический каркас.



\## Входит

\- структура каталогов;

\- solution и проекты:

&#x20; - Engine

&#x20; - Game

&#x20; - App

&#x20; - Tests

\- базовые настройки Visual Studio;

\- документы:

&#x20; - `ArchitectureFreeze\_v1.md`

&#x20; - `MathSpec\_v1.md`

&#x20; - `DeterminismCharter\_v1.md`

&#x20; - `EntityStorageSpec\_v1.md`

&#x20; - `Roadmap\_v1.md`



\## Результат фазы

\- проект собирается как пустой каркас;

\- файловая структура зафиксирована;

\- документы лежат в `docs/`;

\- build layout и project dependencies настроены;

\- дальнейшая разработка идет уже внутри зафиксированного скелета.



\---



\## 5. Фаза 1 — Core и Math



\## Цель

Создать надежную математическую и low-level базу simulation domain.



\## Подсистемы

\### `engine/core`

\- `Types.h`

\- `Assert.h`

\- `Hash.h`

\- `Random.h/.cpp`



\### `engine/math`

\- `Scalar.h/.cpp`

\- `Vec2.h/.cpp`

\- `Math.h/.cpp`



\### `tests/math`

\- `ScalarTests.cpp`

\- `Vec2Tests.cpp`

\- `MathTests.cpp`



\## Что должно быть реализовано

\- fixed-point scalar;

\- deterministic multiply/divide;

\- integer sqrt;

\- базовые vector operations;

\- deterministic RNG;

\- hash utilities, достаточные для state hashing.



\## Acceptance criteria

\- math tests проходят;

\- нет UB в известных math путях;

\- golden tests зелёные;

\- long-run deterministic tests зелёные;

\- одинаковые сценарии дают одинаковый raw/result/hash.



\## Stop conditions

Если math layer не зелёная, дальше идти нельзя.



\---



\## 6. Фаза 2 — Entity / Storage



\## Цель

Создать минимальную, но корректную entity/storage систему.



\## Подсистемы

\### `engine/ecs`

\- `Entity.h`

\- `EntityManager.h/.cpp`

\- `ComponentStorage.h`



\### `tests/ecs`

\- `EntityManagerTests.cpp`



\## Что должно быть реализовано

\- `EntityHandle { index, generation }`

\- null entity

\- entity creation

\- validation

\- deferred deletion

\- end-of-tick cleanup

\- freelist

\- alive mask

\- stable iteration assumptions



\## Acceptance criteria

\- create/delete/reuse работает;

\- generation checks работают;

\- invalid old handles детектятся;

\- freelist ведет себя детерминированно;

\- tests зелёные.



\---



\## 7. Фаза 3 — Simulation Core



\## Цель

Получить deterministic simulation skeleton без полноценного мира.



\## Подсистемы

\### `engine/sim`

\- `Command.h`

\- `CommandQueue.h/.cpp`

\- `TickClock.h`

\- `Simulation.h/.cpp`

\- `StateHash.h/.cpp`



\### `tests/sim`

\- `CommandQueueTests.cpp`

\- `SimulationTests.cpp`

\- `StateHashTests.cpp`



\## Что должно быть реализовано

\- fixed tick loop;

\- command queue;

\- deterministic update order;

\- end-of-tick cleanup point;

\- state hashing;

\- replay-friendly command application foundation.



\## Acceptance criteria

\- simulation tick стабилен;

\- команды применяются в фиксированном порядке;

\- state hash считается;

\- одинаковый input sequence дает одинаковый hash;

\- tests зелёные.



\---



\## 8. Фаза 4 — World Representation



\## Цель

Смоделировать минимальный мир, пригодный для pathfinding и occupancy.



\## Подсистемы

\### `engine/world`

\- `WorldTypes.h`

\- `Grid.h/.cpp`

\- `Map.h/.cpp`

\- `Occupancy.h/.cpp`



\## Что должно быть реализовано

\- grid representation;

\- map dimensions;

\- walkable / blocked cells;

\- static blockers;

\- occupancy tracking;

\- layer-ready layout для будущего расширения.



\## Acceptance criteria

\- можно создать карту;

\- можно задать блокеры;

\- occupancy корректно отражает занятые ячейки;

\- deterministic world queries работают стабильно.



\---



\## 9. Фаза 5 — Navigation v0



\## Цель

Получить первый рабочий path query вокруг препятствий.



\## Подсистемы

\### `engine/nav`

\- `PathTypes.h`

\- `Pathfinder.h/.cpp`



\### `tests/nav`

\- `PathfinderTests.cpp`



\## Что должно быть реализовано

\- grid-based path query;

\- A\* v0;

\- path result structure;

\- обход простых статических препятствий.



\## Что НЕ делаем

\- flow fields;

\- hierarchical navigation;

\- JPS;

\- navmesh;

\- dynamic terrain updates beyond minimal blockers.



\## Acceptance criteria

\- путь находится вокруг стены/блока;

\- путь отсутствует, если прохода нет;

\- повторный запрос дает тот же результат;

\- tests зелёные.



\---



\## 10. Фаза 6 — Movement v0



\## Цель

Получить локальное deterministic движение юнита к цели.



\## Подсистемы

\### `engine/movement`

\- `MovementTypes.h`

\- `Arrival.h/.cpp`

\- `SeparationSystem.h/.cpp`

\- `MovementSystem.h/.cpp`



\### `tests/movement`

\- `MovementTests.cpp`

\- `SeparationTests.cpp`

\- `ArrivalTests.cpp`



\## Что должно быть реализовано

\- desired velocity;

\- движение к waypoint/target;

\- arrival slowdown;

\- basic separation;

\- position integration.



\## Что НЕ делаем

\- ORCA;

\- full steering stack;

\- formation-aware movement;

\- advanced crowd behaviors.



\## Acceptance criteria

\- юнит движется к цели;

\- юнит тормозит у цели;

\- два юнита не схлопываются полностью друг в друга;

\- результат воспроизводим;

\- tests зелёные.



\---



\## 11. Фаза 7 — Game Layer



\## Цель

Добавить поверх engine конкретную RTS-логику минимального уровня.



\## Подсистемы

\### `game/rules`

\- `GameRules.h`

\- `UnitRules.h`



\### `game/commands`

\- `MoveCommand.h/.cpp`



\### `game/units`

\- `Unit.h/.cpp`



\### `game/session`

\- `GameSession.h/.cpp`



\## Что должно быть реализовано

\- базовый runtime unit;

\- move command;

\- session-level orchestration;

\- input -> command translation contract.



\## Acceptance criteria

\- можно создать unit;

\- можно выдать move command;

\- команда попадает в simulation;

\- simulation начинает движение юнита.



\---



\## 12. Фаза 8 — Bridge и Debug Visualization



\## Цель

Сделать состояние simulation наблюдаемым.



\## Подсистемы

\### `engine/bridge`

\- `Snapshot.h/.cpp`



\### `engine/debug`

\- `DebugDraw.h/.cpp`



\### `app/win32`

\- `Main.cpp`

\- `Win32Window.cpp`

\- `Win32Input.cpp`



\## Что должно быть реализовано

\- snapshot export;

\- bridge из fixed world в render-friendly representation;

\- debug drawing:

&#x20; - grid;

&#x20; - units;

&#x20; - blockers;

&#x20; - velocity/debug vectors;

\- input handling;

\- move command by click.



\## Acceptance criteria

\- приложение открывается;

\- карта видна;

\- юниты видны;

\- по input можно выдать move command;

\- движение видно на экране;

\- debug visualization подтверждает path/movement behavior.



\---



\## 13. Что считается первым milestone



Первый milestone считается достигнутым, когда есть:



\- приложение запускается;

\- есть карта;

\- есть несколько юнитов;

\- можно выбрать цель перемещения;

\- move command проходит через session -> game -> simulation;

\- path query работает;

\- юнит движется детерминированно;

\- arrival работает;

\- separation работает в базовом виде;

\- state hash считается;

\- одинаковый сценарий воспроизводим.



Это и есть первый настоящий vertical slice.



\---



\## 14. Строго запрещенные отвлечения до milestone 1



До завершения первого milestone запрещено переключаться на:



\- advanced renderer;

\- material/shader architecture;

\- editor;

\- audio;

\- animation;

\- job system;

\- networking;

\- rollback;

\- replay productization;

\- scripting;

\- asset pipeline;

\- GPU pathfinding;

\- AVX2 optimization pass;

\- “красивую” architecture refactor ради абстракций без реальной необходимости.



\---



\## 15. Приоритеты после milestone 1



Только после прохождения первого milestone разрешается обсуждать следующие ветки развития:



1\. Расширение movement stack

2\. Более сильный navigation layer

3\. Combat

4\. Resource/economy loop

5\. Replay subsystem

6\. Networking readiness

7\. Performance passes

8\. Rendering improvements

9\. Tooling/editor



\---



\## 16. Правило принятия решений



Если появляется спорный вопрос, решение принимается по следующему приоритету:



1\. Determinism

2\. Simplicity

3\. Testability

4\. Debuggability

5\. Data layout quality

6\. Performance

7\. Convenience

8\. Aesthetics



\---



\## 17. Статус документа



Этот roadmap — обязательный operational plan для первой фазы разработки BGE.

Он может уточняться, но не должен ломать Architecture Freeze без явного архитектурного пересмотра.


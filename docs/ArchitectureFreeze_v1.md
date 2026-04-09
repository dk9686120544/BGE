\# BGE Architecture Freeze v1.0



\## 1. Цель



BGE v1.0 — это фундамент детерминированного RTS-движка с минимальным вертикальным срезом.



Цель текущей фазы:

\- построить правильную архитектурную основу;

\- исключить решения, ведущие к переписыванию ядра;

\- получить рабочий minimal vertical slice:

&#x20; - карта;

&#x20; - несколько юнитов;

&#x20; - команда перемещения;

&#x20; - детерминированный tick loop;

&#x20; - path query v0;

&#x20; - separation;

&#x20; - arrival;

&#x20; - debug visualization.



Текущая фаза \*\*не\*\* ставит целью создание полного RTS, редактора, сетевого кода, продвинутого рендера или production asset pipeline.



\---



\## 2. Основные архитектурные принципы



\### 2.1. Simulation и Presentation разделены жестко

Simulation — это отдельный детерминированный мир.

Presentation — это клиент чтения, не источник истины.



Presentation:

\- не принимает gameplay-решений;

\- не меняет simulation state напрямую;

\- не содержит authoritative данных о мире;

\- получает только snapshot/bridge-данные из Simulation.



\### 2.2. Движок проектируется от Simulation, а не от Render

Порядок приоритетов:

1\. Determinism

2\. Correctness

3\. Data layout

4\. Debuggability

5\. Performance

6\. Presentation



\### 2.3. Все gameplay-решения принимаются только внутри тиков

Нет “полу-обновлений”, нет логики “между кадрами”.

Любое изменение authoritative state должно происходить внутри фиксированного tick loop.



\### 2.4. Основа симуляции — data-oriented layout

Simulation хранит данные в виде плоских структур и массивов.

Приоритет отдается:

\- предсказуемому порядку обхода;

\- сериализуемости;

\- кэш-локальности;

\- простоте state hashing;

\- простоте replay/rollback readiness.



\---



\## 3. Обязательные правила v1.0



\### 3.1. Нет float/double в simulation domain

Внутри simulation запрещены:

\- `float`

\- `double`

\- логика, зависящая от `<cmath>`

\- вычисления, результат которых может различаться между платформами/компиляторами



Числа с плавающей точкой разрешены только в presentation/bridge/input adaptation, но не в authoritative simulation state.



\### 3.2. Fixed timestep

Simulation работает на фиксированном шаге:

\- `20 Hz`

\- `50 ms` на тик



Если машина не успевает, simulation не “ускоряется” и не “подстраивается”.

Допустимо замедление приложения, но не нарушение deterministic behavior.



\### 3.3. Один authoritative state

Есть только один authoritative simulation state.

Никаких вторых “почти таких же” копий данных для логики.



\### 3.4. Replay-first mindset

Система изначально проектируется так, чтобы:

\- команды можно было сохранять;

\- состояние можно было хэшировать;

\- одинаковая последовательность команд давала одинаковый итоговый state hash.



\### 3.5. Debug-first mindset

Каждая критическая система должна быть проверяема:

\- тестами;

\- логами;

\- state hash;

\- debug visualization.



\---



\## 4. Минимальные домены v1.0



\## 4.1. `engine/core`

Базовые системы:

\- типы;

\- assert;

\- hash;

\- deterministic random;

\- общие low-level утилиты.



\## 4.2. `engine/math`

Математика simulation-домена:

\- fixed-point scalar;

\- vectors;

\- integer sqrt;

\- базовая геометрия.



\## 4.3. `engine/ecs`

Минимальная ECS-основа:

\- entity handle;

\- lifecycle;

\- generation;

\- freelist;

\- simple storage.



\## 4.4. `engine/sim`

Simulation core:

\- tick loop;

\- command queue;

\- update order;

\- state hash.



\## 4.5. `engine/world`

Представление мира:

\- grid;

\- blockers;

\- occupancy;

\- pathing data;

\- world layers.



\## 4.6. `engine/nav`

Навигация v0:

\- grid-based path query;

\- A\*;

\- path types.



\## 4.7. `engine/movement`

Локальное движение:

\- desired velocity;

\- arrival;

\- separation;

\- integration.



\## 4.8. `engine/bridge`

Экспорт simulation state в presentation-friendly snapshot.



\## 4.9. `engine/debug`

Debug helpers и структуры визуализации.



\## 4.10. `engine/platform`

Платформенные абстракции, необходимые движку.



\## 4.11. `game`

RTS-логика поверх engine:

\- rules;

\- commands;

\- units;

\- session.



\## 4.12. `app`

Host-приложение:

\- entry point;

\- создание окна;

\- message loop;

\- запуск engine/game.



\## 4.13. `tests`

Модульные и интеграционные тесты ядра.



\---



\## 5. Что входит в минимальный вертикальный срез



В состав первого vertical slice входят:

\- fixed-point math;

\- entity creation/deletion;

\- command queue;

\- tick loop;

\- state hashing;

\- простая карта;

\- static blockers;

\- grid occupancy;

\- A\* path query v0;

\- move command;

\- movement with arrival;

\- separation;

\- snapshot export;

\- debug drawing;

\- input -> command translation.



\---



\## 6. Что НЕ входит в v1.0



Следующие системы осознанно НЕ реализуются в текущей стартовой фазе:



\- rollback netcode;

\- lockstep multiplayer;

\- replay subsystem как полноценный продуктовый модуль;

\- flow fields;

\- ORCA / RVO;

\- hierarchical navigation;

\- GPU-driven rendering;

\- production renderer;

\- audio;

\- animation;

\- editor;

\- scripting;

\- asset pipeline;

\- prefab system;

\- job system;

\- AVX2/AVX-512 optimized paths как обязательная часть старта.



Если что-то из этого появится, это должно быть отдельным осознанным решением после стабилизации базы.



\---



\## 7. Структурные правила



\### 7.1. На диске движок один

На диске проект организован как один движок `BGE`, а не как набор разрозненных одноименных корней.



\### 7.2. В solution минимально 4 проекта

В Visual Studio solution содержит:

\- `Engine`

\- `Game`

\- `App`

\- `Tests`



Это достаточно для текущей фазы.

Дальнейшее дробление на дополнительные проекты запрещено до появления реальной необходимости.



\### 7.3. Папки внутри `engine` не являются отдельными проектами

Папки:

\- `core`

\- `math`

\- `ecs`

\- `sim`

\- `world`

\- `nav`

\- `movement`

\- `bridge`

\- `debug`

\- `platform`



являются модулями одного проекта `Engine`, а не отдельными VS projects.



\---



\## 8. Архитектурные запреты



Запрещено:

\- смешивать simulation и presentation state;

\- использовать float/double в sim;

\- использовать unordered containers в критических sim-путях;

\- использовать глобальный RNG;

\- использовать wall-clock time в sim;

\- использовать многопоточность внутри sim v1;

\- принимать gameplay-решения в render/app слое;

\- делать “временные” хаки, которые меняют authoritative state вне tick loop.



\---



\## 9. Критерий готовности базы



База считается пригодной для дальнейшего развития, если выполнено следующее:

\- math tests зелёные;

\- entity/storage tests зелёные;

\- deterministic tick tests зелёные;

\- одинаковый набор команд даёт одинаковый state hash;

\- unit move scenario воспроизводим;

\- debug visualization подтверждает корректность grid/path/movement behavior.



\---



\## 10. Статус документа



Этот документ является архитектурным контрактом текущей фазы.

Изменения в него вносятся только осознанно и с пониманием последствий для determinism, storage, simulation loop и world model.


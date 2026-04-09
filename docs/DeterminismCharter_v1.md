\# BGE Determinism Charter v1.0



\## 1. Назначение



Этот документ определяет обязательные правила детерминированной симуляции в BGE v1.0.



Цель:

\- гарантировать одинаковое поведение simulation при одинаковом initial state и одинаковой последовательности команд;

\- исключить скрытые источники nondeterminism;

\- обеспечить базу для replay, state hashing и будущего lockstep-ready design.



\---



\## 2. Главное правило



Одинаковый initial state + одинаковые команды + одинаковый порядок выполнения = одинаковый итоговый simulation state.



Если это правило нарушается, это считается критической архитектурной ошибкой.



\---



\## 3. Абсолютные запреты



\## 3.1. Floating-point в simulation

Запрещены:

\- `float`

\- `double`

\- float-based math в authoritative simulation code



\## 3.2. Undefined behavior

Любое UB запрещено.



В том числе:

\- signed overflow;

\- invalid casts;

\- выход за границы массивов;

\- использование неинициализированных данных;

\- опора на implementation-defined arithmetic как на часть deterministic contract.



\## 3.3. Unordered containers в critical sim paths

Запрещены в critical deterministic paths:

\- `std::unordered\_map`

\- `std::unordered\_set`



Причина:

\- непредсказуемый порядок обхода;

\- возможная зависимость от реализации.



\## 3.4. Wall-clock time

Запрещено использовать внутри simulation:

\- frame delta time;

\- wall-clock time;

\- OS timers как источник simulation step.



Simulation использует только фиксированный tick.



\## 3.5. Threads inside sim v1

Многопоточность внутри sim v1 запрещена.



Simulation v1 — строго single-threaded.



\## 3.6. Global mutable state

Запрещены:

\- глобальные PRNG;

\- глобальные mutable caches;

\- скрытые singleton-state зависимости, влияющие на simulation.



\---



\## 4. Fixed timestep contract



Simulation работает на фиксированном шаге:

\- 20 Hz

\- 50 ms per tick



Все gameplay-изменения authoritative state происходят только в рамках тиков.



Никакой логики “между тиками” не существует.



\---



\## 5. Execution order contract



\## 5.1. System order

Порядок обновления систем должен быть:

\- фиксирован;

\- известен;

\- одинаков в каждом тике.



Он не может зависеть от:

\- набора данных;

\- случайного порядка контейнеров;

\- порядка регистрации в рантайме.



\## 5.2. Entity traversal order

Обход сущностей должен быть:

\- последовательным;

\- предсказуемым;

\- стабильным.



Для v1.0 стандарт:

\- flat arrays;

\- индексный порядок от меньшего к большему.



\## 5.3. Command processing order

Очередь команд должна обрабатываться в детерминированном порядке.



Если несколько команд находятся в одном тике, их порядок должен быть:

\- фиксирован контрактом очереди;

\- не зависеть от случайных факторов платформы.



\---



\## 6. State ownership contract



Есть только один authoritative simulation state.



Запрещено:

\- дублировать gameplay-данные в presentation как “почти authoritative”;

\- принимать решения на основании render-only представлений;

\- иметь несколько конкурирующих источников истины.



\---



\## 7. PRNG contract



Если randomness нужен в simulation:

\- используется только deterministic PRNG;

\- его состояние хранится внутри simulation state;

\- seed и текущее состояние являются частью replayable world state.



Запрещено:

\- использовать `rand()`;

\- использовать глобальный RNG;

\- использовать OS randomness внутри simulation.



\---



\## 8. Input boundary contract



Пользовательский input сам по себе не является simulation state.



Input:

\- принимается на boundary layer;

\- переводится в deterministic command representation;

\- попадает в command queue;

\- применяется только на нужном тике.



\---



\## 9. Hashing contract



\## 9.1. Hash every tick

Каждый тик обязан иметь возможность вычислить deterministic state hash.



Минимум для v1.0:

\- hash всех активных позиций сущностей;

\- по мере роста системы hash может расширяться.



\## 9.2. Hash mismatch

Если при одинаковом сценарии два запуска дают разный state hash, разработка не продолжается до нахождения причины.



Это не warning.

Это stop condition.



\---



\## 10. Replay contract



Replay-ready design является обязательным требованием архитектуры.



Для v1.0 это означает:

\- команды могут быть записаны;

\- simulation можно прогнать повторно;

\- одинаковый набор команд даёт одинаковый итоговый hash.



Полноценный replay subsystem может быть реализован позже, но архитектура должна быть replay-safe уже сейчас.



\---



\## 11. Compiler parity



Поддерживаемые compiler paths должны давать одинаковый результат.



Минимум:

\- MSVC

\- Clang/GCC path, если он поддерживается проектом



Проверка:

\- golden tests;

\- deterministic scenario tests.



\---



\## 12. Debugging contract



Каждая критическая deterministic system должна быть проверяема:

\- unit tests;

\- integration tests;

\- logs;

\- state hash;

\- debug visualization.



Без возможности проверить систему она не считается production-worthy.



\---



\## 13. Acceptance criteria



Simulation core считается соответствующим determinism charter, если:

\- одинаковый initial state воспроизводим;

\- одинаковая последовательность команд воспроизводима;

\- state hash совпадает;

\- порядок выполнения систем стабилен;

\- порядок обхода сущностей стабилен;

\- math layer соответствует math spec;

\- нет известных источников UB или nondeterminism.



\---



\## 14. Статус документа



Этот документ — обязательный закон simulation domain.

Нарушение любого из его пунктов считается архитектурной проблемой, а не локальной технической деталью.


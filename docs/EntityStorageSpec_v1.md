\# BGE Entity \& Storage Specification v1.0



\## 1. Назначение



Этот документ определяет минимальную entity/storage модель для BGE v1.0.



Цели:

\- детерминированный lifecycle сущностей;

\- безопасные handle semantics;

\- простая и проверяемая модель хранения;

\- совместимость с state hashing, replay и deferred deletion;

\- отсутствие лишней сложности на старте.



v1.0 сознательно использует прагматичную, минимальную ECS-основу, а не “максимально абстрактную” ECS.



\---



\## 2. Основные принципы



\### 2.1. Entity — это handle, а не объект

Entity не хранит бизнес-логику и не является heavyweight object.

Entity — это handle для доступа к данным.



\### 2.2. Данные хранятся отдельно

Компоненты и состояние хранятся в плоских массивах и структурах хранения.



\### 2.3. Порядок важнее универсальности

Приоритет:

\- deterministic traversal;

\- простота;

\- проверяемость;

\- простая сериализация;

\- предсказуемое поведение удаления и переиспользования слотов.



\---



\## 3. Entity handle



\## 3.1. Формат

Entity handle:



```cpp

struct EntityHandle

{

&#x20;   uint32\_t index;

&#x20;   uint32\_t generation;

};


3.2. Смысл полей
index — индекс слота в storage
generation — версия сущности в этом слоте
3.3. Null entity

Недействительная сущность определяется как:

index = 0xFFFFFFFF

Generation у null-entity не имеет смысловой нагрузки.

4. Slot model

Каждая сущность живет в слоте с индексом index.

Слот может быть:

свободным;
занятым живой сущностью;
помеченным к удалению до конца текущего тика.

Generation позволяет отличать:

старую удалённую сущность;
новую сущность, созданную в том же слоте позже.
5. Storage model
5.1. Базовый подход

Для v1.0 используется простая storage model:

flat arrays;
индексный доступ;
один и тот же index используется для entity slot и component slot.
5.2. Component storage

Для каждого component/data type используется собственный массив хранения.

Принцип:

componentArray[entity.index] соответствует слоту сущности;
наличие или отсутствие компонента управляется явно;
критичен стабильный индексный доступ.
5.3. Alive tracking

Система обязана хранить информацию о том, какие слоты живы.

Допустимые формы:

std::vector<bool>
bitset-like структура
отдельный alive mask

Требование:

проверка живости сущности должна быть дешевой и детерминированной.
6. Creation policy
6.1. Freelist

Создание новой сущности использует freelist.

Правило:

берется свободный индекс из freelist;
слот активируется;
generation уже должен соответствовать текущей версии слота.
6.2. Lowest available index

Для v1.0 рекомендовано использовать предсказуемую политику повторного использования слотов.

Предпочтительно:

lowest available index
или
иная политика, если она стабильна и документирована.

Главное требование:

поведение должно быть детерминированным.
7. Deletion policy
7.1. Deferred deletion

Удаление сущностей в середине тика напрямую не выполняется.

Вместо этого:

сущность помечается на удаление;
её handle попадает в DeferredDeleteQueue;
реальная очистка происходит в конце тика.
7.2. Cleanup at end of tick

На фазе cleanup:

слот помечается как неактивный;
компоненты очищаются или инвалидируются согласно правилам хранилища;
generation увеличивается;
индекс возвращается во freelist.
7.3. Dangling handles

После удаления старый handle должен перестать считаться валидным.

Проверка валидности обязана учитывать:

корректность индекса;
alive state;
совпадение generation.
8. Validation contract

Любая система, принимающая EntityHandle, обязана иметь возможность проверить его валидность.

Минимальная проверка:

index в допустимом диапазоне;
слот жив;
generation совпадает.

Если хотя бы один пункт не выполняется, handle считается недействительным.

9. Iteration contract
9.1. Stable order

Итерация по сущностям и компонентам в sim должна быть:

стабильной;
предсказуемой;
индексной.
9.2. No hidden reordering

Запрещены структуры хранения, которые:

случайно меняют порядок обхода;
скрывают перемещения данных без явного контракта;
создают nondeterministic iteration order.

Для v1.0 приоритет у простоты, а не у “наиболее умной ECS”.

10. Component presence

v1.0 допускает простую model presence tracking:

отдельные presence masks;
отдельные alive flags для нужных storage;
прямую логику “компонент есть/нет”.

Текущая фаза не требует сложной archetype/chunk architecture.

Если проект вырастет, это может быть пересмотрено позже отдельным решением.

11. Grid storage policy

Spatial grid не хранит полные entity objects и не хранит raw pointers.

Для v1.0 grid хранит только:

uint32_t index
или иную компактную индексную ссылку, если она явно документирована.

Обязательное правило:
любая система, читающая индекс из grid, должна проверить:

что слот еще жив;
что это действительно ожидаемая сущность;
при необходимости — что generation совпадает через authoritative entity manager.

Grid не является источником истины о lifecycle сущности.
Grid — это вспомогательная spatial structure.

12. Ownership and authority

EntityManager / storage layer является authoritative источником истины о:

валидности сущности;
lifecycle;
generation;
alive state.

Ни одна вспомогательная система не может считаться authoritative по этому вопросу.

13. Minimal required capabilities for v1.0

Storage layer v1.0 обязана поддерживать:

create entity;
validate entity;
mark entity for deletion;
end-of-tick cleanup;
deterministic iteration;
direct index-based component access;
basic component presence tracking.
14. Acceptance criteria

Entity/storage layer считается принятой, если:

generation checks работают корректно;
удаленная сущность невалидна;
повторное создание в старом слоте даёт новый generation;
freelist работает детерминированно;
iteration order стабилен;
deferred deletion работает;
tests покрывают lifecycle и invalid handle cases.
15. Статус документа

Этот документ фиксирует минимальный pragmatic ECS contract для BGE v1.0.
Любая попытка заменить его на более сложную storage architecture должна рассматриваться как отдельное архитектурное решение, а не как “естественный рефакторинг”.

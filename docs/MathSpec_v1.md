\# BGE Math Specification v1.0



\## 1. Назначение



Этот документ описывает математическую основу simulation domain в BGE v1.0.



Цели:

\- обеспечить детерминированность;

\- исключить зависимость от floating-point поведения платформы;

\- обеспечить одинаковый результат на разных компиляторах при одинаковом коде и данных;

\- дать простой и проверяемый fixed-point фундамент для movement, navigation и simulation systems.



\---



\## 2. Общий принцип



В authoritative simulation state запрещены числа с плавающей точкой.



Все simulation-значения, требующие дробной части, хранятся в signed fixed-point формате с raw storage на `int64\_t` и 16 дробными битами.



\---



\## 3. Scalar format



\## 3.1. Представление

Основной scalar type:

\- signed fixed-point

\- raw type: `int64\_t`

\- fractional bits: `16`



Логически это fixed-point scalar с 16 дробными битами.

Важно не текстовое имя формата, а фактический контракт:

\- `raw` хранится в `int64\_t`

\- масштаб равен `1 << 16`



\## 3.2. Константы

Обязательные константы:

\- `FRACTION\_BITS = 16`

\- `ONE\_RAW = 1 << 16`

\- `HALF\_RAW = 1 << 15`



\## 3.3. Диапазон

Диапазон определяется пределами `int64\_t`.

Спецификация не фиксирует “красивое” десятичное число диапазона, чтобы избежать двусмысленностей.



Правило:

\- любой выход за безопасный рабочий диапазон simulation считается ошибкой проектирования или ошибкой данных;

\- signed overflow недопустим.



\---



\## 4. Семантика операций



\## 4.1. Сложение и вычитание

Сложение и вычитание выполняются как операции над `raw`.



Правила:

\- результат должен быть детерминирован;

\- signed overflow недопустим;

\- при необходимости допустимы dev-assert проверки диапазона.



\## 4.2. Умножение

Умножение двух fixed-point значений обязано:

\- использовать расширенную промежуточную разрядность;

\- затем возвращаться в fixed-point domain с сохранением 16 дробных бит;

\- использовать округление `truncate toward zero`.



\### Реализация:

\- MSVC x64: `\_umul128` и явная обработка знака

\- Clang/GCC: `\_\_int128`



Запрещено:

\- полагаться на обычное `int64\_t \* int64\_t` без расширения;

\- использовать implementation-defined поведение как часть контракта;

\- использовать signed overflow.



\## 4.3. Деление

Деление fixed-point значений обязано:

\- сначала расширять делимое в соответствии с количеством дробных бит;

\- затем выполнять integer division;

\- использовать округление `truncate toward zero`.



\### Реализация:

\- MSVC x64: `\_udiv128` и явная обработка знака

\- Clang/GCC: `\_\_int128`



Деление на ноль запрещено.

В dev builds допустим assert.



\## 4.4. Сравнения

Сравнения выполняются напрямую по `raw`.



Разрешены:

\- `==`

\- `!=`

\- `<`

\- `>`

\- `<=`

\- `>=`



\## 4.5. Absolute value

Вычисление модуля обязано избегать UB на `INT64\_MIN`.



Правило:

\- нельзя использовать наивное signed negation без проверки;

\- допустима реализация через unsigned-domain helper;

\- если `INT64\_MIN` в данном контексте недопустим как валидное simulation значение, это должно быть отдельно защищено assert-ом в dev builds.



\---



\## 5. Rounding policy



Для fixed-point arithmetic в v1.0 фиксируется единая политика:



\- multiplication: `truncate toward zero`

\- division: `truncate toward zero`



Запрещено:

\- смешивать разные rounding policy в разных частях math layer без отдельного документированного основания;

\- использовать `floor` как универсальное правило для всех операций.



Отдельные функции типа `FloorToInt` или `FloorToCell` могут существовать как специальные операции с явно обозначенной семантикой.



\---



\## 6. Square root



В simulation разрешен только integer-based sqrt.



Правила:

\- `std::sqrt` и прочие float-based функции запрещены в authoritative sim;

\- sqrt должен быть детерминирован;

\- реализация должна работать через integer algorithm.



Для v1.0 допустим deterministic integer sqrt, пригодный для:

\- distance;

\- normalization support;

\- movement math.



\---



\## 7. Vectors



Минимальный vector type для v1.0:

\- 2D vector на основе fixed-point scalar.



Минимальные операции:

\- addition

\- subtraction

\- scalar multiply

\- scalar divide

\- dot product

\- length squared

\- distance squared



Нормализация допускается в упрощенной deterministic форме, если она не нарушает math contract.



\---



\## 8. Запрещенные математические инструменты в sim



Запрещено использовать внутри simulation:

\- `float`

\- `double`

\- `std::sqrt`

\- `std::sin`

\- `std::cos`

\- `std::atan2`

\- любые функции `<cmath>`, если они работают через floating-point domain



\---



\## 9. Platform / compiler contract



\## 9.1. Поддерживаемые пути реализации

\- MSVC x64 path:

&#x20; - `\_umul128`

&#x20; - `\_udiv128`

\- Clang/GCC path:

&#x20; - `\_\_int128`



\## 9.2. Compiler parity

Поведение math layer на поддерживаемых компиляторах должно совпадать бит-в-бит для одного и того же набора входных данных.



Это подтверждается golden tests.



\---



\## 10. Conversion rules



\## 10.1. Simulation -> Presentation

Разрешено только в bridge/presentation domain.



Пример:

\- `float renderValue = raw / 65536.0f`



Эта операция запрещена внутри authoritative simulation logic.



\## 10.2. Input/Presentation -> Simulation

Разрешено только на границе системы:

\- input adaptation;

\- command creation;

\- bridge import layer, если таковой существует.



Правило:

\- float-based пользовательский ввод может быть преобразован в fixed-point только на boundary layer;

\- authoritative state хранится только в fixed-point.



\---



\## 11. Safety rules



Обязательные требования:

\- никакого UB;

\- никакого signed overflow;

\- никакой скрытой зависимости от поведения конкретного компилятора;

\- явная обработка sign;

\- явная политика округления;

\- deterministic tests обязательны.



\---



\## 12. Обязательные тесты



Math layer считается принятой только при наличии:

\- базовых representation tests;

\- multiply tests;

\- divide tests;

\- negative value tests;

\- zero tests;

\- sqrt tests;

\- vector tests;

\- long-run deterministic tests;

\- golden-value tests;

\- compiler parity tests, если в пайплайне больше одного компилятора.



\---



\## 13. Статус документа



Этот документ является обязательной спецификацией для всех math-типов и math-операций simulation domain.

Любое отклонение от этого контракта должно считаться архитектурным изменением, а не “локальной оптимизацией”.


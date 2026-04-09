#pragma once

#include "movement/MovementTypes.h"

namespace bge
{
    class SeparationSystem
    {
    public:
        static Vec2 ComputeSeparationOffset(
            const MovementState& self,
            const MovementState& other,
            const SeparationSettings& settings) noexcept;

        static void ApplyPairwiseSeparation(
            MovementState& a,
            MovementState& b,
            const SeparationSettings& settings) noexcept;
    };
}
#pragma once

#include "core/Types.h"

namespace bge
{
    struct Entity
    {
        static constexpr u32 InvalidIndex = 0xFFFFFFFFu;

        u32 index;
        u32 generation;

        constexpr Entity() noexcept
            : index(InvalidIndex)
            , generation(0)
        {
        }

        constexpr Entity(u32 inIndex, u32 inGeneration) noexcept
            : index(inIndex)
            , generation(inGeneration)
        {
        }

        static constexpr Entity Invalid() noexcept
        {
            return Entity(InvalidIndex, 0);
        }

        constexpr bool IsValid() const noexcept
        {
            return index != InvalidIndex;
        }

        constexpr bool operator==(const Entity& other) const noexcept
        {
            return index == other.index && generation == other.generation;
        }

        constexpr bool operator!=(const Entity& other) const noexcept
        {
            return !(*this == other);
        }
    };
}
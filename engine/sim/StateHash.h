#pragma once

#include "core/Hash.h"
#include "core/Types.h"
#include "math/Vec2.h"

namespace bge
{
    class StateHash
    {
    public:
        StateHash() = default;

        void Reset() noexcept;

        void AddU32(u32 value) noexcept;
        void AddI32(i32 value) noexcept;
        void AddU64(u64 value) noexcept;
        void AddI64(i64 value) noexcept;
        void AddBool(bool value) noexcept;
        void AddScalar(Scalar value) noexcept;
        void AddVec2(const Vec2& value) noexcept;

        u64 Value() const noexcept;

    private:
        Hasher64 m_Hasher;
    };
}
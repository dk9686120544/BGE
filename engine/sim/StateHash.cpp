#include "sim/StateHash.h"

namespace bge
{
    void StateHash::Reset() noexcept
    {
        m_Hasher.Reset();
    }

    void StateHash::AddU32(u32 value) noexcept
    {
        m_Hasher.AddU32(value);
    }

    void StateHash::AddI32(i32 value) noexcept
    {
        m_Hasher.AddI32(value);
    }

    void StateHash::AddU64(u64 value) noexcept
    {
        m_Hasher.AddU64(value);
    }

    void StateHash::AddI64(i64 value) noexcept
    {
        m_Hasher.AddI64(value);
    }

    void StateHash::AddBool(bool value) noexcept
    {
        m_Hasher.AddU8(value ? static_cast<u8>(1) : static_cast<u8>(0));
    }

    void StateHash::AddScalar(Scalar value) noexcept
    {
        m_Hasher.AddI64(value.Raw());
    }

    void StateHash::AddVec2(const Vec2& value) noexcept
    {
        AddScalar(value.x);
        AddScalar(value.y);
    }

    u64 StateHash::Value() const noexcept
    {
        return m_Hasher.Value();
    }
}
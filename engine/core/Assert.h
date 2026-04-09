#pragma once

#include <cstdlib>

#if defined(_MSC_VER)
#define BGE_DEBUG_BREAK() __debugbreak()
#else
#define BGE_DEBUG_BREAK() ((void)0)
#endif

#if defined(_DEBUG)
#define BGE_ASSERT(condition)                                                      \
        do                                                                             \
        {                                                                              \
            if (!(condition))                                                          \
            {                                                                          \
                BGE_DEBUG_BREAK();                                                     \
                std::abort();                                                          \
            }                                                                          \
        } while (false)
#else
#define BGE_ASSERT(condition) ((void)0)
#endif
#pragma once

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace bge::test
{
    class TestFailure final : public std::exception
    {
    public:
        explicit TestFailure(std::string message) noexcept
            : m_Message(std::move(message))
        {
        }

        const char* what() const noexcept override
        {
            return m_Message.c_str();
        }

    private:
        std::string m_Message;
    };

    struct TestCase
    {
        const char* suiteName;
        const char* testName;
        void (*function)();
    };

    inline std::vector<TestCase>& Registry()
    {
        static std::vector<TestCase> registry;
        return registry;
    }

    class TestRegistrar
    {
    public:
        TestRegistrar(const char* suiteName, const char* testName, void (*function)())
        {
            Registry().push_back({ suiteName, testName, function });
        }
    };

    inline void Fail(
        const char* expression,
        const char* file,
        int line,
        const std::string& message = std::string())
    {
        std::ostringstream stream;
        stream << file << "(" << line << "): test failure: " << expression;

        if (!message.empty())
        {
            stream << " | " << message;
        }

        throw TestFailure(stream.str());
    }

    inline int RunAllTests()
    {
        int passed = 0;
        int failed = 0;

        for (const TestCase& testCase : Registry())
        {
            try
            {
                testCase.function();
                ++passed;
                std::cout << "[PASS] " << testCase.suiteName << "." << testCase.testName << '\n';
            }
            catch (const TestFailure& failure)
            {
                ++failed;
                std::cout << "[FAIL] " << testCase.suiteName << "." << testCase.testName
                          << " -> " << failure.what() << '\n';
            }
            catch (const std::exception& exception)
            {
                ++failed;
                std::cout << "[FAIL] " << testCase.suiteName << "." << testCase.testName
                          << " -> unexpected exception: " << exception.what() << '\n';
            }
            catch (...)
            {
                ++failed;
                std::cout << "[FAIL] " << testCase.suiteName << "." << testCase.testName
                          << " -> unknown exception\n";
            }
        }

        std::cout << '\n';
        std::cout << "Passed: " << passed << '\n';
        std::cout << "Failed: " << failed << '\n';

        return failed == 0 ? 0 : 1;
    }
}

#define BGE_TEST(SUITE_NAME, TEST_NAME)                                                \
    static void SUITE_NAME##_##TEST_NAME();                                            \
    static ::bge::test::TestRegistrar SUITE_NAME##_##TEST_NAME##_registrar(            \
        #SUITE_NAME,                                                                    \
        #TEST_NAME,                                                                     \
        &SUITE_NAME##_##TEST_NAME);                                                     \
    static void SUITE_NAME##_##TEST_NAME()

#define BGE_TEST_FAIL(MESSAGE)                                                         \
    do                                                                                 \
    {                                                                                  \
        ::bge::test::Fail("failure", __FILE__, __LINE__, (MESSAGE));                   \
    } while (false)

#define BGE_TEST_TRUE(EXPRESSION)                                                      \
    do                                                                                 \
    {                                                                                  \
        if (!(EXPRESSION))                                                             \
        {                                                                              \
            ::bge::test::Fail(#EXPRESSION, __FILE__, __LINE__);                        \
        }                                                                              \
    } while (false)

#define BGE_TEST_FALSE(EXPRESSION)                                                     \
    do                                                                                 \
    {                                                                                  \
        if ((EXPRESSION))                                                              \
        {                                                                              \
            ::bge::test::Fail("!(" #EXPRESSION ")", __FILE__, __LINE__);               \
        }                                                                              \
    } while (false)

#define BGE_TEST_EQ(A, B)                                                              \
    do                                                                                 \
    {                                                                                  \
        const auto bge_test_a = (A);                                                   \
        const auto bge_test_b = (B);                                                   \
        if (!(bge_test_a == bge_test_b))                                               \
        {                                                                              \
            ::bge::test::Fail(#A " == " #B, __FILE__, __LINE__);                       \
        }                                                                              \
    } while (false)

#define BGE_TEST_NE(A, B)                                                              \
    do                                                                                 \
    {                                                                                  \
        const auto bge_test_a = (A);                                                   \
        const auto bge_test_b = (B);                                                   \
        if (!(bge_test_a != bge_test_b))                                               \
        {                                                                              \
            ::bge::test::Fail(#A " != " #B, __FILE__, __LINE__);                       \
        }                                                                              \
    } while (false)

#define BGE_TEST_NEAR(A, B, EPSILON)                                                   \
    do                                                                                 \
    {                                                                                  \
        const auto bge_test_a = (A);                                                   \
        const auto bge_test_b = (B);                                                   \
        const auto bge_test_epsilon = (EPSILON);                                       \
        const auto bge_test_diff =                                                     \
            (bge_test_a > bge_test_b) ? (bge_test_a - bge_test_b) : (bge_test_b - bge_test_a); \
        if (!(bge_test_diff <= bge_test_epsilon))                                      \
        {                                                                              \
            ::bge::test::Fail(#A " ~= " #B, __FILE__, __LINE__);                       \
        }                                                                              \
    } while (false)
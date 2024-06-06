#ifndef _TESTING_H
#define _TESTING_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief Used at the start of a test function to define the test.
 *
 */
#define TEST_MAKE(test_name) int test_name##_func()

#define TEST_FAIL()                                                                                                              \
    do                                                                                                                           \
    {                                                                                                                            \
        printf("%sFail in Suite: %s\"%s\"%s: Test %s\"%s\"%s:%s\n\tfile: %s\n\tline: %d\n",                               \
               RED, YELLOW, __current_test_suite_name, RED, YELLOW, __current_test_name, RED ,RESET, __FILE__, __LINE__); \
        return 0;                                                                                                                \
    } while (0)

#define TEST_PASS() return 1

/**
 * @brief If the condition is not met, the test fails.
 *
 */
#define TEST_ASSERT(cond)                                 \
    do                                                    \
    {                                                     \
        if (!(cond))                                      \
        {                                                 \
            printf("%sAssertion failed:%s\n\tcond: %s\n", \
                   RED, RESET, #cond);                    \
            TEST_FAIL();                                  \
        }                                                 \
    } while (0)

/**
 * @brief If the condition is not met, the clean_func is called and the test fails.
 *
 */
#define TEST_ASSERT_CLEAN(cond, clean_func)                                                       \
    do                                                                                            \
    {                                                                                             \
        if (!(cond))                                                                              \
        {                                                                                         \
            printf("%sAssertion failed:%s\n\tcond: %s\n\t%sfunc: %s%s\n\tfile: %s\n\tline: %d\n", \
                   RED, RESET, #cond, YELLOW, __FUNCTION__, RESET, __FILE__, __LINE__);           \
            clean_func;                                                                           \
            TEST_FAIL();                                                                          \
        }                                                                                         \
    } while (0)

/**
 * @brief If the condition is not met, the clean_func is called and the test fails.
 * 
 */
#define TEST_ASSERT_CLEAN_MSG(cond, clean_func, fmt, ...)                                                       \
    do                                                                                            \
    {                                                                                             \
        if (!(cond))                                                                              \
        {                                                                                         \
            printf("%sAssertion failed:%s\n\tcond: %s\n\t%sfunc: %s%s\n\tfile: %s\n\tline: %d\n", \
                   RED, RESET, #cond, YELLOW, __FUNCTION__, RESET, __FILE__, __LINE__);           \
            printf(fmt "\n", __VA_ARGS__);                                                                  \
            clean_func;                                                                           \
            TEST_FAIL();                                                                          \
        }                                                                                         \
    } while (0)

/**
 * @brief Used inside of a test suite to link a test to the suite.
 *
 */
#define TEST_SUITE_LINK(__suite, test)                                                                             \
    do                                                                                                             \
    {                                                                                                              \
        if ((__suite##_suite).count >= (__suite##_suite).capacity)                                                 \
        {                                                                                                          \
            (__suite##_suite).capacity = (__suite##_suite).capacity == 0 ? 1 : (__suite##_suite).capacity * 2;     \
            (__suite##_suite).tests = realloc((__suite##_suite).tests, (__suite##_suite).capacity * sizeof(Test)); \
        }                                                                                                          \
        (__suite##_suite).tests[(__suite##_suite).count].test_func = test##_func;                                  \
        (__suite##_suite).tests[(__suite##_suite).count].test_name = #test;                                        \
        (__suite##_suite).count++;                                                                                 \
    } while (0)

/**
 * @brief Used to define a test suite.
 *
 * @note Don't forget to call TEST_SUITE_END at the end of the suite.
 */
#define TEST_SUITE_MAKE(__name)  \
    TestSuite __name##_suite = { \
        .tests = NULL,           \
        .count = 0,              \
        .capacity = 0,           \
        .name = #__name,         \
    };                           \
    void __name##_suite_func()

/**
 * @brief Used to end a test suite.
 *
 */
#define TEST_SUITE_END(name)            \
    TEST_SUITE_RUN_TESTS(name##_suite); \
    TEST_SUITE_CLEANUP(name##_suite)

/**
 * @brief Call this macro to run a test suite.
 *
 */
#define TEST_SUITE_RUN(name) \
    do                       \
    {                        \
        name##_suite_func(); \
    } while (0)

/* All that follows is used internally. */

/**
 * @brief Expects to receive the name of the actual suite struct.
 *
 */
#define TEST_SUITE_RUN_TESTS(suite)                                                                \
    do                                                                                             \
    {                                                                                              \
        printf("%s%s\nRunning Test Suite: %s\n%s", UNDERLINE, YELLOW, (suite).name, RESET);        \
        __current_test_suite_name = (char*)(suite).name;                                                  \
        int total_tests = 0;                                                                       \
        int passed_tests = 0;                                                                      \
        for (int i = 0; i < (suite).count; i++)                                                    \
        {                                                                                          \
            total_tests++;                                                                         \
            __current_test_name = (char*)(suite).tests[i].test_name;                                      \
            printf("%sRunning Test: %s%s%s...\n%s", BLUE, YELLOW, (suite).tests[i].test_name, BLUE, RESET);          \
            int result = (suite).tests[i].test_func();                                             \
            if (result)                                                                            \
            {                                                                                      \
                printf("%sTest \"%s\" passed.\n%s", GREEN, (suite).tests[i].test_name, RESET);          \
                passed_tests++;                                                                    \
            }                                                                                      \
            else                                                                                   \
            {                                                                                      \
                printf("%sTest %s\"%s\"%s failed.\n%s", RED, YELLOW, (suite).tests[i].test_name, RED , RESET);            \
            }                                                                                      \
        }                                                                                          \
        printf("\nTest suite \"%s\" summary:\n", (suite).name);                                    \
        printf("%sTotal tests: %d\n%s", BLUE, total_tests, RESET);                                 \
        printf("%sPassed tests: %d\n%s", GREEN, passed_tests, RESET);                              \
        printf("%sFailed tests: %d\n%s", RED, total_tests - passed_tests, RESET);                  \
        printf("%sPass rate: %.2f%%\n%s", YELLOW, (float)passed_tests / total_tests * 100, RESET); \
    } while (0)

#define TEST_SUITE_CLEANUP(suite) \
    do                            \
    {                             \
        free((suite).tests);      \
    } while (0)

/* Meant to be crossplatform */
bool __testing_does_terminal_support_ansi_color()
{
    static bool ran_before = false;
    static bool supports_colors = false;

    if (ran_before)
    {
        return supports_colors;
    }

    ran_before = true;

    char *term = getenv("TERM");
    if (term != NULL &&
        (strstr(term, "xterm") != NULL || strstr(term, "color") != NULL ||
         strstr(term, "ansi") != NULL || strstr(term, "cygwin") != NULL ||
         strstr(term, "linux") != NULL || strstr(term, "screen") != NULL ||
         strstr(term, "tmux") != NULL || strstr(term, "vt100") != NULL ||
         strstr(term, "rxvt") != NULL || strstr(term, "konsole") != NULL ||
         strstr(term, "gnome") != NULL || strstr(term, "eterm") != NULL ||
         strstr(term, "vscode") != NULL))
    {
        supports_colors = true;
        return true;
    }

    char *colorterm = getenv("COLORTERM");
    if (colorterm != NULL &&
        (strstr(colorterm, "truecolor") != NULL || strstr(colorterm, "24bit") != NULL))
    {
        supports_colors = true;
        return true;
    }

    supports_colors = false;
    return false;
}

#define RED (__testing_does_terminal_support_ansi_color() ? "\x1b[31m" : "")
#define GREEN (__testing_does_terminal_support_ansi_color() ? "\x1b[32m" : "")
#define YELLOW (__testing_does_terminal_support_ansi_color() ? "\x1b[33m" : "")
#define BLUE (__testing_does_terminal_support_ansi_color() ? "\x1b[34m" : "")
#define UNDERLINE (__testing_does_terminal_support_ansi_color() ? "\x1b[4m" : "")
#define RESET (__testing_does_terminal_support_ansi_color() ? "\x1b[0m" : "")

typedef struct
{
    int (*test_func)();
    const char *test_name;
} Test;

typedef struct
{
    Test *tests;
    int count;
    int capacity;
    const char *name;
} TestSuite;

char *__current_test_suite_name = NULL;
char *__current_test_name = NULL;

#endif /* _TESTING_H */

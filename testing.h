#ifndef _TESTING_FRAMEWORK_H
#define _TESTING_FRAMEWORK_H

/**
 * @file testing.h
 * @author your name (you@domain.com)
 * @brief Single header testing framework for C.
 * @version 0.1
 * @date 2024-06-06
 *
 * @copyright Copyright (c) 2024
 *
 * @warning Untested for multi-threaded signal exceptions.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <setjmp.h>
#include <time.h>

/**
 * @brief Used at the start of a test function to define the test.
 *
 * @return call TEST_PASS() or TEST_FAIL() or return TEST_PASS_VALUE or !TEST_PASS_VALUE
 */
#define TEST_MAKE(test_name) int test_name##_func()

#define TEST_PASS_VALUE 1

#define TEST_PASS() return TEST_PASS_VALUE

#define TEST_FAIL()                                                                                                                                                 \
    do                                                                                                                                                              \
    {                                                                                                                                                               \
        printf("%sFail in Suite: %s\"%s\"%s: Test %s\"%s\"%s:%s\n\tfile: %s\n\tline: %d\n",                                                                         \
               __ANSI_RED, __ANSI_YELLOW, __current_test_suite_name, __ANSI_RED, __ANSI_YELLOW, __current_test_name, __ANSI_RED, __ANSI_RESET, __FILE__, __LINE__); \
        return !(TEST_PASS_VALUE);                                                                                                                                  \
    } while (0)

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
                   __ANSI_RED, __ANSI_RESET, #cond);      \
            TEST_FAIL();                                  \
        }                                                 \
    } while (0)

/**
 * @brief If the condition is not met, the test fails.
 *
 */
#define TEST_ASSERT_FMT(cond, fmt, ...)                   \
    do                                                    \
    {                                                     \
        if (!(cond))                                      \
        {                                                 \
            printf("%sAssertion failed:%s\n\tcond: %s\n", \
                   __ANSI_RED, __ANSI_RESET, #cond);      \
            printf(fmt "\n", __VA_ARGS__);                \
            TEST_FAIL();                                  \
        }                                                 \
    } while (0)

/**
 * @brief If the condition is not met, the test fails.
 *
 */
#define TEST_ASSERT_MSG(cond, MSG)                        \
    do                                                    \
    {                                                     \
        if (!(cond))                                      \
        {                                                 \
            printf("%sAssertion failed:%s\n\tcond: %s\n", \
                   __ANSI_RED, __ANSI_RESET, #cond);      \
            printf(MSG "\n");                             \
            TEST_FAIL();                                  \
        }                                                 \
    } while (0)

/**
 * @brief Allows to run multiple statements in a block.
 *
 */
#define TEST_CLEAN_FUNC(...) \
    {                        \
        __VA_ARGS__          \
    }

/**
 * @brief If the condition is not met, the clean_func is called and the test fails.
 *
 */
#define TEST_ASSERT_CLEAN(cond, clean_func)                                                                         \
    do                                                                                                              \
    {                                                                                                               \
        if (!(cond))                                                                                                \
        {                                                                                                           \
            printf("%sAssertion failed:%s\n\tcond: %s\n\t%sfunc: %s%s\n\tfile: %s\n\tline: %d\n",                   \
                   __ANSI_RED, __ANSI_RESET, #cond, __ANSI_YELLOW, __FUNCTION__, __ANSI_RESET, __FILE__, __LINE__); \
            clean_func;                                                                                             \
            TEST_FAIL();                                                                                            \
        }                                                                                                           \
    } while (0)

/**
 * @brief If the condition is not met, the clean_func is called and the test fails.
 *
 */
#define TEST_ASSERT_CLEAN_FMT(cond, clean_func, fmt, ...)                                                           \
    do                                                                                                              \
    {                                                                                                               \
        if (!(cond))                                                                                                \
        {                                                                                                           \
            printf("%sAssertion failed:%s\n\tcond: %s\n\t%sfunc: %s%s\n\tfile: %s\n\tline: %d\n",                   \
                   __ANSI_RED, __ANSI_RESET, #cond, __ANSI_YELLOW, __FUNCTION__, __ANSI_RESET, __FILE__, __LINE__); \
            printf(fmt "\n", __VA_ARGS__);                                                                          \
            clean_func;                                                                                             \
            TEST_FAIL();                                                                                            \
        }                                                                                                           \
    } while (0)

/**
 * @brief If the condition is not met, the clean_func is called and the test fails.
 *
 */
#define TEST_ASSERT_CLEAN_MSG(cond, clean_func, msg)                                                                \
    do                                                                                                              \
    {                                                                                                               \
        if (!(cond))                                                                                                \
        {                                                                                                           \
            printf("%sAssertion failed:%s\n\tcond: %s\n\t%sfunc: %s%s\n\tfile: %s\n\tline: %d\n",                   \
                   __ANSI_RED, __ANSI_RESET, #cond, __ANSI_YELLOW, __FUNCTION__, __ANSI_RESET, __FILE__, __LINE__); \
            printf(msg "\n");                                                                                       \
            clean_func;                                                                                             \
            TEST_FAIL();                                                                                            \
        }                                                                                                           \
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
#define TEST_SUITE_END(name)                  \
    do                                        \
    {                                         \
        __TEST_SUITE_RUN_TESTS(name##_suite); \
        free((name##_suite).tests);           \
    } while (0);

/**
 * @brief Call this macro to run a test suite.
 *
 */
#define TEST_SUITE_RUN(name)                  \
    do                                        \
    {                                         \
        __TESTING_REGISTER_SIGNAL_HANDLERS(); \
        name##_suite_func();                  \
        __TESTING_RESET_SIGNAL_HANDLERS();    \
    } while (0)

/* All that follows is used internally. */

bool __testing_try_use_colors = true;

/* Meant to be crossplatform */
bool __testing_does_terminal_support_ansi_color()
{
    if (!__testing_try_use_colors)
    {
        return false;
    }
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

#define __ANSI_RED (__testing_does_terminal_support_ansi_color() ? "\x1b[31m" : "")
#define __ANSI_GREEN (__testing_does_terminal_support_ansi_color() ? "\x1b[32m" : "")
#define __ANSI_YELLOW (__testing_does_terminal_support_ansi_color() ? "\x1b[33m" : "")
#define __ANSI_BLUE (__testing_does_terminal_support_ansi_color() ? "\x1b[34m" : "")
#define __ANSI_UNDERLINE (__testing_does_terminal_support_ansi_color() ? "\x1b[4m" : "")
#define __ANSI_RESET (__testing_does_terminal_support_ansi_color() ? "\x1b[0m" : "")

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

jmp_buf __testing_env;
volatile sig_atomic_t __signal_caught = 0;

/**
 * @brief Set to true to ask the user if they want to continue testing after a signal is caught.
 *
 * @note When enabled, if you are running the test program from a debugger it may cause the program to hang.
 */
bool __testing_handle_signal_ask_user = false;

bool __testing_ask_user()
{
    fflush(stdin);
    char c = 0;
    while (c != 'y' && c != 'n' && c != 'Y' && c != 'N' && c != EOF)
    {
        printf("[Y/y|N/n]: ");
        c = getchar();
        fflush(stdin);
    }
    if (c == EOF || c == 'n' || c == 'N')
    {
        return false;
    }
    return true;
}

void __TESTING_REGISTER_SIGNAL_HANDLERS(void);
void __TESTING_RESET_SIGNAL_HANDLERS(void);

/* Signal handling functions */
void __testing_handle_signal(int sig)
{
    const char *signal_name;
    switch (sig)
    {
    case SIGSEGV:
        signal_name = "Segmentation fault";
        break;
    case SIGFPE:
        signal_name = "Floating point exception";
        break;
    case SIGILL:
        signal_name = "Illegal instruction";
        break;
    case SIGABRT:
        signal_name = "Aborted";
        break;
    default:
        signal_name = "Unknown signal";
    }

    printf("\n%sCaught signal: %s (%d)%s\n", __ANSI_RED, signal_name, sig, __ANSI_RESET);
    if (__current_test_name != NULL)
    {
        printf("%sError occurred during test: %s\n%s", __ANSI_RED, __current_test_name, __ANSI_RESET);
    }
    if (__current_test_suite_name != NULL)
    {
        printf("%sIn test suite: %s\n%s", __ANSI_RED, __current_test_suite_name, __ANSI_RESET);
    }

    /*  Reset signal handlers to default */
    __TESTING_RESET_SIGNAL_HANDLERS();

    if (__testing_handle_signal_ask_user)
    {
        printf("Do you want to continue testing? ");
        if (__testing_ask_user())
        {
            raise(sig);
            return;
        }
    }
    __signal_caught = sig;
    /* Jump back to tests */
    longjmp(__testing_env, 1);
}

/* Register signal handlers macro */
void __TESTING_REGISTER_SIGNAL_HANDLERS(void)
{
    signal(SIGSEGV, __testing_handle_signal);
    signal(SIGFPE, __testing_handle_signal);
    signal(SIGILL, __testing_handle_signal);
    signal(SIGABRT, __testing_handle_signal);
}

/* Reset signal handlers macro */
void __TESTING_RESET_SIGNAL_HANDLERS(void)
{
    signal(SIGSEGV, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
}

/**
 * @brief Expects to receive the name of the actual suite struct.
 *
 */
void __TEST_SUITE_RUN_TESTS(TestSuite suite)
{
    printf("%s%s\nRunning Test Suite: %s\n%s", __ANSI_UNDERLINE, __ANSI_YELLOW, (suite).name, __ANSI_RESET);
    __current_test_suite_name = (char *)(suite).name;
    int total_tests = 0;
    int passed_tests = 0;
    for (int i = 0; i < (suite).count; i++)
    {
        total_tests++;
        __current_test_name = (char *)(suite).tests[i].test_name;
        clock_t start = clock();
        printf("%sRunning Test: %s%s%s...\n%s", __ANSI_BLUE, __ANSI_YELLOW, __current_test_name, __ANSI_BLUE, __ANSI_RESET);
        if (setjmp(__testing_env) == 0)
        {
            __signal_caught = 0;
            int result = (suite).tests[i].test_func();
            if (result == TEST_PASS_VALUE && __signal_caught == 0)
            {
                printf("%sTest %s\"%s\"%s passed.%s\n", __ANSI_GREEN, __ANSI_YELLOW, __current_test_name, __ANSI_GREEN, __ANSI_RESET);
                passed_tests++;
            }
            else
            {
                clock_t end = clock();
                double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
                printf("%sTest \"%s\" failed.\n%s", __ANSI_RED, __current_test_name, __ANSI_RESET);
            }
        }
        else
        {
            printf("%sTest %s\"%s\"%s failed due to signal %d.%s\n", __ANSI_RED, __ANSI_YELLOW, __current_test_name, __ANSI_RED, __signal_caught, __ANSI_RESET);
        }
        clock_t end = clock();
        double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
        printf("\t%sElapsed time: %.2fs%s\n", __ANSI_YELLOW, elapsed_time, __ANSI_RESET);
    }
    printf("\nTest suite \"%s\" summary:\n", (suite).name);
    printf("%sTotal tests: %d\n%s", __ANSI_BLUE, total_tests, __ANSI_RESET);
    printf("%sPassed tests: %d\n%s", __ANSI_GREEN, passed_tests, __ANSI_RESET);
    printf("%sFailed tests: %d\n%s", __ANSI_RED, total_tests - passed_tests, __ANSI_RESET);
    printf("%sPass rate: %.2f%%\n%s", __ANSI_YELLOW, (float)passed_tests / total_tests * 100, __ANSI_RESET);
}

#endif /* _TESTING_FRAMEWORK_H */

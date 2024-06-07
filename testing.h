#ifndef _TESTING_FRAMEWORK_H
#define _TESTING_FRAMEWORK_H

/**
 * @file testing.h
 * @author Adam Naghavi
 * @brief Single header testing framework for C.
 * @version 0.2
 * @date 2024-06-07
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
#include <string.h>

char *TEST_LOG_FILE_NAME = "testing.log";

/**
 * @brief Takes the same input as printf. The user does not need newlines at the end of their format strings or messages.
 *
 * @note Also writes output to a log file.
 */
#define TEST_LOG(...) __CTF_LOG_IMPL(TEST_LOG_FILE_NAME, __VA_ARGS__)

#define TEST_LOG_TIME()                                          \
    do                                                           \
    {                                                            \
        time_t t;                                                \
        time(&t);                                                \
        char buff[70];                                           \
        if (strftime(buff, sizeof buff, "%A %c", localtime(&t))) \
            TEST_LOG(buff);                                      \
        else                                                     \
            TEST_LOG("Could not get date and time info.");       \
    } while (0)

/**
 * @brief Used at the start of a test function to define the test.
 *
 * @return call TEST_PASS() or TEST_FAIL() or return TEST_PASS_VALUE or !TEST_PASS_VALUE
 */
#define TEST_MAKE(test_name) int test_name##_func()

#define TEST_PASS_VALUE 1

#define TEST_FAIL_VALUE (!(TEST_PASS_VALUE))

#define TEST_PASS() return TEST_PASS_VALUE

#define TEST_FAIL()             \
    do                          \
    {                           \
        __CTF_FAIL_TEXT();      \
        return TEST_FAIL_VALUE; \
    } while (0)

/**
 * @brief If the condition is not met, the test fails.
 *
 */
#define TEST_ASSERT(cond)            \
    do                               \
    {                                \
        if (!(cond))                 \
        {                            \
            __CTF_ASSERT_TEXT(cond); \
            TEST_FAIL();             \
        }                            \
    } while (0)

/**
 * @brief If the condition is not met, the test fails.
 *
 */
#define TEST_ASSERT_LOG(cond, ...)   \
    do                               \
    {                                \
        if (!(cond))                 \
        {                            \
            __CTF_ASSERT_TEXT(cond); \
            TEST_LOG(__VA_ARGS__);   \
            TEST_FAIL();             \
        }                            \
    } while (0)

#define TEST_BLOCK(...) \
    {                   \
        __VA_ARGS__;    \
    }

/**
 * @brief Allows to run multiple statements in a block.
 *
 */
#define TEST_CLEAN_FUNC(...) TEST_BLOCK(__VA_ARGS__)

#define TEST_CODE(...) TEST_BLOCK(__VA_ARGS__)

/**
 * @brief If the condition is not met, the clean_func is called and the test fails.
 *
 * @note doesnt require second arg to be TEST_BLOCK or TEST_CLEAN_FUNC but it should be.
 *
 */
#define TEST_ASSERT_CLEAN(cond, ...) \
    do                               \
    {                                \
        if (!(cond))                 \
        {                            \
            __CTF_ASSERT_TEXT(cond); \
            __VA_ARGS__;             \
            TEST_FAIL();             \
        }                            \
    } while (0)

/**
 * @brief If the condition is not met, the clean_func is called and the test fails.
 *
 * @note Requires second arg to be TEST_BLOCK or TEST_CLEAN_FUNC.
 *
 */
#define TEST_ASSERT_CLEAN_LOG(cond, clean_func, ...) \
    do                                               \
    {                                                \
        if (!(cond))                                 \
        {                                            \
            __CTF_ASSERT_TEXT(cond);                 \
            TEST_LOG(__VA_ARGS__);                   \
            clean_func;                              \
            TEST_FAIL();                             \
        }                                            \
    } while (0)

/**
 * @brief Creates a test suite with the given name. This is the preferred, but not required, way to create a test suite.
 *
 * @param name Name of the test suite.
 * @param ... Code to run in the test suite, optionally enclose in semicolons. This is where you link your tests to the suite.
 */
#define TEST_SUITE(name, ...) __CTF_SUITE_IMPL(name, __VA_ARGS__)

/**
 * @brief Used inside of a test suite to link a test to the suite.
 *
 */
#define TEST_SUITE_LINK(__suite, test) __CTF_SUITE_LINK_FUNC(&__suite##_suite, test##_func, #test)

/**
 * @brief Call this macro to run a test suite.
 *
 */
#define TEST_SUITE_RUN(name)              \
    do                                    \
    {                                     \
        __CTF_REGISTER_SIGNAL_HANDLERS(); \
        name##_suite_func();              \
        __testing_suites_ran++;           \
        __CTF_RESET_SIGNAL_HANDLERS();    \
    } while (0)

/**
 * @brief Use in a main function that has argv and argc to alter the some aspects of the testing framework.
 *
 */
#define TEST_PROCESS_INIT() __CTF_PROCESS_INIT_IMPL(argc, argv)

#define TEST_PROCESS_EXIT() __CTF_PROCESS_EXIT_IMPL()

/*
    If you use TEST_SUITE you dont need to use the following macros.
*/

/**
 * @brief Used to define a test suite.
 *
 * @note Don't forget to call TEST_SUITE_END at the end of the suite.
 */
#define TEST_SUITE_MAKE(__name)    \
    __TestSuite __name##_suite = { \
        .tests = NULL,             \
        .count = 0,                \
        .capacity = 0,             \
        .name = #__name,           \
    };                             \
    void __name##_suite_func()

/**
 * @brief Used to end a test suite.
 *
 */
#define TEST_SUITE_END(name)                                                                                          \
    do                                                                                                                \
    {                                                                                                                 \
        clock_t test_start_time = clock();                                                                            \
        __CTF_SUITE_RUN_TESTS(name##_suite);                                                                          \
        clock_t test_runtime = clock() - test_start_time;                                                             \
        float test_runtime_sec = (double)(test_runtime) / CLOCKS_PER_SEC;                                             \
        TEST_LOG("\nTest suite %s\"%s\"%s tests ran for %fs.", __ANSI_YELLOW, #name, __ANSI_RESET, test_runtime_sec); \
        free((name##_suite).tests);                                                                                   \
        __testing_current_test_name = NULL;                                                                           \
        __testing_current_test_suite_name = NULL;                                                                     \
    } while (0);

/*
    ====================================================================================================
                                All that follows is used internally.
    ====================================================================================================
*/

typedef struct
{
    int (*test_func)();
    const char *test_name;
} __Test;

typedef struct
{
    __Test *tests;
    int count;
    int capacity;
    const char *name;
} __TestSuite;

char *__testing_current_test_suite_name = NULL;
char *__testing_current_test_name = NULL;

unsigned int __testing_suites_ran = 0;

jmp_buf __testing_env;
volatile sig_atomic_t __signal_caught = 0;

FILE *__testing_log_file = NULL;

clock_t __testing_process_start_time = -1;

/**
 * @brief Set to true to ask the user if they want to continue testing after a signal is caught.
 *
 * @note When enabled, if you are running the test program from a debugger it may cause the program to hang.
 */
bool __TESTING_HANDLE_SIGNAL_ask_user = false;

bool __testing_try_use_colors = true;

/* Meant to be crossplatform */
bool __TESTING_ANSI_SUPPORT()
{
    if (!__testing_try_use_colors)
    {
        return false;
    }
    static bool ran_before = false, supports_colors = false;
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

#define __ANSI_RED (__TESTING_ANSI_SUPPORT() ? "\x1b[31m" : "")
#define __ANSI_GREEN (__TESTING_ANSI_SUPPORT() ? "\x1b[32m" : "")
#define __ANSI_YELLOW (__TESTING_ANSI_SUPPORT() ? "\x1b[33m" : "")
#define __ANSI_BLUE (__TESTING_ANSI_SUPPORT() ? "\x1b[34m" : "")
#define __ANSI_UNDERLINE (__TESTING_ANSI_SUPPORT() ? "\x1b[4m" : "")
#define __ANSI_RESET (__TESTING_ANSI_SUPPORT() ? "\x1b[0m" : "")

#define __CTF_FAIL_TEXT()                                                                      \
    TEST_LOG("\n\t%sFail in Suite:%s\"%s\"%s, Test:%s\"%s\"%s:%s\n\t\tfile: %s\n\t\tline: %d", \
             __ANSI_RED, __ANSI_YELLOW, __testing_current_test_suite_name, __ANSI_RED, __ANSI_YELLOW, __testing_current_test_name, __ANSI_RED, __ANSI_RESET, __FILE__, __LINE__);

#define __CTF_ASSERT_TEXT(cond)                         \
    TEST_LOG("\n\t%sAssertion failed:%s\n\t\tcond: %s", \
             __ANSI_RED, __ANSI_RESET, #cond)

bool __TESTING_ASK_USER()
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

/**
 * @brief Takes a filename rather than FILE* because we expect a critial error to occur and always want to ensure that the log is written.
 *
 * @note If TEST_PROCESS_INIT was called then we will use already opened __testing_log_file. This enables faster logging.
 *
 */
#define __CTF_LOG_IMPL(filename, ...)                                                                                                                                                          \
    do                                                                                                                                                                                         \
    {                                                                                                                                                                                          \
        bool suite = __testing_current_test_suite_name != NULL;                                                                                                                                \
        bool test = __testing_current_test_name != NULL;                                                                                                                                       \
        printf("%s[LOG%s%s%s%s]%s ", __ANSI_YELLOW, suite ? "/" : "", suite ? __testing_current_test_suite_name : "", test ? "/" : "", test ? __testing_current_test_name : "", __ANSI_RESET); \
        printf(__VA_ARGS__);                                                                                                                                                                   \
        putchar('\n');                                                                                                                                                                         \
        FILE *file = __testing_log_file ? __testing_log_file : fopen(filename, "a");                                                                                                           \
        bool old_use_colors = __testing_try_use_colors;                                                                                                                                        \
        __testing_try_use_colors = false;                                                                                                                                                      \
        if (file)                                                                                                                                                                              \
        {                                                                                                                                                                                      \
            fprintf(file, "[LOG%s%s%s%s] ", suite ? "/" : "", suite ? __testing_current_test_suite_name : "", test ? "/" : "", test ? __testing_current_test_name : "");                       \
            fprintf(file, __VA_ARGS__);                                                                                                                                                        \
            fputc('\n', file);                                                                                                                                                                 \
            if (!__testing_log_file)                                                                                                                                                           \
                fclose(file);                                                                                                                                                                  \
        }                                                                                                                                                                                      \
        __testing_try_use_colors = old_use_colors;                                                                                                                                             \
    } while (0)

void __TESTING_HANDLE_SIGNAL(int sig);

/* Register signal handlers macro */
void __CTF_REGISTER_SIGNAL_HANDLERS(void)
{
    signal(SIGSEGV, __TESTING_HANDLE_SIGNAL);
    signal(SIGFPE, __TESTING_HANDLE_SIGNAL);
    signal(SIGILL, __TESTING_HANDLE_SIGNAL);
    signal(SIGABRT, __TESTING_HANDLE_SIGNAL);
}

/* Reset signal handlers macro */
void __CTF_RESET_SIGNAL_HANDLERS(void)
{
    signal(SIGSEGV, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
}

#define __CTF_SIGBUFF_SIZE 60
#define __CTF_ERRBUFF_SIZE (__CTF_SIGBUFF_SIZE + 150)
#define __CTF_BUFF_SIZE (__CTF_ERRBUFF_SIZE + 150)

/* Signal handling functions */
void __TESTING_HANDLE_SIGNAL(int sig)
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
    char sigbuff[__CTF_SIGBUFF_SIZE], errbuff[__CTF_ERRBUFF_SIZE], buff[__CTF_BUFF_SIZE];

    snprintf(sigbuff, __CTF_SIGBUFF_SIZE, "\n\tCaught signal: %s (%d)\n", signal_name, sig);
    if (__testing_current_test_name != NULL)
    {
        snprintf(errbuff, __CTF_ERRBUFF_SIZE, "%s\tError occurred during test: %s\n", sigbuff, __testing_current_test_name);
    }
    if (__testing_current_test_suite_name != NULL)
    {
        snprintf(buff, __CTF_BUFF_SIZE, "%s\tIn test suite: %s", errbuff, __testing_current_test_suite_name);
    }
    TEST_LOG("%s%s%s", __ANSI_RED, buff, __ANSI_RESET);
    /*  Reset signal handlers to default */
    __CTF_RESET_SIGNAL_HANDLERS();

    if (__TESTING_HANDLE_SIGNAL_ask_user)
    {
        printf("Do you want to continue testing? ");
        if (__TESTING_ASK_USER())
        {
            raise(sig);
            return;
        }
    }
    __signal_caught = sig;
    /* Jump back to tests */
    longjmp(__testing_env, 1);
}

#define __CTF_SUITE_RUN_TESTS_IMPL(suite)                                                                                                                                    \
    do                                                                                                                                                                       \
    {                                                                                                                                                                        \
        int i;                                                                                                                                                               \
        printf("%s%sRunning Test Suite: %s\n%s", __ANSI_UNDERLINE, __ANSI_YELLOW, (suite).name, __ANSI_RESET);                                                               \
        __testing_current_test_suite_name = (char *)(suite).name;                                                                                                            \
        int total_tests = 0, passed_tests = 0;                                                                                                                               \
        for (i = 0; i < (suite).count; i++)                                                                                                                                  \
        {                                                                                                                                                                    \
            total_tests++;                                                                                                                                                   \
            __testing_current_test_name = (char *)(suite).tests[i].test_name;                                                                                                \
            clock_t start = clock();                                                                                                                                         \
            printf("%sRunning Test: %s%s%s...\n%s", __ANSI_BLUE, __ANSI_YELLOW, __testing_current_test_name, __ANSI_BLUE, __ANSI_RESET);                                     \
            if (setjmp(__testing_env) == 0)                                                                                                                                  \
            {                                                                                                                                                                \
                __signal_caught = 0;                                                                                                                                         \
                int result = (suite).tests[i].test_func();                                                                                                                   \
                if (result == TEST_PASS_VALUE && __signal_caught == 0)                                                                                                       \
                {                                                                                                                                                            \
                    printf("%sTest %s\"%s\"%s passed.%s\n", __ANSI_GREEN, __ANSI_YELLOW, __testing_current_test_name, __ANSI_GREEN, __ANSI_RESET);                           \
                    passed_tests++;                                                                                                                                          \
                }                                                                                                                                                            \
                else                                                                                                                                                         \
                {                                                                                                                                                            \
                    printf("%sTest \"%s\" failed.\n%s", __ANSI_RED, __testing_current_test_name, __ANSI_RESET);                                                              \
                }                                                                                                                                                            \
            }                                                                                                                                                                \
            else                                                                                                                                                             \
            {                                                                                                                                                                \
                printf("%sTest %s\"%s\"%s failed due to signal %d.%s\n", __ANSI_RED, __ANSI_YELLOW, __testing_current_test_name, __ANSI_RED, __signal_caught, __ANSI_RESET); \
            }                                                                                                                                                                \
            clock_t end = clock();                                                                                                                                           \
            double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;                                                                                                    \
            printf("\t%sElapsed time: %fs%s\n", __ANSI_YELLOW, elapsed_time, __ANSI_RESET);                                                                                  \
        }                                                                                                                                                                    \
        __testing_current_test_name = NULL;                                                                                                                                  \
        TEST_LOG("\nTest suite %s\"%s\"%s summary:\n%sTotal tests: %d\n%sPassed tests: %d\n%sFailed tests: %d\n%sPass rate: %.2f%%%s",                                       \
                 __ANSI_YELLOW, __testing_current_test_suite_name, __ANSI_RESET,                                                                                             \
                 __ANSI_BLUE, total_tests,                                                                                                                                   \
                 __ANSI_GREEN, passed_tests,                                                                                                                                 \
                 __ANSI_RED, total_tests - passed_tests,                                                                                                                     \
                 __ANSI_YELLOW, (float)passed_tests / total_tests * 100, __ANSI_RESET);                                                                                      \
    } while (0)

/**
 * @brief Defines TEST_SUITE_RUN_TESTS as a function for preventing variable name reuse.
 *
 * @note only declare if your test suite is polluted with local/global variable names.
 *
 */
#ifndef TESTING_DECLARE_TEST_SUITE_RUN_TESTS_AS_FUNCTION
void __CTF_SUITE_RUN_TESTS(__TestSuite suite)
{
    __CTF_SUITE_RUN_TESTS_IMPL(suite);
}
#else
#define __CTF_SUITE_RUN_TESTS(suite) __CTF_SUITE_RUN_TESTS_IMPL(suite)
#endif

void __CTF_PROCESS_INIT_IMPL(int argc, char **argv)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-nc") == 0 || strcmp(argv[i], "-no-color") == 0)
        {
            __testing_try_use_colors = false;
        }
        else if (strcmp(argv[i], "-as") == 0 || strcmp(argv[i], "--ask-signal") == 0)
        {
            __TESTING_HANDLE_SIGNAL_ask_user = true;
        }
        else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--log") == 0)
        {
            if (i + 1 < argc)
            {
                TEST_LOG_FILE_NAME = argv[i + 1];
                i++;
            }
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printf("Usage: %s [-nc|-no-color] [-h|-help]\n", argv[0]);
            printf("Options:\n");
            printf("\t-nc, -no-color\t\tDisable colored output.\n");
            printf("\t-as, --ask-signal\tAsk the user if they want to continue testing after a signal is caught.\n");
            printf("\t-l, --log\t\tSpecify a log file name.\n");
            printf("\t-h, -help\t\tShow this help message.\n");
            exit(0);
        }
    }
    __testing_current_test_name = NULL;
    __testing_current_test_suite_name = NULL;
    __testing_process_start_time = clock();
    __testing_log_file = fopen(TEST_LOG_FILE_NAME, "w");
    TEST_LOG("C Testing framework (CTF) initialized.");
    TEST_LOG_TIME();
}

void __CTF_PROCESS_EXIT_IMPL(void)
{
    TEST_LOG("Testing complete. %d suites ran.", __testing_suites_ran);
    clock_t end_time = clock();
    float runtime = (double)(end_time - __testing_process_start_time) / CLOCKS_PER_SEC;
    TEST_LOG("Testing process completed in %fs.", __testing_process_start_time != -1 ? runtime : -1.0f);
    if (__testing_log_file)
    {
        fclose(__testing_log_file);
        __testing_log_file = NULL;
    }
    exit(0);
}

void __CTF_SUITE_LINK_FUNC(__TestSuite *suite, int (*test_func)(), const char *test_name)
{
    if (suite->count >= suite->capacity)
    {
        suite->capacity = suite->capacity == 0 ? 1 : suite->capacity * 2;
        suite->tests = realloc(suite->tests, suite->capacity * sizeof(__Test));
    }
    suite->tests[suite->count].test_func = test_func;
    suite->tests[suite->count].test_name = test_name;
    suite->count++;
}

#define __CTF_SUITE_IMPL(name, ...)                \
    TEST_SUITE_MAKE(name)                          \
    {                                              \
        __testing_current_test_suite_name = #name; \
        int i = 22 + strlen(#name), j = i;         \
        while (i--)                                \
        {                                          \
            putchar('+');                          \
        }                                          \
        putchar('\n');                             \
        TEST_LOG_TIME();                           \
        TEST_BLOCK(__VA_ARGS__);                   \
        TEST_SUITE_END(name);                      \
        i = j;                                     \
        while (i--)                                \
        {                                          \
            putchar('-');                          \
        }                                          \
        putchar('\n');                             \
    }

#endif /* _TESTING_FRAMEWORK_H */

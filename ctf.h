#ifndef _CTF_FRAMEWORK_H
#define _CTF_FRAMEWORK_H

/**
 * @file testing.h
 * @author Adam Naghavi
 * @brief Single header testing framework for C.
 * @version 0.3
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
#include <stdarg.h>

/* Use anywhere to log to CTF_LOG_FILE_NAME */
#define CTF_LOG(...) __CTF_LOG_IMPL(CTF_LOG_FILE_NAME, __VA_ARGS__)
#define CTF_LOG_TIME() __CTF_LOG_TIME_IMPL()
/* Macro to a char* */
#define CTF_LOG_FILE_NAME __CTF_LOG_FILE_NAME
/* Use to declare a test */
#define CTF_TEST(test_name) __CTF_MAKE(test_name)
/* Use in CTF_TEST */
#define CTF_PASS() __CTF_PASS()
#define CTF_FAIL() __CTF_FAIL()
#define CTF_ASSERT(cond) __CTF_ASSERT(cond)
#define CTF_ASSERT_LOG(cond, ...) __CTF_ASSERT_LOG(cond, __VA_ARGS__)
#define CTF_ASSERT_CLEAN(cond, ...) __CTF_ASSERT_CLEAN(cond, __VA_ARGS__)
#define CTF_ASSERT_CLEAN_LOG(cond, clean_func, ...) __CTF_ASSERT_CLEAN_LOG(cond, clean_func, __VA_ARGS__)
/* Wrap clean_func input of CTF_ASSERT_CLEAN_LOG*/
#define CTF_CLEAN_FUNC(...) __CTF_CLEAN_FUNC(__VA_ARGS__)
#define CTF_CODE(...) __CTF_CODE(__VA_ARGS__)
#define CTF_BLOCK(...) __CTF_BLOCK(__VA_ARGS__)
/* Use to create a suite */
#define CTF_SUITE(name, ...) __CTF_SUITE(name, __VA_ARGS__)
#define CTF_SUITE_MAKE(name) __CTF_SUITE_MAKE(name)
/* Use in CTF_SUITE or in CTF_SUITE_MAKE */
#define CTF_SUITE_LINK(__suite, test) __CTF_SUITE_LINK(__suite, test)
#define CTF_SUITE_END(name) __CTF_SUITE_END(name)
/* Use in main function */
#define CTF_SUITE_RUN(name) __CTF_SUITE_RUN(name)
#define CTF_PROCESS_INIT() __CTF_PROCESS_INIT()
#define CTF_PROCESS_EXIT() __CTF_PROCESS_EXIT()
/* Misc */
#define CTF_LOG_TIME() __CTF_LOG_TIME_IMPL()
#define CTF_PASS_VALUE __CTF_PASS_VALUE
#define CTF_FAIL_VALUE __CTF_FAIL_VALUE

/*
    ====================================================================================================
                    Optionally define CTF_TEST_NAMES to use the following macros also.
    ====================================================================================================
*/

#ifdef CTF_TEST_NAMES
/* Use anywhere to log to CTF_LOG_FILE_NAME */
#define TEST_LOG(...) __CTF_LOG_IMPL(CTF_LOG_FILE_NAME, __VA_ARGS__)
#define TEST_LOG_TIME() __CTF_LOG_TIME_IMPL()
/* Macro to a char* */
#define TEST_LOG_FILE_NAME __CTF_LOG_FILE_NAME
/* Use to declare a test */
#define TEST_MAKE(test_name) __CTF_MAKE(test_name)
/* Use in CTF_TEST */
#define TEST_PASS() __CTF_PASS()
#define TEST_FAIL() __CTF_FAIL()
#define TEST_ASSERT(cond) __CTF_ASSERT(cond)
#define TEST_ASSERT_LOG(cond, ...) __CTF_ASSERT_LOG(cond, __VA_ARGS__)
#define TEST_ASSERT_CLEAN(cond, ...) __CTF_ASSERT_CLEAN(cond, __VA_ARGS__)
#define TEST_ASSERT_CLEAN_LOG(cond, clean_func, ...) __CTF_ASSERT_CLEAN_LOG(cond, clean_func, __VA_ARGS__)
/* Wrap clean_func input of CTF_ASSERT_CLEAN_LOG*/
#define TEST_CLEAN_FUNC(...) __CTF_CLEAN_FUNC(__VA_ARGS__)
#define TEST_CODE(...) __CTF_CODE(__VA_ARGS__)
#define TEST_BLOCK(...) __CTF_BLOCK(__VA_ARGS__)
/* Use to create a suite */
#define TEST_SUITE(name, ...) __CTF_SUITE(name, __VA_ARGS__)
#define TEST_SUITE_MAKE(name) __CTF_SUITE_MAKE(name)
/* Use in CTF_SUITE or in CTF_SUITE_MAKE */
#define TEST_SUITE_LINK(__suite, test) __CTF_SUITE_LINK(__suite, test)
#define TEST_SUITE_END(name) __CTF_SUITE_END(name)
/* Use in main function */
#define TEST_SUITE_RUN(name) __CTF_SUITE_RUN(name)
#define TEST_PROCESS_INIT() __CTF_PROCESS_INIT()
#define TEST_PROCESS_EXIT() __CTF_PROCESS_EXIT()
/* Misc */
#define TEST_LOG_TIME() __CTF_LOG_TIME_IMPL()
#define TEST_PASS_VALUE __CTF_PASS_VALUE
#define TEST_FAIL_VALUE __CTF_FAIL_VALUE
#endif

char *__CTF_LOG_FILE_NAME = "testing.log";

/**
 * @brief Takes the same input as printf. The user does not need newlines at the end of their format strings or messages.
 *
 * @note Also writes output to a log file.
 */
#define __CTF_LOG(...) __CTF_LOG_IMPL(__CTF_LOG_FILE_NAME, __VA_ARGS__)

#define __CTF_LOG_TIME() __CTF_LOG_TIME_IMPL()

/**
 * @brief Used at the start of a test function to define the test.
 *
 * @return call __CTF_PASS() or __CTF_FAIL() or return __CTF_PASS_VALUE or !TEST_PASS_VALUE
 */
#define __CTF_MAKE(test_name) int test_name##_func()

#define __CTF_PASS_VALUE 1

#define __CTF_FAIL_VALUE (!(__CTF_PASS_VALUE))

#define __CTF_PASS() return __CTF_PASS_VALUE

#define __CTF_FAIL()             \
    do                           \
    {                            \
        __CTF_FAIL_TEXT();       \
        return __CTF_FAIL_VALUE; \
    } while (0)

/**
 * @brief If the condition is not met, the test fails.
 *
 */
#define __CTF_ASSERT(cond)           \
    do                               \
    {                                \
        if (!(cond))                 \
        {                            \
            __CTF_ASSERT_TEXT(cond); \
            __CTF_FAIL();            \
        }                            \
    } while (0)

/**
 * @brief If the condition is not met, the test fails.
 *
 */
#define __CTF_ASSERT_LOG(cond, ...)  \
    do                               \
    {                                \
        if (!(cond))                 \
        {                            \
            __CTF_ASSERT_TEXT(cond); \
            __CTF_LOG(__VA_ARGS__);  \
            __CTF_FAIL();            \
        }                            \
    } while (0)

#define __CTF_BLOCK(...) \
    {                    \
        __VA_ARGS__;     \
    }

/**
 * @brief Allows to run multiple statements in a block.
 *
 */
#define __CTF_CLEAN_FUNC(...) __CTF_BLOCK(__VA_ARGS__)

#define __CTF_CODE(...) __CTF_BLOCK(__VA_ARGS__)

/**
 * @brief If the condition is not met, the clean_func is called and the test fails.
 *
 * @note doesnt require second arg to be __CTF_BLOCK or __CTF_CLEAN_FUNC but it should be.
 *
 */
#define __CTF_ASSERT_CLEAN(cond, ...) \
    do                                \
    {                                 \
        if (!(cond))                  \
        {                             \
            __CTF_ASSERT_TEXT(cond);  \
            __VA_ARGS__;              \
            __CTF_FAIL();             \
        }                             \
    } while (0)

/**
 * @brief If the condition is not met, the clean_func is called and the test fails.
 *
 * @note Requires second arg to be __CTF_BLOCK or __CTF_CLEAN_FUNC.
 *
 */
#define __CTF_ASSERT_CLEAN_LOG(cond, clean_func, ...) \
    do                                                \
    {                                                 \
        if (!(cond))                                  \
        {                                             \
            __CTF_ASSERT_TEXT(cond);                  \
            __CTF_LOG(__VA_ARGS__);                   \
            clean_func;                               \
            __CTF_FAIL();                             \
        }                                             \
    } while (0)

/**
 * @brief Creates a test suite with the given name. This is the preferred, but not required, way to create a test suite.
 *
 * @param name Name of the test suite.
 * @param ... Code to run in the test suite, optionally enclose in semicolons. This is where you link your tests to the suite.
 */
#define __CTF_SUITE(name, ...) __CTF_SUITE_IMPL(name, __VA_ARGS__)

/**
 * @brief Used inside of a test suite to link a test to the suite.
 *
 */
#define __CTF_SUITE_LINK(__suite, test) __CTF_SUITE_LINK_IMPL(&__suite##_suite, test##_func, #test)

/**
 * @brief Call this macro to run a test suite.
 *
 */
#define __CTF_SUITE_RUN(name) __CTF_SUITE_RUN_IMPL(name)

/**
 * @brief Use in a main function that has argv and argc to alter the some aspects of the testing framework.
 *
 */
#define __CTF_PROCESS_INIT() __CTF_PROCESS_INIT_IMPL(argc, argv)

#define __CTF_PROCESS_EXIT() __CTF_PROCESS_EXIT_IMPL()

/*
    If you use __CTF_SUITE you dont need to use the following macros.
*/

/**
 * @brief Used to define a test suite.
 *
 * @note Don't forget to call __CTF_SUITE_END at the end of the suite.
 */
#define __CTF_SUITE_MAKE(__name) __CTF_SUITE_MAKE_IMPL(__name)

/**
 * @brief Used to end a test suite.
 *
 */
#define __CTF_SUITE_END(name) __CTF_SUITE_END_IMPL(name)

/*
    ====================================================================================================
                                All that follows is used internally.
    ====================================================================================================
*/

#define __CTF_ANSI_RED __CTF_ANSI_COLOR("\x1b[31m")
#define __CTF_ANSI_GREEN __CTF_ANSI_COLOR("\x1b[32m")
#define __CTF_ANSI_YELLOW __CTF_ANSI_COLOR("\x1b[33m")
#define __CTF_ANSI_BLUE __CTF_ANSI_COLOR("\x1b[34m")
#define __CTF_ANSI_UNDERLINE __CTF_ANSI_COLOR("\x1b[4m")
#define __CTF_ANSI_RESET __CTF_ANSI_COLOR("\x1b[0m")

typedef struct
{
    int (*test_func)();
    const char *test_name;
} __CTF_Test;

typedef struct
{
    __CTF_Test *tests;
    int count;
    int capacity;
    const char *name;
} __CTF_Test_Suite;

char *__ctf_current_test_suite_name = NULL;
char *__ctf_current_test_name = NULL;

unsigned int __ctf_suites_ran = 0;

jmp_buf __ctf_env;
volatile sig_atomic_t __signal_caught = 0;

FILE *__ctf_log_file = NULL;

clock_t __ctf_process_start_time = -1;

/**
 * @brief Set to true to ask the user if they want to continue testing after a signal is caught.
 *
 * @note When enabled, if you are running the test program from a debugger it may cause the program to hang.
 */
bool __ctf_handle_signal_ask_user = false;

bool __ctf_try_use_colors = true;

/* Meant to be crossplatform */
bool __CTF_ANSI_COLOR_SUPPORT()
{
    if (!__ctf_try_use_colors)
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

const char *__CTF_ANSI_COLOR(const char *color)
{
    return __CTF_ANSI_COLOR_SUPPORT() ? color : "";
}

bool __CTF_ASK_USER()
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

#define __CTF_FAIL_TEXT()                                                                       \
    __CTF_LOG("\n\t%sFail in Suite:%s\"%s\"%s, Test:%s\"%s\"%s:%s\n\t\tfile: %s\n\t\tline: %d", \
              __CTF_ANSI_RED, __CTF_ANSI_YELLOW, __ctf_current_test_suite_name, __CTF_ANSI_RED, __CTF_ANSI_YELLOW, __ctf_current_test_name, __CTF_ANSI_RED, __CTF_ANSI_RESET, __FILE__, __LINE__);

#define __CTF_ASSERT_TEXT(cond)                          \
    __CTF_LOG("\n\t%sAssertion failed:%s\n\t\tcond: %s", \
              __CTF_ANSI_RED, __CTF_ANSI_RESET, #cond)


#define __CTF_LOG_ARGS_COLOR(suite,test) "%s[LOG%s%s%s%s]%s ", __CTF_ANSI_YELLOW, suite ? "/" : "", suite ? __ctf_current_test_suite_name : "", test ? "/" : "", test ? __ctf_current_test_name : "", __CTF_ANSI_RESET
#define __CTF_LOG_ARGS(suite,test) "[LOG%s%s%s%s] ", suite ? "/" : "", suite ? __ctf_current_test_suite_name : "", test ? "/" : "", test ? __ctf_current_test_name : ""
/**
 * @brief Takes a filename rather than FILE* because we expect a critial error to occur and always want to ensure that the log is written.
 *
 * @note If __CTF_PROCESS_INIT was called then we will use already opened __ctf_log_file. This enables faster logging.
 *
 */
#define __CTF_LOG_IMPL(filename, ...)                                                                                                                                                          \
    do                                                                                                                                                                                         \
    {                                                                                                                                                                                          \
        bool suite = __ctf_current_test_suite_name != NULL;                                                                                                                                    \
        bool test = __ctf_current_test_name != NULL;                                                                                                                                           \
        printf(__CTF_LOG_ARGS_COLOR(suite,test)); \
        printf(__VA_ARGS__);                                                                                                                                                                   \
        putchar('\n');                                                                                                                                                                         \
        FILE *file = __ctf_log_file ? __ctf_log_file : fopen(filename, "a");                                                                                                                   \
        bool old_use_colors = __ctf_try_use_colors;                                                                                                                                            \
        __ctf_try_use_colors = false;                                                                                                                                                          \
        if (file)                                                                                                                                                                              \
        {                                                                                                                                                                                      \
            fprintf(file, __CTF_LOG_ARGS(suite,test));                               \
            fprintf(file, __VA_ARGS__);                                                                                                                                                        \
            fputc('\n', file);                                                                                                                                                                 \
            if (!__ctf_log_file)                                                                                                                                                               \
                fclose(file);                                                                                                                                                                  \
        }                                                                                                                                                                                      \
        __ctf_try_use_colors = old_use_colors;                                                                                                                                                 \
    } while (0)

void __CTF_HANDLE_SIGNAL(int sig);

/* Register signal handlers macro */
void __CTF_REGISTER_SIGNAL_HANDLERS(void)
{
    signal(SIGSEGV, __CTF_HANDLE_SIGNAL);
    signal(SIGFPE, __CTF_HANDLE_SIGNAL);
    signal(SIGILL, __CTF_HANDLE_SIGNAL);
    signal(SIGABRT, __CTF_HANDLE_SIGNAL);
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
void __CTF_HANDLE_SIGNAL(int sig)
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
    if (__ctf_current_test_name != NULL)
    {
        snprintf(errbuff, __CTF_ERRBUFF_SIZE, "%s\tError occurred during test: %s\n", sigbuff, __ctf_current_test_name);
    }
    if (__ctf_current_test_suite_name != NULL)
    {
        snprintf(buff, __CTF_BUFF_SIZE, "%s\tIn test suite: %s", errbuff, __ctf_current_test_suite_name);
    }
    __CTF_LOG("%s%s%s", __CTF_ANSI_RED, buff, __CTF_ANSI_RESET);
    /*  Reset signal handlers to default */
    __CTF_RESET_SIGNAL_HANDLERS();

    if (__ctf_handle_signal_ask_user)
    {
        printf("Do you want to continue testing? ");
        if (__CTF_ASK_USER())
        {
            raise(sig);
            return;
        }
    }
    __signal_caught = sig;
    /* Jump back to tests */
    longjmp(__ctf_env, 1);
}

#define __CTF_SUITE_RUN_TESTS_IMPL(suite)                                                                                                                                                \
    do                                                                                                                                                                                   \
    {                                                                                                                                                                                    \
        int i;                                                                                                                                                                           \
        printf("%s%sRunning Test Suite: %s\n%s", __CTF_ANSI_UNDERLINE, __CTF_ANSI_YELLOW, (suite).name, __CTF_ANSI_RESET);                                                               \
        __ctf_current_test_suite_name = (char *)(suite).name;                                                                                                                            \
        int total_tests = 0, passed_tests = 0;                                                                                                                                           \
        for (i = 0; i < (suite).count; i++)                                                                                                                                              \
        {                                                                                                                                                                                \
            total_tests++;                                                                                                                                                               \
            __ctf_current_test_name = (char *)(suite).tests[i].test_name;                                                                                                                \
            clock_t start = clock();                                                                                                                                                     \
            printf("%sRunning Test: %s%s%s...\n%s", __CTF_ANSI_BLUE, __CTF_ANSI_YELLOW, __ctf_current_test_name, __CTF_ANSI_BLUE, __CTF_ANSI_RESET);                                     \
            if (setjmp(__ctf_env) == 0)                                                                                                                                                  \
            {                                                                                                                                                                            \
                __signal_caught = 0;                                                                                                                                                     \
                int result = (suite).tests[i].test_func();                                                                                                                               \
                if (result == __CTF_PASS_VALUE && __signal_caught == 0)                                                                                                                  \
                {                                                                                                                                                                        \
                    printf("%sTest %s\"%s\"%s passed.%s\n", __CTF_ANSI_GREEN, __CTF_ANSI_YELLOW, __ctf_current_test_name, __CTF_ANSI_GREEN, __CTF_ANSI_RESET);                           \
                    passed_tests++;                                                                                                                                                      \
                }                                                                                                                                                                        \
                else                                                                                                                                                                     \
                {                                                                                                                                                                        \
                    printf("%sTest \"%s\" failed.\n%s", __CTF_ANSI_RED, __ctf_current_test_name, __CTF_ANSI_RESET);                                                                      \
                }                                                                                                                                                                        \
            }                                                                                                                                                                            \
            else                                                                                                                                                                         \
            {                                                                                                                                                                            \
                printf("%sTest %s\"%s\"%s failed due to signal %d.%s\n", __CTF_ANSI_RED, __CTF_ANSI_YELLOW, __ctf_current_test_name, __CTF_ANSI_RED, __signal_caught, __CTF_ANSI_RESET); \
            }                                                                                                                                                                            \
            clock_t end = clock();                                                                                                                                                       \
            double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;                                                                                                                \
            printf("\t%sElapsed time: %fs%s\n", __CTF_ANSI_YELLOW, elapsed_time, __CTF_ANSI_RESET);                                                                                      \
        }                                                                                                                                                                                \
        __ctf_current_test_name = NULL;                                                                                                                                                  \
        __CTF_LOG("\nTest suite %s\"%s\"%s summary:\n%sTotal tests: %d\n%sPassed tests: %d\n%sFailed tests: %d\n%sPass rate: %.2f%%%s",                                                  \
                  __CTF_ANSI_YELLOW, __ctf_current_test_suite_name, __CTF_ANSI_RESET,                                                                                                    \
                  __CTF_ANSI_BLUE, total_tests,                                                                                                                                          \
                  __CTF_ANSI_GREEN, passed_tests,                                                                                                                                        \
                  __CTF_ANSI_RED, total_tests - passed_tests,                                                                                                                            \
                  __CTF_ANSI_YELLOW, (float)passed_tests / total_tests * 100, __CTF_ANSI_RESET);                                                                                         \
    } while (0)

/**
 * @brief Defines __CTF_SUITE_RUN_TESTS as a function for preventing variable name reuse.
 *
 * @note only declare if your test suite is polluted with local/global variable names.
 *
 */
#ifndef CTF_SUITE_RUN_TEST_MACRO_ONLY
void __CTF_SUITE_RUN_TESTS(__CTF_Test_Suite suite)
{
    __CTF_SUITE_RUN_TESTS_IMPL(suite);
}
#else
#define __CTF_SUITE_RUN_TESTS(suite) __CTF_SUITE_RUN_TESTS_IMPL(suite)
#endif

void __CTF_PROCESS_EXIT_IMPL(void)
{
    __CTF_LOG("Testing complete. %d suites ran.", __ctf_suites_ran);
    clock_t end_time = clock();
    float runtime = (double)(end_time - __ctf_process_start_time) / CLOCKS_PER_SEC;
    __CTF_LOG("Testing process completed in %fs.", __ctf_process_start_time != -1 ? runtime : -1.0f);
    if (__ctf_log_file)
    {
        fclose(__ctf_log_file);
        __ctf_log_file = NULL;
    }
    exit(0);
}

void __CTF_SUITE_LINK_IMPL(__CTF_Test_Suite *suite, int (*test_func)(), const char *test_name)
{
    if (suite->count >= suite->capacity)
    {
        suite->capacity = suite->capacity == 0 ? 1 : suite->capacity * 2;
        suite->tests = (__CTF_Test *)realloc(suite->tests, suite->capacity * sizeof(__CTF_Test));
    }
    suite->tests[suite->count].test_func = test_func;
    suite->tests[suite->count].test_name = test_name;
    suite->count++;
}

#define __CTF_SUITE_IMPL(name, ...)            \
    __CTF_SUITE_MAKE(name)                     \
    {                                          \
        __ctf_current_test_suite_name = #name; \
        int i = 22 + ((sizeof(#name)/sizeof(char)) - 3), j = i;     \
        while (i--)                            \
        {                                      \
            putchar('+');                      \
        }                                      \
        putchar('\n');                         \
        __CTF_LOG_TIME();                      \
        __CTF_BLOCK(__VA_ARGS__);              \
        __CTF_SUITE_END(name);                 \
        i = j;                                 \
        while (i--)                            \
        {                                      \
            putchar('-');                      \
        }                                      \
        putchar('\n');                         \
    }

#define __CTF_LOG_TIME_IMPL()                                    \
    {                                                            \
        time_t t;                                                \
        time(&t);                                                \
        char buff[70];                                           \
        if (strftime(buff, sizeof buff, "%A %c", localtime(&t))) \
            __CTF_LOG(buff);                                     \
        else                                                     \
            __CTF_LOG("Could not get date and time info.");      \
    }

#define __CTF_SUITE_RUN_IMPL(name)        \
    do                                    \
    {                                     \
        __CTF_REGISTER_SIGNAL_HANDLERS(); \
        name##_suite_func();              \
        __ctf_suites_ran++;               \
        __CTF_RESET_SIGNAL_HANDLERS();    \
    } while (0)

#define __CTF_SUITE_MAKE_IMPL(__name)   \
    __CTF_Test_Suite __name##_suite = { \
        .tests = NULL,                  \
        .count = 0,                     \
        .capacity = 0,                  \
        .name = #__name,                \
    };                                  \
    void __name##_suite_func()

#define __CTF_SUITE_END_IMPL(name)                                                                                             \
    do                                                                                                                         \
    {                                                                                                                          \
        clock_t test_start_time = clock();                                                                                     \
        __CTF_SUITE_RUN_TESTS(name##_suite);                                                                                   \
        clock_t test_runtime = clock() - test_start_time;                                                                      \
        float test_runtime_sec = (double)(test_runtime) / CLOCKS_PER_SEC;                                                      \
        __CTF_LOG("\nTest suite %s\"%s\"%s tests ran for %fs.", __CTF_ANSI_YELLOW, #name, __CTF_ANSI_RESET, test_runtime_sec); \
        free((name##_suite).tests);                                                                                            \
        __ctf_current_test_name = NULL;                                                                                        \
        __ctf_current_test_suite_name = NULL;                                                                                  \
    } while (0);

void __CTF_PROCESS_INIT_IMPL(int argc, char **argv)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-nc") == 0 || strcmp(argv[i], "-no-color") == 0)
        {
            __ctf_try_use_colors = false;
        }
        else if (strcmp(argv[i], "-as") == 0 || strcmp(argv[i], "--ask-signal") == 0)
        {
            __ctf_handle_signal_ask_user = true;
        }
        else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--log") == 0)
        {
            if (i + 1 < argc)
            {
                __CTF_LOG_FILE_NAME = argv[i + 1];
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
    __ctf_current_test_name = NULL;
    __ctf_current_test_suite_name = NULL;
    __ctf_process_start_time = clock();
    __ctf_log_file = fopen(__CTF_LOG_FILE_NAME, "w");
    __CTF_LOG("C Testing framework (CTF) initialized.");
    __CTF_LOG_TIME();
}
#endif /* _CTF_FRAMEWORK_H */

#include "testing.h"
#include <stdlib.h>

/* Define tests */
TEST_MAKE(test_example)
{
    int a = 5;
    int b = 10;
    TEST_ASSERT(a + b == 15);
    TEST_PASS();
}

TEST_MAKE(test_fail)
{
    int x = 7;
    TEST_ASSERT(x > 10); /* This will fail */
    TEST_FAIL();
}

/* This is one way of defining a test suite. */
TEST_SUITE_MAKE(Example)
{
    TEST_SUITE_LINK(Example, test_example);
    TEST_SUITE_LINK(Example, test_fail);
    TEST_SUITE_END(Example);
}

#include "../Vector/vector.h"
#include "../Vector/vector.c"

TEST_MAKE(Vector_Test)
{
    Vec *int_vec = VEC(int);
    TEST_ASSERT(int_vec != NULL);
    TEST_ASSERT(int_vec->len == 0);
    int tmp = 5;
    vec_push_back(int_vec, &tmp);
    /*If */
    TEST_ASSERT_CLEAN(int_vec->len == 1, vec_free(int_vec));
    /* If you need to do more complex things in your clean up you can run pass a closeure by using a a block */
    TEST_ASSERT_CLEAN(
        *(int *)vec_at(int_vec, 0) == 5,
        TEST_BLOCK( /* Could also use TEST_CODE or TEST_CLEAN_FUNC*/
                   vec_free(int_vec);));
    vec_free(int_vec);
    TEST_PASS();
}
/* This is another way of defining a test suite, which just also logs the time and some extra info
    If you only have one test you dont need semi colons, otherwise you do.
*/
TEST_SUITE(Vec, TEST_SUITE_LINK(Vec, Vector_Test))

#include "../Map/map.h"
#include "../Map/map.c"

int int_map_cmp_int(const void *key1, const void *key2)
{
    return *(int *)key1 - *(int *)key2;
}

TEST_MAKE(Map_Test)
{
    Map *int_map = MAP(int, int);
    int_map->type.key_cmp = int_map_cmp_int;
    TEST_ASSERT(int_map != NULL);
    TEST_ASSERT(int_map->length == 0);
    int tmp = 5;
    map_add(int_map, &tmp, &tmp);
    TEST_ASSERT(int_map->length == 1);
    int *tmp2 = (int *)map_get(int_map, &tmp);
    /* There are a few variations of assert which let you log or clean or do both */
    TEST_ASSERT_CLEAN_LOG(tmp2 != NULL, map_free(int_map), "Failed to get value from map");
    /* Unfortunately when passing a complex cleanup we need to wrap it in TEST_CLEAN_FUNC or a similar macro to ensure that all the code is ran */
    TEST_ASSERT_CLEAN_LOG(*tmp2 == tmp,
                          TEST_CLEAN_FUNC(
                              {
                                  int i = 0;
                                  i += 1;
                                  map_free(int_map);
                              }),
                          "Value in map is not correct");
    map_free(int_map);
    TEST_PASS();
}

TEST_MAKE(Map_Malloc_Test)
{
    Map *int_map = map_new(MAP_TYPE(char *, int, map_default_hash_str, map_default_cmp_str, map_default_free_str, NULL), 2);
    TEST_ASSERT(int_map != NULL);
    TEST_ASSERT(int_map->length == 0);
    char *tmp = malloc(5);
    int tmp2 = 5;
    strcpy(tmp, "test");
    map_add(int_map, &tmp, &tmp2);
    TEST_ASSERT_CLEAN(int_map->length == 1, map_free(int_map));
    int *tmp3 = (int *)map_get(int_map, &tmp);
    if (!tmp3)
    {
        map_free(int_map);
        TEST_FAIL();
    }
    TEST_ASSERT_CLEAN(*tmp3 == tmp2, map_free(int_map));
    map_free(int_map);
    TEST_PASS();
}

/* The second arg of TEST_SUITE can be ran like a closure so you can do anything you want on top of linking tests. The second arg could also be a TEST_BLOCK */
TEST_SUITE(
    Map,
    {
        TEST_SUITE_LINK(Map, Map_Test);
        TEST_SUITE_LINK(Map, Map_Malloc_Test);
    })

TEST_MAKE(Null_Deref)
{
    TEST_LOG("This test should segfault");
    volatile int *ptr = NULL;
    int i = *ptr;
    i++;
    TEST_PASS();
}

TEST_MAKE(Bad_Assert)
{
    TEST_ASSERT(1 == 0);
    TEST_PASS();
}

/* Alternate way of declaring a test suite. This way you just need to link the tests to the suite. */
TEST_SUITE(
    Intentional_Fail,
    {
        TEST_LOG("Tests in this suite are expected to fail");
        TEST_SUITE_LINK(Intentional_Fail, Null_Deref);
        TEST_SUITE_LINK(Intentional_Fail, Bad_Assert);
    })
#include <time.h>

int main(int argc, char **argv)
{
    /* Optional to use, but will allow for command line args to control aspects of the test process*/
    TEST_PROCESS_INIT();
    TEST_LOG("Running tests...");
    TEST_SUITE_RUN(Example);
    TEST_SUITE_RUN(Vec);
    TEST_SUITE_RUN(Map);
    TEST_LOG("The following suite should fail");
    TEST_SUITE_RUN(Intentional_Fail);
    /* Only needs to be used at the end of main if INIT was called otherwise its optional */
    TEST_PROCESS_EXIT();
    return 0;
}

#define CTF_CTF_NAMES
#include "ctf.h"
#include <stdlib.h>

/* Define tests */
CTF_TEST(CTF_example)
{
    int a = 5;
    int b = 10;
    CTF_ASSERT(a + b == 15);
    CTF_PASS();
}

CTF_TEST(CTF_fail)
{
    int x = 7;
    CTF_ASSERT(x > 10); /* This will fail */
    CTF_FAIL();
}

/* This is one way of defining a test suite. */
CTF_SUITE_MAKE(Example)
{
    CTF_SUITE_INIT(Example);
    CTF_SUITE_LINK(Example, CTF_example);
    CTF_SUITE_LINK(Example, CTF_fail);
    CTF_SUITE_END(Example);
}

#include "../Vector/vector.h"
#include "../Vector/vector.c"

CTF_TEST(Vector_Test)
{
    Vec *int_vec = VEC(int);
    CTF_ASSERT(int_vec != NULL);
    CTF_ASSERT(int_vec->len == 0);
    int tmp = 5;
    vec_push_back(int_vec, &tmp);
    /*If */
    CTF_ASSERT_CLEAN(int_vec->len == 1, vec_free(int_vec));
    /* If you need to do more complex things in your clean up you can run pass a closeure by using a a block */
    CTF_ASSERT_CLEAN(
        *(int *)vec_at(int_vec, 0) == 5,
        CTF_BLOCK( /* Could also use CTF_CODE or CTF_CLEAN_FUNC*/
                   vec_free(int_vec);));
    vec_free(int_vec);
    CTF_PASS();
}
/* This is another way of defining a test suite, which just also logs the time and some extra info
    If you only have one test you dont need semi colons, otherwise you do.
*/
CTF_SUITE(Vec, CTF_SUITE_LINK(Vec, Vector_Test))

#include "../Map/map.h"
#include "../Map/map.c"

int int_map_cmp_int(const void *key1, const void *key2)
{
    return *(int *)key1 - *(int *)key2;
}

CTF_TEST(Map_Test)
{
    Map *int_map = MAP(int, int);
    int_map->type.key_cmp = int_map_cmp_int;
    CTF_ASSERT(int_map != NULL);
    CTF_ASSERT(int_map->length == 0);
    int tmp = 5;
    map_add(int_map, &tmp, &tmp);
    CTF_ASSERT(int_map->length == 1);
    int *tmp2 = (int *)map_get(int_map, &tmp);
    /* There are a few variations of assert which let you log or clean or do both */
    CTF_ASSERT_CLEAN_LOG(tmp2 != NULL, map_free(int_map), "Failed to get value from map");
    /* Unfortunately when passing a complex cleanup we need to wrap it in CTF_CLEAN_FUNC or a similar macro to ensure that all the code is ran */
    CTF_ASSERT_CLEAN_LOG(*tmp2 == tmp,
                          CTF_CLEAN_FUNC(
                              {
                                  int i = 0;
                                  i += 1;
                                  map_free(int_map);
                              }),
                          "Value in map is not correct");
    map_free(int_map);
    CTF_PASS();
}

CTF_TEST(Map_Malloc_Test)
{
    Map *int_map = map_new(MAP_TYPE(char *, int, map_default_hash_str, map_default_cmp_str, map_default_free_str, NULL), 2);
    CTF_ASSERT(int_map != NULL);
    CTF_ASSERT(int_map->length == 0);
    char *tmp = malloc(5);
    int tmp2 = 5;
    strcpy(tmp, "test");
    map_add(int_map, &tmp, &tmp2);
    CTF_ASSERT_CLEAN(int_map->length == 1, map_free(int_map));
    int *tmp3 = (int *)map_get(int_map, &tmp);
    if (!tmp3)
    {
        map_free(int_map);
        CTF_FAIL();
    }
    CTF_ASSERT_CLEAN(*tmp3 == tmp2, map_free(int_map));
    map_free(int_map);
    CTF_PASS();
}

/* The second arg of CTF_SUITE can be ran like a closure so you can do anything you want on top of linking tests. The second arg could also be a CTF_BLOCK */
CTF_SUITE(
    Map,
    {
        CTF_SUITE_LINK(Map, Map_Test);
        CTF_SUITE_LINK(Map, Map_Malloc_Test);
    })

CTF_TEST(Null_Deref)
{
    CTF_LOG("This test should segfault");
    volatile int *ptr = NULL;
    int i = *ptr;
    i++;
    CTF_PASS();
}

CTF_TEST(Bad_Assert)
{
    CTF_ASSERT(1 == 0);
    CTF_PASS();
}

/* Alternate way of declaring a test suite. This way you just need to link the tests to the suite. */
CTF_SUITE(
    Intentional_Fail,
    {
        CTF_LOG("Tests in this suite are expected to fail");
        CTF_SUITE_LINK(Intentional_Fail, Null_Deref);
        CTF_SUITE_LINK(Intentional_Fail, Bad_Assert);
    })
#include <time.h>

int main(int argc, char **argv)
{
    /* Optional to use, but will allow for command line args to control aspects of the test process*/
    CTF_PROCESS_INIT();
    CTF_LOG("Running tests...");
    CTF_SUITE_RUN(Example);
    CTF_SUITE_RUN(Vec);
    CTF_SUITE_RUN(Map);
    CTF_LOG("The following suite should fail");
    CTF_SUITE_RUN(Intentional_Fail);
    /* Only needs to be used at the end of main if INIT was called otherwise its optional */
    CTF_PROCESS_EXIT();
    return 0;
}

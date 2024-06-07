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
    TEST_ASSERT(int_vec->len == 1);
    TEST_ASSERT(*(int *)vec_at(int_vec, 0) == 5);
    vec_free(int_vec);
    TEST_PASS();
}

TEST_SUITE_MAKE(Vec)
{
    TEST_SUITE_LINK(Vec, Vector_Test);
    TEST_SUITE_END(Vec);
}

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
    TEST_ASSERT_CLEAN_MSG(tmp2 != NULL, map_free(int_map), "Failed to get value from map");
    TEST_ASSERT_CLEAN_MSG(*tmp2 == tmp,
                          TEST_CLEAN_FUNC(
                              {
                                  int i, j;
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

TEST_SUITE_MAKE(Map)
{
    TEST_SUITE_LINK(Map, Map_Test);
    TEST_SUITE_LINK(Map, Map_Malloc_Test);
    TEST_SUITE_END(Map);
}

TEST_MAKE(Null_Deref)
{
    TEST_LOG("This test should segfault");
    volatile int *ptr = NULL;
    int i = *ptr;
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

int main()
{
    /* __testing_handle_signal_ask_user = 1; */
    TEST_LOG("Running tests...");
    TEST_SUITE_RUN(Example);
    TEST_SUITE_RUN(Vec);
    TEST_SUITE_RUN(Map);
    TEST_LOG("The following suite should fail");
    TEST_SUITE_RUN(Intentional_Fail);
    return 0;
}

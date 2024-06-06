#include "testing.h"
#include <stdlib.h>

/* Define tests */
TEST_MAKE(test_example1)
{
    int a = 5;
    int b = 10;
    TEST_ASSERT(a + b == 15);
    TEST_PASS();
}

TEST_MAKE(test_example2)
{
    int x = 7;
    TEST_ASSERT(x > 10); /* This will fail */
    TEST_FAIL();
}

TEST_SUITE_MAKE(Example)
{
    TEST_SUITE_LINK(Example, test_example1);
    TEST_SUITE_LINK(Example, test_example2);
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

TEST_MAKE(Map_Test)
{
    Map *int_map = MAP(int, int);
    TEST_ASSERT(int_map != NULL);
    TEST_ASSERT(int_map->length == 0);
    int tmp = 5;
    map_add(int_map, &tmp, &tmp);
    TEST_ASSERT(int_map->length == 1);
    TEST_ASSERT(*(int *)map_get(int_map, &tmp) == tmp);
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
    int *ptr = NULL;
    int i = *ptr;
    TEST_PASS();
}

TEST_SUITE_MAKE(Intentional_Fail)
{
    TEST_SUITE_LINK(Intentional_Fail, Null_Deref);
    TEST_SUITE_END(Intentional_Fail);
}

int main()
{
    TEST_SUITE_RUN(Example);
    TEST_SUITE_RUN(Vec);
    TEST_SUITE_RUN(Map);
    TEST_SUITE_RUN(Intentional_Fail);
    return 0;
}




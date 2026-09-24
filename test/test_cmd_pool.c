#include "unity/unity.h"
#include "../src/cmd_pool.c"

void setUp(void)    { cmd_pool_init(); }
void tearDown(void) {}

void test_starts_full(void) {
    TEST_ASSERT_EQUAL_UINT(CMD_POOL_BLOCKS, cmd_pool_available());
}
void test_alloc_reduces_count(void) {
    void *a = cmd_pool_alloc();
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_EQUAL_UINT(CMD_POOL_BLOCKS - 1, cmd_pool_available());
}
void test_exhaustion_returns_null(void) {
    for (int i = 0; i < CMD_POOL_BLOCKS; i++) TEST_ASSERT_NOT_NULL(cmd_pool_alloc());
    TEST_ASSERT_NULL(cmd_pool_alloc());          /* pool empty */
    TEST_ASSERT_EQUAL_UINT(0, cmd_pool_available());
}
void test_free_restores(void) {
    void *a = cmd_pool_alloc();
    TEST_ASSERT_TRUE(cmd_pool_free(a));
    TEST_ASSERT_EQUAL_UINT(CMD_POOL_BLOCKS, cmd_pool_available());
}
void test_free_foreign_rejected(void) {
    int stack_var;
    TEST_ASSERT_FALSE(cmd_pool_free(&stack_var));  /* not from the pool */
}
void test_free_null_rejected(void) {
    TEST_ASSERT_FALSE(cmd_pool_free(NULL));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_starts_full);
    RUN_TEST(test_alloc_reduces_count);
    RUN_TEST(test_exhaustion_returns_null);
    RUN_TEST(test_free_restores);
    RUN_TEST(test_free_foreign_rejected);
    RUN_TEST(test_free_null_rejected);
    return UNITY_END();
}

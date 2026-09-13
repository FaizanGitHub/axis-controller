/* Minimal Unity-compatible test framework (subset).
 * The real Unity is at github.com/ThrowTheSwitch/Unity — swap it in later. */
#ifndef UNITY_H
#define UNITY_H
#include <stdio.h>
#include <string.h>
#include <stdint.h>

extern int unity_tests_run, unity_tests_failed;
void setUp(void);
void tearDown(void);

#define RUN_TEST(fn) do {                                        \
    unity_tests_run++;                                           \
    setUp();                                                     \
    printf("  %-45s", #fn);                                      \
    int before = unity_tests_failed;                             \
    fn();                                                        \
    tearDown();                                                  \
    if (unity_tests_failed == before) printf("PASS\n");           \
} while (0)

#define UNITY_FAIL(msg) do {                                     \
    printf("FAIL (%s:%d) %s\n", __FILE__, __LINE__, msg);        \
    unity_tests_failed++;                                        \
    return;                                                      \
} while (0)

#define TEST_ASSERT_TRUE(c)        do { if(!(c)) UNITY_FAIL("expected true");  } while(0)
#define TEST_ASSERT_FALSE(c)       do { if(c)  UNITY_FAIL("expected false"); } while(0)
#define TEST_ASSERT_NULL(p)        do { if((p)!=NULL) UNITY_FAIL("expected NULL"); } while(0)
#define TEST_ASSERT_NOT_NULL(p)    do { if((p)==NULL) UNITY_FAIL("expected non-NULL"); } while(0)
#define TEST_ASSERT_EQUAL_INT(e,a) do { if((int64_t)(e)!=(int64_t)(a)) { \
    printf("FAIL (%s:%d) expected %lld got %lld\n",__FILE__,__LINE__,(long long)(e),(long long)(a)); \
    unity_tests_failed++; return; } } while(0)
#define TEST_ASSERT_EQUAL_UINT(e,a) TEST_ASSERT_EQUAL_INT((int64_t)(e),(int64_t)(a))
#define TEST_ASSERT_EQUAL_HEX16(e,a) TEST_ASSERT_EQUAL_INT((uint16_t)(e),(uint16_t)(a))
#define TEST_ASSERT_EQUAL_HEX32(e,a) TEST_ASSERT_EQUAL_INT((uint32_t)(e),(uint32_t)(a))

#define UNITY_BEGIN() (unity_tests_run=0, unity_tests_failed=0, printf("\n== %s ==\n", __FILE__))
#define UNITY_END()  (printf("-- %d tests, %d failures --\n\n", unity_tests_run, unity_tests_failed), \
                      unity_tests_failed)
#endif

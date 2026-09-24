#include "unity/unity.h"
#include "../src/motion_profile.c"    /* gives direct access to statics */

void setUp(void)    {}
void tearDown(void) {}

/* helper: build a MOVE command to a given target */
static command_t make_move(int32_t target) {
    command_t c = {0};
    c.cmd = CMD_MOVE;
    c.payload[0] = target & 0xFF;
    c.payload[1] = (target >> 8) & 0xFF;
    c.payload[2] = (target >> 16) & 0xFF;
    c.payload[3] = (target >> 24) & 0xFF;
    return c;
}

/* --- 1. profile_start MOVE sets up state correctly (your test, using direct static access) --- */
void test_profile_start_MOVE(void) {
    command_t cmd = make_move(16);
    profile_start(&cmd, 0);
    TEST_ASSERT_EQUAL_INT(0, timeout_counter);              /* direct static access */
    TEST_ASSERT_EQUAL_INT(PROFILE_ACCEL, profile_get_state());
    TEST_ASSERT_EQUAL_INT(CMD_MOVE, profile_get_status().cmd);
    TEST_ASSERT_FALSE(profile_get_status().success);
    TEST_ASSERT_EQUAL_INT(ERR_NONE, profile_get_status().error_code);
    TEST_ASSERT_EQUAL_INT(16, target_position);
    TEST_ASSERT_EQUAL_INT(0,  start_position);
    TEST_ASSERT_EQUAL_INT(16, total_distance);
    TEST_ASSERT_EQUAL_INT(1,  direction);
    TEST_ASSERT_TRUE(is_short_move);                        /* 16 < 66 */
}

/* --- 2. A FULL trapezoid move walks ACCEL -> CRUISE -> DECEL -> DONE --- */
void test_full_trapezoid_walk(void) {
    command_t cmd = make_move(500);       /* 500 > 66 -> full trapezoid */
    profile_start(&cmd, 0);
    TEST_ASSERT_FALSE(is_short_move);
    TEST_ASSERT_EQUAL_INT(PROFILE_ACCEL, profile_get_state());

    profile_update(10);   /* traveled=10 < 33 -> still ACCEL */
    TEST_ASSERT_EQUAL_INT(PROFILE_ACCEL, profile_get_state());

    profile_update(40);   /* traveled=40 >= 33 -> CRUISE */
    TEST_ASSERT_EQUAL_INT(PROFILE_CRUISE, profile_get_state());

    profile_update(400);  /* remaining=100 > 33 -> still CRUISE */
    TEST_ASSERT_EQUAL_INT(PROFILE_CRUISE, profile_get_state());

    profile_update(480);  /* remaining=20 <= 33 -> DECEL */
    TEST_ASSERT_EQUAL_INT(PROFILE_DECEL, profile_get_state());

    profile_update(500);  /* remaining=0 <= tolerance -> DONE */
    TEST_ASSERT_EQUAL_INT(PROFILE_DONE, profile_get_state());
    TEST_ASSERT_TRUE(profile_is_done());
    TEST_ASSERT_TRUE(profile_get_status().success);
    TEST_ASSERT_EQUAL_INT(500, profile_get_status().final_position);
}

/* --- 3. Motion output has correct direction sign (backward move) --- */
void test_backward_move_negative_steps(void) {
    command_t cmd = make_move(100);       /* target 100, start 500 -> move backward */
    profile_start(&cmd, 500);
    TEST_ASSERT_EQUAL_INT(-1, direction);
    motion_output_t out = profile_update(490);   /* moving */
    TEST_ASSERT_EQUAL_INT(-1, out.steps_to_move); /* negative = backward */
    TEST_ASSERT_TRUE(out.motion_active);
}

/* --- 4. Homing walks toward 0 and reaches DONE --- */
void test_homing_reaches_done(void) {
    command_t cmd = {0}; cmd.cmd = CMD_HOME;
    profile_start(&cmd, 300);
    TEST_ASSERT_EQUAL_INT(PROFILE_HOMING, profile_get_state());
    motion_output_t out = profile_update(150);    /* not home yet */
    TEST_ASSERT_EQUAL_INT(-1, out.steps_to_move); /* toward 0 */
    TEST_ASSERT_EQUAL_INT(PROFILE_HOMING, profile_get_state());
    profile_update(0);                             /* home reached */
    TEST_ASSERT_EQUAL_INT(PROFILE_DONE, profile_get_state());
    TEST_ASSERT_TRUE(profile_get_status().success);
}

/* --- 5. Unknown command -> FAULT; RETURN -> not implemented --- */
void test_unknown_command_faults(void) {
    command_t cmd = {0}; cmd.cmd = 0xFF;
    profile_start(&cmd, 0);
    TEST_ASSERT_EQUAL_INT(PROFILE_FAULT, profile_get_state());
}
void test_return_not_implemented(void) {
    command_t cmd = {0}; cmd.cmd = CMD_RETURN;
    profile_start(&cmd, 0);
    TEST_ASSERT_EQUAL_INT(PROFILE_FAULT, profile_get_state());
    TEST_ASSERT_EQUAL_INT(ERR_NOT_IMPLEMENTED, profile_get_status().error_code);
}

/* --- 6. DONE/FAULT emit no motion --- */
void test_done_emits_no_motion(void) {
    command_t cmd = {0}; cmd.cmd = CMD_STOP;
    profile_start(&cmd, 0);
    TEST_ASSERT_EQUAL_INT(PROFILE_DONE, profile_get_state());
    motion_output_t out = profile_update(0);
    TEST_ASSERT_FALSE(out.motion_active);
    TEST_ASSERT_EQUAL_INT(0, out.steps_to_move);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_profile_start_MOVE);
    RUN_TEST(test_full_trapezoid_walk);
    RUN_TEST(test_backward_move_negative_steps);
    RUN_TEST(test_homing_reaches_done);
    RUN_TEST(test_unknown_command_faults);
    RUN_TEST(test_return_not_implemented);
    RUN_TEST(test_done_emits_no_motion);
    return UNITY_END();
}
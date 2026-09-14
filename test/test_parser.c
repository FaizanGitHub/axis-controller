#include "unity/unity.h"
#include "../src/parser.c"

void setUp(void) {}
void tearDown(void) {}

void test_valid_frame_passes(void) {
    uint8_t good[11] = { 0xA5, 0x02, 0xE8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x85, 0xF9, 0x5A };
    command_t cmd;
    TEST_ASSERT_TRUE(parse_command_valid(good, &cmd));
    TEST_ASSERT_EQUAL_INT(0x02, cmd.cmd);           // CMD decoded
    TEST_ASSERT_EQUAL_INT(0xF985, cmd.crc);         // CRC decoded
}



void test_bad_stx_rejected(void) {
    uint8_t bad[11] = { 0x00, 0x02, 0xE8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x85, 0xF9, 0x5A };
    command_t cmd;
    TEST_ASSERT_FALSE(parse_command_valid(bad, &cmd));   // STX wrong
}

void test_crc_matches_known_vector(void) {
    uint8_t vec[] = "123456789";
    TEST_ASSERT_EQUAL_HEX16(0x29B1, crc16_ccitt(vec, 9));  // proves CRC math
}

void test_bad_etx_rejected(void) {
    uint8_t bad[11] = { 0xA5, 0x02, 0xE8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x85, 0xF9, 0x00 };
    command_t cmd;
    TEST_ASSERT_FALSE(parse_command_valid(bad, &cmd));   // ETX wrong
}

void test_corrupted_payload_fails_crc(void) {
    uint8_t bad[11] = { 0xA5, 0x02, 0xE9, 0x03, 0x00, 0x00, 0x00, 0x00, 0x85, 0xF9, 0x5A };
    //                              ^^^^ payload byte flipped E8->E9, CRC no longer matches
    command_t cmd;
    TEST_ASSERT_FALSE(parse_command_valid(bad, &cmd));   // CRC catches corruption
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_valid_frame_passes);
    RUN_TEST(test_bad_stx_rejected);
    RUN_TEST(test_crc_matches_known_vector);
    RUN_TEST(test_bad_etx_rejected);
    RUN_TEST(test_corrupted_payload_fails_crc);
    return UNITY_END();
}
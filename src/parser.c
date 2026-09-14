#include "parser.h"




static uint16_t crc16_ccitt(const uint8_t *data, int len) {
    uint16_t crc = CRC16_CCITT_INIT;  
    for (int i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;      // XOR byte into high byte
        for (int b = 0; b < 8; b++) {        // process 8 bits — NESTED
            if (crc & 0x8000)
                crc = (crc << 1) ^ CRC16_CCITT_POLY;
            else
                crc <<= 1;
        }
    }
    return crc;
}

bool parse_command_valid(const uint8_t *buffer, command_t *cmd) {
    if (buffer[0] != 0xA5 || buffer[10] != 0x5A) {
        return false;
    }
    cmd->cmd = buffer[1];
    for (int i = 0; i < 6; i++) {
        cmd->payload[i] = buffer[2 + i];
    }
    cmd->crc = (uint16_t)buffer[8] | ((uint16_t)buffer[9] << 8);
    // Here you would typically calculate the CRC16 of bytes [1..7] and compare with CRC_CHECK
uint16_t computed = crc16_ccitt(&buffer[1], 7);   // over bytes [1..7]
return (computed == cmd->crc);
}


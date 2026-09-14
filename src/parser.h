

#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdbool.h>

#define CRC16_CCITT_INIT 0xFFFF
#define CRC16_CCITT_POLY 0x1021

/// ==== Fixed 11-byte frame. Wire byte order is little-endian, stated explicitly so both ends agree regardless of CPU. ==== ///
/*
off  field     size  notes
  0  STX        1    0xA5
  1  CMD        1    01 HOME  02 MOVE  03 RETURN  04 STOP  05 STATUS
  2  PAYLOAD    6    command-specific  <-- THE alignment/endian exercise
  8  CRC16      2    CCITT-FALSE over bytes[1..7], little-endian on wire
 10  ETX        1    0x5A
 */


typedef struct command {
    uint8_t  cmd;
    uint8_t  payload[6];
    uint16_t crc;
} command_t;

bool parse_command_valid(const uint8_t *buffer, command_t *cmd);

#endif
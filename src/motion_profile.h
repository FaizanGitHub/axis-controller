#ifndef MOTION_PROFILE_H
#define MOTION_PROFILE_H
#include <stdint.h>
#include <stdbool.h>
#include "parser.h"

#define COUNTS_PER_DEGREE  11
#define ACCEL_COUNTS       (3 * COUNTS_PER_DEGREE)
#define DECEL_COUNTS       (3 * COUNTS_PER_DEGREE)
#define MIN_TRAPEZOID      (ACCEL_COUNTS + DECEL_COUNTS)
#define POS_TOLERANCE      1

#define CMD_MOVE 0x01
#define CMD_HOME 0x02
#define CMD_RETURN 0x03
#define CMD_STOP 0x04
#define CMD_STATUS 0x05

#define ACCEL_INTERVAL     800
#define CRUISE_INTERVAL    300
#define DECEL_INTERVAL     800
#define SHORT_INTERVAL     600
#define TIMEOUT_LIMIT      100000

#define ERR_NONE            0
#define ERR_TIMEOUT         1
#define ERR_NOT_IMPLEMENTED 2

typedef struct {
    int32_t  steps_to_move;    /* SIGNED: sign = direction */
    uint16_t step_interval;
    bool     motion_active;
} motion_output_t;

typedef enum {
    PROFILE_IDLE, PROFILE_ACCEL, PROFILE_CRUISE, PROFILE_DECEL,
    PROFILE_HOMING, PROFILE_DONE, PROFILE_FAULT
} profile_state_t;

typedef struct {
    uint8_t  cmd;
    bool     success;
    uint16_t final_position;
    uint8_t  error_code;
} status_t;

void            profile_start(const command_t *cmd, uint16_t current_pos);
motion_output_t profile_update(uint16_t current_pos);
profile_state_t profile_get_state(void);
status_t        profile_get_status(void);
bool            profile_is_done(void);
#endif

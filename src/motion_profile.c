#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>            /* abs() */
#include "motion_profile.h"

static profile_state_t current_state = PROFILE_IDLE;
static status_t        current_status;
static motion_output_t current_output;

static int32_t  target_position;
static int32_t  current_position;
static int32_t  start_position;
static int32_t  total_distance;
static uint32_t timeout_counter;
static bool     is_short_move;
static int8_t   direction;

void profile_start(const command_t *cmd, uint16_t current_pos) {
    timeout_counter = 0;
    current_status.cmd = cmd->cmd;
    current_status.success = false;
    current_status.error_code = ERR_NONE;

    switch (cmd->cmd) {
        case CMD_HOME:
            current_state = PROFILE_HOMING;
            target_position = 0;
            start_position = current_pos;
            current_position = current_pos;
            break;
        case CMD_MOVE: {
            target_position = (int32_t)((uint32_t)cmd->payload[0]
                            | ((uint32_t)cmd->payload[1] << 8)
                            | ((uint32_t)cmd->payload[2] << 16)
                            | ((uint32_t)cmd->payload[3] << 24));
            start_position   = current_pos;
            current_position = current_pos;
            total_distance = abs(target_position - (int32_t)current_pos);
            direction = (target_position >= (int32_t)current_pos) ? 1 : -1;
            is_short_move = (total_distance < MIN_TRAPEZOID);
            current_state = PROFILE_ACCEL;
            break;
        }
        case CMD_RETURN:
            current_state = PROFILE_FAULT;
            current_status.error_code = ERR_NOT_IMPLEMENTED;
            break;
        case CMD_STOP:
            current_state = PROFILE_DONE;
            break;
        case CMD_STATUS:
            break;
        default:
            current_state = PROFILE_FAULT;
            break;
    }
}

motion_output_t profile_update(uint16_t current_pos) {
    current_position = current_pos;
    timeout_counter++;
    if (timeout_counter > TIMEOUT_LIMIT) {
        current_state = PROFILE_FAULT;
        current_status.error_code = ERR_TIMEOUT;
    }

    int32_t traveled  = abs((int32_t)current_pos - start_position);
    int32_t remaining = abs(target_position - (int32_t)current_pos);

    switch (current_state) {
    case PROFILE_ACCEL:
        if (is_short_move) {
            current_state = PROFILE_CRUISE;
            current_output.step_interval = SHORT_INTERVAL;
        } else {
            if (traveled >= ACCEL_COUNTS) 
            {
                current_state = PROFILE_CRUISE;
            }
            current_output.step_interval = ACCEL_INTERVAL;
        }
        current_output.steps_to_move = direction;
        current_output.motion_active = true;
        break;

    case PROFILE_CRUISE:
        if (remaining <= DECEL_COUNTS) current_state = PROFILE_DECEL;
        current_output.step_interval = is_short_move ? SHORT_INTERVAL : CRUISE_INTERVAL;
        current_output.steps_to_move = direction;
        current_output.motion_active = true;
        break;

    case PROFILE_DECEL:
        if (remaining <= POS_TOLERANCE) {
            current_state = PROFILE_DONE;
            current_status.success = true;
            current_status.final_position = current_pos;
            current_output.motion_active = false;
            current_output.steps_to_move = 0;
        } else {
            current_output.step_interval = DECEL_INTERVAL;
            current_output.steps_to_move = direction;
            current_output.motion_active = true;
        }
        break;

    case PROFILE_HOMING:
        if (current_pos <= POS_TOLERANCE) {
            current_state = PROFILE_DONE;
            current_status.success = true;
            current_status.final_position = current_pos;
            current_output.motion_active = false;
            current_output.steps_to_move = 0;
        } else {
            current_output.step_interval = CRUISE_INTERVAL;
            current_output.steps_to_move = -1;
            current_output.motion_active = true;
        }
        break;

    case PROFILE_DONE:
    case PROFILE_FAULT:
        current_output.motion_active = false;
        current_output.steps_to_move = 0;
        break;

    default:
        break;
    }
    return current_output;
}

profile_state_t profile_get_state(void) { return current_state; }
status_t        profile_get_status(void) { return current_status; }
bool            profile_is_done(void) {
    return (current_state == PROFILE_DONE || current_state == PROFILE_FAULT);
}

uint32_t get_timeout_counter(void) { return timeout_counter; }
uint32_t profile_get_target_position(void) { return target_position; }
uint32_t profile_get_start_position(void) { return start_position; }
uint32_t profile_get_current_position(void) { return current_position; }
uint32_t profile_get_total_distance(void) { return total_distance; }
int8_t   profile_get_direction(void) { return direction; }
bool     profile_get_is_short_move(void) { return is_short_move; }

#ifndef BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_H
#define BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_H

#include <stdint.h>

#include "bx_ntvdm_readonly_namespace.h"

#define BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_PATH_BYTES 66u
#define BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_TAIL_BYTES 128u

typedef enum bx_ntvdm_cmd_launch_extension_v1 {
    BX_NTVDM_CMD_LAUNCH_EXTENSION_V1_NONE = 0,
    BX_NTVDM_CMD_LAUNCH_EXTENSION_V1_COM = 1
} bx_ntvdm_cmd_launch_extension_v1;

typedef enum bx_ntvdm_cmd_launch_state_v1 {
    BX_NTVDM_CMD_LAUNCH_STATE_V1_EMPTY = 0,
    BX_NTVDM_CMD_LAUNCH_STATE_V1_PENDING = 1,
    BX_NTVDM_CMD_LAUNCH_STATE_V1_DELIVERED = 2
} bx_ntvdm_cmd_launch_state_v1;

/* A copied adapter-session value.  It has no host pathname, handle, pointer,
 * CPU state, BOP record or guest-memory address. */
typedef struct bx_ntvdm_cmd_launch_envelope_v1 {
    uint32_t version;
    uint32_t struct_bytes;
    uint32_t state;
    uint32_t drive_index;
    uint32_t extension;
    uint32_t executable_path_bytes;
    uint32_t command_tail_bytes;
    char executable_path[BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_PATH_BYTES];
    uint8_t command_tail[BX_NTVDM_CMD_LAUNCH_ENVELOPE_V1_TAIL_BYTES];
} bx_ntvdm_cmd_launch_envelope_v1;

void bx_ntvdm_cmd_launch_envelope_v1_initialize(bx_ntvdm_cmd_launch_envelope_v1 *value);
int bx_ntvdm_cmd_launch_envelope_v1_derive_contained_command(
    bx_ntvdm_cmd_launch_envelope_v1 *value,
    const bx_ntvdm_readonly_namespace_v1 *readonly_namespace);
int bx_ntvdm_cmd_launch_envelope_v1_take_once(
    bx_ntvdm_cmd_launch_envelope_v1 *value,
    bx_ntvdm_cmd_launch_envelope_v1 *out_value);

#endif

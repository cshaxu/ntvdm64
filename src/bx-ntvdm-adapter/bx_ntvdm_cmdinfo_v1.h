#ifndef BX_NTVDM_CMDINFO_V1_H
#define BX_NTVDM_CMDINFO_V1_H

#include <stdint.h>

#define BX_NTVDM_CMDINFO_V1_BYTES 36u
#define BX_NTVDM_CMDINFO_V1_UCOMBUF_BYTES 128u
#define BX_NTVDM_CMDINFO_V1_EXECPATH_BYTES 273u

/* All fields are copied 16/32-bit OpenNT wire values.  Segment/offset pairs
 * remain opaque here: this codec neither calculates addresses nor owns RAM. */
typedef struct bx_ntvdm_cmdinfo_v1 {
    uint16_t environment_segment;
    uint16_t environment_bytes;
    uint16_t current_drive;
    uint16_t drive_count;
    uint16_t command_segment;
    uint16_t command_offset;
    uint16_t command_bytes;
    uint16_t return_code;
    uint16_t standard_handles;
    uint32_t redirector_info;
    uint16_t code_page;
    uint16_t tsr_exit;
    uint16_t batch_status;
    uint16_t executable_segment;
    uint16_t executable_offset;
    uint16_t executable_bytes;
    uint16_t executable_extension;
} bx_ntvdm_cmdinfo_v1;

typedef enum bx_ntvdm_cmdinfo_response_v1 {
    BX_NTVDM_CMDINFO_RESPONSE_V1_DELIVER = 1,
    BX_NTVDM_CMDINFO_RESPONSE_V1_ENVIRONMENT_RETRY = 2,
    BX_NTVDM_CMDINFO_RESPONSE_V1_COMMAND_REJECTED = 3,
    BX_NTVDM_CMDINFO_RESPONSE_V1_EXECUTABLE_REJECTED = 4
} bx_ntvdm_cmdinfo_response_v1;

typedef struct bx_ntvdm_cmdinfo_response_plan_v1 {
    uint32_t disposition;
    uint32_t required_environment_bytes;
    uint32_t required_command_bytes;
    uint32_t required_executable_bytes;
} bx_ntvdm_cmdinfo_response_plan_v1;

int bx_ntvdm_cmdinfo_v1_decode(const uint8_t *bytes, uint32_t byte_count,
    bx_ntvdm_cmdinfo_v1 *out_value);
int bx_ntvdm_cmdinfo_v1_plan_response(const bx_ntvdm_cmdinfo_v1 *request,
    uint32_t environment_bytes, uint32_t command_bytes, uint32_t executable_bytes,
    bx_ntvdm_cmdinfo_response_plan_v1 *out_plan);

#endif

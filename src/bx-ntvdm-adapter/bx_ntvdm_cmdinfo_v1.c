#include "bx_ntvdm_cmdinfo_v1.h"

#include <string.h>

static uint16_t load_u16(const uint8_t *bytes, uint32_t offset)
{
    return (uint16_t)((uint16_t)bytes[offset] | ((uint16_t)bytes[offset + 1u] << 8));
}

static uint32_t load_u32(const uint8_t *bytes, uint32_t offset)
{
    return (uint32_t)load_u16(bytes, offset) | ((uint32_t)load_u16(bytes, offset + 2u) << 16);
}

int bx_ntvdm_cmdinfo_v1_decode(const uint8_t *bytes, uint32_t byte_count,
    bx_ntvdm_cmdinfo_v1 *out_value)
{
    if (bytes == 0 || out_value == 0 || byte_count != BX_NTVDM_CMDINFO_V1_BYTES) return 0;
    memset(out_value, 0, sizeof(*out_value));
    out_value->environment_segment = load_u16(bytes, 0u);
    out_value->environment_bytes = load_u16(bytes, 2u);
    out_value->current_drive = load_u16(bytes, 4u);
    out_value->drive_count = load_u16(bytes, 6u);
    out_value->command_segment = load_u16(bytes, 8u);
    out_value->command_offset = load_u16(bytes, 10u);
    out_value->command_bytes = load_u16(bytes, 12u);
    out_value->return_code = load_u16(bytes, 14u);
    out_value->standard_handles = load_u16(bytes, 16u);
    out_value->redirector_info = load_u32(bytes, 18u);
    out_value->code_page = load_u16(bytes, 22u);
    out_value->tsr_exit = load_u16(bytes, 24u);
    out_value->batch_status = load_u16(bytes, 26u);
    out_value->executable_segment = load_u16(bytes, 28u);
    out_value->executable_offset = load_u16(bytes, 30u);
    out_value->executable_bytes = load_u16(bytes, 32u);
    out_value->executable_extension = load_u16(bytes, 34u);
    return 1;
}

int bx_ntvdm_cmdinfo_v1_plan_response(const bx_ntvdm_cmdinfo_v1 *request,
    uint32_t environment_bytes, uint32_t command_bytes, uint32_t executable_bytes,
    bx_ntvdm_cmdinfo_response_plan_v1 *out_plan)
{
    if (request == 0 || out_plan == 0 || command_bytes > BX_NTVDM_CMDINFO_V1_UCOMBUF_BYTES ||
        executable_bytes > BX_NTVDM_CMDINFO_V1_EXECPATH_BYTES) return 0;
    memset(out_plan, 0, sizeof(*out_plan));
    out_plan->required_environment_bytes = environment_bytes;
    out_plan->required_command_bytes = command_bytes;
    out_plan->required_executable_bytes = executable_bytes;
    if (environment_bytes > request->environment_bytes) {
        out_plan->disposition = BX_NTVDM_CMDINFO_RESPONSE_V1_ENVIRONMENT_RETRY;
    } else if (command_bytes > request->command_bytes) {
        out_plan->disposition = BX_NTVDM_CMDINFO_RESPONSE_V1_COMMAND_REJECTED;
    } else if (executable_bytes > request->executable_bytes) {
        out_plan->disposition = BX_NTVDM_CMDINFO_RESPONSE_V1_EXECUTABLE_REJECTED;
    } else {
        out_plan->disposition = BX_NTVDM_CMDINFO_RESPONSE_V1_DELIVER;
    }
    return 1;
}

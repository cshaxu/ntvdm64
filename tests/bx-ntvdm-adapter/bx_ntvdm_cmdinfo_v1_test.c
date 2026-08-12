#include "bx_ntvdm_cmdinfo_v1.h"

#include <stdio.h>
#include <string.h>

static void store_u16(uint8_t *bytes, uint32_t offset, uint16_t value)
{
    bytes[offset] = (uint8_t)value;
    bytes[offset + 1u] = (uint8_t)(value >> 8);
}

int main(void)
{
    uint8_t bytes[BX_NTVDM_CMDINFO_V1_BYTES] = {0};
    bx_ntvdm_cmdinfo_v1 value;
    bx_ntvdm_cmdinfo_response_plan_v1 plan;

    store_u16(bytes, 0u, 0x1111u); store_u16(bytes, 2u, 64u);
    store_u16(bytes, 4u, 2u); store_u16(bytes, 6u, 3u);
    store_u16(bytes, 8u, 0x2222u); store_u16(bytes, 10u, 0x3333u);
    store_u16(bytes, 12u, 128u); store_u16(bytes, 14u, 0x4444u);
    store_u16(bytes, 16u, 7u); bytes[18] = 0x78u; bytes[19] = 0x56u;
    bytes[20] = 0x34u; bytes[21] = 0x12u; store_u16(bytes, 22u, 437u);
    store_u16(bytes, 24u, 1u); store_u16(bytes, 26u, 2u);
    store_u16(bytes, 28u, 0x5555u); store_u16(bytes, 30u, 0x6666u);
    store_u16(bytes, 32u, 273u); store_u16(bytes, 34u, 8u);
    if (bx_ntvdm_cmdinfo_v1_decode(bytes, sizeof(bytes) - 1u, &value) ||
        !bx_ntvdm_cmdinfo_v1_decode(bytes, sizeof(bytes), &value) ||
        value.environment_segment != 0x1111u || value.command_offset != 0x3333u ||
        value.redirector_info != 0x12345678u || value.executable_extension != 8u ||
        !bx_ntvdm_cmdinfo_v1_plan_response(&value, 64u, 4u, 14u, &plan) ||
        plan.disposition != BX_NTVDM_CMDINFO_RESPONSE_V1_DELIVER ||
        !bx_ntvdm_cmdinfo_v1_plan_response(&value, 65u, 4u, 14u, &plan) ||
        plan.disposition != BX_NTVDM_CMDINFO_RESPONSE_V1_ENVIRONMENT_RETRY ||
        plan.required_environment_bytes != 65u ||
        bx_ntvdm_cmdinfo_v1_plan_response(&value, 64u, 129u, 14u, &plan) ||
        bx_ntvdm_cmdinfo_v1_plan_response(&value, 64u, 4u, 274u, &plan)) return 1;
    value.command_bytes = 3u;
    if (!bx_ntvdm_cmdinfo_v1_plan_response(&value, 64u, 4u, 14u, &plan) ||
        plan.disposition != BX_NTVDM_CMDINFO_RESPONSE_V1_COMMAND_REJECTED) return 1;
    value.command_bytes = 128u; value.executable_bytes = 13u;
    if (!bx_ntvdm_cmdinfo_v1_plan_response(&value, 64u, 4u, 14u, &plan) ||
        plan.disposition != BX_NTVDM_CMDINFO_RESPONSE_V1_EXECUTABLE_REJECTED) return 1;
    puts("bx-ntvdm-cmdinfo-v1-test: copied CMDINFO layout and response capacities verified");
    return 0;
}

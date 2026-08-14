#include "bx_ntvdm_cmd_get_next_service.h"

#include <stdio.h>
#include <string.h>

static void event(bx_ntvdm_exception_event_v1 *e)
{ *e = (bx_ntvdm_exception_event_v1){BX_NTVDM_EXCEPTION_ABI_MAGIC,BX_NTVDM_EXCEPTION_ABI_VERSION,sizeof(*e),BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,0,6,0,0,0x200u}; }

int main(void)
{
    bx_ntvdm_cmd_get_next_state_v1 state; bx_ntvdm_readonly_namespace_v1 ns = {0};
    bx_ntvdm_host_drive_snapshot_v1 drives; bx_ntvdm_cmd_set_info_registration_v1 reg = {0x1000u,0x1100u,0x1200u,0};
    byob_launch_plan_v2 plan = {2u,2u,{1u,BYOB_LAUNCH_TARGET_KIND_V1_COM,8u,{'/','c',' ','s','m','o','k','e'}}};
    bx_ntvdm_exception_event_v1 e; bx_ntvdm_cpu_state_v1 c; bx_ntvdm_instruction_window_v1 w;
    bx_ntvdm_guest_gather_read_action_v1 a; bx_ntvdm_multi_write_transaction_v1 t;
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD], record[36] = {0}, types[26] = {0};
    const uint8_t bop[] = {0xc4,0xc4,0x54,1};
    const uint8_t returned_bop[] = {0xc4,0xc4,0x54,0x0b};
    types[2] = 3u;
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(UINT32_C(4),types,0,0,&drives)) return 1;
    ns.drive_index = 2u; ns.file_count = 5u;
    ns.files[3].bytes = (const uint8_t *)"x"; ns.files[3].byte_count = 1u; wcscpy(ns.files[3].path,L"\\TARGET.COM");
    ns.files[4].bytes = (const uint8_t *)"\xc4\xc4\xfe"; ns.files[4].byte_count = 3u; wcscpy(ns.files[4].path,L"\\QUIT.COM");
    event(&e); bx_ntvdm_cpu_state_v1_initialize(&c,BX_NTVDM_CPU_EXECUTION_REAL); c.ds=0x20u; c.edx=0x10u;
    bx_ntvdm_instruction_window_v1_capture(&w,bop,sizeof(bop));
    record[8]=0x30; record[10]=0x40; record[12]=128; record[28]=0x50; record[30]=0x60; record[32]=17; record[33]=1;
    bx_ntvdm_cmd_get_next_state_v1_initialize(&state);
    if (!bx_ntvdm_cmd_get_next_v1_prepare(&state,&plan,&e,&c,&w,&a) ||
        !bx_ntvdm_cmd_get_next_v1_complete(&ns,&plan,&drives,0u,&reg,&state,&e,&c,&a,record,sizeof(record),&t,payload) ||
        memcmp(payload + 1u,"TARGET /c smoke\r\n",17u) != 0) return 2;
    bx_ntvdm_cmd_get_next_state_v1_commit(&state);
    bx_ntvdm_instruction_window_v1_capture(&w,returned_bop,sizeof(returned_bop));
    if (!bx_ntvdm_cmd_return_exit_code_v1_dispatch(&state,&plan,&e,&c,&w,&t.result) ||
        (t.result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
        state.returned != 1u) return 11;
    state.returned = 0u;
    bx_ntvdm_instruction_window_v1_capture(&w,bop,sizeof(bop));
    if (!bx_ntvdm_cmd_get_next_v1_prepare(&state,&plan,&e,&c,&w,&a) ||
        !bx_ntvdm_cmd_get_next_v1_complete(&ns,&plan,&drives,0u,&reg,&state,&e,&c,&a,record,sizeof(record),&t,payload) ||
        memcmp(payload + 1u,"QUIT\r\n",6u) != 0) return 3;
    bx_ntvdm_cmd_get_next_state_v1_commit(&state);
    if (bx_ntvdm_cmd_get_next_v1_prepare(&state,&plan,&e,&c,&w,&a)) return 4;
    plan.slot_count = 1u;
    bx_ntvdm_cmd_get_next_state_v1_initialize(&state);
    ns.file_count = 4u;
    if (!bx_ntvdm_cmd_get_next_v1_prepare(&state,&plan,&e,&c,&w,&a) ||
        !bx_ntvdm_cmd_get_next_v1_complete(&ns,&plan,&drives,0u,&reg,&state,&e,&c,&a,record,sizeof(record),&t,payload)) return 5;
    bx_ntvdm_cmd_get_next_state_v1_commit(&state);
    if (bx_ntvdm_cmd_get_next_v1_prepare(&state,&plan,&e,&c,&w,&a) ||
        bx_ntvdm_cmd_return_exit_code_v1_dispatch(&state,&plan,&e,&c,&w,&t.result)) return 6;
    bx_ntvdm_instruction_window_v1_capture(&w,returned_bop,sizeof(returned_bop));
    if (!bx_ntvdm_cmd_return_exit_code_v1_dispatch(&state,&plan,&e,&c,&w,&t.result) ||
        t.result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        (t.result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        t.result.cpu_delta.gpr16_values[0] != 0u || state.returned != 1u ||
        bx_ntvdm_cmd_return_exit_code_v1_dispatch(&state,&plan,&e,&c,&w,&t.result)) return 7;
    /* cmdGetNextCmd does not consume cmdSetInfo's SCS/DOSDATA locators.
       A pre-registration entry may still build the copied CMDINFO response;
       it must not manufacture zero-address registration writes. */
    bx_ntvdm_instruction_window_v1_capture(&w,bop,sizeof(bop));
    bx_ntvdm_cmd_get_next_state_v1_initialize(&state);
    if (!bx_ntvdm_cmd_get_next_v1_prepare(&state,&plan,&e,&c,&w,&a) ||
        !bx_ntvdm_cmd_get_next_v1_complete(&ns,&plan,&drives,0u,0,&state,&e,&c,
            &a,record,sizeof(record),&t,payload) ||
        t.writes.write_count < 7u ||
        !bx_ntvdm_multi_write_transaction_v1_preflight(&t,UINT64_C(0x100000),
            t.writes.payload_bytes)) return 8;
    /* `cmdGetNextCmd` returns CF/AX before delivery when its environment
       capacity is below the bytes already produced by 54:0F. */
    bx_ntvdm_cmd_get_next_state_v1_initialize(&state);
    record[0] = 0x70u; record[1] = 0u; record[2] = 1u; record[3] = 0u;
    if (!bx_ntvdm_cmd_get_next_v1_prepare(&state,&plan,&e,&c,&w,&a) ||
        !bx_ntvdm_cmd_get_next_v1_complete(&ns,&plan,&drives,2u,0,&state,&e,&c,
            &a,record,sizeof(record),&t,payload) || t.writes.write_count != 0u ||
        t.result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        (t.result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
        t.result.cpu_delta.gpr16_values[0] != 2u) return 9;
    /* A caller-shaped CMDINFO must not retain or dereference a range that
       crosses the real-mode aperture.  The checked provider declines before
       publishing any write transaction. */
    record[0] = 0xffu; record[1] = 0xffu; record[2] = 32u; record[3] = 0u;
    if (!bx_ntvdm_cmd_get_next_v1_prepare(&state,&plan,&e,&c,&w,&a) ||
        bx_ntvdm_cmd_get_next_v1_complete(&ns,&plan,&drives,32u,0,&state,&e,&c,
            &a,record,sizeof(record),&t,payload)) return 10;
    puts("bx-ntvdm COMMAND CMDINFO delivery, environment retry and terminal lifecycles verified");
    return 0;
}

#include "bx_ntvdm_cmd_get_next_service.h"

#include <string.h>

#define BX_NTVDM_CMD_APERTURE UINT64_C(0x100000)

static int match(const bx_ntvdm_exception_event_v1 *e, const bx_ntvdm_cpu_state_v1 *c,
    const bx_ntvdm_instruction_window_v1 *w)
{
    return e && c && w && bx_ntvdm_exception_event_v1_valid(e) &&
        bx_ntvdm_cpu_state_v1_valid(c) && bx_ntvdm_instruction_window_v1_valid(w) &&
        e->vector == 6u && c->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        w->valid_bytes >= 4u && w->bytes[0] == 0xc4u && w->bytes[1] == 0xc4u &&
        w->bytes[2] == 0x54u && w->bytes[3] == 0x01u && e->fault_rip <= UINT64_MAX - 4u;
}
static int physical(uint16_t seg, uint16_t off, uint64_t bytes, uint64_t *out)
{
    uint64_t address = ((uint64_t)seg << 4) + off;
    if (!out || bytes == 0u || address > BX_NTVDM_CMD_APERTURE - bytes) return 0;
    *out = address; return 1;
}
static int put(bx_ntvdm_multi_write_transaction_v1 *t, uint8_t *p, uint32_t *used,
    uint64_t address, const uint8_t *data, uint32_t bytes)
{
    if (!t || !p || !used || !data || bytes == 0u || *used > BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD - bytes)
        return 0;
    memcpy(p + *used, data, bytes);
    if (!bx_ntvdm_multi_write_v1_add(&t->writes, address, bytes, *used)) return 0;
    *used += bytes; return 1;
}
static void word(uint8_t out[2], uint16_t value) { out[0] = (uint8_t)value; out[1] = (uint8_t)(value >> 8); }
static int path(const bx_ntvdm_readonly_namespace_v1 *ns, const byob_launch_declaration_v1 *launch,
    uint32_t slot, uint8_t out[16], uint32_t *out_bytes, uint16_t *out_extension)
{
    const wchar_t *name;
    uint32_t used = 0u;
    if (!ns || !launch || !out || !out_bytes || !out_extension || ns->drive_index >= 26u ||
        slot >= 2u || ns->file_count != 5u || ns->files[3u + slot].bytes == 0 ||
        ns->files[3u + slot].byte_count == 0u) return 0;
    name = slot == 0u ? (launch->target_kind == BYOB_LAUNCH_TARGET_KIND_V1_COM ? L"TARGET.COM" :
        launch->target_kind == BYOB_LAUNCH_TARGET_KIND_V1_EXE ? L"TARGET.EXE" : 0) : L"QUIT.COM";
    if (!name || wcscmp(ns->files[3u + slot].path, slot == 1u ? L"\\QUIT.COM" :
        (launch->target_kind == BYOB_LAUNCH_TARGET_KIND_V1_COM ? L"\\TARGET.COM" : L"\\TARGET.EXE")) != 0) return 0;
    out[used++] = (uint8_t)('A' + ns->drive_index); out[used++] = ':'; out[used++] = '\\';
    while (*name) { if (*name > 0x7f || used >= 15u) return 0; out[used++] = (uint8_t)*name++; }
    out[used++] = 0;
    *out_bytes = used;
    *out_extension = (slot == 1u || launch->target_kind == BYOB_LAUNCH_TARGET_KIND_V1_COM) ? 8u : 4u;
    return 1;
}
static uint16_t drive_count(const bx_ntvdm_host_drive_snapshot_v1 *drives)
{
    uint32_t index, count = 0u;
    if (!drives || !bx_ntvdm_host_drive_snapshot_v1_valid(drives)) return 0u;
    for (index = 0u; index < 26u; ++index) if ((drives->admitted_mask & (UINT32_C(1) << index)) != 0u) ++count;
    return (uint16_t)count;
}
void bx_ntvdm_cmd_get_next_state_v1_initialize(bx_ntvdm_cmd_get_next_state_v1 *value)
{ if (value) memset(value, 0, sizeof(*value)); }
void bx_ntvdm_cmd_get_next_state_v1_commit(bx_ntvdm_cmd_get_next_state_v1 *state)
{ if (state) state->delivered = 1u; }
int bx_ntvdm_cmd_return_exit_code_v1_dispatch(bx_ntvdm_cmd_get_next_state_v1 *state,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_cpu_result_v2 *result)
{
    if (!state || !event || !cpu || !window || !result || state->delivered != 1u ||
        state->returned != 0u || state->reserved0 != 0u ||
        !bx_ntvdm_exception_event_v1_valid(event) || !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        !bx_ntvdm_instruction_window_v1_valid(window) ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION || event->vector != 6u ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL || event->fault_rip > UINT64_MAX - 4u ||
        window->valid_bytes < 4u || window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u ||
        window->bytes[2] != 0x54u || window->bytes[3] != 0x11u ||
        !bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) ||
        !bx_ntvdm_cpu_result_v2_set_cf(result, 0)) return 0;
    state->terminal_dos_exit_code = (uint16_t)cpu->edx;
    state->returned = 1u;
    return 1;
}
int bx_ntvdm_cmd_get_next_v1_prepare(const bx_ntvdm_cmd_get_next_state_v1 *state,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_guest_gather_read_action_v1 *action)
{
    bx_ntvdm_guest_range range; uint64_t address;
    if (!state || state->delivered >= 2u || !action || !match(event, cpu, window) ||
        !physical(cpu->ds, (uint16_t)cpu->edx, BX_NTVDM_CMDINFO_V1_BYTES, &address)) return 0;
    range.address = address; range.length = BX_NTVDM_CMDINFO_V1_BYTES;
    return bx_ntvdm_guest_gather_read_action_v1_need_read_resume(action, &range, 1u, event->fault_rip + 4u);
}
int bx_ntvdm_cmd_get_next_v1_complete(const bx_ntvdm_readonly_namespace_v1 *ns,
    const byob_launch_plan_v2 *plan, const bx_ntvdm_host_drive_snapshot_v1 *drives,
    const bx_ntvdm_cmd_set_info_registration_v1 *reg, const bx_ntvdm_cmd_get_next_state_v1 *state,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_gather_read_action_v1 *action, const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *t, uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    byob_launch_declaration_v1 launch; bx_ntvdm_cmdinfo_v1 info; uint8_t executable[16], command[130], two[2], zero2[2] = {0,0}, zero4[4] = {0,0,0,0};
    uint32_t executable_bytes, command_count, command_bytes, used = 0u; uint16_t extension, count; uint64_t address;
    if (!ns || !plan || !drives || !reg || !state || !event || !cpu || !action || !bytes || !t || !payload ||
        state->delivered >= 2u || plan->slot_count != 2u || plan->version != 2u) return 0;
    launch = plan->first;
    if (state->delivered == 1u) { memset(&launch, 0, sizeof(launch)); launch.version = 1u; launch.target_kind = BYOB_LAUNCH_TARGET_KIND_V1_COM; }
    if (launch.tail_bytes > 118u || !path(ns, &launch, state->delivered, executable, &executable_bytes, &extension) ||
        !bx_ntvdm_cmdinfo_v1_decode(bytes, (uint32_t)byte_count, &info) || byte_count != BX_NTVDM_CMDINFO_V1_BYTES ||
        info.command_bytes != 128u || info.executable_bytes != BX_NTVDM_CMDINFO_V1_EXECPATH_BYTES ||
        !physical(info.command_segment, info.command_offset, 130u, &address)) return 0;
    command_count = 6u + (launch.tail_bytes ? 1u : 0u) + launch.tail_bytes + 2u;
    if (command_count > 127u) return 0;
    command[0] = (uint8_t)command_count; memcpy(command + 1u, "TARGET", 6u); command_bytes = 7u;
    if (launch.tail_bytes) { command[command_bytes++] = ' '; memcpy(command + command_bytes, launch.tail, launch.tail_bytes); command_bytes += launch.tail_bytes; }
    command[command_bytes++] = '\r'; command[command_bytes++] = '\n'; command[command_bytes++] = 0;
    bx_ntvdm_multi_write_transaction_v1_initialize(t, event, cpu);
    if (!put(t,payload,&used,address + 1u,command,command_bytes) ||
        !physical(info.executable_segment, info.executable_offset, executable_bytes, &address) || !put(t,payload,&used,address,executable,executable_bytes) ||
        !physical(cpu->ds,(uint16_t)cpu->edx + 4u,2u,&address)) return 0;
    word(two,(uint16_t)ns->drive_index); if (!put(t,payload,&used,address,two,2u) || !physical(cpu->ds,(uint16_t)cpu->edx + 6u,2u,&address)) return 0;
    count = drive_count(drives); word(two,count); if (!put(t,payload,&used,address,two,2u) || !physical(cpu->ds,(uint16_t)cpu->edx + 16u,2u,&address) || !put(t,payload,&used,address,zero2,2u) || !physical(cpu->ds,(uint16_t)cpu->edx + 18u,4u,&address) || !put(t,payload,&used,address,zero4,4u) || !physical(cpu->ds,(uint16_t)cpu->edx + 22u,2u,&address) || !put(t,payload,&used,address,zero2,2u) || !physical(cpu->ds,(uint16_t)cpu->edx + 26u,2u,&address) || !put(t,payload,&used,address,zero2,2u)) return 0;
    word(two,(uint16_t)(executable_bytes - 1u)); if (!physical(cpu->ds,(uint16_t)cpu->edx + 32u,2u,&address) || !put(t,payload,&used,address,two,2u)) return 0;
    word(two,extension); if (!physical(cpu->ds,(uint16_t)cpu->edx + 34u,2u,&address) || !put(t,payload,&used,address,two,2u) || !put(t,payload,&used,reg->scs_to_sync,(const uint8_t[]){0xffu},1u) || !put(t,payload,&used,reg->is_dos_binary,(const uint8_t[]){1u},1u) || !bx_ntvdm_cpu_result_v2_resume(&t->result,event->fault_rip+4u) || !bx_ntvdm_cpu_result_v2_set_cf(&t->result,0)) return 0;
    return bx_ntvdm_multi_write_transaction_v1_preflight(t,BX_NTVDM_CMD_APERTURE,used);
}

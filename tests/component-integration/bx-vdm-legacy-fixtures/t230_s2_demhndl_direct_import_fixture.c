#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "opennt-bop/dem/opennt_dem_dispatch_composition.h"
#include "opennt-host/dem/demhndl_shim.h"
#include "opennt-bop/ingress/dem_ingress_shim.h"

typedef struct fixture_context {
    HANDLE handle;
    uint32_t token;
    int released;
    uint8_t guest[0x20000];
} fixture_context;

static int publish_handle(void *state, HANDLE handle, uint32_t *token_out,
    DWORD *error_out)
{
    fixture_context *context = (fixture_context *)state;
    if (token_out != 0) *token_out = 0u;
    if (error_out != 0) *error_out = ERROR_INVALID_HANDLE;
    if (context == 0 || context->handle != handle || context->released) return 0;
    if (token_out != 0) *token_out = context->token;
    if (error_out != 0) *error_out = ERROR_SUCCESS;
    return 1;
}

static int lookup_handle(void *state, uint32_t token, HANDLE *handle_out)
{
    fixture_context *context = (fixture_context *)state;
    if (handle_out != 0) *handle_out = INVALID_HANDLE_VALUE;
    if (context == 0 || token != context->token || context->released ||
        context->handle == INVALID_HANDLE_VALUE) return 0;
    *handle_out = context->handle;
    return 1;
}

static int release_handle(void *state, uint32_t token, DWORD *error_out)
{
    fixture_context *context = (fixture_context *)state;
    if (error_out != 0) *error_out = ERROR_INVALID_HANDLE;
    if (context == 0 || token != context->token || context->released ||
        !CloseHandle(context->handle)) return 0;
    context->handle = INVALID_HANDLE_VALUE;
    context->released = 1;
    if (error_out != 0) *error_out = ERROR_SUCCESS;
    return 1;
}

static int query_attributes(void *state, uint8_t drive, const wchar_t *path,
    DWORD *attributes_out, DWORD *error_out)
{
    (void)state; (void)drive; (void)path;
    if (attributes_out != 0) *attributes_out = FILE_ATTRIBUTE_NORMAL;
    if (error_out != 0) *error_out = ERROR_SUCCESS;
    return 1;
}

static int set_attributes(void *state, uint8_t drive, const wchar_t *path,
    DWORD attributes, DWORD *error_out)
{
    (void)state; (void)drive; (void)path; (void)attributes;
    if (error_out != 0) *error_out = ERROR_SUCCESS;
    return 1;
}

static int guest_read(void *state, uint32_t address, uint8_t *buffer,
    uint32_t bytes)
{
    fixture_context *context = (fixture_context *)state;
    if (context == 0 || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(buffer, context->guest + address, bytes);
    return 1;
}

static int guest_write(void *state, uint32_t address, const uint8_t *buffer,
    uint32_t bytes)
{
    fixture_context *context = (fixture_context *)state;
    if (context == 0 || address > sizeof(context->guest) ||
        bytes > sizeof(context->guest) - address) return 0;
    memcpy(context->guest + address, buffer, bytes);
    return 1;
}

static void event_initialize(bx_ntvdm_exception_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->fault_rip = 0x1000u;
}

static void direct_initialize(bx_ntvdm_dem_direct_context *direct,
    fixture_context *state)
{
    memset(direct, 0, sizeof(*direct));
    direct->magic = BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC;
    direct->abi_version = BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION;
    direct->struct_bytes = sizeof(*direct);
    direct->state = state;
    direct->publish_handle = publish_handle;
    direct->lookup_handle = lookup_handle;
    direct->release_handle = release_handle;
    direct->query_attributes = query_attributes;
    direct->set_attributes = set_attributes;
}

static int invoke(fixture_context *state, bx_ntvdm_dem_direct_context *direct,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result, uint32_t service)
{
    bx_ntvdm_demhndl_call call;
    memset(&call, 0, sizeof(call));
    call.magic = BX_NTVDM_DEMHNDL_CALL_MAGIC;
    call.abi_version = BX_NTVDM_DEMHNDL_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = service;
    call.direct = direct;
    call.boundary = event;
    call.cpu = cpu;
    call.result = result;
    call.guest_state = state;
    call.guest_read = guest_read;
    call.guest_write = guest_write;
    return bx_ntvdm_demdisp_invoke(&call) &&
        result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == event->fault_rip + 4u;
}

static int invoke_dispatch_noop(fixture_context *state,
    bx_ntvdm_dem_direct_context *direct, bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_state_v1 *cpu, bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_demhndl_call call;
    memset(&call, 0, sizeof(call));
    call.magic = BX_NTVDM_DEMHNDL_CALL_MAGIC;
    call.abi_version = BX_NTVDM_DEMHNDL_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = 0x1fu; /* original demNotYetImplemented table slot */
    call.direct = direct;
    call.boundary = event;
    call.cpu = cpu;
    call.result = result;
    call.guest_state = state;
    call.guest_read = guest_read;
    call.guest_write = guest_write;
    return bx_ntvdm_demdisp_invoke(&call) && CurrentISVC == call.service &&
        result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == event->fault_rip + 4u &&
        (result->eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u;
}

static int invoke_fast_read(fixture_context *state,
    bx_ntvdm_dem_direct_context *direct, bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_state_v1 *cpu, bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_demhndl_call call;
    bx_ntvdm_instruction_window_v1 window;
    static const uint8_t instruction[] = {0xc4u, 0xc4u, 0x50u, 0x42u};
    memset(&call, 0, sizeof(call));
    call.magic = BX_NTVDM_DEMHNDL_CALL_MAGIC;
    call.abi_version = BX_NTVDM_DEMHNDL_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = 0x42u;
    call.direct = direct;
    call.boundary = event;
    call.cpu = cpu;
    call.result = result;
    call.guest_state = state;
    call.guest_read = guest_read;
    call.guest_write = guest_write;
    bx_ntvdm_instruction_window_v1_capture(&window, instruction,
        (uint32_t)sizeof(instruction));
    return bx_ntvdm_dem_ingress_dispatch(&window, &call) &&
        result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == event->fault_rip + 4u;
}

int main(void)
{
    wchar_t path[MAX_PATH], directory[MAX_PATH];
    bx_ntvdm_dem_direct_context direct;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    fixture_context state;
    uint32_t address = 0x1000u;

    if (GetTempPathW(MAX_PATH, directory) == 0u ||
        GetTempFileNameW(directory, L"dhs", 0u, path) == 0u) return 1;
    memset(&state, 0, sizeof(state));
    state.token = 0x00010001u;
    state.handle = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, 0u, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (state.handle == INVALID_HANDLE_VALUE) { DeleteFileW(path); return 2; }
    direct_initialize(&direct, &state);
    event_initialize(&event);
    if (!bx_ntvdm_dem_direct_context_valid(&direct) ||
        bx_ntvdm_demdisp_handler_at(BX_NTVDM_DEMHNDL_READ) != demRead) return 3;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eflags = BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF;
    if (!invoke_dispatch_noop(&state, &direct, &event, &cpu, &result)) return 4;
    cpu.eax = 1u; cpu.ebp = 1u; cpu.ds = 0x100u; cpu.edx = 0u;
    memcpy(state.guest + address, "hello", 5u);
    cpu.ecx = 5u; cpu.eflags = 0x40u;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_WRITE) || result.cpu_delta.gpr16_values[0] != 5u ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u) return 5;

    cpu.ebx = 0u; cpu.esi = 0u; cpu.ecx = 0u; cpu.edx = 0u; cpu.eflags = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_CHG_FILE_PTR) || result.cpu_delta.gpr16_values[0] != 0u)
        return 5;
    memset(state.guest + address, 0, 5u);
    cpu.ecx = 5u; cpu.eflags = 0x40u;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_READ) || result.cpu_delta.gpr16_values[0] != 5u ||
        memcmp(state.guest + address, "hello", 5u) != 0) return 6;

    /* The historical x86 caller uses 50:42, whose visible dispatcher entry
     * is a no-op.  The admitted exact-selector seam must instead run the
     * imported demRead body, copy bytes, return their actual count and resume. */
    cpu.ebx = 0u; cpu.esi = 0u; cpu.ecx = 0u; cpu.edx = 0u; cpu.eflags = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_CHG_FILE_PTR) || result.cpu_delta.gpr16_values[0] != 0u)
        return 14;
    memset(state.guest + address, 0, 5u);
    cpu.ecx = 5u; cpu.eflags = 0x40u;
    if (!invoke_fast_read(&state, &direct, &event, &cpu, &result) ||
        result.cpu_delta.gpr16_values[0] != 5u ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        memcmp(state.guest + address, "hello", 5u) != 0) return 15;

    cpu.ebx = 0u; cpu.eflags = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_FILE_TIMES) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u) return 7;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_COMMIT)) return 8;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_PIPE_DATA_EOF) ||
        (result.eflags_write_mask & BX_NTVDM_CPU_RESULT_V2_EFLAGS_ZF) == 0u ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_ZF) != 0u ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u) return 9;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_PIPE_EOF)) return 10;

    /* Original demClientError route: invalid native token must be surfaced as
     * AX=ERROR_INVALID_HANDLE with CF, not a shim-local success. */
    cpu.eax = 1u; cpu.ebp = 2u;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_CHG_FILE_PTR) ||
        result.cpu_delta.gpr16_values[0] != ERROR_INVALID_HANDLE ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u) return 11;

    /* The complete original pair is a same-width opaque ID. An unknown
     * high-word value follows the original invalid-handle failure path. */
    cpu.eax = 2u; cpu.ebp = 1u;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_CHG_FILE_PTR) ||
        result.cpu_delta.gpr16_values[0] != ERROR_INVALID_HANDLE ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u) return 12;

    cpu.eax = 1u;
    cpu.ebp = 1u;
    cpu.ecx = 0xffffu; cpu.edx = 0xffffu;
    if (!invoke(&state, &direct, &event, &cpu, &result,
            BX_NTVDM_DEMHNDL_CLOSE) || !state.released) return 13;
    DeleteFileW(path);
    puts("T230 S2 direct OpenNT demhndl import: handle, guest-memory and result seam verified");
    return 0;
}

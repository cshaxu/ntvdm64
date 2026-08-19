#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "bop/opennt/dem/demdisp.h"
#include "bop/opennt/dem/demfile.h"
#include "bop/opennt/dem/demhndl.h"

typedef struct fixture_context {
    HANDLE handle;
    uint32_t token;
    int released;
} fixture_context;

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
    DWORD value;
    (void)state; (void)drive;
    value = GetFileAttributesW(path);
    if (value == INVALID_FILE_ATTRIBUTES) {
        if (error_out != 0) *error_out = GetLastError();
        return 0;
    }
    *attributes_out = value;
    if (error_out != 0) *error_out = ERROR_SUCCESS;
    return 1;
}

static int set_attributes(void *state, uint8_t drive, const wchar_t *path,
    DWORD attributes, DWORD *error_out)
{
    (void)state; (void)drive;
    if (!SetFileAttributesW(path, attributes)) {
        if (error_out != 0) *error_out = GetLastError();
        return 0;
    }
    if (error_out != 0) *error_out = ERROR_SUCCESS;
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

static void context_initialize(bx_ntvdm_dem_direct_context *context,
    fixture_context *state)
{
    memset(context, 0, sizeof(*context));
    context->magic = BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC;
    context->abi_version = BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION;
    context->struct_bytes = sizeof(*context);
    context->state = state;
    context->lookup_handle = lookup_handle;
    context->release_handle = release_handle;
    context->query_attributes = query_attributes;
    context->set_attributes = set_attributes;
}

int main(void)
{
    wchar_t temp_path[MAX_PATH];
    wchar_t temp_dir[MAX_PATH];
    bx_ntvdm_dem_direct_context context;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    fixture_context state;
    const bx_ntvdm_opennt_dem_service_entry *entry;

    if (GetTempPathW(MAX_PATH, temp_dir) == 0u ||
        GetTempFileNameW(temp_dir, L"d23", 0u, temp_path) == 0u) return 1;
    memset(&state, 0, sizeof(state));
    state.token = 0x00010001u;
    state.handle = CreateFileW(temp_path, GENERIC_READ | GENERIC_WRITE,
        0u, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (state.handle == INVALID_HANDLE_VALUE) { DeleteFileW(temp_path); return 2; }
    context_initialize(&context, &state);
    event_initialize(&event);
    if (!bx_ntvdm_dem_direct_context_valid(&context) ||
        bx_ntvdm_opennt_dem_service_count() != 73u) return 3;
    entry = bx_ntvdm_opennt_dem_service_at(0x00u);
    if (entry == 0 || strcmp(entry->handler, "demChgFilePtr") != 0 ||
        strcmp(entry->owner_file, "demhndl.c") != 0) return 4;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 1u; cpu.ebp = 1u; cpu.ecx = 0u; cpu.edx = 0u; cpu.ebx = 0u;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_bop_dem_chg_file_ptr_v2(&context, &event, &cpu, &result) ||
        !bx_ntvdm_cpu_result_v2_valid(&result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) return 5;

    cpu.eax = 0u;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_bop_dem_ch_mod_v2(&context, &event, &cpu, 2u, temp_path,
            &result) || !bx_ntvdm_cpu_result_v2_valid(&result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) return 6;

    cpu.eax = 1u; cpu.ebp = 1u; cpu.ecx = 0xffffu; cpu.edx = 0xffffu;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_bop_dem_close_v2(&context, &event, &cpu, &result) ||
        !state.released || result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME)
        return 7;
    DeleteFileW(temp_path);
    return 0;
}

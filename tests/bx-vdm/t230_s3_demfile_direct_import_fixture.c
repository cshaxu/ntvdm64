#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "bop/shim/demdisp_shim.h"
#include "bop/shim/demfile_shim.h"

typedef struct fixture_context {
    HANDLE handles[8];
    uint8_t guest[0x20000];
} fixture_context;

static int publish_handle(void *state, HANDLE handle, uint32_t *token_out,
    DWORD *error_out)
{
    fixture_context *context = (fixture_context *)state;
    uint32_t index;
    if (token_out != NULL) *token_out = 0u;
    if (error_out != NULL) *error_out = ERROR_TOO_MANY_OPEN_FILES;
    if (context == NULL || handle == INVALID_HANDLE_VALUE) return 0;
    for (index = 0u; index < 8u; ++index) if (context->handles[index] == NULL) {
        context->handles[index] = handle;
        if (token_out != NULL) *token_out = index + 1u;
        if (error_out != NULL) *error_out = ERROR_SUCCESS;
        return 1;
    }
    return 0;
}
static int lookup_handle(void *state, uint32_t token, HANDLE *out)
{ fixture_context *c = state; if (out) *out = INVALID_HANDLE_VALUE; if (!c || token == 0u || token > 8u || c->handles[token - 1u] == NULL) return 0; *out = c->handles[token - 1u]; return 1; }
static int release_handle(void *state, uint32_t token, DWORD *error)
{ fixture_context *c = state; if (error) *error = ERROR_INVALID_HANDLE; if (!c || token == 0u || token > 8u || c->handles[token - 1u] == NULL || !CloseHandle(c->handles[token - 1u])) return 0; c->handles[token - 1u] = NULL; if (error) *error = ERROR_SUCCESS; return 1; }
static int attr_get(void *s, uint8_t d, const wchar_t *p, DWORD *a, DWORD *e)
{ (void)s; (void)d; (void)p; if (a) *a = FILE_ATTRIBUTE_NORMAL; if (e) *e = ERROR_SUCCESS; return 1; }
static int attr_set(void *s, uint8_t d, const wchar_t *p, DWORD a, DWORD *e)
{ (void)s; (void)d; (void)p; (void)a; if (e) *e = ERROR_SUCCESS; return 1; }
static int guest_read(void *state, uint32_t address, uint8_t *buffer, uint32_t bytes)
{ fixture_context *c = state; if (!c || address > sizeof(c->guest) || bytes > sizeof(c->guest) - address) return 0; memcpy(buffer, c->guest + address, bytes); return 1; }
static int guest_write(void *state, uint32_t address, const uint8_t *buffer, uint32_t bytes)
{ fixture_context *c = state; if (!c || address > sizeof(c->guest) || bytes > sizeof(c->guest) - address) return 0; memcpy(c->guest + address, buffer, bytes); return 1; }

static void put_path(fixture_context *context, uint32_t address, const char *path)
{ memcpy(context->guest + address, path, strlen(path) + 1u); }
static int carry_set(const bx_ntvdm_cpu_result_v2 *result)
{ return (result->eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u; }
static int invoke(fixture_context *state, bx_ntvdm_dem_direct_context *direct,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result, uint32_t service)
{
    bx_ntvdm_demhndl_call call;
    memset(&call, 0, sizeof(call));
    call.magic = BX_NTVDM_DEMHNDL_CALL_MAGIC; call.abi_version = BX_NTVDM_DEMHNDL_CALL_VERSION;
    call.struct_bytes = sizeof(call); call.service = service; call.direct = direct;
    call.boundary = event; call.cpu = cpu; call.result = result; call.guest_state = state;
    call.guest_read = guest_read; call.guest_write = guest_write;
    return bx_ntvdm_demdisp_invoke(&call) && result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME;
}
static void reset_cpu(bx_ntvdm_cpu_state_v1 *cpu)
{ bx_ntvdm_cpu_state_v1_initialize(cpu, BX_NTVDM_CPU_EXECUTION_REAL); cpu->ds = 0x100u; cpu->es = 0x100u; }
static void close_published(fixture_context *c)
{ uint32_t i; for (i = 0u; i < 8u; ++i) if (c->handles[i] != NULL) { CloseHandle(c->handles[i]); c->handles[i] = NULL; } }

int main(void)
{
    char directory[MAX_PATH], base[MAX_PATH], source[MAX_PATH], destination[MAX_PATH];
    bx_ntvdm_dem_direct_context direct; bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu; bx_ntvdm_cpu_result_v2 result; fixture_context state;
    HANDLE file;
    if (GetTempPathA(MAX_PATH, directory) == 0u || GetTempFileNameA(directory, "dfs", 0u, base) == 0u) return 1;
    DeleteFileA(base); sprintf_s(source, sizeof(source), "%s.src", base); sprintf_s(destination, sizeof(destination), "%s.dst", base);
    memset(&state, 0, sizeof(state)); memset(&direct, 0, sizeof(direct));
    direct.magic = BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC; direct.abi_version = BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION; direct.struct_bytes = sizeof(direct); direct.state = &state;
    direct.publish_handle = publish_handle; direct.lookup_handle = lookup_handle; direct.release_handle = release_handle; direct.query_attributes = attr_get; direct.set_attributes = attr_set;
    memset(&event, 0, sizeof(event)); event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC; event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes = sizeof(event); event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.fault_rip = 0x1000u;
    if (!bx_ntvdm_dem_direct_context_valid(&direct)) return 2;

    file = CreateFileA(base, GENERIC_READ | GENERIC_WRITE, 0u, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); if (file == INVALID_HANDLE_VALUE) return 3; CloseHandle(file);
    put_path(&state, 0x1000u, base); reset_cpu(&cpu); cpu.edx = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result, BX_NTVDM_DEMFILE_CHMOD) || carry_set(&result)) return 4;

    DeleteFileA(base); put_path(&state, 0x1000u, base); reset_cpu(&cpu); cpu.esi = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result, BX_NTVDM_DEMFILE_CREATE) || carry_set(&result)) return 5; close_published(&state);
    put_path(&state, 0x1000u, base); reset_cpu(&cpu); cpu.esi = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result, BX_NTVDM_DEMFILE_OPEN) || carry_set(&result)) return 6; close_published(&state);
    put_path(&state, 0x1000u, base); reset_cpu(&cpu); cpu.edx = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result, BX_NTVDM_DEMFILE_DELETE) || carry_set(&result) || GetFileAttributesA(base) != INVALID_FILE_ATTRIBUTES) return 7;
    put_path(&state, 0x1000u, base); reset_cpu(&cpu); cpu.esi = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result, BX_NTVDM_DEMFILE_CREATE_NEW) || carry_set(&result)) return 8; close_published(&state);

    file = CreateFileA(source, GENERIC_READ | GENERIC_WRITE, 0u, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); if (file == INVALID_HANDLE_VALUE) return 9; CloseHandle(file);
    put_path(&state, 0x1000u, source); put_path(&state, 0x1100u, destination); reset_cpu(&cpu); cpu.edx = 0u; cpu.edi = 0x100u;
    if (!invoke(&state, &direct, &event, &cpu, &result, BX_NTVDM_DEMFILE_RENAME) || carry_set(&result) || GetFileAttributesA(destination) == INVALID_FILE_ATTRIBUTES) return 10;
    put_path(&state, 0x1000u, "\\DEV\\"); reset_cpu(&cpu); cpu.esi = 0u; cpu.edx = 3u;
    if (!invoke(&state, &direct, &event, &cpu, &result, BX_NTVDM_DEMFILE_CHECK_PATH) || carry_set(&result)) return 11;
    put_path(&state, 0x1000u, "?:LABEL"); reset_cpu(&cpu); cpu.esi = 0u; cpu.ecx = ATTR_VOLUME_ID;
    if (!invoke(&state, &direct, &event, &cpu, &result, BX_NTVDM_DEMFILE_CREATE) || !carry_set(&result)) return 12;
    close_published(&state); DeleteFileA(base); DeleteFileA(source); DeleteFileA(destination);
    puts("T230 S3 direct OpenNT demfile/demlabel import: seven file services and safe label failure verified");
    return 0;
}

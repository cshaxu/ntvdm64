#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "opennt-bop-overlay/dem/opennt_dem_dispatch_composition.h"
#include "opennt-bop-overlay/dem/opennt_demfile_composition.h"
#include "adapter-mvdm-host-out/softpc/host_handle_manager.h"

typedef struct fixture_context {
    runtime_host_handle_manager *handles;
    uint8_t guest[0x20000];
} fixture_context;

static int publish_handle(void *state, HANDLE handle, uint32_t *token_out,
    DWORD *error_out)
{
    fixture_context *context = (fixture_context *)state;
    uint32_t guest_handle;
    if (token_out != NULL) *token_out = 0u;
    if (error_out != NULL) *error_out = ERROR_TOO_MANY_OPEN_FILES;
    if (context == NULL || !runtime_host_handle_manager_publish(context->handles,
            handle, RUNTIME_HOST_HANDLE_OWNED, &guest_handle, error_out)) return 0;
    if (token_out != NULL) *token_out = guest_handle;
    return 1;
}
static int lookup_handle(void *state, uint32_t token, HANDLE *out)
{ fixture_context *c = state; if (out) *out = INVALID_HANDLE_VALUE; return c != NULL && token != 0u && token != UINT32_MAX && runtime_host_handle_manager_lookup_handle(c->handles, token, out); }
static int release_handle(void *state, uint32_t token, DWORD *error)
{ fixture_context *c = state; if (error) *error = ERROR_INVALID_HANDLE; return c != NULL && token != 0u && token != UINT32_MAX && runtime_host_handle_manager_release(c->handles, token, error); }
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
static int carry_set(const runtime_cpu_result *result)
{ return (result->eflags_values & RUNTIME_CPU_RESULT_EFLAGS_CF) != 0u; }
static int invoke(fixture_context *state, runtime_dem_direct_context *direct,
    runtime_exception_event *event, runtime_cpu_state *cpu,
    runtime_cpu_result *result, uint32_t service)
{
    runtime_demhndl_call call;
    memset(&call, 0, sizeof(call));
    call.magic = RUNTIME_DEMHNDL_CALL_MAGIC; call.abi_version = RUNTIME_DEMHNDL_CALL_VERSION;
    call.struct_bytes = sizeof(call); call.service = service; call.direct = direct;
    call.boundary = event; call.cpu = cpu; call.result = result; call.guest_state = state;
    call.guest_read = guest_read; call.guest_write = guest_write;
    return runtime_demdisp_invoke(&call) && result->disposition == RUNTIME_CPU_RESULT_RESUME;
}
static void reset_cpu(runtime_cpu_state *cpu)
{ runtime_cpu_state_initialize(cpu, RUNTIME_CPU_EXECUTION_REAL); cpu->ds = 0x100u; cpu->es = 0x100u; }
static void close_published(fixture_context *c)
{ if (c != NULL) runtime_host_handle_manager_reset(c->handles); }

int main(void)
{
    char directory[MAX_PATH], base[MAX_PATH], source[MAX_PATH], destination[MAX_PATH];
    runtime_dem_direct_context direct; runtime_exception_event event;
    runtime_cpu_state cpu; runtime_cpu_result result; fixture_context state;
    HANDLE file;
    if (GetTempPathA(MAX_PATH, directory) == 0u || GetTempFileNameA(directory, "dfs", 0u, base) == 0u) return 1;
    DeleteFileA(base); sprintf_s(source, sizeof(source), "%s.src", base); sprintf_s(destination, sizeof(destination), "%s.dst", base);
    memset(&state, 0, sizeof(state)); memset(&direct, 0, sizeof(direct));
    state.handles = runtime_host_handle_manager_session();
    if (!runtime_host_handle_manager_initialize(state.handles)) return 2;
    direct.magic = RUNTIME_DEM_DIRECT_CONTEXT_MAGIC; direct.abi_version = RUNTIME_DEM_DIRECT_CONTEXT_VERSION; direct.struct_bytes = sizeof(direct); direct.state = &state;
    direct.publish_handle = publish_handle; direct.lookup_handle = lookup_handle; direct.release_handle = release_handle; direct.query_attributes = attr_get; direct.set_attributes = attr_set;
    memset(&event, 0, sizeof(event)); event.magic = RUNTIME_EXCEPTION_ABI_MAGIC; event.abi_version = RUNTIME_EXCEPTION_ABI_VERSION; event.struct_bytes = sizeof(event); event.kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION; event.fault_rip = 0x1000u;
    if (!runtime_dem_direct_context_valid(&direct)) return 2;

    file = CreateFileA(base, GENERIC_READ | GENERIC_WRITE, 0u, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); if (file == INVALID_HANDLE_VALUE) return 3; CloseHandle(file);
    put_path(&state, 0x1000u, base); reset_cpu(&cpu); cpu.edx = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result, RUNTIME_DEMFILE_CHMOD) || carry_set(&result)) return 4;

    DeleteFileA(base); put_path(&state, 0x1000u, base); reset_cpu(&cpu); cpu.esi = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result, RUNTIME_DEMFILE_CREATE) || carry_set(&result)) return 5; close_published(&state);
    put_path(&state, 0x1000u, base); reset_cpu(&cpu); cpu.esi = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result, RUNTIME_DEMFILE_OPEN) || carry_set(&result)) return 6; close_published(&state);
    put_path(&state, 0x1000u, base); reset_cpu(&cpu); cpu.edx = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result, RUNTIME_DEMFILE_DELETE) || carry_set(&result) || GetFileAttributesA(base) != INVALID_FILE_ATTRIBUTES) return 7;
    put_path(&state, 0x1000u, base); reset_cpu(&cpu); cpu.esi = 0u;
    if (!invoke(&state, &direct, &event, &cpu, &result, RUNTIME_DEMFILE_CREATE_NEW) || carry_set(&result)) return 8; close_published(&state);

    file = CreateFileA(source, GENERIC_READ | GENERIC_WRITE, 0u, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); if (file == INVALID_HANDLE_VALUE) return 9; CloseHandle(file);
    put_path(&state, 0x1000u, source); put_path(&state, 0x1100u, destination); reset_cpu(&cpu); cpu.edx = 0u; cpu.edi = 0x100u;
    if (!invoke(&state, &direct, &event, &cpu, &result, RUNTIME_DEMFILE_RENAME) || carry_set(&result) || GetFileAttributesA(destination) == INVALID_FILE_ATTRIBUTES) return 10;
    put_path(&state, 0x1000u, "\\DEV\\"); reset_cpu(&cpu); cpu.esi = 0u; cpu.edx = 3u;
    if (!invoke(&state, &direct, &event, &cpu, &result, RUNTIME_DEMFILE_CHECK_PATH) || carry_set(&result)) return 11;
    put_path(&state, 0x1000u, "?:LABEL"); reset_cpu(&cpu); cpu.esi = 0u; cpu.ecx = ATTR_VOLUME_ID;
    if (!invoke(&state, &direct, &event, &cpu, &result, RUNTIME_DEMFILE_CREATE) || !carry_set(&result)) return 12;
    close_published(&state); DeleteFileA(base); DeleteFileA(source); DeleteFileA(destination);
    puts("T230 S3 direct OpenNT demfile/demlabel import: seven file services and safe label failure verified");
    return 0;
}

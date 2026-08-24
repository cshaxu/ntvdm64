/* Focused Direct witness for the directly imported OpenNT demmisc.c owner.
 * It invokes the original bodies through the neutral per-call seam; it does
 * not route a BOP selector or claim native guest integration. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opennt-host/dem/demdisp_shim.h"
#include "opennt-host/dem/demmisc_shim.h"

typedef struct fixture_context {
    uint8_t guest[0x40000];
} fixture_context;

static int publish(void *state, HANDLE file, uint32_t *token, DWORD *error)
{ (void)state; (void)file; if (token != NULL) *token = 1u; if (error != NULL) *error = ERROR_SUCCESS; return 1; }
static int lookup(void *state, uint32_t token, HANDLE *file)
{ (void)state; (void)token; if (file != NULL) *file = INVALID_HANDLE_VALUE; return 0; }
static int release(void *state, uint32_t token, DWORD *error)
{ (void)state; (void)token; if (error != NULL) *error = ERROR_INVALID_HANDLE; return 0; }
static int attr_get(void *state, uint8_t drive, const wchar_t *path, DWORD *attributes, DWORD *error)
{ (void)state; (void)drive; (void)path; if (attributes != NULL) *attributes = FILE_ATTRIBUTE_NORMAL; if (error != NULL) *error = ERROR_SUCCESS; return 1; }
static int attr_set(void *state, uint8_t drive, const wchar_t *path, DWORD attributes, DWORD *error)
{ (void)state; (void)drive; (void)path; (void)attributes; if (error != NULL) *error = ERROR_SUCCESS; return 1; }
static int read_guest(void *state, uint32_t address, uint8_t *buffer, uint32_t bytes)
{ fixture_context *c = (fixture_context *)state; if (c == NULL || address > sizeof(c->guest) || bytes > sizeof(c->guest) - address) return 0; memcpy(buffer, c->guest + address, bytes); return 1; }
static int write_guest(void *state, uint32_t address, const uint8_t *buffer, uint32_t bytes)
{ fixture_context *c = (fixture_context *)state; if (c == NULL || address > sizeof(c->guest) || bytes > sizeof(c->guest) - address) return 0; memcpy(c->guest + address, buffer, bytes); return 1; }

static void reset_cpu(bx_ntvdm_cpu_state_v1 *cpu)
{ bx_ntvdm_cpu_state_v1_initialize(cpu, BX_NTVDM_CPU_EXECUTION_REAL); cpu->ds = 0x100u; cpu->es = 0x100u; }

static int invoke(fixture_context *state, bx_ntvdm_dem_direct_context *direct,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result, uint32_t service)
{
    bx_ntvdm_demhndl_call call;
    memset(&call, 0, sizeof(call));
    call.magic = BX_NTVDM_DEMHNDL_CALL_MAGIC;
    call.abi_version = BX_NTVDM_DEMHNDL_CALL_VERSION;
    call.struct_bytes = sizeof(call); call.service = service; call.direct = direct;
    call.boundary = event; call.cpu = cpu; call.result = result;
    call.guest_state = state; call.guest_read = read_guest; call.guest_write = write_guest;
    return bx_ntvdm_demdisp_invoke(&call) && bx_ntvdm_cpu_result_v2_valid(result);
}

int main(void)
{
    fixture_context state; bx_ntvdm_dem_direct_context direct;
    bx_ntvdm_exception_event_v1 event; bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result; char temp[MAX_PATH], directory[MAX_PATH];
    char dos_path[MAX_PATH + 14]; uint8_t payload[20000]; HANDLE file;
    DWORD written; WORD fd_access = 0x55aau; size_t index;

    memset(&state, 0, sizeof(state)); memset(&direct, 0, sizeof(direct));
    direct.magic = BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC;
    direct.abi_version = BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION;
    direct.struct_bytes = sizeof(direct); direct.state = &state;
    direct.publish_handle = publish; direct.lookup_handle = lookup; direct.release_handle = release;
    direct.query_attributes = attr_get; direct.set_attributes = attr_set;
    memset(&event, 0, sizeof(event)); event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.fault_rip = 0x1000u;
    if (!bx_ntvdm_dem_direct_context_valid(&direct) || !GetTempPathA(MAX_PATH, temp) ||
        !GetTempFileNameA(temp, "s9", 0u, directory) || !DeleteFileA(directory) ||
        !CreateDirectoryA(directory, NULL) ||
        sprintf_s(dos_path, sizeof(dos_path), "%s\\ntdos.sys", directory) < 0) return 1;
    for (index = 0u; index < sizeof(payload); ++index) payload[index] = (uint8_t)(index * 13u);
    file = CreateFileA(dos_path, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !WriteFile(file, payload, sizeof(payload), &written, NULL) || written != sizeof(payload)) return 2;
    CloseHandle(file);

    /* Original demLoadDos appends its file name, reads in 16 KiB blocks and
     * frees this process-owned default DOS directory in the non-debug case. */
    pszDefaultDOSDirectory = (char *)malloc(MAX_PATH + 14u);
    if (pszDefaultDOSDirectory == NULL) return 3;
    strcpy_s(pszDefaultDOSDirectory, MAX_PATH + 14u, directory);
    reset_cpu(&cpu); cpu.edi = 0x2000u;
    if (!invoke(&state, &direct, &event, &cpu, &result, 0x11u) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        memcmp(state.guest + 0x20000u, payload, sizeof(payload)) != 0) return 4;
    pszDefaultDOSDirectory = NULL; /* Original body freed it but cannot clear this global. */

    bx_ntvdm_demmisc_bind_fd_access(&fd_access);
    reset_cpu(&cpu);
    if (!invoke(&state, &direct, &event, &cpu, &result, 0x23u) || fd_access != 0u) return 5;

    /* The symbol and diagnostic service bodies retain their original debug
     * gate.  This normal Direct process is not a debuggee, so all must resume
     * without substituting an invented provider. */
    IsFirstCall = TRUE; VDMForWOW = FALSE; fShowSVCMsg = 0u;
    memcpy(state.guest + 0x1000u, "C:\\APP.EXE", sizeof("C:\\APP.EXE"));
    reset_cpu(&cpu); if (!invoke(&state, &direct, &event, &cpu, &result, 0x34u)) return 6;
    reset_cpu(&cpu); if (!invoke(&state, &direct, &event, &cpu, &result, 0x35u)) return 7;
    reset_cpu(&cpu); if (!invoke(&state, &direct, &event, &cpu, &result, 0x36u)) return 8;
    reset_cpu(&cpu); if (!invoke(&state, &direct, &event, &cpu, &result, 0x37u)) return 9;
    reset_cpu(&cpu); if (!invoke(&state, &direct, &event, &cpu, &result, 0x38u)) return 10;
    reset_cpu(&cpu); if (!invoke(&state, &direct, &event, &cpu, &result, 0x39u)) return 11;
    reset_cpu(&cpu); if (!invoke(&state, &direct, &event, &cpu, &result, 0x3au)) return 12;
    reset_cpu(&cpu); if (!invoke(&state, &direct, &event, &cpu, &result, 0x3bu) ||
        (result.cpu_delta.gpr16_values[0] & 0xffu) != 0u) return 13;
    reset_cpu(&cpu); if (!invoke(&state, &direct, &event, &cpu, &result, 0x3eu) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) return 14;
    VDMForWOW = TRUE; reset_cpu(&cpu);
    if (!invoke(&state, &direct, &event, &cpu, &result, 0x3eu) ||
        (result.cpu_delta.gpr16_values[0] & 0xffu) != 255u) return 15;
    reset_cpu(&cpu); if (!invoke(&state, &direct, &event, &cpu, &result, 0x45u)) return 16;

    /* Original demExitVDM reports then terminates.  The shim expresses that
     * non-returning lifecycle effect through the typed controlled-stop ABI. */
    bx_ntvdm_opennt_error_dialog_fixture_suppress(TRUE);
    reset_cpu(&cpu);
    if (!invoke(&state, &direct, &event, &cpu, &result, 0x3du) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_STOP ||
        bx_ntvdm_opennt_error_dialog_fixture_last_error() != ED_BADSYSFILE ||
        bx_ntvdm_opennt_error_dialog_fixture_count() == 0u) return 17;
    bx_ntvdm_opennt_error_dialog_fixture_suppress(FALSE);
    VDMForWOW = FALSE; bx_ntvdm_demmisc_bind_fd_access(NULL);
    DeleteFileA(dos_path); RemoveDirectoryA(directory);
    puts("T230 S9 direct OpenNT demmisc import: all lifecycle, loader and diagnostic service bodies verified");
    return 0;
}

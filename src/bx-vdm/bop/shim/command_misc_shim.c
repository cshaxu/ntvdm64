#include "command_misc_shim.h"

#include <stdlib.h>
#include <string.h>

void cmdComSpec(void);
void cmdSaveWorld(void);
void cmdGetCurrentDir(void);
void cmdSetInfo(void);
void cmdGetKbdLayout(void);
void cmdGetStdHandle(void);

CHAR lpszComSpec[64 + 8];
USHORT cbComSpec;
BOOL IsFirstCall;
BOOL VDMForWOW;
PSCSINFO pSCSInfo;
PCHAR pSCS_ToSync;
BYTE *pIsDosBinary;
WORD *pFDAccess;
BOOL bPifFastPaste;

typedef struct bx_ntvdm_command_misc_active_call {
    bx_ntvdm_command_misc_call *call;
    uint8_t *guest_buffer;
    uint32_t guest_address;
    uint32_t guest_bytes;
    uint32_t write_back;
} bx_ntvdm_command_misc_active_call;

static __declspec(thread) bx_ntvdm_command_misc_active_call *g_active_call;

static uint32_t real_mode_address(USHORT segment, USHORT offset)
{
    return ((uint32_t)segment << 4) + (uint32_t)offset;
}

static int set_ax(USHORT value)
{
    return g_active_call != NULL &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&g_active_call->call->result->cpu_delta,
            0u, value);
}

void bx_ntvdm_command_misc_session_initialize(bx_ntvdm_command_misc_session *session)
{
    if (session == NULL) return;
    memset(session, 0, sizeof(*session));
    session->magic = BX_NTVDM_COMMAND_MISC_SESSION_MAGIC;
    session->abi_version = BX_NTVDM_COMMAND_MISC_SESSION_VERSION;
    session->struct_bytes = sizeof(*session);
}

int bx_ntvdm_command_misc_session_valid(const bx_ntvdm_command_misc_session *session)
{
    return session != NULL && session->magic == BX_NTVDM_COMMAND_MISC_SESSION_MAGIC &&
        session->abi_version == BX_NTVDM_COMMAND_MISC_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session);
}

static int validate_comspec_input(const bx_ntvdm_command_misc_call *call)
{
    uint8_t value;
    uint32_t address;
    uint32_t index;
    if (call == NULL) return 0;
    address = real_mode_address(call->cpu->ds, (USHORT)call->cpu->edx);
    if (address > 0x100000u - (BX_NTVDM_COMMAND_MISC_COMSPEC_MAX + 1u))
        return 0;
    for (index = 0u; index <= BX_NTVDM_COMMAND_MISC_COMSPEC_MAX; ++index) {
        if (!call->guest_read(call->guest_state, address + index, &value, 1u)) return 0;
        if (value == 0u) return 1;
    }
    return 0;
}

int bx_ntvdm_command_misc_call_valid(const bx_ntvdm_command_misc_call *call)
{
    return call != NULL && call->magic == BX_NTVDM_COMMAND_MISC_CALL_MAGIC &&
        call->abi_version == BX_NTVDM_COMMAND_MISC_CALL_VERSION &&
        call->struct_bytes == sizeof(*call) &&
        (call->service == BX_NTVDM_COMMAND_MISC_COMSPEC ||
         call->service == BX_NTVDM_COMMAND_MISC_SAVE_WORLD ||
         call->service == BX_NTVDM_COMMAND_MISC_GET_CURRENT_DIR ||
         call->service == BX_NTVDM_COMMAND_MISC_SET_INFO ||
         call->service == BX_NTVDM_COMMAND_MISC_INIT_CONSOLE ||
         call->service == BX_NTVDM_COMMAND_MISC_GET_KBD_LAYOUT ||
         call->service == 0x06u) &&
        call->boundary != NULL && bx_ntvdm_exception_event_v1_valid(call->boundary) &&
        call->cpu != NULL && bx_ntvdm_cpu_state_v1_valid(call->cpu) &&
        call->cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        call->result != NULL && call->guest_read != NULL && call->guest_write != NULL &&
        (call->service != BX_NTVDM_COMMAND_MISC_SET_INFO ||
         bx_ntvdm_command_misc_session_valid(call->session));
}

USHORT bx_ntvdm_command_misc_get_dx(void) { return (USHORT)g_active_call->call->cpu->edx; }
USHORT bx_ntvdm_command_misc_get_bx(void) { return (USHORT)g_active_call->call->cpu->ebx; }
USHORT bx_ntvdm_command_misc_get_cx(void) { return (USHORT)g_active_call->call->cpu->ecx; }
USHORT bx_ntvdm_command_misc_get_si(void) { return (USHORT)g_active_call->call->cpu->esi; }
USHORT bx_ntvdm_command_misc_get_ds(void) { return g_active_call->call->cpu->ds; }
USHORT bx_ntvdm_command_misc_get_ax(void) { return (USHORT)g_active_call->call->cpu->eax; }
UCHAR bx_ntvdm_command_misc_get_al(void) { return (UCHAR)(g_active_call->call->cpu->eax & 0xffu); }
void bx_ntvdm_command_misc_set_ax(USHORT value) { (void)set_ax(value); }
void bx_ntvdm_command_misc_set_al(USHORT value)
{ bx_ntvdm_command_misc_set_ax((USHORT)((bx_ntvdm_command_misc_get_ax() & 0xff00u) | (value & 0xffu))); }
void bx_ntvdm_command_misc_set_cf(int value)
{ (void)bx_ntvdm_cpu_result_v2_set_cf(g_active_call->call->result, value); }
void bx_ntvdm_command_misc_set_dx(USHORT value)
{ (void)bx_ntvdm_cpu_delta_v1_set_gpr16(&g_active_call->call->result->cpu_delta, 2u, value); }
void bx_ntvdm_command_misc_set_bx(USHORT value)
{ (void)bx_ntvdm_cpu_delta_v1_set_gpr16(&g_active_call->call->result->cpu_delta, 3u, value); }
void bx_ntvdm_command_misc_set_cx(USHORT value)
{ (void)bx_ntvdm_cpu_delta_v1_set_gpr16(&g_active_call->call->result->cpu_delta, 1u, value); }

PREDIRCOMPLETE_INFO bx_ntvdm_command_misc_redirection_from_guest(uint32_t token)
{
    bx_ntvdm_command_misc_session *session;
    if (g_active_call == NULL || (session = g_active_call->call->session) == NULL ||
        token == 0u || token != session->redirection_token) return NULL;
    return &session->redirection_info;
}

int bx_ntvdm_command_misc_publish_handle(HANDLE handle)
{
    bx_ntvdm_command_misc_session *session;
    uint32_t index;
    if (g_active_call == NULL || (session = g_active_call->call->session) == NULL ||
        handle == NULL || handle == INVALID_HANDLE_VALUE) return 0;
    for (index = 0u; index < 64u; ++index) {
        if (session->handle_tokens[index] == handle) break;
        if (session->handle_tokens[index] == NULL) { session->handle_tokens[index] = handle; break; }
    }
    if (index == 64u) return 0;
    /* The original guest ABI is BX:CX. Preserve it as a fixed-width token,
     * never as a truncated host HANDLE. */
    bx_ntvdm_command_misc_set_cx((USHORT)(index + 1u));
    bx_ntvdm_command_misc_set_bx(0u);
    return 1;
}

BOOL cmdHandleStdinWithPipe(PREDIRCOMPLETE_INFO pRdrInfo)
{ (void)pRdrInfo; SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE; }
BOOL cmdHandleStdOutErrWithPipe(PREDIRCOMPLETE_INFO pRdrInfo, USHORT handle_type)
{ (void)pRdrInfo; (void)handle_type; SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE; }
void RcErrorDialogBox(UINT error, PVOID first, PVOID second)
{ (void)error; (void)first; (void)second; }
void TerminateVDM(void) { }
void nt_init_event_thread(void)
{
    if (g_active_call != NULL && g_active_call->call->session != NULL)
        g_active_call->call->session->console_initialized = 1u;
}

/* OpenNT cmdkeyb.c called the old NTVDM console-composition export
 * GetConsoleKeyboardLayoutNameA.  It is not linkable from the modern public
 * Win32 import libraries.  GetKeyboardLayoutNameA is the public supported
 * capability with the same current-layout-name result; keep this replacement
 * at the host shim and leave cmdkeyb.c's registry/failure algorithm intact. */
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name)
{
    return GetKeyboardLayoutNameA(name);
}

LPVOID bx_ntvdm_command_misc_get_vdm_addr(USHORT segment, USHORT offset)
{
    bx_ntvdm_command_misc_active_call *active = g_active_call;
    uint32_t bytes;
    uint32_t index;
    if (active == NULL) return NULL;
    active->guest_address = real_mode_address(segment, offset);
    if (active->call->service == BX_NTVDM_COMMAND_MISC_SET_INFO) {
        bx_ntvdm_command_misc_session *session = active->call->session;
        if (active->guest_address > 0x100000u - sizeof(SCSINFO)) return NULL;
        if (active->guest_bytes == 0u) {
            if (!active->call->guest_read(active->call->guest_state,
                    active->guest_address, (uint8_t *)&session->scs_info,
                    sizeof(session->scs_info))) return NULL;
            session->scs_info_address = active->guest_address;
            active->guest_bytes = 1u;
            return &session->scs_info;
        }
        if (active->guest_bytes == 1u) {
            if (active->guest_address >= 0x100000u) return NULL;
            if (!active->call->guest_read(active->call->guest_state,
                    active->guest_address, &session->is_dos_binary, 1u)) return NULL;
            session->is_dos_binary_address = active->guest_address;
            active->guest_bytes = 2u;
            return &session->is_dos_binary;
        }
        if (active->guest_bytes == 2u) {
            if (active->guest_address > 0x100000u - sizeof(WORD)) return NULL;
            if (!active->call->guest_read(active->call->guest_state,
                    active->guest_address, (uint8_t *)&session->fd_access,
                    sizeof(session->fd_access))) return NULL;
            session->fd_access_address = active->guest_address;
            active->guest_bytes = 3u;
            return &session->fd_access;
        }
        return NULL;
    }
    if (active->guest_buffer != NULL) return NULL;
    bytes = active->call->service == BX_NTVDM_COMMAND_MISC_COMSPEC ?
        BX_NTVDM_COMMAND_MISC_COMSPEC_MAX + 1u :
        BX_NTVDM_COMMAND_MISC_CURRENT_DIR_BYTES;
    if (active->guest_address > 0x100000u - bytes) return NULL;
    active->guest_buffer = (uint8_t *)calloc(bytes, 1u);
    if (active->guest_buffer == NULL) return NULL;
    active->guest_bytes = bytes;
    active->write_back = active->call->service == BX_NTVDM_COMMAND_MISC_GET_CURRENT_DIR;
    if (active->write_back) return active->guest_buffer;
    for (index = 0u; index < bytes; ++index) {
        if (!active->call->guest_read(active->call->guest_state,
                active->guest_address + index, active->guest_buffer + index, 1u))
            return NULL;
        if (active->guest_buffer[index] == 0u) return active->guest_buffer;
    }
    return NULL;
}

UINT demGetPhysicalDriveType(UCHAR drive)
{
    (void)drive;
    return DRIVE_UNKNOWN;
}

UINT GetDriveTypeOem(LPSTR root)
{
    CHAR ansi[4];
    if (root == NULL || !OemToCharBuffA(root, ansi, 4u)) return DRIVE_UNKNOWN;
    return GetDriveTypeA(ansi);
}

DWORD GetEnvironmentVariableOem(LPSTR name, LPSTR buffer, DWORD bytes)
{
    CHAR ansi_name[4];
    CHAR ansi_value[BX_NTVDM_COMMAND_MISC_CURRENT_DIR_BYTES];
    DWORD result;
    if (name == NULL || buffer == NULL || bytes == 0u ||
        !OemToCharBuffA(name, ansi_name, (DWORD)(strlen(name) + 1u))) return 0u;
    result = GetEnvironmentVariableA(ansi_name, ansi_value, (DWORD)sizeof(ansi_value));
    if (result == 0u || result >= sizeof(ansi_value) ||
        !CharToOemBuffA(ansi_value, buffer, result + 1u)) return result;
    return result;
}

BOOL SetEnvironmentVariableOem(LPSTR name, LPSTR value)
{
    CHAR ansi_name[4];
    CHAR ansi_value[BX_NTVDM_COMMAND_MISC_CURRENT_DIR_BYTES];
    if (name == NULL || !OemToCharBuffA(name, ansi_name, (DWORD)(strlen(name) + 1u)))
        return FALSE;
    if (value == NULL) return SetEnvironmentVariableA(ansi_name, NULL);
    if (!OemToCharBuffA(value, ansi_value, (DWORD)(strlen(value) + 1u))) return FALSE;
    return SetEnvironmentVariableA(ansi_name, ansi_value);
}

int bx_ntvdm_command_misc_invoke(bx_ntvdm_command_misc_call *call)
{
    bx_ntvdm_command_misc_active_call active;
    void (*body)(void);
    if (!bx_ntvdm_command_misc_call_valid(call) || g_active_call != NULL ||
        call->boundary->fault_rip > UINT64_MAX - 4u) return 0;
    if (call->service == BX_NTVDM_COMMAND_MISC_COMSPEC &&
        !validate_comspec_input(call)) return 0;
    memset(&active, 0, sizeof(active));
    if (call->service == BX_NTVDM_COMMAND_MISC_COMSPEC)
        body = cmdComSpec;
    else if (call->service == BX_NTVDM_COMMAND_MISC_GET_CURRENT_DIR)
        body = cmdGetCurrentDir;
    else if (call->service == BX_NTVDM_COMMAND_MISC_SET_INFO)
        body = cmdSetInfo;
    else if (call->service == BX_NTVDM_COMMAND_MISC_INIT_CONSOLE)
        body = cmdInitConsole;
    else if (call->service == BX_NTVDM_COMMAND_MISC_GET_KBD_LAYOUT)
        body = cmdGetKbdLayout;
    else if (call->service == 0x06u)
        body = cmdGetStdHandle;
    else
        body = cmdSaveWorld;
    IsFirstCall = call->first_call ? TRUE : FALSE;
    VDMForWOW = call->vdm_for_wow ? TRUE : FALSE;
    memset(lpszComSpec, 0, sizeof(lpszComSpec));
    cbComSpec = 0u;
    bx_ntvdm_cpu_result_v2_pass_through(call->result);
    if (!bx_ntvdm_cpu_result_v2_resume(call->result, call->boundary->fault_rip + 4u))
        return 0;
    active.call = call;
    g_active_call = &active;
    body();
    if (call->service == BX_NTVDM_COMMAND_MISC_SET_INFO && active.guest_bytes != 3u) {
        g_active_call = NULL;
        return 0;
    }
    if (active.write_back && active.guest_buffer != NULL &&
        !call->guest_write(call->guest_state, active.guest_address,
            active.guest_buffer, active.guest_bytes)) {
        free(active.guest_buffer); g_active_call = NULL; return 0;
    }
    free(active.guest_buffer);
    g_active_call = NULL;
    return bx_ntvdm_cpu_result_v2_valid(call->result);
}

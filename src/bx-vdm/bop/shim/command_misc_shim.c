#include "command_misc_shim.h"

#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

void cmdComSpec(void);
void cmdSaveWorld(void);
void cmdGetCurrentDir(void);
void cmdSetInfo(void);
void cmdGetKbdLayout(void);
void cmdGetStdHandle(void);
void cmdGetStartInfo(void);
void cmdGetConfigSys(void);
void cmdGetAutoexecBat(void);
void cmdGetInitEnvironment(void);
void cmdCheckBinary(void);
void cmdGetNextCmd(void);
void cmdExec(void);
void cmdExecComspec32(void);
void cmdReturnExitCode(void);
void cmdExitVDM(void);
BOOL CmdDispatch(ULONG service);

CHAR lpszComSpec[64 + 8];
USHORT cbComSpec;
BOOL IsFirstCall;
BOOL VDMForWOW;
PSCSINFO pSCSInfo;
PCHAR pSCS_ToSync;
BYTE *pIsDosBinary;
WORD *pFDAccess;
BOOL bPifFastPaste;
ULONG DosSessionId;
BOOL fSoftpcRedirection;
BOOL IsRepeatCall;
BOOL DosEnvCreated;
BOOL IsFirstVDM;
BOOL fBlock;
WORD Exe32ActiveCount;
USHORT nDrives;
VDMINFO VDMInfo;
VDMENVBLK cmdVDMEnvBlk;
CHAR cmdHomeDirectory[MAX_PATH + 1];
PIF_DATA pfdata;
UINT VdmExitCode;
DWORD dwExitCode32;
CHAR chDefaultDrive;
BOOL fSoftpcRedirectionOnShellOut;
ULONG CntrlHandlerState;
/* OpenNT cmddata.c owns these scratch globals.  cmddata.c itself is not a
 * composable COMMAND input here; retain the exact storage contract in the
 * session shim so the directly admitted cmdmisc.c body remains unchanged. */
CHAR *lpszzCurrentDirectories;
DWORD cchCurrentDirectories;
void nt_std_handle_notification(BOOL enabled) { (void)enabled; }

#pragma warning(push)
#pragma warning(disable: 4324) /* jmp_buf has platform-required alignment; this private stack record never crosses an ABI. */
typedef struct bx_ntvdm_command_misc_active_call {
    bx_ntvdm_command_misc_call *call;
    uint8_t *guest_buffer;
    uint32_t guest_address;
    uint32_t guest_bytes;
    uint32_t write_back;
    uint8_t *guest_buffer2;
    uint8_t *guest_buffer3;
    uint8_t *guest_buffer4;
    uint32_t guest_address2;
    uint32_t guest_bytes2;
    uint32_t guest_address3;
    uint32_t guest_bytes3;
    jmp_buf terminal_exit;
} bx_ntvdm_command_misc_active_call;
#pragma warning(pop)

static __declspec(thread) bx_ntvdm_command_misc_active_call *g_active_call;
static CHAR g_test_system_directory[MAX_PATH + 1];
static const CHAR g_empty_environment[2] = { '\0', '\0' };

static uint32_t real_mode_address(USHORT segment, USHORT offset)
{
    return ((uint32_t)segment << 4) + (uint32_t)offset;
}

static int copy_guest_multisz(bx_ntvdm_command_misc_active_call *active,
    uint32_t address, uint8_t **buffer_out, uint32_t *bytes_out)
{
    uint8_t *buffer;
    uint32_t index;
    if (active == NULL || buffer_out == NULL || bytes_out == NULL || *buffer_out != NULL ||
        address > 0x100000u - 2u) return 0;
    buffer = (uint8_t *)malloc(256u);
    if (buffer == NULL) return 0;
    for (index = 0u; index < USHRT_MAX; ++index) {
        uint8_t value;
        if (address > 0x100000u - 1u - index ||
            !active->call->guest_read(active->call->guest_state, address + index, &value, 1u)) {
            free(buffer);
            return 0;
        }
        if (index == 256u || (index > 256u && (index & (index - 1u)) == 0u)) {
            uint8_t *expanded = (uint8_t *)realloc(buffer, index * 2u);
            if (expanded == NULL) { free(buffer); return 0; }
            buffer = expanded;
        }
        buffer[index] = value;
        if (index != 0u && buffer[index - 1u] == 0u && value == 0u) {
            *buffer_out = buffer;
            *bytes_out = index + 1u;
            return 1;
        }
    }
    free(buffer);
    return 0;
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
    (void)bx_ntvdm_host_handle_manager_initialize(&session->handles);
}

void bx_ntvdm_command_misc_session_dispose(bx_ntvdm_command_misc_session *session)
{
    if (!bx_ntvdm_command_misc_session_valid(session)) return;
    free(session->command_source_environment);
    free(session->command_source_vdm_environment);
    free(session->command_source_current_directories);
    session->command_source_environment = NULL;
    session->command_source_environment_bytes = 0u;
    session->command_source_vdm_environment = NULL;
    session->command_source_vdm_environment_bytes = 0u;
    session->command_source_current_directories = NULL;
    session->command_source_current_directories_bytes = 0u;
    bx_ntvdm_host_handle_manager_reset(&session->handles);
}

static int replace_environment(CHAR **destination, uint32_t *destination_bytes,
    const CHAR *source, uint32_t bytes)
{
    CHAR *replacement;
    if (destination == NULL || destination_bytes == NULL || source == NULL ||
        bytes < 2u || bytes > USHRT_MAX || source[bytes - 2u] != '\0' ||
        source[bytes - 1u] != '\0') return 0;
    replacement = (CHAR *)malloc(bytes);
    if (replacement == NULL) return 0;
    memcpy(replacement, source, bytes);
    free(*destination);
    *destination = replacement;
    *destination_bytes = bytes;
    return 1;
}

int bx_ntvdm_command_misc_session_valid(const bx_ntvdm_command_misc_session *session)
{
    return session != NULL && session->magic == BX_NTVDM_COMMAND_MISC_SESSION_MAGIC &&
        session->abi_version == BX_NTVDM_COMMAND_MISC_SESSION_VERSION &&
        session->struct_bytes == sizeof(*session) &&
        bx_ntvdm_host_handle_manager_valid(&session->handles);
}

int bx_ntvdm_command_misc_session_set_command_source(
    bx_ntvdm_command_misc_session *session, const CHAR *application,
    const CHAR *tail, USHORT drive, USHORT code_page)
{
    size_t application_bytes, tail_bytes;
    if (!bx_ntvdm_command_misc_session_valid(session) || application == NULL ||
        tail == NULL || session->command_source_ready != 0u) return 0;
    application_bytes = strlen(application);
    tail_bytes = strlen(tail);
    if (application_bytes == 0u || application_bytes >= sizeof(session->command_source_app) ||
        tail_bytes >= sizeof(session->command_source_tail)) return 0;
    memcpy(session->command_source_app, application, application_bytes + 1u);
    memcpy(session->command_source_tail, tail, tail_bytes + 1u);
    session->command_source_drive = drive;
    session->command_source_code_page = code_page;
    session->command_source_ready = 1u;
    return 1;
}

int bx_ntvdm_command_misc_session_set_command_environment(
    bx_ntvdm_command_misc_session *session, const CHAR *environment,
    uint32_t bytes)
{
    if (!bx_ntvdm_command_misc_session_valid(session)) return 0;
    return replace_environment(&session->command_source_environment,
        &session->command_source_environment_bytes, environment, bytes);
}

BOOL GetNextVDMCommand(PVDMINFO vdm_info)
{
    bx_ntvdm_command_misc_session *session = bx_ntvdm_command_misc_active_session();
    size_t application_bytes, tail_bytes;
    if (vdm_info == NULL || session == NULL || session->command_source_ready == 0u)
        return FALSE;
    if ((vdm_info->VDMState & ASKING_FOR_ENVIRONMENT) != 0u) {
        uint32_t bytes = session->command_source_environment_bytes;
        const CHAR *environment = session->command_source_environment;
        if (bytes == 0u || environment == NULL) {
            environment = g_empty_environment;
            bytes = sizeof(g_empty_environment);
        }
        if (vdm_info->Enviornment == NULL || vdm_info->EnviornmentSize < bytes) {
            vdm_info->EnviornmentSize = bytes;
            return FALSE;
        }
        memcpy(vdm_info->Enviornment, environment, bytes);
        vdm_info->EnviornmentSize = bytes;
        return TRUE;
    }
    if (session->command_source_delivered != 0u || vdm_info->AppName == NULL ||
        vdm_info->CmdLine == NULL) return FALSE;
    application_bytes = strlen(session->command_source_app) + 1u;
    tail_bytes = strlen(session->command_source_tail);
    if (application_bytes > vdm_info->AppLen || tail_bytes + 2u > vdm_info->CmdSize ||
        application_bytes > USHRT_MAX || tail_bytes + 2u > USHRT_MAX) return FALSE;
    memcpy(vdm_info->AppName, session->command_source_app, application_bytes);
    memcpy(vdm_info->CmdLine, session->command_source_tail, tail_bytes);
    ((CHAR *)vdm_info->CmdLine)[tail_bytes] = '\r';
    ((CHAR *)vdm_info->CmdLine)[tail_bytes + 1u] = '\n';
    ((CHAR *)vdm_info->CmdLine)[tail_bytes + 2u] = '\0';
    vdm_info->AppLen = (USHORT)application_bytes;
    vdm_info->CmdSize = (USHORT)(tail_bytes + 2u);
    vdm_info->CurDrive = session->command_source_drive;
    vdm_info->CodePage = session->command_source_code_page;
    session->command_source_delivered = 1u;
    return TRUE;
}

void host_lpt_flush_initialize(void) { }
BOOL SetVDMCurrentDirectories(ULONG current_directory_bytes,
    LPSTR current_directories)
{
    bx_ntvdm_command_misc_session *session = bx_ntvdm_command_misc_active_session();
    /* DIVERGENCE: OpenNT's client stub sends these bytes to BaseSrv/CSR for a
     * console-bound VDM.  That NT4 product service is not independently
     * composable in the CLI; retain its copied multisz publication contract in
     * the active session instead. */
    return session != NULL && replace_environment(
        &session->command_source_current_directories,
        &session->command_source_current_directories_bytes,
        current_directories, current_directory_bytes);
}
void cmdPushExitInConsoleBuffer(void) { }
void nt_block_event_thread(int block) { (void)block; }
void nt_resume_event_thread(void) { }
void GetWowKernelCmdLine(void) { TerminateVDM(); }
ULONG bx_ntvdm_command_misc_redirection_token(PREDIRCOMPLETE_INFO info)
{ return info == NULL ? 0u : bx_ntvdm_command_misc_active_session()->redirection_token; }

void bx_ntvdm_command_lifecycle_exec(LPSTR command, LPSTR environment)
{
    STARTUPINFOA startup;
    PROCESS_INFORMATION process;
    VDMINFO next;
    CHAR command_copy[1024u];
    CHAR *environment_copy = NULL;
    DWORD environment_bytes = 0u;
    DWORD exit_code = ERROR_BAD_FORMAT;
    uint32_t standard_handles[3];
    size_t command_bytes;
    if (g_active_call == NULL || command == NULL) return;
    command_bytes = strlen(command);
    if (command_bytes == 0u || command_bytes >= sizeof(command_copy)) {
        bx_ntvdm_command_misc_set_cf(0); bx_ntvdm_command_misc_set_al((UCHAR)ERROR_BAD_FORMAT); return;
    }
    memcpy(command_copy, command, command_bytes + 1u);
    /* DIVERGENCE: cmdCreateProcess used a CCPU worker thread and temporarily
     * changed process-global standard handles.  The portable CLI executes the
     * same declared command synchronously with explicit inherited handles,
     * keeping the caller's host process untouched. */
    if (environment != NULL) {
        const uint8_t *environment_bytes_source = NULL;
        if (g_active_call != NULL && environment == (CHAR *)g_active_call->guest_buffer) {
            environment_bytes = g_active_call->guest_bytes;
            environment_bytes_source = g_active_call->guest_buffer;
        } else if (g_active_call != NULL && environment == (CHAR *)g_active_call->guest_buffer2) {
            environment_bytes = g_active_call->guest_bytes2;
            environment_bytes_source = g_active_call->guest_buffer2;
        }
        if (environment_bytes_source == NULL || environment_bytes < 2u ||
            environment_bytes_source[environment_bytes - 2u] != '\0' ||
            environment_bytes_source[environment_bytes - 1u] != '\0') {
            bx_ntvdm_command_misc_set_cf(0); bx_ntvdm_command_misc_set_al((UCHAR)ERROR_BAD_ENVIRONMENT); return;
        }
        environment_copy = (CHAR *)malloc(environment_bytes);
        if (environment_copy == NULL ||
            !OemToCharBuffA(environment, environment_copy, environment_bytes)) {
            free(environment_copy);
            bx_ntvdm_command_misc_set_cf(0); bx_ntvdm_command_misc_set_al((UCHAR)ERROR_BAD_ENVIRONMENT); return;
        }
    }
    memset(&startup, 0, sizeof(startup)); memset(&process, 0, sizeof(process));
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    if (g_active_call->call->service == BX_NTVDM_COMMAND_MISC_EXEC) {
        bx_ntvdm_command_misc_session *session = g_active_call->call->session;
        uint32_t address = real_mode_address(g_active_call->call->cpu->ss,
            (USHORT)g_active_call->call->cpu->ebp);
        HANDLE *targets[3] = { &startup.hStdError, &startup.hStdOutput, &startup.hStdInput };
        uint32_t index;
        if (session == NULL || address > 0x100000u - sizeof(standard_handles) ||
            !g_active_call->call->guest_read(g_active_call->call->guest_state, address,
                (uint8_t *)standard_handles, sizeof(standard_handles))) {
            bx_ntvdm_command_misc_set_cf(0); bx_ntvdm_command_misc_set_al((UCHAR)ERROR_INVALID_HANDLE); return;
        }
        for (index = 0u; index < 3u; ++index) {
            uint32_t token = standard_handles[index];
            if (token == UINT32_MAX) continue;
            /* DIVERGENCE: original cmdCreateProcess temporarily installed the
             * decoded guest handles into the parent process.  Resolve the
             * fixed token directly into STARTUPINFO instead. */
            if (token == 0u || token == UINT32_MAX ||
                !bx_ntvdm_host_handle_manager_lookup_handle(&session->handles,
                    token, targets[index])) {
                bx_ntvdm_command_misc_set_cf(0); bx_ntvdm_command_misc_set_al((UCHAR)ERROR_INVALID_HANDLE); return;
            }
        }
    }
    if (!CreateProcessA(NULL, command_copy, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE,
            environment == NULL ? NULL : environment_copy, NULL, &startup, &process)) {
        exit_code = GetLastError();
    } else {
        (void)WaitForSingleObject(process.hProcess, INFINITE);
        if (!GetExitCodeProcess(process.hProcess, &exit_code)) exit_code = GetLastError();
        CloseHandle(process.hThread); CloseHandle(process.hProcess);
    }
    free(environment_copy);
    dwExitCode32 = exit_code;
    memset(&next, 0, sizeof(next));
    next.VDMState = 0x00c0u; /* NO_PARENT_TO_WAKE | RETURN_ON_NO_COMMAND */
    (void)GetNextVDMCommand(&next);
    if (next.CmdSize > 0u) { IsRepeatCall = TRUE; bx_ntvdm_command_misc_set_cf(1); }
    else { IsRepeatCall = FALSE; bx_ntvdm_command_misc_set_cf(0); bx_ntvdm_command_misc_set_al((UCHAR)dwExitCode32); }
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
         (call->service == BX_NTVDM_COMMAND_MISC_EXIT ||
          call->service == BX_NTVDM_COMMAND_MISC_GET_NEXT ||
          call->service == BX_NTVDM_COMMAND_MISC_COMSPEC ||
         call->service == BX_NTVDM_COMMAND_MISC_SAVE_WORLD ||
         call->service == BX_NTVDM_COMMAND_MISC_GET_CURRENT_DIR ||
         call->service == BX_NTVDM_COMMAND_MISC_SET_INFO ||
         call->service == BX_NTVDM_COMMAND_MISC_INIT_CONSOLE ||
         call->service == BX_NTVDM_COMMAND_MISC_GET_CONFIG_SYS ||
         call->service == BX_NTVDM_COMMAND_MISC_GET_AUTOEXEC_BAT ||
          call->service == BX_NTVDM_COMMAND_MISC_GET_KBD_LAYOUT ||
          call->service == BX_NTVDM_COMMAND_MISC_CHECK_BINARY ||
          call->service == BX_NTVDM_COMMAND_MISC_EXEC ||
          call->service == BX_NTVDM_COMMAND_MISC_EXEC_COMSPEC32 ||
          call->service == BX_NTVDM_COMMAND_MISC_RETURN_EXIT_CODE ||
         call->service == BX_NTVDM_COMMAND_MISC_GET_INIT_ENVIRONMENT ||
         call->service == BX_NTVDM_COMMAND_MISC_GET_START_INFO ||
         call->service == 0x06u) &&
        call->boundary != NULL && bx_ntvdm_exception_event_v1_valid(call->boundary) &&
        call->cpu != NULL && bx_ntvdm_cpu_state_v1_valid(call->cpu) &&
        call->cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        call->result != NULL && call->guest_read != NULL && call->guest_write != NULL &&
         ((call->service != BX_NTVDM_COMMAND_MISC_SET_INFO &&
           call->service != BX_NTVDM_COMMAND_MISC_GET_NEXT &&
          call->service != BX_NTVDM_COMMAND_MISC_GET_CONFIG_SYS &&
          call->service != BX_NTVDM_COMMAND_MISC_GET_AUTOEXEC_BAT) ||
         bx_ntvdm_command_misc_session_valid(call->session));
}

USHORT bx_ntvdm_command_misc_get_dx(void) { return (USHORT)g_active_call->call->cpu->edx; }
USHORT bx_ntvdm_command_misc_get_bx(void) { return (USHORT)g_active_call->call->cpu->ebx; }
USHORT bx_ntvdm_command_misc_get_cx(void) { return (USHORT)g_active_call->call->cpu->ecx; }
USHORT bx_ntvdm_command_misc_get_si(void) { return (USHORT)g_active_call->call->cpu->esi; }
USHORT bx_ntvdm_command_misc_get_ds(void) { return g_active_call->call->cpu->ds; }
USHORT bx_ntvdm_command_misc_get_es(void) { return g_active_call->call->cpu->es; }
USHORT bx_ntvdm_command_misc_get_ss(void) { return g_active_call->call->cpu->ss; }
USHORT bx_ntvdm_command_misc_get_bp(void) { return (USHORT)g_active_call->call->cpu->ebp; }
USHORT bx_ntvdm_command_misc_get_ax(void) { return (USHORT)g_active_call->call->cpu->eax; }
UCHAR bx_ntvdm_command_misc_get_al(void) { return (UCHAR)(g_active_call->call->cpu->eax & 0xffu); }
UCHAR bx_ntvdm_command_misc_get_ah(void) { return (UCHAR)((g_active_call->call->cpu->eax >> 8u) & 0xffu); }
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
void bx_ntvdm_command_misc_set_ds(USHORT value)
{ (void)bx_ntvdm_cpu_delta_v1_set_segment(&g_active_call->call->result->cpu_delta, 3u, value); }
void bx_ntvdm_command_misc_set_es(USHORT value)
{ (void)bx_ntvdm_cpu_delta_v1_set_segment(&g_active_call->call->result->cpu_delta, 0u, value); }

bx_ntvdm_command_misc_session *bx_ntvdm_command_misc_active_session(void)
{
    return g_active_call == NULL ? NULL : g_active_call->call->session;
}

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
    uint32_t guest_handle;
    DWORD error;
    if (g_active_call == NULL || (session = g_active_call->call->session) == NULL ||
        !bx_ntvdm_host_handle_manager_publish(&session->handles, handle,
            BX_NTVDM_HOST_HANDLE_BORROWED, &guest_handle, &error)) return 0;
    /* The original guest ABI is BX:CX. Preserve it as a fixed-width token,
     * never as a truncated host HANDLE. */
    bx_ntvdm_command_misc_set_cx((USHORT)guest_handle);
    bx_ntvdm_command_misc_set_bx((USHORT)(guest_handle >> 16u));
    return 1;
}

void RcErrorDialogBox(UINT error, PVOID first, PVOID second)
{ (void)error; (void)first; (void)second; }
void TerminateVDM(void)
{
    /* OpenNT's terminal path does not return.  The typed composition models
     * that directly as a controlled stop instead of resuming after an error. */
    if (g_active_call != NULL) {
        (void)bx_ntvdm_cpu_result_v2_stop(g_active_call->call->result);
        longjmp(g_active_call->terminal_exit, 1);
    }
}
void nt_init_event_thread(void)
{
    if (g_active_call != NULL && g_active_call->call->session != NULL)
        g_active_call->call->session->console_initialized = 1u;
}

UINT bx_ntvdm_command_misc_get_system_directory(LPSTR buffer, UINT bytes)
{
    if (g_test_system_directory[0] != '\0') {
        size_t length = strlen(g_test_system_directory);
        if (buffer == NULL || bytes == 0u) return (UINT)length;
        if (length >= bytes) return (UINT)length;
        memcpy(buffer, g_test_system_directory, length + 1u);
        return (UINT)length;
    }
    return GetSystemDirectoryA(buffer, bytes);
}
void bx_ntvdm_command_misc_set_test_system_directory(const CHAR *path)
{
    if (path == NULL) { g_test_system_directory[0] = '\0'; return; }
    strncpy(g_test_system_directory, path, MAX_PATH);
    g_test_system_directory[MAX_PATH] = '\0';
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
    if (active->call->service == BX_NTVDM_COMMAND_MISC_EXEC ||
        active->call->service == BX_NTVDM_COMMAND_MISC_EXEC_COMSPEC32) {
        uint32_t address = real_mode_address(segment, offset);
        uint32_t maximum = active->call->service == BX_NTVDM_COMMAND_MISC_EXEC &&
            segment == active->call->cpu->ds && offset == (USHORT)active->call->cpu->esi ? 124u : USHRT_MAX;
        uint8_t **buffer = active->guest_buffer == NULL ? &active->guest_buffer : &active->guest_buffer2;
        uint32_t *buffer_address = active->guest_buffer == NULL ? &active->guest_address : &active->guest_address2;
        uint32_t *buffer_bytes = active->guest_buffer == NULL ? &active->guest_bytes : &active->guest_bytes2;
        if (*buffer != NULL || (maximum == 124u && address > 0x100000u - maximum)) return NULL;
        *buffer_address = address;
        if (maximum != 124u) {
            /* DIVERGENCE: the old 1 KiB probe was an adapter limit, not an
             * OpenNT environment contract.  Read a bounded DOS multisz and
             * retain only its exact copied extent for the CLI backend. */
            if (!copy_guest_multisz(active, address, buffer, buffer_bytes)) return NULL;
            return *buffer;
        }
        *buffer = (uint8_t *)calloc(maximum, 1u);
        if (*buffer == NULL || !active->call->guest_read(active->call->guest_state,
                address, *buffer, maximum)) return NULL;
        *buffer_bytes = maximum;
        return *buffer;
    }
    if (active->call->service == BX_NTVDM_COMMAND_MISC_GET_NEXT) {
        CMDINFO *info;
        uint32_t address = real_mode_address(segment, offset);
        uint8_t **buffer;
        uint32_t *buffer_address;
        uint32_t *buffer_bytes;
        if (active->guest_buffer == NULL) {
            if (address > 0x100000u - sizeof(CMDINFO)) return NULL;
            active->guest_buffer = (uint8_t *)calloc(sizeof(CMDINFO), 1u);
            if (active->guest_buffer == NULL || !active->call->guest_read(active->call->guest_state,
                    address, active->guest_buffer, sizeof(CMDINFO))) return NULL;
            active->guest_address = address;
            active->guest_bytes = sizeof(CMDINFO);
            active->write_back = 1u;
            return active->guest_buffer;
        }
        info = (CMDINFO *)active->guest_buffer;
        if (segment == info->ExecPathSeg && offset == info->ExecPathOff) {
            bytes = info->ExecPathSize;
            buffer = &active->guest_buffer2; buffer_address = &active->guest_address2;
            buffer_bytes = &active->guest_bytes2;
        } else if (segment == info->CmdLineSeg && offset == info->CmdLineOff) {
            bytes = info->CmdLineSize;
            buffer = &active->guest_buffer3; buffer_address = &active->guest_address2;
            buffer_bytes = &active->guest_bytes2;
        } else if (segment == info->EnvSeg && offset == 0u) {
            bytes = info->EnvSize;
            buffer = &active->guest_buffer4; buffer_address = &active->guest_address3;
            buffer_bytes = &active->guest_bytes3;
        } else return NULL;
        /* DIVERGENCE: each historical SAS pointer is materialized as a
         * bounded copied span.  This preserves cmdmisc.c's pointer order but
         * rejects an alias or an unbounded real-mode address. */
        if (bytes == 0u || address > 0x100000u - bytes || *buffer != NULL) return NULL;
        *buffer = (uint8_t *)calloc(bytes, 1u);
        if (*buffer == NULL) return NULL;
        *buffer_address = address;
        *buffer_bytes = bytes;
        return *buffer;
    }
    if (active->call->service == BX_NTVDM_COMMAND_MISC_CHECK_BINARY) {
        uint32_t address = real_mode_address(segment, offset);
        uint32_t binary_bytes = active->guest_bytes == 0u ? MAX_PATH :
            active->guest_bytes == MAX_PATH ? sizeof(PARAMBLOCK) : 129u;
        uint8_t **buffer = active->guest_bytes == 0u ? &active->guest_buffer :
            active->guest_bytes == MAX_PATH ? &active->guest_buffer2 : &active->guest_buffer3;
        if (address > 0x100000u - binary_bytes || *buffer != NULL) return NULL;
        *buffer = (uint8_t *)calloc(binary_bytes, 1u);
        if (*buffer == NULL || !active->call->guest_read(active->call->guest_state,
                address, *buffer, binary_bytes)) return NULL;
        if (active->guest_bytes == 0u) {
            if (memchr(*buffer, 0, binary_bytes) == NULL) return NULL;
            active->guest_bytes = MAX_PATH;
        } else if (active->guest_bytes == MAX_PATH) active->guest_bytes = MAX_PATH + 1u;
        return *buffer;
    }
    if (active->call->service == BX_NTVDM_COMMAND_MISC_GET_INIT_ENVIRONMENT) {
        uint32_t requested = (uint32_t)(USHORT)active->call->cpu->ebx << 4;
        active->guest_address = real_mode_address(segment, offset);
        if (active->guest_address > 0x100000u ||
            requested > 0x100000u - active->guest_address || active->guest_buffer != NULL)
            return NULL;
        active->guest_buffer = (uint8_t *)calloc(requested == 0u ? 1u : requested, 1u);
        if (active->guest_buffer == NULL) return NULL;
        active->guest_bytes = requested;
        active->write_back = 1u;
        return active->guest_buffer;
    }
    if (active->call->service == BX_NTVDM_COMMAND_MISC_GET_KBD_LAYOUT) {
        uint32_t keyboard_bytes = active->guest_buffer == NULL ? 128u : 300u;
        uint8_t **buffer = active->guest_buffer == NULL ? &active->guest_buffer : &active->guest_buffer2;
        uint32_t *address = active->guest_buffer == NULL ? &active->guest_address : &active->guest_address2;
        uint32_t *size = active->guest_buffer == NULL ? &active->guest_bytes : &active->guest_bytes2;
        *address = real_mode_address(segment, offset);
        if (*address > 0x100000u - keyboard_bytes || *buffer != NULL) return NULL;
        *buffer = (uint8_t *)calloc(keyboard_bytes, 1u);
        if (*buffer == NULL) return NULL;
        *size = keyboard_bytes;
        return *buffer;
    }
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
        (active->call->service == BX_NTVDM_COMMAND_MISC_GET_CONFIG_SYS ||
         active->call->service == BX_NTVDM_COMMAND_MISC_GET_AUTOEXEC_BAT) ?
        64u : BX_NTVDM_COMMAND_MISC_CURRENT_DIR_BYTES;
    if (active->guest_address > 0x100000u - bytes) return NULL;
    active->guest_buffer = (uint8_t *)calloc(bytes, 1u);
    if (active->guest_buffer == NULL) return NULL;
    active->guest_bytes = bytes;
    active->write_back = active->call->service == BX_NTVDM_COMMAND_MISC_GET_CURRENT_DIR ||
        active->call->service == BX_NTVDM_COMMAND_MISC_GET_CONFIG_SYS ||
        active->call->service == BX_NTVDM_COMMAND_MISC_GET_AUTOEXEC_BAT;
    if (active->write_back) return active->guest_buffer;
    for (index = 0u; index < bytes; ++index) {
        if (!active->call->guest_read(active->call->guest_state,
                active->guest_address + index, active->guest_buffer + index, 1u))
            return NULL;
        if (active->guest_buffer[index] == 0u) return active->guest_buffer;
    }
    return NULL;
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

/* Shared OEM environment capability for the directly imported DEM and
 * COMMAND owners.  OpenNT exposed one process environment to both owners;
 * keeping the only definition here prevents test-local shim duplicates. */
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
    if (!bx_ntvdm_command_misc_call_valid(call) || g_active_call != NULL ||
        call->boundary->fault_rip > UINT64_MAX - 4u || call->service >= 17u)
        return 0;
    if (call->service == BX_NTVDM_COMMAND_MISC_COMSPEC &&
        !validate_comspec_input(call)) return 0;
    memset(&active, 0, sizeof(active));
    IsFirstCall = call->first_call ? TRUE : FALSE;
    IsRepeatCall = call->service == BX_NTVDM_COMMAND_MISC_GET_NEXT && call->session != NULL &&
        call->session->command_source_repeat_pending != 0u;
    DosEnvCreated = IsRepeatCall;
    IsFirstVDM = TRUE;
    fBlock = FALSE;
    Exe32ActiveCount = 0u;
    nDrives = 1u;
    memset(&VDMInfo, 0, sizeof(VDMInfo));
    memset(&cmdVDMEnvBlk, 0, sizeof(cmdVDMEnvBlk));
    if (IsRepeatCall) {
        cmdVDMEnvBlk.cchEnv = call->session->command_source_vdm_environment_bytes;
        if (cmdVDMEnvBlk.cchEnv < 2u || call->session->command_source_vdm_environment == NULL)
            return 0;
        cmdVDMEnvBlk.lpszzEnv = (CHAR *)malloc(cmdVDMEnvBlk.cchEnv);
        if (cmdVDMEnvBlk.lpszzEnv == NULL) return 0;
        memcpy(cmdVDMEnvBlk.lpszzEnv, call->session->command_source_vdm_environment,
            cmdVDMEnvBlk.cchEnv);
    }
    VDMForWOW = call->vdm_for_wow ? TRUE : FALSE;
    DosSessionId = call->session != NULL ? call->session->dos_session_id : 0u;
    memset(lpszComSpec, 0, sizeof(lpszComSpec));
    cbComSpec = 0u;
    if (call->service != BX_NTVDM_COMMAND_MISC_COMSPEC && call->session != NULL) {
        memcpy(lpszComSpec, call->session->comspec, sizeof(lpszComSpec));
        cbComSpec = call->session->comspec_bytes;
    }
    bx_ntvdm_cpu_result_v2_pass_through(call->result);
    if (!bx_ntvdm_cpu_result_v2_resume(call->result, call->boundary->fault_rip + 4u))
        return 0;
    active.call = call;
    g_active_call = &active;
    if (call->session != NULL) {
        pSCSInfo = &call->session->scs_info;
        pSCS_ToSync = &call->session->scs_info.SCS_ToSync;
        pIsDosBinary = &call->session->is_dos_binary;
        pFDAccess = &call->session->fd_access;
    }
    /* Preserve OpenNT's original 17-slot cmddisp.c table rather than growing
     * an adapter-owned service recognizer.  The preceding range guard is the
     * required modern boundary check because the retail body checked this
     * index only in DBG builds. */
    if (setjmp(active.terminal_exit) == 0 && !CmdDispatch(call->service)) {
        g_active_call = NULL;
        return 0;
    }
    if (call->service == BX_NTVDM_COMMAND_MISC_EXEC && active.guest_buffer != NULL &&
        !call->guest_write(call->guest_state, active.guest_address,
            active.guest_buffer, active.guest_bytes)) {
        free(active.guest_buffer); free(active.guest_buffer2); g_active_call = NULL; return 0;
    }
    if (call->service == BX_NTVDM_COMMAND_MISC_GET_NEXT && call->session != NULL) {
        call->session->command_source_repeat_pending = IsRepeatCall ? 1u : 0u;
        if (IsRepeatCall && cmdVDMEnvBlk.lpszzEnv != NULL && cmdVDMEnvBlk.cchEnv >= 2u) {
            if (!replace_environment(&call->session->command_source_vdm_environment,
                    &call->session->command_source_vdm_environment_bytes,
                    cmdVDMEnvBlk.lpszzEnv, cmdVDMEnvBlk.cchEnv)) {
                free(cmdVDMEnvBlk.lpszzEnv);
                cmdVDMEnvBlk.lpszzEnv = NULL;
                g_active_call = NULL;
                return 0;
            }
            free(cmdVDMEnvBlk.lpszzEnv);
            cmdVDMEnvBlk.lpszzEnv = NULL;
        }
    }
    if (call->service == BX_NTVDM_COMMAND_MISC_GET_NEXT &&
        ((active.guest_buffer != NULL && !call->guest_write(call->guest_state,
             active.guest_address, active.guest_buffer, active.guest_bytes)) ||
         (active.guest_buffer2 != NULL && !call->guest_write(call->guest_state,
             real_mode_address(((CMDINFO *)active.guest_buffer)->ExecPathSeg,
                 ((CMDINFO *)active.guest_buffer)->ExecPathOff), active.guest_buffer2,
             ((CMDINFO *)active.guest_buffer)->ExecPathSize)) ||
         (active.guest_buffer3 != NULL && !call->guest_write(call->guest_state,
             real_mode_address(((CMDINFO *)active.guest_buffer)->CmdLineSeg,
                 ((CMDINFO *)active.guest_buffer)->CmdLineOff), active.guest_buffer3,
             ((CMDINFO *)active.guest_buffer)->CmdLineSize)) ||
         (active.guest_buffer4 != NULL && !call->guest_write(call->guest_state,
             active.guest_address3, active.guest_buffer4, active.guest_bytes3)))) {
        free(active.guest_buffer); free(active.guest_buffer2); free(active.guest_buffer3);
        free(active.guest_buffer4); g_active_call = NULL; return 0;
    }
    if (call->service == BX_NTVDM_COMMAND_MISC_CHECK_BINARY && call->session != NULL &&
        call->session->scs_info_address != 0u &&
        !call->guest_write(call->guest_state, call->session->scs_info_address,
            (const uint8_t *)&call->session->scs_info, sizeof(call->session->scs_info))) {
        free(active.guest_buffer); free(active.guest_buffer2); free(active.guest_buffer3); free(active.guest_buffer4);
        g_active_call = NULL; return 0;
    }
    if (call->service == BX_NTVDM_COMMAND_MISC_COMSPEC && call->session != NULL) {
        memcpy(call->session->comspec, lpszComSpec, sizeof(lpszComSpec));
        call->session->comspec_bytes = cbComSpec;
    }
    if (call->service == BX_NTVDM_COMMAND_MISC_GET_INIT_ENVIRONMENT) {
        uint32_t index;
        /* cmdenv.c receives only a segment pointer.  Recover the exact
         * multi-string extent it populated so a successful bounded request
         * does not write arbitrary zeroed capacity back into guest RAM. */
        active.write_back = 0u;
        for (index = 0u; index + 1u < active.guest_bytes; ++index) {
            if (active.guest_buffer[index] == 0u && active.guest_buffer[index + 1u] == 0u) {
                active.guest_bytes = index + 2u;
                active.write_back = 1u;
                break;
            }
        }
    }
    if (call->service == BX_NTVDM_COMMAND_MISC_SET_INFO && active.guest_bytes != 3u) {
        g_active_call = NULL;
        return 0;
    }
    if (active.write_back && active.guest_buffer != NULL &&
        !call->guest_write(call->guest_state, active.guest_address,
            active.guest_buffer, active.guest_bytes)) {
        free(active.guest_buffer); g_active_call = NULL; return 0;
    }
    if (call->service == BX_NTVDM_COMMAND_MISC_GET_KBD_LAYOUT &&
        ((active.guest_buffer != NULL && !call->guest_write(call->guest_state, active.guest_address, active.guest_buffer, active.guest_bytes)) ||
         (active.guest_buffer2 != NULL && !call->guest_write(call->guest_state, active.guest_address2, active.guest_buffer2, active.guest_bytes2)))) {
        free(active.guest_buffer); free(active.guest_buffer2); g_active_call = NULL; return 0;
    }
    free(active.guest_buffer);
    free(active.guest_buffer2);
    free(active.guest_buffer3);
    free(active.guest_buffer4);
    g_active_call = NULL;
    return bx_ntvdm_cpu_result_v2_valid(call->result);
}

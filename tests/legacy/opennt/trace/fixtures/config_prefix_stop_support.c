/*
 * Trace-only lower-boundary guards for original config.c. They neither
 * initialize a machine nor allow config() to continue beyond sas_term().
 */
#include <windows.h>
#include <setjmp.h>

BOOL VDMForWOW = FALSE;
BOOL fSeparateWow = FALSE;
ULONG DosSessionId = 0;
ULONG CntrlHandlerState = 0;
HANDLE hWndConsole = NULL;
#ifndef CONFIG_PREFIX_USE_ORIGINAL_XMS
ULONG xmsMemorySize = 0;
#endif

jmp_buf ntdos64_config_prefix_stop_environment;
static int config_prefix_stop_reason;

int ntdos64_config_prefix_stop_reason(void) {
    return config_prefix_stop_reason;
}

static void config_prefix_stop(int reason) {
    config_prefix_stop_reason = reason;
    longjmp(ntdos64_config_prefix_stop_environment, reason);
}

#ifdef NTDOS64_RESET_TRACE
#ifndef NTDOS64_RESET_TRACE_STOP_STAGE
#define NTDOS64_RESET_TRACE_STOP_STAGE 1
#endif
void ntdos64_reset_trace_checkpoint(int stage) {
    if (stage == NTDOS64_RESET_TRACE_STOP_STAGE)
        config_prefix_stop(100 + stage);
}
#endif

#ifdef NTDOS64_CPU_INIT_TRACE
#ifndef NTDOS64_CPU_INIT_TRACE_STOP_STAGE
#define NTDOS64_CPU_INIT_TRACE_STOP_STAGE 1
#endif
void ntdos64_cpu_init_trace_checkpoint(int stage) {
    if (stage == NTDOS64_CPU_INIT_TRACE_STOP_STAGE)
        config_prefix_stop(120 + stage);
}
#endif

#ifdef NTDOS64_SAS_SINGLE_WRITE_TRACE
void ntdos64_sas_single_write_trace_stop(void) {
    config_prefix_stop(130);
}
#endif

void ntdos64_config_prefix_unexpected(int reason) {
    config_prefix_stop(reason);
}

#ifdef NTDOS64_STARTUP_PREFIX_TRACE
void ntdos64_startup_prefix_trace_checkpoint(int stage) {
    char value[16];
    DWORD length = GetEnvironmentVariableA(
        "NTDOS64_STARTUP_PREFIX_TRACE_STOP_STAGE", value, sizeof(value));

    if (length != 0 && length < sizeof(value) && atoi(value) == stage)
        config_prefix_stop(140 + stage);
}
#endif

#ifndef CONFIG_PREFIX_TRACE_SAS_INIT
void sas_term(void) { config_prefix_stop(1); }
#endif
void SetConsoleKeyShortcuts(void) { config_prefix_stop(2); }

/* These are unreachable after the sas_term boundary. Keep any accidental
 * execution fail-closed while allowing the original config object to link. */
#ifndef CONFIG_PREFIX_TRACE_SESSION_INIT
int init_lim_configuration_data(void) { config_prefix_stop(3); return 0; }
#endif
#ifndef CONFIG_PREFIX_TRACE_SAS_INIT
void sas_init(void) { config_prefix_stop(4); }
#endif
void InitNtCpuInfo(void) { config_prefix_stop(5); }
#ifndef CONFIG_PREFIX_USE_ORIGINAL_UMB
int InitUMBList(void) { config_prefix_stop(6); return 0; }
#endif
#if defined(CONFIG_PREFIX_USE_ORIGINAL_UMB) && !defined(CONFIG_PREFIX_USE_ORIGINAL_XMS)
/* The original UMB object references this XMS notification from ReleaseUMB.
 * The bounded initialization trace never reaches it; stop if that changes. */
void xmsReleaseUMBNotify(void *address, unsigned long size) {
    (void)address;
    (void)size;
    config_prefix_stop(17);
}
#endif
#ifndef CONFIG_PREFIX_USE_ORIGINAL_GFI
short gfi_floppy_active(void) { config_prefix_stop(7); return 0; }
#elif !defined(CONFIG_PREFIX_USE_MODERN_MEDIA_CAPABILITY)
short host_gfi_rdiskette_active(unsigned char host_id, int active, char *err) {
    (void)host_id;
    (void)active;
    (void)err;
    config_prefix_stop(18);
    return 0;
}
#endif
#if defined(CONFIG_PREFIX_USE_ORIGINAL_GFI) && !defined(CONFIG_PREFIX_USE_HISTORICAL_GFI_EMPTY)
short host_gfi_rdiskette_valid() {
    config_prefix_stop(19);
    return 0;
}
void host_gfi_rdiskette_change() { config_prefix_stop(20); }
short gfi_empty_active() {
    config_prefix_stop(21);
    return 0;
}
#endif
void host_using_fdisk(void) { config_prefix_stop(8); }
void host_fdisk_valid(void) { config_prefix_stop(9); }
void host_fdisk_change(void) { config_prefix_stop(10); }
void host_fdisk_active(void) { config_prefix_stop(11); }
char *host_find_file(void) { config_prefix_stop(12); return NULL; }
void host_block_timer(void) { config_prefix_stop(13); }
void host_release_timer(void) { config_prefix_stop(14); }
#ifdef CONFIG_PREFIX_TRACE_SESSION_INIT
DWORD TlsDirectError;
void ntdos64_session_trace_io_init(void) { config_prefix_stop(22); }
void ntdos64_session_trace_after_io_init(void) { config_prefix_stop(36); }
void ntdos64_session_trace_after_reset(void) { config_prefix_stop(37); }
void ntdos64_session_trace_before_reset(void) { config_prefix_stop(38); }
#ifdef NTDOS64_SESSION_TRACE_VINT
void ntdos64_session_trace_vint_checkpoint(int stage) {
    if (stage == NTDOS64_SESSION_TRACE_VINT_STOP_STAGE)
        config_prefix_stop(40 + stage);
}
#endif
/* These resolve the unused remainder of the original nt_msscs translation
 * unit. Every one is non-returning so this fixture cannot emulate a session. */
int CMDInit() { config_prefix_stop(23); return 0; }
#ifndef CONFIG_PREFIX_USE_HISTORICAL_DEM
int DemInit() { config_prefix_stop(24); return 0; }
#endif
void TerminateVDM() { config_prefix_stop(25); }
#ifndef CONFIG_PREFIX_USE_ORIGINAL_XMS
int XMSInit() { config_prefix_stop(26); return 0; }
#endif
int DBGInit() { config_prefix_stop(27); return 0; }
#ifndef CONFIG_PREFIX_TRACE_SESSION_AFTER_RESET
void reset() { config_prefix_stop(28); }
#endif
void lim_page_frame_init() { config_prefix_stop(29); }
int RcErrorDialogBox() { config_prefix_stop(30); return 0; }
#ifndef CONFIG_PREFIX_USE_HISTORICAL_NT_BOP
void *SafeLoadLibrary() { config_prefix_stop(31); return NULL; }
#endif
void *Sim32pGetVDMPointer() { config_prefix_stop(32); return NULL; }
int stdoutRedirected() { config_prefix_stop(33); return 0; }
void FloppyTerminatePDB() { config_prefix_stop(34); }
void FdiskTerminatePDB() { config_prefix_stop(35); }
#endif
#ifndef CONFIG_PREFIX_TRACE_SAS_INIT
void host_error(void) { config_prefix_stop(15); }
#endif

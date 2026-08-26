#ifndef RUNTIME_OPENNT_BOP_COMMAND_COMPOSITION_H
#define RUNTIME_OPENNT_BOP_COMMAND_COMPOSITION_H

#include "adapter-win32/facade/opennt_error_dialog_facade.h"
#include "adapter-win32/facade/opennt_command_oem_facade.h"
#include "adapter-win32/facade/opennt_vdm_api_facade.h"
#include "adapter-softpc/softpc_printer_openclose_shim.h"
#include "opennt-host/opennt_host_event_state.h"
#include "opennt-host/opennt_host_child_runtime.h"
#include "opennt-host/softpc.new/host/inc/nt_event.h"
#include "session/session_input.h"

/* Compatibility surface for the directly imported OpenNT file
 * src/opennt/base/mvdm/dos/command/cmdmisc.c.  It is deliberately a scoped
 * CCPU/SAS replacement, not a COMMAND provider or BOP dispatcher. */

#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "adapter-softpc/cpu_result.h"
#include "adapter-softpc/cpu_state_abi.h"
#include "adapter-softpc/exception_abi.h"
#include "host_handle_manager.h"

typedef uint16_t USHORT;
typedef uint16_t WORD;
typedef uint8_t BYTE;
typedef uint8_t UCHAR;
typedef char CHAR;
typedef char *PCHAR;
typedef char *LPSTR;
typedef void *PVOID;
typedef void *LPVOID;
typedef uint8_t BOOLEAN;
typedef void (*PFNSVC)(void);

#include "adapter-win32/facade/opennt_rtl_compat.h"

#define NT_SUCCESS(status) ((NTSTATUS)(status) >= 0)
#define ED_BADSYSFILE 336u
#define ED_INITMEMERR 337u
#define ED_INITTMPFILE 338u

/* Exact OpenNT cmd.h pipe layouts, admitted only for cmdredir.c's original
 * ownership.  They remain host-private and never cross the guest boundary. */
#define PIPE_INPUT_BUFFER_SIZE 512u
#define PIPE_OUTPUT_BUFFER_SIZE PIPE_INPUT_BUFFER_SIZE
#define PIPE_INPUT_TIMEOUT 55u
#define PIPE_OUTPUT_TIMEOUT PIPE_INPUT_TIMEOUT
typedef struct _PIPE_INPUT {
    struct _PIPE_INPUT *Next;
    HANDLE hFileRead, hFileWrite, hPipe, hDataEvent, hThread;
    CHAR *pFileName; DWORD BufferSize; BOOL fEOF, WaitData; BYTE *Buffer;
    CRITICAL_SECTION CriticalSection;
} PIPE_INPUT, *PPIPE_INPUT;
typedef struct _PIPE_OUTPUT {
    HANDLE hFile, hPipe, hExitEvent; CHAR *pFileName; DWORD BufferSize; BYTE *Buffer;
} PIPE_OUTPUT, *PPIPE_OUTPUT;

typedef struct _RedirComplete_Info {
    HANDLE ri_hStdErr, ri_hStdOut, ri_hStdIn;
    HANDLE ri_hStdErrFile, ri_hStdOutFile, ri_hStdInFile;
    HANDLE ri_hStdOutThread, ri_hStdErrThread;
    PPIPE_INPUT ri_pPipeStdIn;
    PPIPE_OUTPUT ri_pPipeStdOut, ri_pPipeStdErr;
} REDIRCOMPLETE_INFO, *PREDIRCOMPLETE_INFO;
/* Exact OpenNT vdmapi.h and cmdsvc.h records used by cmdGetNextCmd.  Pointer
 * members remain host-private; guest access is always copied by this shim. */
#ifndef CREATE_FORCEDOS
#define CREATE_FORCEDOS          0x00000200u
#endif
#define EXE_EXTENTION_STRING ".EXE"
#define COM_EXTENTION_STRING ".COM"
#define BAT_EXTENTION_STRING ".BAT"
#define EXTENTION_STRING_LEN 4u
#define EXE_EXTENTION 4u
#define COM_EXTENTION 8u
#define BAT_EXTENTION 2u
#define UNKNOWN_EXTENTION 9u
#pragma pack(push, 2)
typedef struct _CMDINFO {
    USHORT EnvSeg, EnvSize, CurDrive, NumDrives, CmdLineSeg, CmdLineOff;
    USHORT CmdLineSize, ReturnCode, bStdHandles;
    ULONG pRdrInfo;
    USHORT CodePage, fTSRExit, fBatStatus, ExecPathSeg, ExecPathOff;
    USHORT ExecPathSize, ExecExtType;
} CMDINFO, *PCMDINFO;
#pragma pack(pop)
typedef struct _VDMENVBLK { DWORD cchEnv, cchRemain; CHAR *lpszzEnv; } VDMENVBLK, *PVDMENVBLK;
/* OpenNT command/cmdpif.h: retain the original PIF record layout because
 * cmdpif.c and the original nt_pif.c parser exchange it directly. */
typedef struct _BX_NTVDM_PIF_DATA {
    char *WinTitle;
    char *CmdLine;
    char *StartDir;
    char *StartFile;
    WORD fullorwin;
    WORD graphicsortext;
    WORD memreq;
    WORD memdes;
    WORD emsreq;
    WORD emsdes;
    WORD xmsreq;
    WORD xmsdes;
    char menuclose;
    char reskey;
    WORD ShortMod;
    WORD ShortScan;
    char idledetect;
    char CloseOnExit;
    char AppHasPIFFile;
    char IgnoreTitleInPIF;
    char IgnoreStartDirInPIF;
    char IgnoreShortKeyInPIF;
    char IgnoreCmdLineInPIF;
    char IgnoreConfigAutoexec;
    char SubSysId;
} PIF_DATA;
#define MASK_STDIN  1u
#define MASK_STDOUT 2u
#define MASK_STDERR 4u

#define HANDLE_STDIN  0u
#define HANDLE_STDOUT 1u
#define HANDLE_STDERR 2u
#define EG_MALLOC_FAILURE ERROR_NOT_ENOUGH_MEMORY
#define EG_ENVIRONMENT_ERR ERROR_ENVVAR_NOT_FOUND
#define EXIT_NO_CLOSE 0u
#define RMB_ICON_BANG 0x00000010u
/* OpenNT softpc.new/host/inc/error.h keeps the dialog result and option bit
 * at one.  This same source-visible spelling is shared by cmdenv/cmdpif and
 * nt_pif; do not translate it per caller. */
#define RMB_ABORT 0x00000001u

#pragma pack(push, 1)
typedef struct _PARAMBLOCK {
    USHORT SegEnv;
    USHORT OffCmdTail;
    USHORT SegCmdTail;
    ULONG pFCB1;
    ULONG pFCB2;
} PARAMBLOCK, *PPARAMBLOCK;
typedef struct _SCSINFO {
    CHAR SCS_ComSpec[64];
    CHAR SCS_CmdTail[128];
    PARAMBLOCK SCS_ParamBlock;
    CHAR SCS_ToSync;
} SCSINFO, *PSCSINFO;
#pragma pack(pop)

#define MAXIMUM_VDM_CURRENT_DIR 64u
#define RUNTIME_COMMAND_MISC_COMSPEC_MAX 64u
#define RUNTIME_COMMAND_MISC_CURRENT_DIR_BYTES (MAXIMUM_VDM_CURRENT_DIR + 3u)
#define RUNTIME_COMMAND_CONTINUATION_COMMAND_MAX OPENNT_HOST_CHILD_COMMAND_MAX
#define RUNTIME_COMMAND_CONTINUATION_ENV_MAX OPENNT_HOST_CHILD_ENV_MAX
#define MAX_SHORTCUT_SIZE 128u

enum runtime_command_misc_service {
    RUNTIME_COMMAND_MISC_EXIT = 0x00u,
    RUNTIME_COMMAND_MISC_GET_NEXT = 0x01u,
    RUNTIME_COMMAND_MISC_COMSPEC = 0x02u,
    RUNTIME_COMMAND_MISC_SAVE_WORLD = 0x03u,
    RUNTIME_COMMAND_MISC_GET_CURRENT_DIR = 0x04u,
    RUNTIME_COMMAND_MISC_SET_INFO = 0x05u,
    RUNTIME_COMMAND_MISC_INIT_CONSOLE = 0x09u,
    RUNTIME_COMMAND_MISC_GET_CONFIG_SYS = 0x0cu,
    RUNTIME_COMMAND_MISC_GET_AUTOEXEC_BAT = 0x0du,
    RUNTIME_COMMAND_MISC_GET_KBD_LAYOUT = 0x0eu,
    RUNTIME_COMMAND_MISC_GET_INIT_ENVIRONMENT = 0x0fu,
    RUNTIME_COMMAND_MISC_CHECK_BINARY = 0x07u,
    RUNTIME_COMMAND_MISC_EXEC = 0x08u,
    RUNTIME_COMMAND_MISC_EXEC_COMSPEC32 = 0x0au,
    RUNTIME_COMMAND_MISC_RETURN_EXIT_CODE = 0x0bu,
    RUNTIME_COMMAND_MISC_GET_START_INFO = 0x10u
};

typedef struct runtime_command_misc_session {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t scs_info_address;
    uint32_t is_dos_binary_address;
    uint32_t fd_access_address;
    uint32_t console_initialized;
    uint32_t dos_session_id;
    CHAR comspec[64 + 8];
    USHORT comspec_bytes;
    CHAR config_input_path[MAX_PATH + 13u];
    CHAR autoexec_input_path[MAX_PATH + 13u];
    /* The original cmdconf.c writes the NTVDM-installed COMMAND.COM path
     * into the guest's SHELL= line.  This is the admitted CLI bundle's
     * equivalent, reduced to the guest sysinit `commnd` 64-byte limit. */
    CHAR bootstrap_command_path[64u];
    uint32_t redirection_token;
    REDIRCOMPLETE_INFO redirection_info;
    runtime_host_handle_manager *handles;
    SCSINFO scs_info;
    BYTE is_dos_binary;
    WORD fd_access;
    /* The modern historical-VDM API facade reads copied input/publish state
     * through this neutral session record; it never receives this COMMAND
     * source object or guest state. */
    session_input input;
    /* Source-shaped COMMAND state ends above. The host child record contains
     * only copied execution inputs and opaque IDs; no native HANDLE enters
     * this session object or guest RAM. */
    opennt_host_child_record pending;
} runtime_command_misc_session;

#define RUNTIME_COMMAND_MISC_SESSION_MAGIC 0x42584353u
#define RUNTIME_COMMAND_MISC_SESSION_VERSION 6u

void runtime_command_misc_session_initialize(runtime_command_misc_session *session);
void runtime_command_misc_session_dispose(runtime_command_misc_session *session);
int runtime_command_misc_session_valid(const runtime_command_misc_session *session);
int runtime_command_misc_session_set_command_source(
    runtime_command_misc_session *session, const CHAR *application,
    const CHAR *tail, USHORT drive, USHORT code_page);
int runtime_command_misc_session_set_command_environment(
    runtime_command_misc_session *session, const CHAR *environment,
    uint32_t bytes);

typedef int (*runtime_command_misc_guest_read_fn)(void *state,
    uint32_t physical_address, uint8_t *buffer, uint32_t bytes);
typedef int (*runtime_command_misc_guest_write_fn)(void *state,
    uint32_t physical_address, const uint8_t *buffer, uint32_t bytes);

typedef struct runtime_command_misc_call {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t service;
    const runtime_exception_event *boundary;
    const runtime_cpu_state *cpu;
    runtime_cpu_result *result;
    void *guest_state;
    runtime_command_misc_guest_read_fn guest_read;
    runtime_command_misc_guest_write_fn guest_write;
    runtime_command_misc_session *session;
    uint32_t first_call;
    uint32_t vdm_for_wow;
} runtime_command_misc_call;

#define RUNTIME_COMMAND_MISC_CALL_MAGIC 0x4258434du
#define RUNTIME_COMMAND_MISC_CALL_VERSION 1u

int runtime_command_misc_call_valid(const runtime_command_misc_call *call);
int runtime_command_misc_invoke(runtime_command_misc_call *call);
int runtime_command_misc_invoke_body(runtime_command_misc_call *call,
    void (*body)(void));

/* Original COMMAND service entries retained by cmddisp.c's table. */
BOOL CmdDispatch(ULONG service);
BOOL CMDInit(int argc, char *argv[]);
void cmdExitVDM(void); void cmdGetNextCmd(void); void cmdComSpec(void);
void cmdCreateProcess(void);
void cmdSaveWorld(void); void cmdGetCurrentDir(void); void cmdSetInfo(void);
void cmdGetStdHandle(void); void cmdCheckBinary(void); void cmdExec(void);
void cmdInitConsole(void); void cmdExecComspec32(void); void cmdReturnExitCode(void);
void cmdGetConfigSys(void); void cmdGetAutoexecBat(void); void cmdGetKbdLayout(void);
void cmdGetInitEnvironment(void); void cmdGetStartInfo(void);

USHORT runtime_command_misc_get_dx(void);
USHORT runtime_command_misc_get_bx(void);
USHORT runtime_command_misc_get_cx(void);
USHORT runtime_command_misc_get_si(void);
USHORT runtime_command_misc_get_ds(void);
USHORT runtime_command_misc_get_es(void);
USHORT runtime_command_misc_get_ss(void);
USHORT runtime_command_misc_get_bp(void);
USHORT runtime_command_misc_get_ax(void);
USHORT runtime_command_misc_get_cs(void);
USHORT runtime_command_misc_get_ip(void);
UCHAR runtime_command_misc_get_al(void);
UCHAR runtime_command_misc_get_ah(void);
void runtime_command_misc_set_ax(USHORT value);
void runtime_command_misc_set_al(USHORT value);
void runtime_command_misc_set_cf(int value);
void runtime_command_misc_set_dx(USHORT value);
void runtime_command_misc_set_bx(USHORT value);
void runtime_command_misc_set_cx(USHORT value);
void runtime_command_misc_set_ds(USHORT value);
void runtime_command_misc_set_es(USHORT value);
void runtime_command_misc_set_ip(USHORT value);
void runtime_command_misc_sas_load(ULONG address, UCHAR *target);
BOOL runtime_command_misc_dispatch_source_command(ULONG service);
BOOL runtime_command_misc_dispatch_source_command(ULONG service);
runtime_command_misc_session *runtime_command_misc_active_session(void);
PREDIRCOMPLETE_INFO runtime_command_misc_redirection_from_guest(uint32_t token);
int runtime_command_misc_publish_handle(HANDLE handle);
BOOL cmdHandleStdinWithPipe(PREDIRCOMPLETE_INFO pRdrInfo);
BOOL cmdHandleStdOutErrWithPipe(PREDIRCOMPLETE_INFO pRdrInfo, USHORT handle_type);
BOOL cmdCreateTempFile(PHANDLE handle, PCHAR *name);
VOID cmdPipeOutThread(LPVOID parameter);
VOID cmdPipeInThread(LPVOID parameter);
BOOL cmdPipeFileDataEOF(HANDLE file, BOOL *eof_out);
BOOL cmdPipeFileEOF(HANDLE file);
void RcErrorDialogBox(UINT error, PVOID first, PVOID second);
int RcMessageBox(UINT error, PVOID first, PVOID second, UINT flags);
void TerminateVDM(void);
extern BOOL fSoftpcRedirection;
#define ASSERT(value) ((void)(value))
LPVOID runtime_command_misc_get_vdm_addr(USHORT segment, USHORT offset);
void nt_init_event_thread(void);
void cmdUpdateCurrentDirectories(BYTE current_drive);
extern CHAR *lpszzCurrentDirectories;
extern DWORD cchCurrentDirectories;
void cmdSetDirectories(PCHAR environment, PVDMINFO vdm_info);
BOOL cmdCheckCopyForRedirection(PREDIRCOMPLETE_INFO info);
BOOL cmdCreateVDMEnvironment(PVDMENVBLK block);
BOOL cmdXformEnvironment(PCHAR environment16, PANSI_STRING environment_a);
BOOL cmdSetEnvironmentVariable(PVDMENVBLK block, PCHAR name, PCHAR value);
DWORD cmdExpandEnvironmentStrings(PVDMENVBLK block, PCHAR source, PCHAR destination,
    DWORD destination_bytes);
DWORD cmdGetEnvironmentVariable(PVDMENVBLK block, PCHAR name, PCHAR value,
    DWORD value_bytes);
void cmdCheckForPIF(PVDMINFO vdm_info);
USHORT cmdMapCodePage(ULONG code_page);
PREDIRCOMPLETE_INFO cmdCheckStandardHandles(PVDMINFO vdm_info,
    USHORT UNALIGNED *standard_handles);
void demCloseAllPSPRecords(void);
VOID cmdInitConsole(VOID);
#define GetConsoleKeyboardLayoutNameA runtime_opennt_console_keyboard_layout_name
#define GetConsoleKeyboardLayoutName runtime_opennt_console_keyboard_layout_name

UCHAR demGetPhysicalDriveType(UCHAR drive);

/* command_config_shim.c is the smallest source-derived replacement for the
 * non-composable SoftPC/PIF/CCPU closure consumed by cmdconf.c. */
void GetPIFConfigFiles(BOOL bConfig, CHAR *file_name);
void cmdGetConfigSys(void);
void cmdGetAutoexecBat(void);
void DeleteConfigFiles(void);
void cmdGetInitEnvironment(void);
void runtime_command_config_set_inputs(runtime_command_misc_session *session,
    const CHAR *config_path, const CHAR *autoexec_path);
int runtime_command_config_set_bootstrap_command(
    runtime_command_misc_session *session, const CHAR *command_path);
const CHAR *runtime_command_config_bootstrap_command(void);
/* `cmdXformEnvironment` historically snapshots the process environment.
 * These helpers instead expose a caller-owned copy of the active session's
 * ANSI multisz; it never aliases guest memory or ambient host state. */
PWCHAR runtime_command_environment_snapshot(void);
PWCHAR runtime_command_environment_snapshot_session(
    const runtime_command_misc_session *session);
void runtime_command_environment_free_snapshot(PWCHAR snapshot);
uint32_t runtime_command_binary_scs_address(uint32_t offset);
BOOL IsWowAppRunnable(LPSTR app_name);
/* T236 S2 preserves the imported detached cmdCreateProcess body. These seams
 * replace only CCPU/CSR transport and process-global standard-handle
 * installation; no HANDLE enters guest/session ABI. */
BOOL runtime_command_worker_prepare_startup(STARTUPINFO *startup);
BOOL runtime_command_worker_reentry_pending(void);
BOOL runtime_command_create_process(LPCSTR application, LPSTR command,
    LPSECURITY_ATTRIBUTES process_attributes, LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit_handles, DWORD creation_flags, LPVOID environment, LPCSTR current_directory,
    LPSTARTUPINFOA startup, LPPROCESS_INFORMATION process_information);
void runtime_command_worker_attach_process(HANDLE process);
void runtime_command_worker_finish(BOOL child_created, DWORD exit_code);
/* These are the bounded replacement for the NT4 CCPU/CSR transport.  They
 * preserve cmdExec32's worker ordering while retaining no active BOP call. */
BOOL runtime_command_worker_begin(PCHAR command, PCHAR environment);
BOOL runtime_command_worker_complete(void);
DWORD WINAPI runtime_command_worker_thread(LPVOID ignored);
BOOL runtime_command_misc_set_pending(void);

#ifndef SCS_DOS_BINARY
#define SCS_DOS_BINARY 1u
#endif
#ifndef SCS_WOW_BINARY
#define SCS_WOW_BINARY 2u
#endif
#define WOWCF_NOTDOSSPAWNABLE 0x00000001u
#define FETCHWORD(value) (value)
#define STOREWORD(target, value) ((target) = (USHORT)(value))
#define STOREDWORD(target, value) ((target) = (ULONG)(value))
extern PCHAR lpszzcmdEnv16;
extern PCHAR pCommand32;
extern PCHAR pEnv32;

extern CHAR lpszComSpec[64 + 8];
extern USHORT cbComSpec;
extern BOOL IsFirstCall;
extern BOOL VDMForWOW;
extern PSCSINFO pSCSInfo;
extern PCHAR pSCS_ToSync;
extern BYTE *pIsDosBinary;
extern WORD *pFDAccess;
extern BOOL bPifFastPaste;
extern ULONG DosSessionId;
extern CHAR comspec[];
extern CHAR *lpszzInitEnvironment;
extern WORD cchInitEnvironment;
extern CHAR *lpszzVDMEnv32;
extern DWORD cchVDMEnv32;
extern BOOL IsRepeatCall;
extern BOOL DosEnvCreated;
extern BOOL IsFirstVDM;
extern BOOL fBlock;
extern WORD Exe32ActiveCount;
extern USHORT nDrives;
extern VDMINFO VDMInfo;
extern VDMENVBLK cmdVDMEnvBlk;
extern CHAR cmdHomeDirectory[];
extern CHAR chDefaultDrive;
extern DWORD dwExitCode32;
extern BOOL fSoftpcRedirectionOnShellOut;
extern PIF_DATA pfdata;
extern UINT VdmExitCode;
extern BOOL DontCheckDosBinaryType;
extern BOOL IsFirstWOWCheckBinary;
ULONG runtime_command_misc_redirection_token(PREDIRCOMPLETE_INFO info);

#define getDX() runtime_command_misc_get_dx()
#define getBX() runtime_command_misc_get_bx()
#define getCX() runtime_command_misc_get_cx()
#define getSI() runtime_command_misc_get_si()
#define getDS() runtime_command_misc_get_ds()
#define getES() runtime_command_misc_get_es()
#define getSS() runtime_command_misc_get_ss()
#define getBP() runtime_command_misc_get_bp()
#define getAL() runtime_command_misc_get_al()
#define getAH() runtime_command_misc_get_ah()
#define getAX() runtime_command_misc_get_ax()
#define setAX(value) runtime_command_misc_set_ax(value)
#define setAL(value) runtime_command_misc_set_al(value)
#define setCF(value) runtime_command_misc_set_cf(value)
#define setDX(value) runtime_command_misc_set_dx(value)
#define setBX(value) runtime_command_misc_set_bx(value)
#define setCX(value) runtime_command_misc_set_cx(value)
#define setDS(value) runtime_command_misc_set_ds(value)
#define setES(value) runtime_command_misc_set_es(value)
#define GetVDMAddr(segment, offset) runtime_command_misc_get_vdm_addr(segment, offset)
/* The production default is the public Win32 system directory.  The narrow
 * test override only supplies historical KB16 fixture media; cmdkeyb.c keeps
 * its original registry, file-presence and result algorithm. */
#undef GetSystemDirectory
#define GetSystemDirectory(buffer, bytes) runtime_command_misc_get_system_directory((buffer), (bytes))
/* DIVERGENCE (T236): the NT4 environment carried hidden `=X:` current-drive
 * entries. Public modern process environments do not reliably expose those
 * entries, so preserve cmdUpdateCurrentDirectories' source order through a
 * checked session/process-current-directory fallback. */
#undef GetEnvironmentVariable
#define GetEnvironmentVariable(name, buffer, bytes) \
    runtime_opennt_command_environment_variable((name), (buffer), (bytes))

#endif

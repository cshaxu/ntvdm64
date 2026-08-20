#ifndef BX_NTVDM_BOP_SHIM_COMMAND_MISC_SHIM_H
#define BX_NTVDM_BOP_SHIM_COMMAND_MISC_SHIM_H

/* Compatibility surface for the directly imported OpenNT file
 * src/opennt/base/mvdm/dos/command/cmdmisc.c.  It is deliberately a scoped
 * CCPU/SAS replacement, not a COMMAND provider or BOP dispatcher. */

#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "../../bx_ntvdm_cpu_result_v2.h"
#include "../../bx_ntvdm_cpu_state_abi.h"
#include "../../bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_host_handle_manager.h"

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

/* The OpenNT command configuration owner uses these NT string layouts.  They
 * are reproduced here rather than pulling the old ntdll import closure into
 * the modern in-process target. */
typedef LONG NTSTATUS;
typedef struct _ANSI_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} ANSI_STRING, *PANSI_STRING;
typedef ANSI_STRING OEM_STRING, *POEM_STRING;
typedef ANSI_STRING STRING, *PSTRING;
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

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
#define ASKING_FOR_FIRST_COMMAND 0x0001u
#define ASKING_FOR_DOS_BINARY    0x0004u
#define ASKING_FOR_SECOND_TIME   0x0008u
#define ASKING_FOR_ENVIRONMENT   0x0400u
#define NO_PARENT_TO_WAKE        0x0040u
#define RETURN_ON_NO_COMMAND     0x0080u
#define CNTRL_SHELLCOUNT         0x0000ffffu
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
typedef struct _VDMINFO {
    ULONG iTask, dwCreationFlags, ErrorCode, CodePage;
    HANDLE StdIn, StdOut, StdErr;
    LPVOID CmdLine, AppName, PifFile, CurDirectory, Enviornment;
    ULONG EnviornmentSize;
    STARTUPINFOA StartupInfo;
    LPVOID Desktop; ULONG DesktopLen; LPVOID Title; ULONG TitleLen;
    LPVOID Reserved; ULONG ReservedLen;
    USHORT CmdSize, AppLen, PifLen, CurDirectoryLen, VDMState, CurDrive;
    BOOLEAN fComingFromBat;
} VDMINFO, *PVDMINFO;
typedef struct _VDMENVBLK { DWORD cchEnv, cchRemain; CHAR *lpszzEnv; } VDMENVBLK, *PVDMENVBLK;
typedef struct _BX_NTVDM_PIF_DATA { BOOL CloseOnExit; } PIF_DATA;
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
#define RMB_ABORT 0x00000002u

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
#define BX_NTVDM_COMMAND_MISC_COMSPEC_MAX 64u
#define BX_NTVDM_COMMAND_MISC_CURRENT_DIR_BYTES (MAXIMUM_VDM_CURRENT_DIR + 3u)

enum bx_ntvdm_command_misc_service {
    BX_NTVDM_COMMAND_MISC_EXIT = 0x00u,
    BX_NTVDM_COMMAND_MISC_GET_NEXT = 0x01u,
    BX_NTVDM_COMMAND_MISC_COMSPEC = 0x02u,
    BX_NTVDM_COMMAND_MISC_SAVE_WORLD = 0x03u,
    BX_NTVDM_COMMAND_MISC_GET_CURRENT_DIR = 0x04u,
    BX_NTVDM_COMMAND_MISC_SET_INFO = 0x05u,
    BX_NTVDM_COMMAND_MISC_INIT_CONSOLE = 0x09u,
    BX_NTVDM_COMMAND_MISC_GET_CONFIG_SYS = 0x0cu,
    BX_NTVDM_COMMAND_MISC_GET_AUTOEXEC_BAT = 0x0du,
    BX_NTVDM_COMMAND_MISC_GET_KBD_LAYOUT = 0x0eu,
    BX_NTVDM_COMMAND_MISC_GET_INIT_ENVIRONMENT = 0x0fu,
    BX_NTVDM_COMMAND_MISC_CHECK_BINARY = 0x07u,
    BX_NTVDM_COMMAND_MISC_EXEC = 0x08u,
    BX_NTVDM_COMMAND_MISC_EXEC_COMSPEC32 = 0x0au,
    BX_NTVDM_COMMAND_MISC_RETURN_EXIT_CODE = 0x0bu,
    BX_NTVDM_COMMAND_MISC_GET_START_INFO = 0x10u
};

typedef struct bx_ntvdm_command_misc_session {
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
    uint32_t redirection_token;
    REDIRCOMPLETE_INFO redirection_info;
    bx_ntvdm_host_handle_manager handles;
    SCSINFO scs_info;
    BYTE is_dos_binary;
    WORD fd_access;
    /* Direct CLI input at the historical BaseSrv GetNextVDMCommand seam. */
    CHAR command_source_app[MAX_PATH + 1u];
    CHAR command_source_tail[128u];
    USHORT command_source_drive;
    USHORT command_source_code_page;
    uint32_t command_source_ready;
    uint32_t command_source_delivered;
    uint32_t command_source_repeat_pending;
    uint32_t command_source_environment_bytes;
    CHAR command_source_environment[1024u];
    uint32_t command_source_vdm_environment_bytes;
    CHAR command_source_vdm_environment[1024u];
} bx_ntvdm_command_misc_session;

#define BX_NTVDM_COMMAND_MISC_SESSION_MAGIC 0x42584353u
#define BX_NTVDM_COMMAND_MISC_SESSION_VERSION 1u

void bx_ntvdm_command_misc_session_initialize(bx_ntvdm_command_misc_session *session);
int bx_ntvdm_command_misc_session_valid(const bx_ntvdm_command_misc_session *session);
int bx_ntvdm_command_misc_session_set_command_source(
    bx_ntvdm_command_misc_session *session, const CHAR *application,
    const CHAR *tail, USHORT drive, USHORT code_page);
int bx_ntvdm_command_misc_session_set_command_environment(
    bx_ntvdm_command_misc_session *session, const CHAR *environment,
    uint32_t bytes);

typedef int (*bx_ntvdm_command_misc_guest_read_fn)(void *state,
    uint32_t physical_address, uint8_t *buffer, uint32_t bytes);
typedef int (*bx_ntvdm_command_misc_guest_write_fn)(void *state,
    uint32_t physical_address, const uint8_t *buffer, uint32_t bytes);

typedef struct bx_ntvdm_command_misc_call {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t service;
    const bx_ntvdm_exception_event_v1 *boundary;
    const bx_ntvdm_cpu_state_v1 *cpu;
    bx_ntvdm_cpu_result_v2 *result;
    void *guest_state;
    bx_ntvdm_command_misc_guest_read_fn guest_read;
    bx_ntvdm_command_misc_guest_write_fn guest_write;
    bx_ntvdm_command_misc_session *session;
    uint32_t first_call;
    uint32_t vdm_for_wow;
} bx_ntvdm_command_misc_call;

#define BX_NTVDM_COMMAND_MISC_CALL_MAGIC 0x4258434du
#define BX_NTVDM_COMMAND_MISC_CALL_VERSION 1u

int bx_ntvdm_command_misc_call_valid(const bx_ntvdm_command_misc_call *call);
int bx_ntvdm_command_misc_invoke(bx_ntvdm_command_misc_call *call);

/* Original COMMAND service entries retained by cmddisp.c's table. */
void cmdExitVDM(void); void cmdGetNextCmd(void); void cmdComSpec(void);
void cmdSaveWorld(void); void cmdGetCurrentDir(void); void cmdSetInfo(void);
void cmdGetStdHandle(void); void cmdCheckBinary(void); void cmdExec(void);
void cmdInitConsole(void); void cmdExecComspec32(void); void cmdReturnExitCode(void);
void cmdGetConfigSys(void); void cmdGetAutoexecBat(void); void cmdGetKbdLayout(void);
void cmdGetInitEnvironment(void); void cmdGetStartInfo(void);

USHORT bx_ntvdm_command_misc_get_dx(void);
USHORT bx_ntvdm_command_misc_get_bx(void);
USHORT bx_ntvdm_command_misc_get_cx(void);
USHORT bx_ntvdm_command_misc_get_si(void);
USHORT bx_ntvdm_command_misc_get_ds(void);
USHORT bx_ntvdm_command_misc_get_es(void);
USHORT bx_ntvdm_command_misc_get_ss(void);
USHORT bx_ntvdm_command_misc_get_bp(void);
USHORT bx_ntvdm_command_misc_get_ax(void);
UCHAR bx_ntvdm_command_misc_get_al(void);
UCHAR bx_ntvdm_command_misc_get_ah(void);
void bx_ntvdm_command_misc_set_ax(USHORT value);
void bx_ntvdm_command_misc_set_al(USHORT value);
void bx_ntvdm_command_misc_set_cf(int value);
void bx_ntvdm_command_misc_set_dx(USHORT value);
void bx_ntvdm_command_misc_set_bx(USHORT value);
void bx_ntvdm_command_misc_set_cx(USHORT value);
void bx_ntvdm_command_misc_set_ds(USHORT value);
void bx_ntvdm_command_misc_set_es(USHORT value);
bx_ntvdm_command_misc_session *bx_ntvdm_command_misc_active_session(void);
PREDIRCOMPLETE_INFO bx_ntvdm_command_misc_redirection_from_guest(uint32_t token);
int bx_ntvdm_command_misc_publish_handle(HANDLE handle);
BOOL cmdHandleStdinWithPipe(PREDIRCOMPLETE_INFO pRdrInfo);
BOOL cmdHandleStdOutErrWithPipe(PREDIRCOMPLETE_INFO pRdrInfo, USHORT handle_type);
BOOL cmdCreateTempFile(PHANDLE handle, PCHAR *name);
VOID cmdPipeOutThread(LPVOID parameter);
VOID cmdPipeInThread(LPVOID parameter);
BOOL cmdPipeFileDataEOF(HANDLE file, BOOL *eof_out);
BOOL cmdPipeFileEOF(HANDLE file);
void RcErrorDialogBox(UINT error, PVOID first, PVOID second);
void RcMessageBox(UINT error, PVOID first, PVOID second, UINT flags);
void TerminateVDM(void);
void nt_std_handle_notification(BOOL enabled);
extern BOOL fSoftpcRedirection;
#define ASSERT(value) ((void)(value))
LPVOID bx_ntvdm_command_misc_get_vdm_addr(USHORT segment, USHORT offset);
void nt_init_event_thread(void);
UINT bx_ntvdm_command_misc_get_system_directory(LPSTR buffer, UINT bytes);
void bx_ntvdm_command_misc_set_test_system_directory(const CHAR *path);
BOOL GetNextVDMCommand(PVDMINFO vdm_info);
void host_lpt_flush_initialize(void);
void cmdUpdateCurrentDirectories(BYTE current_drive);
void cmdSetDirectories(PCHAR environment, PVDMINFO vdm_info);
BOOL cmdCheckCopyForRedirection(PREDIRCOMPLETE_INFO info);
BOOL cmdCreateVDMEnvironment(PVDMENVBLK block);
void cmdCheckForPIF(PVDMINFO vdm_info);
USHORT cmdMapCodePage(ULONG code_page);
PREDIRCOMPLETE_INFO cmdCheckStandardHandles(PVDMINFO vdm_info,
    USHORT UNALIGNED *standard_handles);
void cmdPushExitInConsoleBuffer(void);
void demCloseAllPSPRecords(void);
void nt_block_event_thread(int block);
void nt_resume_event_thread(void);
VOID cmdInitConsole(VOID);
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name);
#define GetConsoleKeyboardLayoutName GetConsoleKeyboardLayoutNameA

UCHAR demGetPhysicalDriveType(UCHAR drive);
UINT GetDriveTypeOem(LPSTR root);
DWORD GetEnvironmentVariableOem(LPSTR name, LPSTR buffer, DWORD bytes);
BOOL SetEnvironmentVariableOem(LPSTR name, LPSTR value);

/* command_config_shim.c is the smallest source-derived replacement for the
 * non-composable SoftPC/PIF/CCPU closure consumed by cmdconf.c. */
void GetPIFConfigFiles(BOOL bConfig, CHAR *file_name);
void cmdGetConfigSys(void);
void cmdGetAutoexecBat(void);
void DeleteConfigFiles(void);
void cmdGetInitEnvironment(void);
void bx_ntvdm_command_config_set_inputs(bx_ntvdm_command_misc_session *session,
    const CHAR *config_path, const CHAR *autoexec_path);
void RtlInitAnsiString(PANSI_STRING destination, const CHAR *source);
NTSTATUS RtlAnsiStringToUnicodeString(PUNICODE_STRING destination,
    const PANSI_STRING source, BOOL allocate_destination);
NTSTATUS RtlUnicodeStringToOemString(POEM_STRING destination,
    const PUNICODE_STRING source, BOOL allocate_destination);
void RtlFreeUnicodeString(PUNICODE_STRING string);
void RtlInitString(PSTRING destination, const CHAR *source);
NTSTATUS RtlOemStringToUnicodeString(PUNICODE_STRING destination,
    const POEM_STRING source, BOOL allocate_destination);
NTSTATUS RtlUnicodeStringToAnsiString(PANSI_STRING destination,
    const PUNICODE_STRING source, BOOL allocate_destination);
void RtlFreeAnsiString(PANSI_STRING string);
ULONG RtlNtStatusToDosError(NTSTATUS status);
uint32_t bx_ntvdm_command_binary_scs_address(uint32_t offset);
BOOL IsWowAppRunnable(LPSTR app_name);
void bx_ntvdm_command_lifecycle_exec(LPSTR command, LPSTR environment);

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
extern CHAR cmdHomeDirectory[MAX_PATH + 1u];
extern CHAR chDefaultDrive;
extern DWORD dwExitCode32;
extern BOOL fSoftpcRedirectionOnShellOut;
extern ULONG CntrlHandlerState;
extern PIF_DATA pfdata;
extern UINT VdmExitCode;
extern BOOL DontCheckDosBinaryType;
extern BOOL IsFirstWOWCheckBinary;
ULONG bx_ntvdm_command_misc_redirection_token(PREDIRCOMPLETE_INFO info);

#define getDX() bx_ntvdm_command_misc_get_dx()
#define getBX() bx_ntvdm_command_misc_get_bx()
#define getCX() bx_ntvdm_command_misc_get_cx()
#define getSI() bx_ntvdm_command_misc_get_si()
#define getDS() bx_ntvdm_command_misc_get_ds()
#define getES() bx_ntvdm_command_misc_get_es()
#define getSS() bx_ntvdm_command_misc_get_ss()
#define getBP() bx_ntvdm_command_misc_get_bp()
#define getAL() bx_ntvdm_command_misc_get_al()
#define getAH() bx_ntvdm_command_misc_get_ah()
#define getAX() bx_ntvdm_command_misc_get_ax()
#define setAX(value) bx_ntvdm_command_misc_set_ax(value)
#define setAL(value) bx_ntvdm_command_misc_set_al(value)
#define setCF(value) bx_ntvdm_command_misc_set_cf(value)
#define setDX(value) bx_ntvdm_command_misc_set_dx(value)
#define setBX(value) bx_ntvdm_command_misc_set_bx(value)
#define setCX(value) bx_ntvdm_command_misc_set_cx(value)
#define setDS(value) bx_ntvdm_command_misc_set_ds(value)
#define setES(value) bx_ntvdm_command_misc_set_es(value)
#define GetVDMAddr(segment, offset) bx_ntvdm_command_misc_get_vdm_addr(segment, offset)
#if defined(BX_NTVDM_COMMAND_EXEC_ADMIT_LIFECYCLE)
#define cmdExec32(command, environment) bx_ntvdm_command_lifecycle_exec((command), (environment))
#endif
/* The production default is the public Win32 system directory.  The narrow
 * test override only supplies historical KB16 fixture media; cmdkeyb.c keeps
 * its original registry, file-presence and result algorithm. */
#undef GetSystemDirectory
#define GetSystemDirectory(buffer, bytes) bx_ntvdm_command_misc_get_system_directory((buffer), (bytes))

#endif

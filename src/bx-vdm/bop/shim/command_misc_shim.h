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

typedef uint16_t USHORT;
typedef uint16_t WORD;
typedef uint8_t BYTE;
typedef uint8_t UCHAR;
typedef char CHAR;
typedef char *PCHAR;
typedef char *LPSTR;
typedef void *PVOID;
typedef void *LPVOID;

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
typedef struct _VDMINFO { HANDLE StdIn, StdOut, StdErr; } VDMINFO, *PVDMINFO;
#define MASK_STDIN  1u
#define MASK_STDOUT 2u
#define MASK_STDERR 4u

#define HANDLE_STDIN  0u
#define HANDLE_STDOUT 1u
#define HANDLE_STDERR 2u
#define EG_MALLOC_FAILURE ERROR_NOT_ENOUGH_MEMORY

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
    BX_NTVDM_COMMAND_MISC_COMSPEC = 0x02u,
    BX_NTVDM_COMMAND_MISC_SAVE_WORLD = 0x03u,
    BX_NTVDM_COMMAND_MISC_GET_CURRENT_DIR = 0x04u,
    BX_NTVDM_COMMAND_MISC_SET_INFO = 0x05u,
    BX_NTVDM_COMMAND_MISC_INIT_CONSOLE = 0x09u,
    BX_NTVDM_COMMAND_MISC_GET_KBD_LAYOUT = 0x0eu
};

typedef struct bx_ntvdm_command_misc_session {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t scs_info_address;
    uint32_t is_dos_binary_address;
    uint32_t fd_access_address;
    uint32_t console_initialized;
    uint32_t redirection_token;
    REDIRCOMPLETE_INFO redirection_info;
    HANDLE handle_tokens[64];
    SCSINFO scs_info;
    BYTE is_dos_binary;
    WORD fd_access;
} bx_ntvdm_command_misc_session;

#define BX_NTVDM_COMMAND_MISC_SESSION_MAGIC 0x42584353u
#define BX_NTVDM_COMMAND_MISC_SESSION_VERSION 1u

void bx_ntvdm_command_misc_session_initialize(bx_ntvdm_command_misc_session *session);
int bx_ntvdm_command_misc_session_valid(const bx_ntvdm_command_misc_session *session);

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

USHORT bx_ntvdm_command_misc_get_dx(void);
USHORT bx_ntvdm_command_misc_get_bx(void);
USHORT bx_ntvdm_command_misc_get_cx(void);
USHORT bx_ntvdm_command_misc_get_si(void);
USHORT bx_ntvdm_command_misc_get_ds(void);
USHORT bx_ntvdm_command_misc_get_ax(void);
UCHAR bx_ntvdm_command_misc_get_al(void);
void bx_ntvdm_command_misc_set_ax(USHORT value);
void bx_ntvdm_command_misc_set_al(USHORT value);
void bx_ntvdm_command_misc_set_cf(int value);
void bx_ntvdm_command_misc_set_dx(USHORT value);
void bx_ntvdm_command_misc_set_bx(USHORT value);
void bx_ntvdm_command_misc_set_cx(USHORT value);
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
void TerminateVDM(void);
void nt_std_handle_notification(BOOL enabled);
extern BOOL fSoftpcRedirection;
#define ASSERT(value) ((void)(value))
LPVOID bx_ntvdm_command_misc_get_vdm_addr(USHORT segment, USHORT offset);
void nt_init_event_thread(void);
VOID cmdInitConsole(VOID);
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name);
#define GetConsoleKeyboardLayoutName GetConsoleKeyboardLayoutNameA

UINT demGetPhysicalDriveType(UCHAR drive);
UINT GetDriveTypeOem(LPSTR root);
DWORD GetEnvironmentVariableOem(LPSTR name, LPSTR buffer, DWORD bytes);
BOOL SetEnvironmentVariableOem(LPSTR name, LPSTR value);

extern CHAR lpszComSpec[64 + 8];
extern USHORT cbComSpec;
extern BOOL IsFirstCall;
extern BOOL VDMForWOW;
extern PSCSINFO pSCSInfo;
extern PCHAR pSCS_ToSync;
extern BYTE *pIsDosBinary;
extern WORD *pFDAccess;
extern BOOL bPifFastPaste;

#define getDX() bx_ntvdm_command_misc_get_dx()
#define getBX() bx_ntvdm_command_misc_get_bx()
#define getCX() bx_ntvdm_command_misc_get_cx()
#define getSI() bx_ntvdm_command_misc_get_si()
#define getDS() bx_ntvdm_command_misc_get_ds()
#define getAL() bx_ntvdm_command_misc_get_al()
#define getAX() bx_ntvdm_command_misc_get_ax()
#define setAX(value) bx_ntvdm_command_misc_set_ax(value)
#define setAL(value) bx_ntvdm_command_misc_set_al(value)
#define setCF(value) bx_ntvdm_command_misc_set_cf(value)
#define setDX(value) bx_ntvdm_command_misc_set_dx(value)
#define setBX(value) bx_ntvdm_command_misc_set_bx(value)
#define setCX(value) bx_ntvdm_command_misc_set_cx(value)
#define GetVDMAddr(segment, offset) bx_ntvdm_command_misc_get_vdm_addr(segment, offset)

#endif

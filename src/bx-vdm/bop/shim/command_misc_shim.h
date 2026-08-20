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
#define setAX(value) bx_ntvdm_command_misc_set_ax(value)
#define setAL(value) bx_ntvdm_command_misc_set_al(value)
#define setCF(value) bx_ntvdm_command_misc_set_cf(value)
#define setDX(value) bx_ntvdm_command_misc_set_dx(value)
#define GetVDMAddr(segment, offset) bx_ntvdm_command_misc_get_vdm_addr(segment, offset)

#endif

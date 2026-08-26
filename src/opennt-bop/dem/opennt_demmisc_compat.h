#ifndef RUNTIME_BOP_SHIM_DEMMISC_SHIM_H
#define RUNTIME_BOP_SHIM_DEMMISC_SHIM_H

/* Compatibility surface for directly imported OpenNT demmisc.c.  The
 * imported translation unit remains the DEM owner; this seam contains only
 * historic product-host bindings that cannot compose with typed bx-vdm. */
#include "opennt-bop-overlay/dem/opennt_demerror_lock_composition.h"
#include "adapter-win32/facade/opennt_error_dialog_facade.h"

extern BOOL IsFirstCall;
extern LPSTR pszBIOSDirectory;
extern char *pszDefaultDOSDirectory;
extern uint32_t fShowSVCMsg;
extern BOOL VDMForWOW;
extern WORD *pFDAccess;

/* Original demDiskReset writes a SoftPC-owned FDAccess word.  The product
 * composition binds that location; the Direct fixture supplies a bounded
 * stand-in only to exercise the unchanged owner body. */
void runtime_demmisc_bind_fd_access(WORD *fd_access);

void runtime_demmisc_terminate_vdm(void);
LPVOID runtime_demmisc_get_vdm_pointer(ULONG far_pointer, ULONG bytes,
    UCHAR protected_mode);
HANDLE runtime_demmisc_create_file_oem(LPCSTR path, DWORD access,
    DWORD share, LPSECURITY_ATTRIBUTES security, DWORD creation,
    DWORD attributes, HANDLE template_file);
BOOL runtime_demmisc_read_file(HANDLE file, LPVOID buffer, DWORD bytes,
    LPDWORD read_out, LPOVERLAPPED overlapped);
PBYTE runtime_demmisc_advance_loader_pointer(PBYTE pointer, DWORD bytes);
DWORD runtime_demmisc_get_full_path_name_oem(LPCSTR path, DWORD bytes,
    LPSTR output, LPSTR *file_part);
void runtime_demmisc_vdd_create_user_hook(USHORT pdb);
void runtime_demmisc_module_load(LPCSTR module, LPCSTR path,
    WORD segment, DWORD bytes);
void runtime_demmisc_module_free(LPCSTR module, LPCSTR path);
void runtime_demmisc_module_segment_move(LPCSTR module, LPCSTR path,
    WORD old_segment, WORD new_segment);
void runtime_demmisc_dbg_prompt(LPCSTR prompt, LPSTR buffer, DWORD bytes);
void runtime_demmisc_rc_error_dialog(WORD error, LPCSTR text, LPVOID reserved);
void runtime_demmisc_host_floppy_reset(void);
void runtime_demmisc_host_fdisk_reset(void);
void HostFdiskReset(void);
int runtime_demmisc_invoke(runtime_demhndl_call *call);

#define CreateFileOem runtime_demmisc_create_file_oem
#define ReadFile runtime_demmisc_read_file
#define GetFullPathNameOem runtime_demmisc_get_full_path_name_oem
#define VDDCreateUserHook runtime_demmisc_vdd_create_user_hook
#define ModuleLoad runtime_demmisc_module_load
#define ModuleFree runtime_demmisc_module_free
#define ModuleSegmentMove runtime_demmisc_module_segment_move
#define DbgPrompt runtime_demmisc_dbg_prompt
#define RcErrorDialogBox runtime_demmisc_rc_error_dialog
#define HostFloppyReset runtime_demmisc_host_floppy_reset
#define HostFdiskReset runtime_demmisc_host_fdisk_reset
#define TerminateVDM runtime_demmisc_terminate_vdm
#define Sim32GetVDMPointer runtime_demmisc_get_vdm_pointer
#define OutputDebugStringOem OutputDebugStringA

#define ISPESET 0u
#define ID_NTIO 1u
#define ID_NTDOS 2u
#define SYMOP_LOAD 1u
#define SYMOP_FREE 2u
#define SYMOP_MOVE 3u
#define SYMOP_CLEANUP 0x80u
#define DBG_SEGMOVE 1u
#define DBG_MODLOAD 3u
#define DBG_MODFREE 4u
#define ED_BADSYSFILE 336u
#define ISDBG_DEBUGGEE 1u
#define ISDBG_SHOWSVC 2u

/* The imported 32-bit owner stores bounded MAX_PATH strlen results in DWORD.
 * On x64 that conversion is still bounded by its local fixed array. */
#pragma warning(disable:4267)

#endif

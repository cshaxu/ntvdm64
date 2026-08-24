#include "opennt_demmisc_compat.h"
#include "opennt_demfile_compat.h"

/* The imported owner redirects ReadFile to the checked loader bridge.  This
 * shim itself must call the real Win32 primitive after the bridge has been
 * declared. */
#undef ReadFile
#undef CreateFileOem
/* Keep the imported demmisc.c spelling mapped to this shim, but let the shim
 * itself call the recovered nt_fdisk lifecycle rather than recursively call
 * its exported compatibility wrapper. */
#undef HostFdiskReset

void demLoadDos(void); void demDiskReset(void); void demLoadDosAppSym(void);
void demFreeDosAppSym(void); void demEntryDosApp(void); void demDOSDispCall(void);
void demDOSDispRet(void); void demOutputString(void); void demInputString(void);
void demIsDebug(void); void demExitVDM(void); void demWOWFiles(void);
void demSystemSymbolOp(void);

BOOL IsFirstCall;
BOOL VDMForWOW;
static WORD g_fd_access;
WORD *pFDAccess = &g_fd_access;

void bx_ntvdm_demmisc_bind_fd_access(WORD *fd_access)
{ pFDAccess = fd_access != NULL ? fd_access : &g_fd_access; }

void bx_ntvdm_demmisc_terminate_vdm(void)
{ bx_ntvdm_demhndl_terminate(); }
LPVOID bx_ntvdm_demmisc_get_vdm_pointer(ULONG far_pointer, ULONG bytes,
    UCHAR protected_mode)
{ (void)bytes; (void)protected_mode; return bx_ntvdm_demhndl_get_vdm_addr(
    (USHORT)(far_pointer >> 16), (USHORT)far_pointer); }
HANDLE bx_ntvdm_demmisc_create_file_oem(LPCSTR path, DWORD access, DWORD share,
    LPSECURITY_ATTRIBUTES security, DWORD creation, DWORD attributes, HANDLE template_file)
{
    /* Reuse the existing OpenNT-shaped OEM->Unicode CreateFile facade rather
     * than bypassing it with the process ANSI code page. */
    return CreateFileOem((LPSTR)path, access, share, security, creation,
        attributes, template_file);
}
BOOL bx_ntvdm_demmisc_read_file(HANDLE file, LPVOID buffer, DWORD bytes,
    LPDWORD read_out, LPOVERLAPPED overlapped)
{
    BOOL success = ReadFile(file, buffer, bytes, read_out, overlapped);
    if (success && read_out != NULL && *read_out != 0u &&
        !bx_ntvdm_demhndl_loader_write(buffer, *read_out)) {
        SetLastError(ERROR_INVALID_ADDRESS);
        return FALSE;
    }
    return success;
}
PBYTE bx_ntvdm_demmisc_advance_loader_pointer(PBYTE pointer, DWORD bytes)
{ return pointer + bytes; }
DWORD bx_ntvdm_demmisc_get_full_path_name_oem(LPCSTR path, DWORD bytes,
    LPSTR output, LPSTR *file_part)
{ return GetFullPathNameA(path,bytes,output,file_part); }
void bx_ntvdm_demmisc_vdd_create_user_hook(USHORT pdb)
{
    /* DIVERGENCE(BOP-DIV-042): VDDCreateUserHook requires the NT4 VDD broker.  Do not
     * recreate that product shell; the VDD/debug owner retains this call. */
    (void)pdb;
}
void bx_ntvdm_demmisc_module_load(LPCSTR module,LPCSTR path,WORD segment,DWORD bytes)
{
    /* DIVERGENCE(BOP-DIV-043): these symbols are VDD/debugger notifications, not DOS
     * loader actions.  Their NT4 receiver is intentionally not recreated. */
    (void)module; (void)path; (void)segment; (void)bytes;
}
void bx_ntvdm_demmisc_module_free(LPCSTR module,LPCSTR path)
{ (void)module; (void)path; }
void bx_ntvdm_demmisc_module_segment_move(LPCSTR module,LPCSTR path,WORD old_segment,WORD new_segment)
{ (void)module; (void)path; (void)old_segment; (void)new_segment; }
void bx_ntvdm_demmisc_dbg_prompt(LPCSTR prompt,LPSTR buffer,DWORD bytes)
{
    /* DIVERGENCE(BOP-DIV-044): a debugger console can write guest-visible input only once
     * its own owner package supplies that lifecycle. */
    (void)prompt; (void)buffer; (void)bytes;
}
void bx_ntvdm_demmisc_rc_error_dialog(WORD error,LPCSTR text,LPVOID reserved)
{
    bx_ntvdm_opennt_rc_error_dialog((UINT)error, (CHAR *)text,
        (CHAR *)reserved);
}
void bx_ntvdm_demmisc_host_floppy_reset(void)
{
    /* DIVERGENCE(BOP-DIV-045): HostFloppyReset is FDC/DMA/CMOS machine behavior, not a
     * DEM facade.  No false controller reset is reported here. */
}
/* The imported demDiskReset owns the ordering: reset host disk handles, then
 * clear DOSDATA's FDAccess word.  FDISK is a composable host-volume lifecycle
 * rather than a floppy/controller operation, so preserve that original call. */
void bx_ntvdm_demmisc_host_fdisk_reset(void) { HostFdiskReset(); }

int bx_ntvdm_demmisc_invoke(bx_ntvdm_demhndl_call *call)
{
    void (*body)(void) = NULL;
    if (!bx_ntvdm_demhndl_call_valid(call)) return 0;
    switch (call->service) {
    case 0x11u: body=demLoadDos; break; case 0x23u: body=demDiskReset; break;
    case 0x34u: body=demLoadDosAppSym; break; case 0x35u: body=demFreeDosAppSym; break;
    case 0x36u: body=demEntryDosApp; break; case 0x37u: body=demDOSDispCall; break;
    case 0x38u: body=demDOSDispRet; break; case 0x39u: body=demOutputString; break;
    case 0x3au: body=demInputString; break; case 0x3bu: body=demIsDebug; break;
    case 0x3du: body=demExitVDM; break; case 0x3eu: body=demWOWFiles; break;
    case 0x45u: body=demSystemSymbolOp; break; default: return 0;
    }
    CurrentISVC=call->service;
    return bx_ntvdm_demhndl_invoke_body(call,body);
}

/* Neutral raw-DASD/IOCTL composition seam for the directly imported OpenNT
 * sources src/opennt/base/mvdm/dos/dem/demdasd.c and demioctl.c.
 *
 * The OpenNT files retain all DOS register, layout, sequencing and failure
 * decisions.  This file contains only the unavailable historical SoftPC
 * recursive-CPU and raw-device module ABI; it never routes BOPs itself.
 */

#include "demdasd_ioctl_shim.h"

void demIOCTL(void);
void demAbsRead(void);
void demAbsWrite(void);

/* Original source: src/opennt/base/mvdm/dos/v86/doskrnl/bios/spckbd.asm
 * publishes these four INT 13 continuation/vector words.  The imported DASD
 * code owns their interpretation.  They are zero until the future Bochs
 * machine/BIOS composition supplies the real guest addresses, which keeps a
 * direct call on its original low-level failure path rather than inventing a
 * firmware address in bx-vdm. */
WORD int13h_vector_off;
WORD int13h_vector_seg;
WORD int13h_caller_off;
WORD int13h_caller_seg;

typedef struct bx_ntvdm_demdasd_cpu_shadow {
    const bx_ntvdm_demhndl_call *call;
    USHORT cs;
    USHORT ip;
    USHORT es;
} bx_ntvdm_demdasd_cpu_shadow;

static __declspec(thread) bx_ntvdm_demdasd_cpu_shadow g_cpu_shadow;

USHORT bx_ntvdm_demdasd_get_cs(void) { return g_cpu_shadow.cs; }
USHORT bx_ntvdm_demdasd_get_ip(void) { return g_cpu_shadow.ip; }
int bx_ntvdm_demdasd_get_cf(void) { return 1; }
USHORT bx_ntvdm_demdasd_get_ah(void) { return bx_ntvdm_demhndl_get_ah(); }
void bx_ntvdm_demdasd_set_cs(USHORT value) { g_cpu_shadow.cs = value; }
void bx_ntvdm_demdasd_set_ip(USHORT value) { g_cpu_shadow.ip = value; }
void bx_ntvdm_demdasd_set_es(USHORT value) { g_cpu_shadow.es = value; }
void bx_ntvdm_demdasd_set_ah(USHORT value) { bx_ntvdm_demhndl_set_ah(value); }

void bx_ntvdm_demdasd_host_simulate(void)
{
    /* Original source: src/opennt/base/mvdm/dos/dem/demdasd.c.  Its floppy
     * hook path recursively entered SoftPC at an INT 13 continuation.  The
     * copied result ABI intentionally cannot mutate selectors/IP, so fail
     * this unavailable mechanism explicitly instead of fabricating I/O. */
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    bx_ntvdm_demhndl_set_ah(BIOS_INVALID_FUNCTION);
    bx_ntvdm_demhndl_set_cf(1);
}

void diskette_io(void) { bx_ntvdm_demdasd_host_simulate(); }

void sas_loadw(DWORD address, WORD *value)
{
    /* Original source: src/opennt/base/mvdm/dos/dem/demdasd.c reads IVT words
     * through SAS.  Translate that finite real-mode address through the same
     * checked guest span used by all imported DEM callers. */
    if (value == NULL || !bx_ntvdm_demhndl_copy_guest((USHORT)(address >> 4),
            (USHORT)(address & 0x0fu), value, sizeof(*value))) {
        if (value != NULL) *value = 0u;
        SetLastError(ERROR_INVALID_ADDRESS);
    }
}

void host_direct_access_error(ULONG type)
{
    (void)type;
    /* Original source: src/opennt/base/mvdm/softpc.new/host/src/nt_error.c.
     * Its action was a product GUI dialog; the CLI preserves the following
     * original DOS failure return without requiring a GUI product shell. */
}

/* Original source: src/opennt/base/mvdm/softpc.new/host/src/nt_fdisk.c and
 * nt_rflop.c.  Those historical host modules are not independently composed
 * yet.  Report a genuine host failure: demdasd.c/demioctl.c retain the
 * original conversion to DOS errors.  No synthetic drive or success exists. */
BOOL nt_floppy_close(BYTE drive) { (void)drive; SetLastError(ERROR_NOT_SUPPORTED); return FALSE; }
ULONG nt_floppy_read(BYTE drive, ULONG offset, ULONG size, PBYTE buffer)
{ (void)drive; (void)offset; (void)size; (void)buffer; SetLastError(ERROR_NOT_SUPPORTED); return 0u; }
ULONG nt_floppy_write(BYTE drive, ULONG offset, ULONG size, PBYTE buffer)
{ (void)drive; (void)offset; (void)size; (void)buffer; SetLastError(ERROR_NOT_SUPPORTED); return 0u; }
ULONG nt_floppy_format(BYTE drive, WORD cylinder, WORD head, MEDIA_TYPE media)
{ (void)drive; (void)cylinder; (void)head; (void)media; SetLastError(ERROR_NOT_SUPPORTED); return 0u; }
BOOL nt_floppy_media_check(BYTE drive) { (void)drive; SetLastError(ERROR_NOT_SUPPORTED); return FALSE; }
MEDIA_TYPE nt_floppy_get_media_type(BYTE drive, WORD cylinders, WORD sectors, WORD heads)
{ (void)drive; (void)cylinders; (void)sectors; (void)heads; SetLastError(ERROR_NOT_SUPPORTED); return Unknown; }
BOOL nt_floppy_verify(BYTE drive, DWORD offset, DWORD size)
{ (void)drive; (void)offset; (void)size; SetLastError(ERROR_NOT_SUPPORTED); return FALSE; }
BOOL nt_fdisk_init(BYTE drive, PBPB bpb, PDISK_GEOMETRY geometry)
{ (void)drive; (void)bpb; (void)geometry; SetLastError(ERROR_NOT_SUPPORTED); return FALSE; }
ULONG nt_fdisk_read(BYTE drive, PLARGE_INTEGER offset, ULONG size, PBYTE buffer)
{ (void)drive; (void)offset; (void)size; (void)buffer; SetLastError(ERROR_NOT_SUPPORTED); return 0u; }
ULONG nt_fdisk_write(BYTE drive, PLARGE_INTEGER offset, ULONG size, PBYTE buffer)
{ (void)drive; (void)offset; (void)size; (void)buffer; SetLastError(ERROR_NOT_SUPPORTED); return 0u; }
BOOL nt_fdisk_verify(BYTE drive, PLARGE_INTEGER offset, ULONG size)
{ (void)drive; (void)offset; (void)size; SetLastError(ERROR_NOT_SUPPORTED); return FALSE; }
BOOL nt_fdisk_close(BYTE drive) { (void)drive; SetLastError(ERROR_NOT_SUPPORTED); return FALSE; }

int bx_ntvdm_demdasd_ioctl_invoke(bx_ntvdm_demhndl_call *call)
{
    void (*body)(void) = NULL;

    if (!bx_ntvdm_demhndl_call_valid(call)) return 0;
    switch (call->service) {
    case 0x21u: body = demIOCTL; break;
    case 0x29u: body = demAbsRead; break;
    case 0x2au: body = demAbsWrite; break;
    default: return 0;
    }
    /* See the host_simulate divergence above: this shadow is private to the
     * imported routine's save/restore protocol and never crosses bx-vdm. */
    g_cpu_shadow.call = call;
    g_cpu_shadow.cs = call->cpu->cs;
    g_cpu_shadow.ip = (USHORT)call->cpu->eip;
    g_cpu_shadow.es = call->cpu->es;
    if (!bx_ntvdm_demhndl_invoke_body(call, body)) return 0;
    g_cpu_shadow.call = NULL;
    return 1;
}

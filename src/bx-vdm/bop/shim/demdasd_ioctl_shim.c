/* Neutral raw-DASD/IOCTL composition seam for the directly imported OpenNT
 * sources src/opennt/base/mvdm/dos/dem/demdasd.c and demioctl.c.
 *
 * The OpenNT files retain all DOS register, layout, sequencing and failure
 * decisions.  This file contains only the unavailable historical SoftPC
 * recursive-CPU and raw-device module ABI; it never routes BOPs itself.
 */

#include "demdasd_ioctl_shim.h"

#include <stdlib.h>
#include <string.h>

void demIOCTL(void);
void demAbsRead(void);
void demAbsWrite(void);
void demFdiskInit(void);

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

typedef struct bx_ntvdm_opennt_raw_volume {
    CHAR root[8];
    BYTE logical_drive;
} bx_ntvdm_opennt_raw_volume;

/* Source-derived replacement for the reached nt_fdisk.c subset.  Original
 * nt_fdisk.c bound its data table to a historical SoftPC host product shell;
 * this table preserves its physical-index-to-logical-drive contract while
 * binding the direct CLI to real Win32 volume paths. */
static bx_ntvdm_opennt_raw_volume g_raw_volumes[26];
static BYTE g_raw_volume_count;
static int g_dasd_initialized;

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

/* The floppy source is a distinct FDC/DMA/CMOS device component, not a volume
 * capability.  Its uncomposed paths remain explicit failures. */
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
static int raw_volume(BYTE physical, bx_ntvdm_opennt_raw_volume **out)
{
    if (physical >= g_raw_volume_count || out == NULL) {
        SetLastError(ERROR_INVALID_DRIVE);
        return 0;
    }
    *out = &g_raw_volumes[physical];
    return 1;
}

/* Divergence from nt_fdisk.c: use documented Win32 handles in place of its
 * NT native `\\DosDevices` opens and FAT-only FSCTL.  The original output
 * BPB/geometry contract and failure propagation remain with demdasd.c. */
BOOL nt_fdisk_init(BYTE drive, PBPB bpb, PDISK_GEOMETRY geometry)
{
    CHAR root[] = "A:\\";
    CHAR volume[] = "\\\\.\\A:";
    HANDLE handle;
    DWORD bytes;
    DWORD spc, bps, free_clusters, total_clusters;

    if (drive >= 26u || bpb == NULL || geometry == NULL ||
        g_raw_volume_count >= 26u) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    root[0] = (CHAR)('A' + drive); volume[4] = root[0];
    if (GetDriveTypeA(root) != DRIVE_FIXED) { SetLastError(ERROR_INVALID_DRIVE); return FALSE; }
    handle = CreateFileA(volume, FILE_READ_ATTRIBUTES | SYNCHRONIZE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) return FALSE;
    if (!DeviceIoControl(handle, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0,
            geometry, sizeof(*geometry), &bytes, NULL) ||
        !GetDiskFreeSpaceA(root, &spc, &bps, &free_clusters, &total_clusters)) {
        CloseHandle(handle); return FALSE;
    }
    CloseHandle(handle);
    memset(bpb, 0, sizeof(*bpb));
    bpb->SectorSize = (WORD)bps; bpb->ClusterSize = (BYTE)spc;
    bpb->ReservedSectors = 1u; bpb->FATs = 2u; bpb->RootDirs = 512u;
    bpb->MediaID = 0xf8u; bpb->TrackSize = (WORD)geometry->SectorsPerTrack;
    bpb->Heads = (WORD)geometry->TracksPerCylinder;
    bpb->BigSectors = total_clusters * spc;
    g_raw_volumes[g_raw_volume_count].root[0] = root[0];
    g_raw_volumes[g_raw_volume_count].root[1] = ':';
    g_raw_volumes[g_raw_volume_count].root[2] = '\\';
    g_raw_volumes[g_raw_volume_count].root[3] = 0;
    g_raw_volumes[g_raw_volume_count].logical_drive = drive;
    ++g_raw_volume_count;
    return TRUE;
}

static ULONG raw_transfer(BYTE physical, PLARGE_INTEGER offset, ULONG size,
    PBYTE buffer, int write)
{
    bx_ntvdm_opennt_raw_volume *volume;
    CHAR path[] = "\\\\.\\A:";
    HANDLE handle;
    DWORD transferred = 0u;
    DWORD access = write ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ;

    if (!raw_volume(physical, &volume) || offset == NULL || buffer == NULL)
        return 0u;
    path[4] = volume->root[0];
    handle = CreateFileA(path, access, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, NULL);
    if (handle == INVALID_HANDLE_VALUE) return 0u;
    if (!SetFilePointerEx(handle, *offset, NULL, FILE_BEGIN) ||
        !(write ? WriteFile(handle, buffer, size, &transferred, NULL) :
                  ReadFile(handle, buffer, size, &transferred, NULL)))
        transferred = 0u;
    CloseHandle(handle);
    return transferred;
}

ULONG nt_fdisk_read(BYTE drive, PLARGE_INTEGER offset, ULONG size, PBYTE buffer)
{ return raw_transfer(drive, offset, size, buffer, 0); }
ULONG nt_fdisk_write(BYTE drive, PLARGE_INTEGER offset, ULONG size, PBYTE buffer)
{ return raw_transfer(drive, offset, size, buffer, 1); }
BOOL nt_fdisk_verify(BYTE drive, PLARGE_INTEGER offset, ULONG size)
{
    bx_ntvdm_opennt_raw_volume *volume; CHAR path[]="\\\\.\\A:"; HANDLE h; DWORD bytes;
    VERIFY_INFORMATION info;
    if (!raw_volume(drive,&volume)||offset==NULL) return FALSE;
    path[4]=volume->root[0]; h=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);
    if(h==INVALID_HANDLE_VALUE)return FALSE; info.StartingOffset=*offset; info.Length=size;
    if(!DeviceIoControl(h,IOCTL_DISK_VERIFY,&info,sizeof(info),NULL,0,&bytes,NULL)){CloseHandle(h);return FALSE;} CloseHandle(h);return TRUE;
}
BOOL nt_fdisk_close(BYTE drive) { bx_ntvdm_opennt_raw_volume *volume; return raw_volume(drive,&volume); }

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
    /* Original demDasdInit also probes floppy hardware through the historical
     * SoftPC BIOS path.  This direct host-volume composition admits only its
     * fixed-disk half; retain the original demFdiskInit owner verbatim. */
    if (!g_dasd_initialized) { demFdiskInit(); g_dasd_initialized = 1; }
    if (!bx_ntvdm_demhndl_invoke_body(call, body)) return 0;
    g_cpu_shadow.call = NULL;
    return 1;
}

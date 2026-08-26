/* Neutral raw-DASD/IOCTL composition seam for the directly imported OpenNT
 * sources src/opennt/base/mvdm/dos/dem/demdasd.c and demioctl.c.
 *
 * The OpenNT files retain all DOS register, layout, sequencing and failure
 * decisions.  This file contains only the unavailable historical SoftPC
 * recursive-CPU and raw-device module ABI; it never routes BOPs itself.
 */

#include "opennt_demdasd_ioctl_compat.h"
#include "opennt-host/softpc.new/host/inc/error.h"

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

typedef struct runtime_demdasd_cpu_shadow {
    USHORT cs;
    USHORT ip;
} runtime_demdasd_cpu_shadow;

static __declspec(thread) runtime_demdasd_cpu_shadow g_cpu_shadow;

/* Directly shaped from OpenNT softpc.new/host/src/nt_fdisk.c.  Keep its
 * physical-drive table, PDB ownership and handle lifecycle intact.  The
 * documented divergence is public Win32 volume/geometry queries in place of
 * NT4's private Nt* entry points. */
#define FDISK_IDLE_PERIOD 30u
typedef struct runtime_opennt_fdisk_data {
    BYTE drive, idle_counter;
    CHAR drive_letter;
    BOOLEAN auto_locked;
    HANDLE fdisk_fd;
    DWORD num_heads;
    LARGE_INTEGER num_cylinders;
    DWORD sectors_per_track, bytes_per_sector, align_factor;
    USHORT owner_pdb;
    CHAR device_name[9];
} runtime_opennt_fdisk_data;

static runtime_opennt_fdisk_data *fdisk_data_table;
static BYTE number_of_fdisk;
static DWORD max_align_factor, cur_align_factor;
static WORD fdisk_open_count;
static int g_dasd_initialized;

extern PUSHORT pusCurrentPDB;
extern WORD *pFDAccess;

USHORT runtime_demdasd_get_cs(void) { return g_cpu_shadow.cs; }
USHORT runtime_demdasd_get_ip(void) { return g_cpu_shadow.ip; }
int runtime_demdasd_get_cf(void) { return runtime_demhndl_get_cf(); }
USHORT runtime_demdasd_get_ah(void) { return runtime_demhndl_get_ah(); }
void runtime_demdasd_set_cs(USHORT value) { g_cpu_shadow.cs = value; }
void runtime_demdasd_set_ip(USHORT value) { g_cpu_shadow.ip = value; }
void runtime_demdasd_set_es(USHORT value) { runtime_demhndl_set_es(value); }
void runtime_demdasd_set_ah(USHORT value) { runtime_demhndl_set_ah(value); }

void runtime_demdasd_host_simulate(void)
{
    /* DIVERGENCE(BOP-DIV-046): recursive SoftPC execution has no bounded
     * Bochs equivalent at this source call boundary. */
    /* Original source: src/opennt/base/mvdm/dos/dem/demdasd.c.  Its floppy
     * hook path recursively entered SoftPC at an INT 13 continuation.  The
     * copied result ABI intentionally cannot mutate selectors/IP, so fail
     * this unavailable mechanism explicitly instead of fabricating I/O. */
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    runtime_demhndl_set_ah(BIOS_INVALID_FUNCTION);
    runtime_demhndl_set_cf(1);
}

void diskette_io(void) { runtime_demdasd_host_simulate(); }

void sas_loadw(DWORD address, WORD *value)
{
    /* Original source: src/opennt/base/mvdm/dos/dem/demdasd.c reads IVT words
     * through SAS.  Translate that finite real-mode address through the same
     * checked guest span used by all imported DEM callers. */
    if (value == NULL || !runtime_demhndl_copy_guest((USHORT)(address >> 4),
            (USHORT)(address & 0x0fu), value, sizeof(*value))) {
        if (value != NULL) *value = 0u;
        SetLastError(ERROR_INVALID_ADDRESS);
    }
}

    /* DIVERGENCE(BOP-DIV-054): the floppy source is a distinct FDC/DMA/CMOS device component, not a volume
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
/* DIVERGENCE(BOP-DIV-055): use documented Win32 handles in place of its
 * NT native `\\DosDevices` opens and FAT-only FSCTL.  The original output
 * BPB/geometry contract and failure propagation remain with demdasd.c. */
BOOL nt_fdisk_init(BYTE drive, PBPB bpb, PDISK_GEOMETRY geometry)
{
    CHAR root[] = "A:\\";
    CHAR volume[] = "\\\\.\\A:";
    runtime_opennt_fdisk_data *data;
    HANDLE handle;
    DWORD bytes;
    DWORD spc, bps, free_clusters, total_clusters;
    DWORD alignment_requirement = 0u;

    if (drive >= 26u || bpb == NULL || geometry == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    root[0] = (CHAR)('A' + drive); volume[4] = root[0];
    /* Original nt_fdisk/demFdiskInit eligibility: a letter must be a present
     * fixed host volume. */
    if (GetDriveTypeA(root) != DRIVE_FIXED) {
        SetLastError(ERROR_INVALID_DRIVE); return FALSE;
    }
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
    /* DIVERGENCE(BOP-DIV-055): modern
     * fixed volumes can be NTFS/ReFS.  The existing OpenNT BPB contract is
     * populated from public Win32 geometry/free-space data, then its original
     * BDS/read/write/verify logic remains untouched in demdasd.c. */
    data = (runtime_opennt_fdisk_data *)realloc(fdisk_data_table,
        (number_of_fdisk + 1u) * sizeof(*data));
    if (data == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    fdisk_data_table = data;
    data = &fdisk_data_table[number_of_fdisk];
    memset(data, 0, sizeof(*data));
    data->drive_letter = root[0]; data->drive = number_of_fdisk;
    data->fdisk_fd = INVALID_HANDLE_VALUE;
    data->num_heads = geometry->TracksPerCylinder;
    data->sectors_per_track = geometry->SectorsPerTrack;
    data->bytes_per_sector = geometry->BytesPerSector;
    data->num_cylinders = geometry->Cylinders;
    /* DIVERGENCE(BOP-DIV-055):
     * FileAlignmentInformation): this pinned historical public-header surface
     * cannot name that newer structure.  Buffered public volume handles do
     * not require caller alignment, so retain the original zero-alignment
     * branch rather than introduce a private NT declaration. */
    data->align_factor = alignment_requirement;
    memcpy(data->device_name, volume, sizeof(volume));
    if (data->align_factor > max_align_factor) max_align_factor = data->align_factor;
    ++number_of_fdisk;
    return TRUE;
}

static runtime_opennt_fdisk_data *get_fdisk_data(BYTE drive)
{
    BYTE index;
    for (index = 0u; index < number_of_fdisk; ++index)
        if (fdisk_data_table[index].drive == drive) return &fdisk_data_table[index];
    SetLastError(ERROR_INVALID_DRIVE); return NULL;
}

static BOOL close_fdisk(runtime_opennt_fdisk_data *data)
{
    if (data == NULL) return FALSE;
    if (data->fdisk_fd != INVALID_HANDLE_VALUE) {
        CloseHandle(data->fdisk_fd); data->fdisk_fd = INVALID_HANDLE_VALUE;
        data->auto_locked = FALSE; data->owner_pdb = 0u;
        if (fdisk_open_count != 0u) --fdisk_open_count;
        if (pFDAccess != NULL && *pFDAccess != 0u) --*pFDAccess;
    }
    return TRUE;
}

static BOOL get_fdisk_handle(runtime_opennt_fdisk_data *data, USHORT pdb,
    BOOL auto_lock)
{
    DWORD access, share;
    if (data == NULL) return FALSE;
    if (data->fdisk_fd != INVALID_HANDLE_VALUE &&
        ((auto_lock && !data->auto_locked) || data->owner_pdb != pdb)) close_fdisk(data);
    access = GENERIC_READ | (auto_lock ? GENERIC_WRITE : 0u);
    share = auto_lock ? FILE_SHARE_READ : FILE_SHARE_READ | FILE_SHARE_WRITE;
    if (data->fdisk_fd == INVALID_HANDLE_VALUE) {
        data->fdisk_fd = CreateFileA(data->device_name, access, share, NULL,
            OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, NULL);
        if (data->fdisk_fd == INVALID_HANDLE_VALUE) return FALSE;
        data->auto_locked = auto_lock; data->owner_pdb = pdb;
        ++fdisk_open_count; if (pFDAccess != NULL) ++*pFDAccess;
    }
    data->idle_counter = FDISK_IDLE_PERIOD;
    cur_align_factor = data->align_factor;
    return TRUE;
}

static ULONG disk_transfer(HANDLE handle, PLARGE_INTEGER offset, ULONG size,
    PBYTE buffer, BOOL write)
{
    DWORD done = 0u, total = 0u, chunk;
    if (handle == INVALID_HANDLE_VALUE || offset == NULL || buffer == NULL ||
        !SetFilePointerEx(handle, *offset, NULL, FILE_BEGIN)) return 0u;
    while (size != 0u) {
        chunk = size > 0x9000u ? 0x9000u : size;
        if (!(write ? WriteFile(handle, buffer, chunk, &done, NULL) :
              ReadFile(handle, buffer, chunk, &done, NULL)) || done != chunk) break;
        size -= chunk; total += done; buffer += done;
    }
    return total;
}

ULONG nt_fdisk_read(BYTE drive, PLARGE_INTEGER offset, ULONG size, PBYTE buffer)
{ runtime_opennt_fdisk_data *d = get_fdisk_data(drive); return d != NULL &&
    get_fdisk_handle(d, pusCurrentPDB != NULL ? *pusCurrentPDB : 0u, FALSE) ?
    disk_transfer(d->fdisk_fd, offset, size, buffer, FALSE) : 0u; }
ULONG nt_fdisk_write(BYTE drive, PLARGE_INTEGER offset, ULONG size, PBYTE buffer)
{ runtime_opennt_fdisk_data *d = get_fdisk_data(drive); return d != NULL &&
    get_fdisk_handle(d, pusCurrentPDB != NULL ? *pusCurrentPDB : 0u, TRUE) ?
    disk_transfer(d->fdisk_fd, offset, size, buffer, TRUE) : 0u; }
BOOL nt_fdisk_verify(BYTE drive, PLARGE_INTEGER offset, ULONG size)
{
    runtime_opennt_fdisk_data *d; DWORD bytes;
    VERIFY_INFORMATION info;
    d = get_fdisk_data(drive);
    if (d == NULL || offset == NULL || !get_fdisk_handle(d,
        pusCurrentPDB != NULL ? *pusCurrentPDB : 0u, FALSE)) return FALSE;
    info.StartingOffset = *offset; info.Length = size;
    return DeviceIoControl(d->fdisk_fd, IOCTL_DISK_VERIFY, &info, sizeof(info),
        NULL, 0, &bytes, NULL);
}
BOOL nt_fdisk_close(BYTE drive) { return close_fdisk(get_fdisk_data(drive)); }
void fdisk_heart_beat(void)
{
    BYTE index;
    /* Same idle-close rule as nt_fdisk.c: only open fixed-volume handles
     * participate, and each source-owned access resets its countdown. */
    if (fdisk_open_count == 0u) return;
    for (index = 0u; index < number_of_fdisk; ++index) {
        runtime_opennt_fdisk_data *data = &fdisk_data_table[index];
        if (data->fdisk_fd != INVALID_HANDLE_VALUE && data->idle_counter != 0u &&
            --data->idle_counter == 0u) close_fdisk(data);
    }
}

void FdiskTerminatePDB(USHORT pdb)
{
    BYTE index;
    if (fdisk_open_count == 0u) return;
    for (index = 0u; index < number_of_fdisk; ++index) {
        runtime_opennt_fdisk_data *data = &fdisk_data_table[index];
        if (data->fdisk_fd != INVALID_HANDLE_VALUE &&
            (pdb == 0u || data->owner_pdb == pdb)) close_fdisk(data);
    }
}

void HostFdiskReset(void) { FdiskTerminatePDB(0u); }

int runtime_demdasd_ioctl_invoke(runtime_demhndl_call *call)
{
    void (*body)(void) = NULL;

    if (!runtime_demhndl_call_valid(call)) return 0;
    switch (call->service) {
    case 0x21u:
        /* OpenNT demIOCTL performs this range check only under DBG.  The
         * imported release body would otherwise index apfnSVCIoctl beyond
         * its 18-entry source table when a copied modern guest boundary
         * supplies a malformed AL.  Keep the original failure body and its
         * AX/CF contract; this is boundary validation, not a new IOCTL
         * policy or a change to the mirrored source's dispatch order. */
        body = ((call->cpu->eax & 0xffu) >= 18u) ? demIoctlInvalid : demIOCTL;
        break;
    case 0x29u: body = demAbsRead; break;
    case 0x2au: body = demAbsWrite; break;
    default: return 0;
    }
    /* See the host_simulate divergence above: this shadow is private to the
     * imported routine's save/restore protocol and never crosses bx-vdm. */
    g_cpu_shadow.cs = call->cpu->cs;
    g_cpu_shadow.ip = (USHORT)call->cpu->eip;
    /* Original demDasdInit also probes floppy hardware through the historical
     * SoftPC BIOS path.  This direct host-volume composition admits only its
     * fixed-disk half; retain the original demFdiskInit owner verbatim. */
    if (!g_dasd_initialized) { demFdiskInit(); g_dasd_initialized = 1; }
    if (!runtime_demhndl_invoke_body(call, body)) return 0;
    memset(&g_cpu_shadow, 0, sizeof(g_cpu_shadow));
    return 1;
}

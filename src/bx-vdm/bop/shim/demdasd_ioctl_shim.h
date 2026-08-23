#ifndef BX_NTVDM_BOP_SHIM_DEMDASD_IOCTL_SHIM_H
#define BX_NTVDM_BOP_SHIM_DEMDASD_IOCTL_SHIM_H

/* Compatibility boundary for directly imported OpenNT demdasd.c/demioctl.c.
 * It starts with the proven register/guest-memory/drive layout surface;
 * compile probes admit each remaining historical raw-device dependency. */

#include "demgset_shim.h"
#include "dem_drive_policy_shim.h"

/* Original source: src/opennt/base/mvdm/dos/dem/dem.h and dosdef.h.
 * The imported units need only these dispatcher spellings; their selection
 * remains in the original tables in demioctl.c. */
typedef VOID (*PFNSVC)(VOID);
#define IOCTL_CHANGEABLE 8u
#define IOCTL_DeviceLocOrRem 9u

/* Original demioctl.c initializes this table before its function bodies.
 * The historic headers supplied these declarations transitively. */
void demIoctlInvalid(void);
void demIoctlChangeable(void);
void demIoctlDiskGeneric(void);
void demIoctlDiskQuery(void);
UCHAR demGetPhysicalDriveType(UCHAR drive);

/* Original source: src/opennt/base/mvdm/inc/softpc.h.  The former call site
 * displayed a product-host dialog.  bx-vdm supplies a neutral diagnostic
 * seam instead; it neither identifies BOPs nor changes disk semantics. */
#define NOSUPPORT_FLOPPY 0u
#define NOSUPPORT_HARDDISK 1u
void host_direct_access_error(ULONG type);

/* Original source: src/opennt/base/mvdm/inc/softpc.h.  The imported DASD
 * body uses these only while delegating a hooked floppy INT 13h back to the
 * historical SoftPC recursive executor.  They are declared at this adapter
 * seam so the original body remains intact; the future Bochs-backed bridge
 * is deliberately independent of selector/service routing. */
USHORT bx_ntvdm_demdasd_get_cs(void);
USHORT bx_ntvdm_demdasd_get_ip(void);
int bx_ntvdm_demdasd_get_cf(void);
USHORT bx_ntvdm_demdasd_get_ah(void);
void bx_ntvdm_demdasd_set_cs(USHORT value);
void bx_ntvdm_demdasd_set_ip(USHORT value);
void bx_ntvdm_demdasd_set_es(USHORT value);
void bx_ntvdm_demdasd_set_ah(USHORT value);
void bx_ntvdm_demdasd_host_simulate(void);
void diskette_io(void);
/* Directly retained `nt_fdisk.c` host lifecycle.  The imported DEM reset and
 * process-termination owners use these spellings; no machine behavior is
 * implied by them. */
void fdisk_heart_beat(void);
void FdiskTerminatePDB(USHORT pdb);
void HostFdiskReset(void);

int bx_ntvdm_demdasd_ioctl_invoke(bx_ntvdm_demhndl_call *call);

#define getCS() bx_ntvdm_demdasd_get_cs()
#define getIP() bx_ntvdm_demdasd_get_ip()
#define getCF() bx_ntvdm_demdasd_get_cf()
#define getAH() bx_ntvdm_demdasd_get_ah()
#define setCS(value) bx_ntvdm_demdasd_set_cs(value)
#define setIP(value) bx_ntvdm_demdasd_set_ip(value)
#define setES(value) bx_ntvdm_demdasd_set_es(value)
#define setAH(value) bx_ntvdm_demdasd_set_ah(value)
#define host_simulate() bx_ntvdm_demdasd_host_simulate()

/* This identifier shadow is present in the original imported source. */
#pragma warning(disable:4459)

#endif

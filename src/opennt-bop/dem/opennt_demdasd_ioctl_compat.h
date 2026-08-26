#ifndef RUNTIME_BOP_SHIM_DEMDASD_IOCTL_SHIM_H
#define RUNTIME_BOP_SHIM_DEMDASD_IOCTL_SHIM_H

/* Compatibility boundary for directly imported OpenNT demdasd.c/demioctl.c.
 * It starts with the proven register/guest-memory/drive layout surface;
 * compile probes admit each remaining historical raw-device dependency. */

#include "opennt-bop-overlay/dem/opennt_demgset_compat.h"

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

/* Original SoftPC direct-access categories and host notification ABI. */
#include "opennt-host/softpc.new/host/inc/error.h"

/* Original source: src/opennt/base/mvdm/inc/softpc.h.  The imported DASD
 * body uses these only while delegating a hooked floppy INT 13h back to the
 * historical SoftPC recursive executor.  They are declared at this adapter
 * seam so the original body remains intact; the future Bochs-backed bridge
 * is deliberately independent of selector/service routing. */
USHORT runtime_demdasd_get_cs(void);
USHORT runtime_demdasd_get_ip(void);
int runtime_demdasd_get_cf(void);
USHORT runtime_demdasd_get_ah(void);
void runtime_demdasd_set_cs(USHORT value);
void runtime_demdasd_set_ip(USHORT value);
void runtime_demdasd_set_es(USHORT value);
void runtime_demdasd_set_ah(USHORT value);
void runtime_demdasd_host_simulate(void);
void diskette_io(void);
/* Directly retained `nt_fdisk.c` host lifecycle.  The imported DEM reset and
 * process-termination owners use these spellings; no machine behavior is
 * implied by them. */
void fdisk_heart_beat(void);
void FdiskTerminatePDB(USHORT pdb);
void HostFdiskReset(void);

int runtime_demdasd_ioctl_invoke(runtime_demhndl_call *call);

/* demhndl_shim exports the common CCPU spelling for other imported DEM
 * bodies.  DASD owns a separate local CS:IP staging pair around the original
 * recursive BIOS call, so rebind only those spellings here deliberately. */
#undef getCS
#undef getIP
#undef getCF
#undef setCS
#undef setIP
#define getCS() runtime_demdasd_get_cs()
#define getIP() runtime_demdasd_get_ip()
#define getCF() runtime_demdasd_get_cf()
#define getAH() runtime_demdasd_get_ah()
#define setCS(value) runtime_demdasd_set_cs(value)
#define setIP(value) runtime_demdasd_set_ip(value)
#define setES(value) runtime_demdasd_set_es(value)
#define setAH(value) runtime_demdasd_set_ah(value)
#define host_simulate() runtime_demdasd_host_simulate()

/* This identifier shadow is present in the original imported source. */
#pragma warning(disable:4459)

#endif

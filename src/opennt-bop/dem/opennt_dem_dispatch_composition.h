#ifndef RUNTIME_BOP_SHIM_DEMDISP_SHIM_H
#define RUNTIME_BOP_SHIM_DEMDISP_SHIM_H

/* Compatibility surface for the directly imported OpenNT source
 * src/opennt/base/mvdm/dos/dem/demdisp.c.  Its original dem.h/softpc.h
 * include closure supplied these declarations through the NTVDM product
 * composition.  This seam supplies only the declared DEM bodies and the
 * bounded typed-call bridge; it performs no BOP classification. */

#include "opennt-bop/dem/opennt_demerror_lock_composition.h"

#define SVC_DEMLASTSVC 0x49u
#define DEMSVCTRACE 0u

extern PVHE pHardErrPacket;
extern char demDebugBuffer[256];
void OutputDebugStringOem(LPSTR text);

VOID demChgFilePtr(VOID); VOID demChMod(VOID); VOID demClose(VOID);
VOID demCreate(VOID); VOID demCreateDir(VOID); VOID demDelete(VOID);
VOID demDeleteDir(VOID); VOID demDeleteFCB(VOID); VOID demFileTimes(VOID);
VOID demFindFirst(VOID); VOID demFindFirstFCB(VOID); VOID demFindNext(VOID);
VOID demFindNextFCB(VOID); VOID demGetBootDrive(VOID);
VOID demGetDriveFreeSpace(VOID); VOID demGetDrives(VOID);
VOID demGSetMediaID(VOID); VOID demLoadDos(VOID); VOID demOpen(VOID);
VOID demQueryCurrentDir(VOID); VOID demQueryDate(VOID); VOID demQueryTime(VOID);
VOID demRead(VOID); VOID demRename(VOID); VOID demSetCurrentDir(VOID);
VOID demSetDate(VOID); VOID demSetDefaultDrive(VOID);
VOID demSetDTALocation(VOID); VOID demSetTime(VOID); VOID demWrite(VOID);
VOID demRenameFCB(VOID); VOID demIOCTL(VOID); VOID demCreateNew(VOID);
VOID demDiskReset(VOID); VOID demGetDPB(VOID); VOID demCommit(VOID);
VOID demAbsRead(VOID); VOID demAbsWrite(VOID); VOID demCreateFCB(VOID);
VOID demOpenFCB(VOID); VOID demCloseFCB(VOID); VOID demFCBIO(VOID);
VOID demDate16(VOID); VOID demGetFileInfo(VOID); VOID demSetHardErrorInfo(VOID);
VOID demRetry(VOID); VOID demLoadDosAppSym(VOID); VOID demFreeDosAppSym(VOID);
VOID demEntryDosApp(VOID); VOID demDOSDispCall(VOID); VOID demDOSDispRet(VOID);
VOID demOutputString(VOID); VOID demInputString(VOID); VOID demIsDebug(VOID);
VOID demTerminatePDB(VOID); VOID demExitVDM(VOID); VOID demWOWFiles(VOID);
VOID demLockOper(VOID); VOID demGetComputerName(VOID); VOID demCheckPath(VOID);
VOID demSystemSymbolOp(VOID); VOID demGetDPBList(VOID);
VOID demPipeFileDataEOF(VOID); VOID demPipeFileEOF(VOID);
VOID demNotYetImplemented(VOID); VOID demSetV86KernelAddr(VOID);
BOOL DemDispatch(ULONG iSvc);

/* The adapter calls this only after its higher-level BOP route has selected
 * the OpenNT DEM family.  The wrapper binds a typed checked-RAM call and then
 * invokes the original DemDispatch table unchanged. */
int runtime_demdisp_invoke(runtime_demhndl_call *call);
size_t runtime_demdisp_service_count(void);
PFNSVC runtime_demdisp_handler_at(uint8_t service);

#endif

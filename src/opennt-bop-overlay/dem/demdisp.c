/*
 *  demdisp.c - SVC dispatch module
 *
 *  Modification History:
 *
 *  Sudeepb 31-Mar-1991 Created
 */

/* Direct import from src/opennt/base/mvdm/dos/dem/demdisp.c.
 * DIVERGENCE(BOP-DIV-090): the historical dem.h/softpc.h include closure belongs to the
 * removed NTVDM product composition.  opennt_dem_dispatch_composition.h supplies only the same
 * declarations and the typed checked-call entry boundary; the table, dispatch
 * order, CurrentISVC and original ordinary-return bodies remain unchanged. */
#include "opennt-bop/dem/opennt_dem_dispatch_composition.h"
#include <stdio.h>

#if DBG
PCHAR aSVCNames[] = {
 "demChgFilePtr","demChMod","demClose","demCreate","demCreateDir","demDelete",
 "demDeleteDir","demDeleteFCB","demFileTimes","demFindFirst","demFindFirstFCB",
 "demFindNext","demFindNextFCB","demGetBootDrive","demGetDriveFreeSpace","demGetDrives",
 "demGSetMediaID","demLoadDos","demOpen","demQueryCurrentDir","demQueryDate","demQueryTime",
 "demRead","demRename","demSetCurrentDir","demSetDate","demSetDefaultDrive","demSetDTALocation",
 "demSetTime","demSetV86KernelAddr","demWrite","demGetDriveInfo","demRenameFCB","demIOCTL",
 "demCreateNew","DemDiskReset","DemSetDPB","DemGetDPB","DemSleazeFunc","demCommit","DemExtHandle",
 "DemAbsDRD","DemAbsDWRT","DemGsetCDPG","DemCreateFCB","DemOpenFCB","DemCloseFCB","DemFCBIO",
 "DemDate16","DemGetFileInfo","DemSetHardErrorInfo","DemRetry","DemLoadDosAppSym","DemFreeDosAppSym",
 "DemEntryDosApp","DemDOSDispCall","DemDOSDispRet","DemOutputString","DemInputString","DemIsDebug",
 "DemTerminatePDB","DemExitVDM","DemWOWFiles","DemLockOper","demNotYetImplemented","DemGetComputerName",
 "DemFastRead","DemFastWrite","DemCheckPath","DemSystemSymbolOp","DemGetDpbList","DemPipeFileDataEOF",
 "DemPipeFileEOF"
};
#endif

DWORD fShowSVCMsg = 0;
ULONG CurrentISVC;

PFNSVC apfnSVC[] = {
 demChgFilePtr, demChMod, demClose, demCreate, demCreateDir, demDelete, demDeleteDir,
 demDeleteFCB, demFileTimes, demFindFirst, demFindFirstFCB, demFindNext, demFindNextFCB,
 demGetBootDrive, demGetDriveFreeSpace, demGetDrives, demGSetMediaID, demLoadDos, demOpen,
 demQueryCurrentDir, demQueryDate, demQueryTime, demRead, demRename, demSetCurrentDir,
 demSetDate, demSetDefaultDrive, demSetDTALocation, demSetTime, demSetV86KernelAddr, demWrite,
 demNotYetImplemented, demRenameFCB, demIOCTL, demCreateNew, demDiskReset, demNotYetImplemented,
 demGetDPB, demNotYetImplemented, demCommit, demNotYetImplemented, demAbsRead, demAbsWrite,
 demNotYetImplemented, demCreateFCB, demOpenFCB, demCloseFCB, demFCBIO, demDate16, demGetFileInfo,
 demSetHardErrorInfo, demRetry, demLoadDosAppSym, demFreeDosAppSym, demEntryDosApp, demDOSDispCall,
 demDOSDispRet, demOutputString, demInputString, demIsDebug, demTerminatePDB, demExitVDM, demWOWFiles,
 demLockOper, demNotYetImplemented, demGetComputerName, demNotYetImplemented, demNotYetImplemented,
 demCheckPath, demSystemSymbolOp, demGetDPBList, demPipeFileDataEOF, demPipeFileEOF
};

/* DemDispatch - Dispatch SVC call to right handler. */
BOOL DemDispatch(ULONG iSvc)
{
#if DBG
    if(iSvc < SVC_DEMLASTSVC && (fShowSVCMsg & DEMSVCTRACE) &&
       apfnSVC[iSvc] != demNotYetImplemented){
	sprintf(demDebugBuffer,"DemDispatch: Entering %s\n\tAX=%.4x BX=%.4x CX=%.4x DX=%.4x DI=%.4x SI=%.4x\n",
	       aSVCNames[iSvc],getAX(),getBX(),getCX(),getDX(),getDI(),getSI());
        OutputDebugStringOem(demDebugBuffer);
	sprintf(demDebugBuffer,"\tCS=%.4x IP=%.4x DS=%.4x ES=%.4x SS=%.4x SP=%.4x BP=%.4x\n",
                getCS(),getIP(), getDS(),getES(),getSS(),getSP(),getBP());
        OutputDebugStringOem(demDebugBuffer);
    }
#endif
    if (iSvc >= SVC_DEMLASTSVC) {
#if DBG
        sprintf(demDebugBuffer,"Unimplemented SVC index %x\n",iSvc);
        OutputDebugStringOem(demDebugBuffer);
#endif
        setCF(1);
        return FALSE;
    }
    if (pHardErrPacket)
        pHardErrPacket->vhe_fbInt24 = 0;
    CurrentISVC = iSvc;
    (apfnSVC[iSvc])();
#if DBG
    if((fShowSVCMsg & DEMSVCTRACE)){
	sprintf(demDebugBuffer,"DemDispatch:On Leaving %s\n\tAX=%.4x BX=%.4x CX=%.4x DX=%.4x DI=%.4x SI=%.4x\n",
               aSVCNames[iSvc],getAX(),getBX(),getCX(),getDX(),getDI(),getSI());
        OutputDebugStringOem(demDebugBuffer);
	sprintf(demDebugBuffer,"\tCS=%.4x IP=%.4x DS=%.4x ES=%.4x SS=%.4x SP=%.4x BP=%.4x CF=%x\n",
                getCS(),getIP(), getDS(),getES(),getSS(),getSP(),getBP(),getCF());
        OutputDebugStringOem(demDebugBuffer);
    }
#endif
    return TRUE;
}

VOID demNotYetImplemented(VOID)
{
    if (fShowSVCMsg) {
        sprintf(demDebugBuffer,"Unimplemented SVC %d\n",CurrentISVC);
        OutputDebugStringOem(demDebugBuffer);
    }
    setCF(0);
}

VOID demSetV86KernelAddr(VOID)
{
    return;
}

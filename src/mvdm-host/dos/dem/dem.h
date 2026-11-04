/* dem.h - main include file for dem
 *
 * Modification History
 *
 * Sudeepb 31-Mar-1991 Created
 */

#ifndef _DEMINCLUDED_
#define _DEMINCLUDED_

/*
#define WIN
#define FLAT_32
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>

#define _WINDOWS
#include <windows.h>

*/

#ifdef DOS
#line 24
#error Unsupported excluded original DEM host configuration
#endif
#line 26

#ifdef OS2_16
#line 28
#error DIVERGENCE MVDM-HOST-DIV-528: OS2_16 platform branch is not selected; see mirror register.
#endif
#line 31

#ifdef OS2_32
#line 33
#error DIVERGENCE MVDM-HOST-DIV-528: OS2_32 platform branch is not selected; see mirror register.
#endif
#line 36

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <process.h>

#ifdef WIN_16
#line 45
#error Unsupported excluded original DEM host configuration
#endif
#line 48

#ifdef WIN_32
#define WIN
#define FLAT_32
#define TRUE_IF_WIN32   1
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdddisk.h>
#else
#line 58
#error Unsupported excluded original DEM host configuration
#endif
#line 60

#ifdef FLAT_32
#ifndef i386
#define ALIGN_32
#else
#line 65
#error Unsupported excluded original DEM host configuration
#endif
#line 67
#endif

#ifdef WIN
#define _WINDOWS
#include <windows.h>
#endif

#ifdef SIGNALS
#line 75
#error Unsupported excluded original DEM host configuration
#endif
#line 78

#ifdef OS2_32
#line 80
#error DIVERGENCE MVDM-HOST-DIV-528: OS2_32 platform branch is not selected; see mirror register.
#endif
#line 83
#define SIGHIT(flChk)   ((iSigCheck++ & 0x7FF)?(flSignals & (flChk)):(kbhit(),(flSignals & (flChk))))

#include <oemuni.h>




/** Basic Typedefs of DEM **/

typedef VOID (*PFNSVC)(VOID);

typedef struct _SAVEDEMWORLD {
    USHORT  ax;
    USHORT  bx;
    USHORT  cx;
    USHORT  dx;
    USHORT  ds;
    USHORT  es;
    USHORT  si;
    USHORT  di;
    USHORT  bp;
    ULONG   iSVC;
} SAVEDEMWORLD, *PSAVEDEMWORLD;


typedef struct _DISKINFO {
    WORD   wSectorsPerCluster;
    WORD   wBytesPerSector;
    WORD   wFreeClusters;
    WORD   wTotalClusters;
} DISKINFO, *PDISKINFO;

#include "dosdef.h"
#include "dossvc.h"
#include <mvdm_guest_location.h>



/** DEM Externs **/

/* DIVERGENCE MVDM-HOST-DIV-005: the original x86 pointer identifies a
 * four-byte guest far-address cell; it is retained numerically. */
extern mvdm_guest_location dta_location;
extern BOOL   VDMForWOW;
extern PVHE   pHardErrPacket;
extern ULONG  CurrentISVC;
extern PCHAR  aSVCNames[];
extern PFNSVC apfnSVC[];
extern PSZ    pszDefaultDOSDirectory;
extern USHORT nDrives;
/* DIVERGENCE MVDM-HOST-DIV-006: current PDB is a two-byte guest scalar;
 * callers must read it under a fresh bounded lease. */
extern mvdm_guest_location current_pdb_location;
/* DIVERGENCE MVDM-HOST-DIV-007: the original SFT-chain head was retained as
 * a native pointer.  It remains the original numeric DS:offset location and
 * VDD obtains only fresh bounded shadows from it. */
extern mvdm_guest_location sft_head_location;
/* DIVERGENCE MVDM-HOST-DIV-005: the original x86 pointer is retained as a
 * numeric guest descriptor; every use must acquire a fresh bounded lease. */
extern mvdm_guest_location extended_error_location;


#include "demexp.h"
#if DEVL
extern CHAR demDebugBuffer [];
#endif




/** DEM Macros **/

#define GETULONG(hi,lo)     (DWORD)((((int) hi) << 16) + ((int) lo))
#define GETHANDLE(hi,lo)    (HANDLE)(GETULONG(hi,lo))
#define IS_ASCII_PATH_SEPARATOR(ch)     (((ch) == '/') || ((ch) == '\\'))


/** Function Prototypes */

VOID demChgFilePtr      (VOID);
VOID demChMod           (VOID);
VOID demClose           (VOID);
VOID demCloseFCB        (VOID);
VOID demCreate          (VOID);
VOID demCreateFCB       (VOID);
VOID demCreateDir       (VOID);
VOID demCreateNew       (VOID);
VOID demDate16          (VOID);
VOID demDelete          (VOID);
VOID demDeleteDir       (VOID);
VOID demDeleteFCB       (VOID);
VOID demFCBIO           (VOID);
VOID demFileTimes       (VOID);
VOID demFindFirst       (VOID);
VOID demFindFirstFCB    (VOID);
VOID demFindNext        (VOID);
VOID demFindNextFCB     (VOID);
VOID demGetBootDrive    (VOID);
VOID demGetDriveFreeSpace   (VOID);
VOID demGetDrives       (VOID);
VOID demGetFileInfo     (VOID);
VOID demGSetMediaID     (VOID);
VOID demIOCTL           (VOID);
VOID demLoadDos         (VOID);
VOID demLockOper        (VOID);
VOID demOpen            (VOID);
VOID demOpenFCB         (VOID);
VOID demQueryCurrentDir (VOID);
VOID demQueryDate       (VOID);
VOID demQueryTime       (VOID);
VOID demRead            (VOID);
VOID demRename          (VOID);
VOID demRenameFCB       (VOID);
VOID demRetry           (VOID);
VOID demSetCurrentDir   (VOID);
VOID demSetDate         (VOID);
VOID demSetDefaultDrive (VOID);
VOID demSetDTALocation  (VOID);
VOID demSetHardErrorInfo(VOID);
VOID demSetTime         (VOID);
VOID demSetV86KernelAddr(VOID);
VOID demWrite           (VOID);
VOID demGetDriveInfo    (VOID);
VOID demDiskReset       (VOID);
VOID demLoadDosAppSym   (VOID);
VOID demFreeDosAppSym   (VOID);
VOID demEntryDosApp     (VOID);
VOID demDOSDispCall     (VOID);
VOID demDOSDispRet      (VOID);
VOID demOutputString    (VOID);
VOID demInputString     (VOID);
VOID demIsDebug         (VOID);
VOID demTerminatePDB    (VOID);
VOID demExitVDM         (VOID);
VOID demWOWFiles        (VOID);
VOID demGetComputerName (VOID);
VOID demCheckPath	(VOID);
VOID demSystemSymbolOp  (VOID);
VOID demCommit		(VOID);
VOID demClientError         (HANDLE,CHAR);
ULONG demClientErrorEx      (HANDLE,CHAR,BOOL);
VOID demCreateCommon        (ULONG);
BOOL demGetMiscInfo         (HANDLE, LPWORD, LPWORD, LPDWORD);
VOID demFCBCommon           (ULONG);

VOID demIoctlChangeable     (VOID);
VOID demIoctlInvalid        (VOID);
VOID demSaveHardErrInfo     (VOID);
VOID demRestoreHardErrInfo  (VOID);
VOID demAbsRead 	    (VOID);
VOID demAbsWrite	    (VOID);
VOID demIoctlDiskGeneric    (VOID);
VOID demIoctlDiskQuery	    (VOID);
VOID demGetDPB		    (VOID);
VOID demGetDPBList	    (VOID);
VOID demNotYetImplemented   (VOID);
BOOL GetDiskSpaceInformation(CHAR chDrive, PDISKINFO pDiskInfo);
BOOL demGetDiskFreeSpace(BYTE Drive, WORD * BytesPerSector,
			 WORD * SectorsPerCluster, WORD * TotalClusters,
                         WORD * FreeClusters);
BOOL IsCdRomFile            (PSTR pszPath);

BOOL GetMediaId( CHAR DriveNum, PVOLINFO pVolInfo);
VOID demPipeFileDataEOF     (VOID);
VOID demPipeFileEOF	    (VOID);

/** Debug Function Prototypes */

#if DBG
#line 241
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#else
#line 245

#define demPrintMsg(x)

#endif
#line 249

/* Label functions and constants */
USHORT demDeleteLabel(BYTE Drive);
USHORT demCreateLabel(BYTE Drive, LPSTR szName);
#define DRIVEBYTE   0
#define LABELOFF    3

extern	BOOL cmdPipeFileDataEOF (HANDLE,BOOL *);
extern	BOOL cmdPipeFileEOF(HANDLE);


#endif /* _DEMINCLUDED_ */

/* DIVERGENCE(MVDM-HOST-DIV-294): explicit cdecl for the existing CPU40
 * worker exports consumed by the original /Gz WOW32 DLL. Parameter types,
 * implementation and ownership are unchanged; no stack-incompatible aliases. */
/*++ BUILD Version: 0001

Copyright (c) 1990  Microsoft Corporation

Module Name:

    SOFTPC.H

Abstract:

    High-level include file for components interfacing to SoftPC

Author:

    Dave Hastings (daveh) 25-Apr-1991

Revision History:
    Sudeep Bharati        23-Aug-1991  Added SOFTPC_BLD
    Matt Felton            8-FEB-1992  Added getIntelRegistersPointer
    Jonle                 18-Sep-1992  Add popup for unsupported functionality
    Jonle                 21-Nov-1992  Add Standard Resource Error Dialog Box
                                           GetPIFConfigFiles

--*/


/********** COMMON STUFF FOR MIPS AND V86 *************/

/* XLATOFF */
#line 30
#ifdef i386
#line 31
#error Excluded unsupported i386 kernel VDM declarations
#endif
#line 33


/* XLATON */
extern VOID nt_block_event_thread(ULONG);
extern VOID nt_resume_event_thread(VOID);
/* notification of PDB(Process Data Block, A.K.A PSP) termination */
extern VOID HostTerminatePDB(USHORT pdb);
/* disk subsystem reset notification. These functions will close all
 * outstanding opened handles for DASD(Direct AcceS Disk) */
extern VOID HostFloppyReset(VOID);
extern VOID HostFdiskReset(VOID);


// unsupported services dialog box
extern VOID host_direct_access_error(ULONG);

extern VOID host_lpt_flush_initialize(VOID);

#define NOSUPPORT_FLOPPY      0
#define NOSUPPORT_HARDDISK    1
#define NOSUPPORT_DRIVER      2
#define NOSUPPORT_OLDPIF      3
#define NOSUPPORT_ILLBOP      4
#define NOSUPPORT_NOLIM       5
#define NOSUPPORT_MOUSEDRVR   6

// standard error dialog box using resources
void RcErrorDialogBox(USHORT wId, CHAR *msg1, CHAR *msg2);


/*
 *  RcMessageBox\EditBox stuff
 */
#define RMB_ABORT        1
#define RMB_RETRY        2
#define RMB_IGNORE       4
#define RMB_ICON_INFO    8
#define RMB_ICON_BANG   16
#define RMB_ICON_STOP   32
#define RMB_ICON_WHAT   64
#define RMB_EDIT       128
#define RMB_FLAGS_MASK 0x0000FFFF
#define RMB_EDITBUFFERSIZE_MASK 0xFFFF0000
// hiword of dwOptions is reserved for RMB_EDIT text buffer size

int RcMessageBox(USHORT wId, CHAR *msg1, CHAR *msg2, ULONG dwOptions);


// sudeepb 02-May-1993 these following defines are actually defined
// in host\inc\error.h and host\inc\nt_uis.h. this stuff needs
// major cleanup after product 1.0.

#define EG_MALLOC_FAILURE       7
#define EG_PIF_BAD_FORMAT      18
#define EG_PIF_STARTDIR_ERR    19
#define	EG_PIF_STARTFILE_ERR   20
#define EG_PIF_CMDLINE_ERR     21
#define EG_PIF_ASK_CMDLINE     22
#define EG_ENVIRONMENT_ERR     23
#define EG_BAD_FAULT           27
#define EG_DOS_PROG_EXTENSION  28

#define ED_BADSYSFILE		336
#define ED_INITMEMERR           337
#define ED_INITTMPFILE          338


//
// SysErrorBox stuff -- duplicated in usersrv.h *and* kernel.inc
//
#define  SEB_OK         1  /* Button with "OK".     */
#define  SEB_CANCEL     2  /* Button with "Cancel"  */
#define  SEB_YES        3  /* Button with "&Yes"     */
#define  SEB_NO         4  /* Button with "&No"      */
#define  SEB_RETRY      5  /* Button with "&Retry"   */
#define  SEB_ABORT      6  /* Button with "&Abort"   */
#define  SEB_IGNORE     7  /* Button with "&Ignore"  */
#define  SEB_CLOSE      8  /* Button with "&Close"   */

#define  SEB_DEFBUTTON  0x8000  /* Mask to make this button default */

ULONG __cdecl WOWSysErrorBox(
    LPSTR  szTitle,
    LPSTR  szMessage,
    USHORT wBtn1,
    USHORT wBtn2,
    USHORT wBtn3
    );

// Called by WOW to force VdmAllocateVirtualMemory to alloc blocks of
// memory with ever increasing linear address's.
VOID __cdecl SetWOWforceIncrAlloc(
    BOOL iEnable
    );

// call out to softpc to get config.sys\autoexec.bat file names
VOID GetPIFConfigFiles(BOOL bConfig, char *pchFileName);

// exported interfaces
extern VOID    TerminateVDM (VOID);
extern ULONG   DosSessionId;

// VDD idle callouts
void WaitIfIdle(void);
void WakeUpNow(void);


//
// Constants
//
#define MSW_PE              0x1

/* XLATOFF */
#define ISPESET             (UCHAR) (getMSW() & MSW_PE ? 1 : 0)
/* XLATON */

//
// Macros
//

#define EXPORT

// Flag Register constants

#define FLG_CARRY           0x00000001
#define FLG_CARRY_BIT       0x00000000
#define FLG_PARITY          0x00000004
#define FLG_PARITY_BIT      0x00000003
#define FLG_AUXILIARY       0x00000010
#define FLG_AUXILIARY_BIT   0x00000005
#define FLG_ZERO            0x00000040
#define FLG_ZERO_BIT        0x00000006
#define FLG_SIGN            0x00000080
#define FLG_SIGN_BIT        0x00000007
#define FLG_TRAP            0x00000100

#define FLG_INTERRUPT       0x00000200
#define FLG_INTERRUPT_BIT   0x00000009
#define FLG_DIRECTION       0x00000400
#define FLG_DIRECTION_BIT   0x0000000A
#define FLG_OVERFLOW        0x00000800
#define FLG_OVERFLOW_BIT    0x0000000B

EXPORT
VOID
host_cpu_init(
     VOID
     );

//EXPORT
//VOID
//sas_init(
//    IN sys_addr Size
//    );

EXPORT
VOID
__cdecl host_simulate(
    VOID
    );

HANDLE
WINAPI
#line 195
host_CreateThread(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    DWORD dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId
    );

VOID
__cdecl cpu_createthread(
    HANDLE  hThread
    );

VOID
WINAPI
#line 210
host_ExitThread(
    DWORD dwExitCode
    );


EXPORT VOID __cdecl host_com_close(int PortId);
EXPORT int __cdecl SuspendTimerThread(VOID);
EXPORT int __cdecl ResumeTimerThread(VOID);

void *ch_malloc(unsigned int NumBytes);


UCHAR *__cdecl Sim32pGetVDMPointer(ULONG addr, UCHAR pm);
#define Sim32GetVDMPointer(Addr,Size,Mode) Sim32pGetVDMPointer(Addr,Mode)

#line 225
#ifdef i386
#line 226
#error Excluded unsupported i386 kernel VDM register interface
#else
#line 532

/*********************** FOR MIPS BUILD ***************************/


#line 531
#define GetVDMAddr(usSeg,usOff) Sim32pGetVDMPointer((ULONG)(((ULONG)usSeg << 16) | usOff),FALSE)

#define RMSEGOFFTOLIN(seg, off) (PVOID)(((ULONG)(seg) << 4) + (ULONG)(off))

extern BOOL     Sim32FlushVDMPointer (ULONG, USHORT, PBYTE , BOOL);
extern BOOL     Sim32FreeVDMPointer (ULONG, USHORT, PBYTE , BOOL);
extern BOOL     Sim32GetVDMMemory (ULONG, USHORT, PBYTE , BOOL);
extern BOOL     Sim32SetVDMMemory (ULONG, USHORT, PBYTE , BOOL);
extern VOID     sas_overwrite_memory(PBYTE, ULONG);

extern UCHAR getAL(VOID);
extern UCHAR getCL(VOID);
extern UCHAR getDL(VOID);
extern UCHAR getBL(VOID);
extern UCHAR getAH(VOID);
extern UCHAR getCH(VOID);
extern UCHAR getDH(VOID);
extern UCHAR getBH(VOID);

extern VOID setAL(UCHAR val);
extern VOID setCL(UCHAR val);
extern VOID setDL(UCHAR val);
extern VOID setBL(UCHAR val);
extern VOID setAH(UCHAR val);
extern VOID setCH(UCHAR val);
extern VOID setDH(UCHAR val);
extern VOID setBH(UCHAR val);

extern USHORT __cdecl getAX(VOID);
extern USHORT __cdecl getCX(VOID);
extern USHORT __cdecl getDX(VOID);
extern USHORT __cdecl getBX(VOID);
extern USHORT __cdecl getSP(VOID);
extern USHORT __cdecl getBP(VOID);
extern USHORT __cdecl getSI(VOID);
extern USHORT __cdecl getDI(VOID);
extern USHORT __cdecl getIP(VOID);

extern VOID __cdecl setAX(USHORT val);
extern VOID __cdecl setCX(USHORT val);
extern VOID __cdecl setDX(USHORT val);
extern VOID __cdecl setBX(USHORT val);
extern VOID __cdecl setSP(USHORT val);
extern VOID __cdecl setBP(USHORT val);
extern VOID __cdecl setSI(USHORT val);
extern VOID __cdecl setDI(USHORT val);
extern VOID __cdecl setIP(USHORT val);

extern ULONG getEAX(VOID);
extern ULONG getECX(VOID);
extern ULONG getEDX(VOID);
extern ULONG getEBX(VOID);
extern ULONG getESP(VOID);
extern ULONG getEBP(VOID);
extern ULONG getESI(VOID);
extern ULONG getEDI(VOID);
extern ULONG getEIP(VOID);

extern VOID setEAX(ULONG val);
extern VOID setECX(ULONG val);
extern VOID setEDX(ULONG val);
extern VOID setEBX(ULONG val);
extern VOID setESP(ULONG val);
extern VOID setEBP(ULONG val);
extern VOID setESI(ULONG val);
extern VOID setEDI(ULONG val);
extern VOID setEIP(ULONG val);

extern USHORT __cdecl getES(VOID);
extern USHORT __cdecl getCS(VOID);
extern USHORT __cdecl getSS(VOID);
extern USHORT __cdecl getDS(VOID);
extern USHORT getFS(VOID);
extern USHORT getGS(VOID);

extern VOID __cdecl setES(USHORT val);
extern VOID __cdecl setCS(USHORT val);
extern VOID __cdecl setSS(USHORT val);
extern VOID __cdecl setDS(USHORT val);
extern VOID setFS(USHORT val);
extern VOID setGS(USHORT val);

extern ULONG getAF(VOID);
extern ULONG getCF(VOID);
extern ULONG getDF(VOID);
extern ULONG getIF(VOID);
extern ULONG getOF(VOID);
extern ULONG getPF(VOID);
extern ULONG getSF(VOID);
extern ULONG getTF(VOID);
extern ULONG getZF(VOID);
extern ULONG getIOPL(VOID);
extern ULONG getNT(VOID);
extern USHORT getSTATUS(VOID);
extern ULONG getEFLAGS(VOID);
extern USHORT __cdecl getMSW(VOID);

extern VOID setAF(ULONG val);
extern VOID setCF(ULONG val);
extern VOID setDF(ULONG val);
extern VOID setIF(ULONG val);
extern VOID setOF(ULONG val);
extern VOID setPF(ULONG val);
extern VOID setSF(ULONG val);
extern VOID setTF(ULONG val);
extern VOID setZF(ULONG val);
extern VOID setIOPL(ULONG val);
extern VOID setNT(ULONG val);
extern VOID setSTATUS(USHORT val);
extern VOID setEFLAGS(ULONG val);
extern VOID setMSW(USHORT val);

extern ULONG getCR0(VOID);
extern VOID setCR0(ULONG val);
extern ULONG getSS_AR(VOID);
extern ULONG getSS_BASE(VOID);
extern VOID setSS_BASE_LIMIT_AR(ULONG base, ULONG limit, ULONG ar);
extern VOID setCPL(ULONG val);


#endif

/*++

Copyright (c) 1992 Microsoft Corporation

Module Name:

    VDDSVC.H

Abstract:

    Include file contains VDM services provided for installable VDDs.


--*/


/**
 * This file contains VDM services prototype defintions only; their
 * related structures and macros are defined in NT_VDD.H.
 * If we have not included the file yet, include it and set a signal
 * to tell anybody the fact.
**/

#ifndef _NT_VDD
#include "nt_vdd.h"
#define _NT_VDD
#endif


/** Memory Accessing services **/

#define GetVDMAddress(usSeg, usOff) (((ULONG)usSeg << 4) + (ULONG)usOff)

#define GetVDMPointer(Address, Size, Mode) Sim32GetVDMPointer(\
						Address, Size, Mode)

#define FlushVDMPointer(Address, Size, Buffer, Mode) Sim32FlushVDMPointer(\
					   Address, Size, Buffer, Mode)

#define FreeVDMPointer(Address, Size, Buffer, Mode) Sim32FreeVDMPointer(\
					   Address, Size, Buffer, Mode)

/** interrupt simualtion services **/

#define ICA_MASTER 0
#define ICA_SLAVE  1
#define VDDSimulateInterrupt(ms, line, count) (call_ica_hw_interrupt)(\
						   ms, line, 1)


/** Register manipulation services **/

#ifndef i386
#error
#line 135
#endif


/** Real function prototype declarations **/


/** interrupt simulation functions **/

VOID
call_ica_hw_interrupt (
 int ms,
 BYTE line,
 int count
 );

#define Sim32FreeVDMPointer(address, size, buffer, mode) TRUE

/** memory address manipulation functions **/

#ifdef i386

#define Sim32GetVDMPointer(address, size, mode) MGetVdmPointer(address,\
                                                               size, mode)
#define Sim32FlushVDMPointer(addess, size, buffer, mode) TRUE

PBYTE
MGetVdmPointer(
 ULONG	 Address,
 ULONG	 Size,
 UCHAR   ProtectedMode
);

#else
#error
#line 184
#endif


/** Register manipulation functions **/

#ifdef i386
ULONG   getEAX(VOID);
USHORT  getAX(VOID);
UCHAR   getAL(VOID);
UCHAR   getAH(VOID);
ULONG   getEBX(VOID);
USHORT  getBX(VOID);
UCHAR   getBL(VOID);
UCHAR   getBH(VOID);
ULONG   getECX(VOID);
USHORT  getCX(VOID);
UCHAR   getCL(VOID);
UCHAR   getCH(VOID);
ULONG   getEDX(VOID);
USHORT  getDX(VOID);
UCHAR   getDL(VOID);
UCHAR   getDH(VOID);
ULONG   getESP(VOID);
USHORT  getSP(VOID);
ULONG   getEBP(VOID);
USHORT  getBP(VOID);
ULONG   getESI(VOID);
USHORT  getSI(VOID);
ULONG   getEDI(VOID);
USHORT  getDI(VOID);
ULONG   getEIP(VOID);
USHORT  getIP(VOID);
USHORT  getCS(VOID);
USHORT  getSS(VOID);
USHORT  getDS(VOID);
USHORT  getES(VOID);
USHORT  getFS(VOID);
USHORT  getGS(VOID);
ULONG   getCF(VOID);
ULONG   getPF(VOID);
ULONG   getAF(VOID);
ULONG   getZF(VOID);
ULONG   getSF(VOID);
ULONG   getIF(VOID);
ULONG   getDF(VOID);
ULONG   getOF(VOID);
USHORT	getMSW(VOID);

VOID    setEAX(ULONG);
VOID    setAX(USHORT);
VOID    setAH(UCHAR);
VOID    setAL(UCHAR);
VOID    setEBX(ULONG);
VOID    setBX(USHORT);
VOID    setBH(UCHAR);
VOID    setBL(UCHAR);
VOID    setECX(ULONG);
VOID    setCX(USHORT);
VOID    setCH(UCHAR);
VOID    setCL(UCHAR);
VOID    setEDX(ULONG);
VOID    setDX(USHORT);
VOID    setDH(UCHAR);
VOID    setDL(UCHAR);
VOID    setESP(ULONG);
VOID    setSP(USHORT);
VOID    setEBP(ULONG);
VOID    setBP(USHORT);
VOID    setESI(ULONG);
VOID    setSI(USHORT);
VOID    setEDI(ULONG);
VOID    setDI(USHORT);
VOID    setEIP(ULONG);
VOID    setIP(USHORT);
VOID    setCS(USHORT);
VOID    setSS(USHORT);
VOID    setDS(USHORT);
VOID    setES(USHORT);
VOID    setFS(USHORT);
VOID    setGS(USHORT);
VOID    setCF(ULONG);
VOID    setPF(ULONG);
VOID    setAF(ULONG);
VOID    setZF(ULONG);
VOID    setSF(ULONG);
VOID    setIF(ULONG);
VOID    setDF(ULONG);
VOID    setOF(ULONG);
VOID    setMSW(USHORT);


#else
#error
#line 346
#endif


/* end of VDDSVC.H */

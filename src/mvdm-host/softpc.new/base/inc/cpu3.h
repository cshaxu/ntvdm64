
/*
 * SoftPC Revision 3.0
 *
 * Title	: Definitions for the 3.x CPU 
 *
 * Description	: Structures, macros and definitions for access to the 
 *		  CPU registers
 *
 * Author	: Wayne Plummer
 *
 * Derived from : cpu.h
 *
 * Notes	: This file should be portable - but includes a file
 *		  host_cpu.h which contains machine specific definitions
 *		  of CPU register mappings etc.
 */

/* SccsID[]="@(#)cpu3.h	1.5 12/22/93 Copyright Insignia Solutions Ltd."; */

#include "host_cpu.h"

IMPORT VOID host_set_hw_int IPT0();
IMPORT VOID host_clear_hw_int IPT0();

/*
 * These variables are obsolete - however they are referenced
 * by:-
 *
 *	1. ica.c
 */

extern  word            cpu_interrupt_map;
extern  half_word       cpu_int_translate[];
extern  word            cpu_int_delay;
extern  half_word       ica_lock;
extern  void            (*(jump_ptrs[]))();
extern  void            (*(b_write_ptrs[]))();
extern  void            (*(w_write_ptrs[]))();
extern  void            (*(b_fill_ptrs[]))();
extern  void            (*(w_fill_ptrs[]))();
extern  void            (*(b_move_ptrs[]))();
extern  void            (*(w_move_ptrs[]))();
extern  half_word       *haddr_of_src_string;

/*
 *	Host functions to be provided for the base to use with respect to the CPU.
 *	These must be done in host_cpu.h because some hosts may want functions and
 *	others may want #defines.
 */

/*
 *	This macro specifies the maximum recursion depth the CPU is required to support.
 *	(Note that a particular host may not actually use this value if it is capable
 *	of supporting abirtarily deep recursion).
 */
#define CPU_MAX_RECURSION	32

/*
 *	Interrupt types...
 */

typedef enum {	CPU_HW_RESET,
		CPU_TIMER_TICK,
		CPU_SW_INT,
		CPU_HW_INT,
		CPU_YODA_INT,
		CPU_SIGIO_EVENT
} CPU_INT_TYPE;

#ifdef CPU_PRIVATE
/*
   Function returns for private i/f procedures handling segment loading.
 */

#define SELECTOR_OK  0
#define GP_ERROR    13
#define NP_ERROR    11
#define SF_ERROR    12
#endif /* CPU_PRIVATE */

#ifdef CCPU

/* Fuctions provided by CPU */
IMPORT void		c_cpu_init	IPT0 ();
IMPORT void		c_cpu_interrupt	IPT2(CPU_INT_TYPE, type, IU16, number);
IMPORT void		c_cpu_simulate	IPT0();
IMPORT void		c_cpu_q_ev_set_count	IPT1(ULONG, new_count);
IMPORT ULONG		c_cpu_q_ev_get_count	IPT0();
IMPORT ULONG		c_cpu_calc_q_ev_inst_for_time	IPT1(ULONG, time);
IMPORT void		c_cpu_EOA_hook	IPT0();
IMPORT void		c_cpu_terminate	IPT0();

#define cpu_init		c_cpu_init
#define cpu_interrupt		c_cpu_interrupt
#define cpu_simulate		c_cpu_simulate
#define	host_q_ev_set_count	c_cpu_q_ev_set_count
#define	host_q_ev_get_count	c_cpu_q_ev_get_count
#ifndef host_calc_q_ev_inst_for_time
#define	host_calc_q_ev_inst_for_time	c_cpu_calc_q_ev_inst_for_time
#endif /* host_calc_q_ev_inst_for_time */
#define cpu_EOA_hook		c_cpu_EOA_hook
#define cpu_terminate		c_cpu_terminate

#ifndef CCPU_MAIN


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Provide Access to Byte Registers.                                  */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

IMPORT half_word c_getAL	IPT0();
IMPORT half_word c_getCL	IPT0();
IMPORT half_word c_getDL	IPT0();
IMPORT half_word c_getBL	IPT0();
IMPORT half_word c_getAH	IPT0();
IMPORT half_word c_getCH	IPT0();
IMPORT half_word c_getDH	IPT0();
IMPORT half_word c_getBH	IPT0();
   
IMPORT void c_setAL	IPT1(half_word, val);
IMPORT void c_setCL	IPT1(half_word, val);
IMPORT void c_setDL	IPT1(half_word, val);
IMPORT void c_setBL	IPT1(half_word, val);
IMPORT void c_setAH	IPT1(half_word, val);
IMPORT void c_setCH	IPT1(half_word, val);
IMPORT void c_setDH	IPT1(half_word, val);
IMPORT void c_setBH	IPT1(half_word, val);

#define getAL() c_getAL()
#define getCL() c_getCL()
#define getDL() c_getDL()
#define getBL() c_getBL()
#define getAH() c_getAH()
#define getCH() c_getCH()
#define getDH() c_getDH()
#define getBH() c_getBH()

#define setAL(x) c_setAL(x)
#define setCL(x) c_setCL(x)
#define setDL(x) c_setDL(x)
#define setBL(x) c_setBL(x)
#define setAH(x) c_setAH(x)
#define setCH(x) c_setCH(x)
#define setDH(x) c_setDH(x)
#define setBH(x) c_setBH(x)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Provide Access to Word Registers.                                  */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

IMPORT word c_getAX	IPT0();
IMPORT word c_getCX	IPT0();
IMPORT word c_getDX	IPT0();
IMPORT word c_getBX	IPT0();
IMPORT word c_getSP	IPT0();
IMPORT word c_getBP	IPT0();
IMPORT word c_getSI	IPT0();
IMPORT word c_getDI	IPT0();
IMPORT word c_getIP	IPT0();

IMPORT void c_setAX	IPT1(word, val);
IMPORT void c_setCX	IPT1(word, val);
IMPORT void c_setDX	IPT1(word, val);
IMPORT void c_setBX	IPT1(word, val);
IMPORT void c_setSP	IPT1(word, val);
IMPORT void c_setBP	IPT1(word, val);
IMPORT void c_setSI	IPT1(word, val);
IMPORT void c_setDI	IPT1(word, val);
IMPORT void c_setIP	IPT1(word, val);

#define getAX() c_getAX()
#define getCX() c_getCX()
#define getDX() c_getDX()
#define getBX() c_getBX()
#define getSP() c_getSP()
#define getBP() c_getBP()
#define getSI() c_getSI()
#define getDI() c_getDI()
#define getIP() c_getIP()

#define setAX(x) c_setAX(x)
#define setCX(x) c_setCX(x)
#define setDX(x) c_setDX(x)
#define setBX(x) c_setBX(x)
#define setSP(x) c_setSP(x)
#define setBP(x) c_setBP(x)
#define setSI(x) c_setSI(x)
#define setDI(x) c_setDI(x)
#define setIP(x) c_setIP(x)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Provide Access to Segment Registers.                               */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

IMPORT word c_getES	IPT0();
IMPORT word c_getCS	IPT0();
IMPORT word c_getSS	IPT0();
IMPORT word c_getDS	IPT0();

IMPORT INT c_setES	IPT1(word, val);
IMPORT INT c_setCS	IPT1(word, val);
IMPORT INT c_setSS	IPT1(word, val);
IMPORT INT c_setDS	IPT1(word, val);

#define getES() c_getES()
#define getCS() c_getCS()
#define getSS() c_getSS()
#define getDS() c_getDS()

#define setES(x) c_setES(x)
#define setCS(x) c_setCS(x)
#define setSS(x) c_setSS(x)
#define setDS(x) c_setDS(x)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Provide Access to Full(Private) Segment Registers.                 */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef CPU_PRIVATE

IMPORT word c_getES_SELECTOR	IPT0();
IMPORT word c_getCS_SELECTOR	IPT0();
IMPORT word c_getSS_SELECTOR	IPT0();
IMPORT word c_getDS_SELECTOR	IPT0();

IMPORT word c_getCS_LIMIT	IPT0();
IMPORT word c_getDS_LIMIT	IPT0();
IMPORT word c_getES_LIMIT	IPT0();
IMPORT word c_getSS_LIMIT	IPT0();

IMPORT long c_getCS_BASE	IPT0();
IMPORT long c_getDS_BASE	IPT0();
IMPORT long c_getES_BASE	IPT0();
IMPORT long c_getSS_BASE	IPT0();

IMPORT half_word c_getCS_AR	IPT0();
IMPORT half_word c_getDS_AR	IPT0();
IMPORT half_word c_getES_AR	IPT0();
IMPORT half_word c_getSS_AR	IPT0();

IMPORT void c_setES_SELECTOR	IPT1(word, val);
IMPORT void c_setCS_SELECTOR	IPT1(word, val);
IMPORT void c_setSS_SELECTOR	IPT1(word, val);
IMPORT void c_setDS_SELECTOR	IPT1(word, val);

IMPORT void c_setCS_LIMIT	IPT1(word, val);
IMPORT void c_setDS_LIMIT	IPT1(word, val);
IMPORT void c_setES_LIMIT	IPT1(word, val);
IMPORT void c_setSS_LIMIT	IPT1(word, val);

IMPORT void c_setCS_BASE	IPT1(long, val);
IMPORT void c_setDS_BASE	IPT1(long, val);
IMPORT void c_setES_BASE	IPT1(long, val);
IMPORT void c_setSS_BASE	IPT1(long, val);

IMPORT void c_setCS_AR	IPT1(half_word, val);
IMPORT void c_setDS_AR	IPT1(half_word, val);
IMPORT void c_setES_AR	IPT1(half_word, val);
IMPORT void c_setSS_AR	IPT1(half_word, val);

#define getES_SELECTOR c_getES_SELECTOR
#define getCS_SELECTOR c_getCS_SELECTOR
#define getSS_SELECTOR c_getSS_SELECTOR
#define getDS_SELECTOR c_getDS_SELECTOR

#define getDS_LIMIT c_getDS_LIMIT
#define getCS_LIMIT c_getCS_LIMIT
#define getES_LIMIT c_getES_LIMIT
#define getSS_LIMIT c_getSS_LIMIT

#define getDS_BASE c_getDS_BASE
#define getCS_BASE c_getCS_BASE
#define getES_BASE c_getES_BASE
#define getSS_BASE c_getSS_BASE

#define getDS_AR c_getDS_AR
#define getCS_AR c_getCS_AR
#define getES_AR c_getES_AR
#define getSS_AR c_getSS_AR

#define setES_SELECTOR c_setES_SELECTOR
#define setCS_SELECTOR c_setCS_SELECTOR
#define setSS_SELECTOR c_setSS_SELECTOR
#define setDS_SELECTOR c_setDS_SELECTOR

#define setDS_LIMIT c_setDS_LIMIT
#define setCS_LIMIT c_setCS_LIMIT
#define setES_LIMIT c_setES_LIMIT
#define setSS_LIMIT c_setSS_LIMIT

#define setDS_BASE c_setDS_BASE
#define setCS_BASE c_setCS_BASE
#define setES_BASE c_setES_BASE
#define setSS_BASE c_setSS_BASE

#define setDS_AR c_setDS_AR
#define setCS_AR c_setCS_AR
#define setES_AR c_setES_AR
#define setSS_AR c_setSS_AR

#endif /* CPU_PRIVATE */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Provide Access to Flags.                                           */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

IMPORT INT c_getAF	IPT0();
IMPORT INT c_getCF	IPT0();
IMPORT INT c_getDF	IPT0();
IMPORT INT c_getIF	IPT0();
IMPORT INT c_getOF	IPT0();
IMPORT INT c_getPF	IPT0();
IMPORT INT c_getSF	IPT0();
IMPORT INT c_getTF	IPT0();
IMPORT INT c_getZF	IPT0();
IMPORT INT c_getIOPL	IPT0();
IMPORT INT c_getNT	IPT0();
IMPORT word c_getSTATUS	IPT0();

IMPORT void c_setAF	IPT1(INT, val);
IMPORT void c_setCF	IPT1(INT, val);
IMPORT void c_setDF	IPT1(INT, val);
IMPORT void c_setIF	IPT1(INT, val);
IMPORT void c_setOF	IPT1(INT, val);
IMPORT void c_setPF	IPT1(INT, val);
IMPORT void c_setSF	IPT1(INT, val);
IMPORT void c_setTF	IPT1(INT, val);
IMPORT void c_setZF	IPT1(INT, val);
IMPORT void c_setIOPL	IPT1(INT, val);
IMPORT void c_setNT	IPT1(INT, val);

#define getAF()     c_getAF()
#define getCF()     c_getCF()
#define getDF()     c_getDF()
#define getIF()     c_getIF()
#define getOF()     c_getOF()
#define getPF()     c_getPF()
#define getSF()     c_getSF()
#define getTF()     c_getTF()
#define getZF()     c_getZF()
#define getIOPL()   c_getIOPL()
#define getNT()     c_getNT()
#define getSTATUS() c_getSTATUS()

#define setAF(x)     c_setAF(x)
#define setCF(x)     c_setCF(x)
#define setDF(x)     c_setDF(x)
#define setIF(x)     c_setIF(x)
#define setOF(x)     c_setOF(x)
#define setPF(x)     c_setPF(x)
#define setSF(x)     c_setSF(x)
#define setTF(x)     c_setTF(x)
#define setZF(x)     c_setZF(x)
#define setIOPL(x)   c_setIOPL(x)
#define setNT(x)     c_setNT(x)

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Provide Access to Machine Status Word.                             */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

IMPORT INT c_getPE	IPT0();
IMPORT INT c_getMP	IPT0();
IMPORT INT c_getEM	IPT0();
IMPORT INT c_getTS	IPT0();
IMPORT word c_getMSW	IPT0();

#define getPE() c_getPE()
#define getMP() c_getMP()
#define getEM() c_getEM()
#define getTS() c_getTS()
#define getMSW() c_getMSW()

#ifdef CPU_PRIVATE

IMPORT void c_setPE	IPT1(INT, val);
IMPORT void c_setMP	IPT1(INT, val);
IMPORT void c_setEM	IPT1(INT, val);
IMPORT void c_setTS	IPT1(INT, val);
IMPORT void c_setMSW	IPT1(word, val);

#define setPE(x) c_setPE(x)
#define setMP(x) c_setMP(x)
#define setEM(x) c_setEM(x)
#define setTS(x) c_setTS(x)
#define setMSW(x) c_setMSW(x)

#endif /* CPU_PRIVATE */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Provide Access to Descriptor Registers.                            */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

IMPORT sys_addr c_getGDT_BASE	IPT0();
IMPORT sys_addr c_getIDT_BASE	IPT0();
IMPORT sys_addr c_getLDT_BASE	IPT0();
IMPORT sys_addr c_getTR_BASE	IPT0();
IMPORT word c_getGDT_LIMIT	IPT0();
IMPORT word c_getIDT_LIMIT	IPT0();
IMPORT word c_getLDT_LIMIT	IPT0();
IMPORT word c_getTR_LIMIT	IPT0();
IMPORT word c_getLDT_SELECTOR	IPT0();
IMPORT word c_getTR_SELECTOR	IPT0();

#define getGDT_BASE() c_getGDT_BASE()
#define getIDT_BASE() c_getIDT_BASE()
#define getLDT_BASE() c_getLDT_BASE()
#define getTR_BASE()  c_getTR_BASE()
#define getGDT_LIMIT() c_getGDT_LIMIT()
#define getIDT_LIMIT() c_getIDT_LIMIT()
#define getLDT_LIMIT() c_getLDT_LIMIT()
#define getTR_LIMIT()  c_getTR_LIMIT()
#define getLDT_SELECTOR() c_getLDT_SELECTOR()
#define getTR_SELECTOR()  c_getTR_SELECTOR()

#ifdef CPU_PRIVATE

IMPORT void c_setGDT_BASE	IPT1(sys_addr, val);
IMPORT void c_setIDT_BASE	IPT1(sys_addr, val);
IMPORT void c_setLDT_BASE	IPT1(sys_addr, val);
IMPORT void c_setTR_BASE	IPT1(sys_addr, val);
IMPORT void c_setGDT_LIMIT	IPT1(word, val);
IMPORT void c_setIDT_LIMIT	IPT1(word, val);
IMPORT void c_setLDT_LIMIT	IPT1(word, val);
IMPORT void c_setTR_LIMIT	IPT1(word, val);
IMPORT void c_setLDT_SELECTOR	IPT1(word, val);
IMPORT void c_setTR_SELECTOR	IPT1(word, val);

#define setGDT_BASE(x) c_setGDT_BASE(x)
#define setIDT_BASE(x) c_setIDT_BASE(x)
#define setLDT_BASE(x) c_setLDT_BASE(x)
#define setTR_BASE(x)  c_setTR_BASE(x)
#define setGDT_LIMIT(x) c_setGDT_LIMIT(x)
#define setIDT_LIMIT(x) c_setIDT_LIMIT(x)
#define setLDT_LIMIT(x) c_setLDT_LIMIT(x)
#define setTR_LIMIT(x)  c_setTR_LIMIT(x)
#define setLDT_SELECTOR(x) c_setLDT_SELECTOR(x)
#define setTR_SELECTOR(x)  c_setTR_SELECTOR(x)

#endif /* CPU_PRIVATE */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Provide Access to Current Privilege Level.                         */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef CPU_PRIVATE

IMPORT INT c_getCPL	IPT0();
IMPORT VOID c_setCPL	IPT1(INT, val);

#define getCPL() c_getCPL()
#define setCPL(x) c_setCPL(x)

#endif /* CPU_PRIVATE */

#endif /* CCPU_MAIN */

#endif /* CCPU */

#ifdef A3CPU
#error Excluded unsupported A3CPU configuration





































































































































































































































































































































































#endif /* A3CPU */

#ifdef SPC386
/*
 * New additions for SPC386 builds follow...
 * eventually it would be nice to change the start of this file to conform
 * to the new style when there is effort to spare.
 *
 *       Wayne Plummer 19/3/93 (GISP project)
 */

#ifdef CCPU
IMPORT void c_setSTATUS	IPT1(IU16, val);
#define setSTATUS()  c_setSTATUS()
#else
IMPORT VOID		a3_setSTATUS IPT1(IU16, val);
#define setSTATUS	a3_setSTATUS
#endif


#ifdef CCPU
IMPORT IU32 c_getEFLAGS	IPT0();
#define getEFLAGS() c_getEFLAGS()
#else
IMPORT IU32		a3_getEFLAGS();
#define getEFLAGS	a3_getEFLAGS
#endif

#ifdef CCPU
IMPORT void c_setEFLAGS	IPT1(IU32, val);
#define setEFLAGS()  c_setEFLAGS()
#else
IMPORT VOID		a3_setEFLAGS IPT1(IU32, val);
#define setEFLAGS	a3_setEFLAGS
#endif


#ifdef CCPU
IMPORT   IU32   c_getEAX               IPT0();
#define           getEAX               c_getEAX
#else
IMPORT   IU32  a3_getEAX               IPT0();
#define           getEAX               a3_getEAX
#endif

#ifdef CCPU
IMPORT   IU32   c_getEBX               IPT0();
#define           getEBX               c_getEBX
#else
IMPORT   IU32  a3_getEBX               IPT0();
#define           getEBX               a3_getEBX
#endif

#ifdef CCPU
IMPORT   IU32   c_getECX               IPT0();
#define           getECX               c_getECX
#else
IMPORT   IU32  a3_getECX               IPT0();
#define           getECX               a3_getECX
#endif

#ifdef CCPU
IMPORT   IU32   c_getEDX               IPT0();
#define           getEDX               c_getEDX
#else
IMPORT   IU32  a3_getEDX               IPT0();
#define           getEDX               a3_getEDX
#endif

#ifdef CCPU
IMPORT   IU32   c_getESP               IPT0();
#define           getESP               c_getESP
#else
IMPORT   IU32  a3_getESP               IPT0();
#define           getESP               a3_getESP
#endif

#ifdef CCPU
IMPORT   IU32   c_getEBP               IPT0();
#define           getEBP               c_getEBP
#else
IMPORT   IU32  a3_getEBP               IPT0();
#define           getEBP               a3_getEBP
#endif

#ifdef CCPU
IMPORT   IU32   c_getESI               IPT0();
#define           getESI               c_getESI
#else
IMPORT   IU32  a3_getESI               IPT0();
#define           getESI               a3_getESI
#endif

#ifdef CCPU
IMPORT   IU32   c_getEDI               IPT0();
#define           getEDI               c_getEDI
#else
IMPORT   IU32  a3_getEDI               IPT0();
#define           getEDI               a3_getEDI
#endif

#ifdef CCPU
IMPORT   IU32   c_getEIP               IPT0();
#define           getEIP               c_getEIP
#else
IMPORT   IU32  a3_getEIP               IPT0();
#define           getEIP               a3_getEIP
#endif

#ifdef CCPU
IMPORT   IU16   c_getFS                IPT0();
#define           getFS                c_getFS 
#else
IMPORT   IU16  a3_getFS                IPT0();
#define           getFS                a3_getFS 
#endif

#ifdef CCPU
IMPORT   IU16   c_getGS                IPT0();
#define           getGS                c_getGS 
#else
IMPORT   IU16  a3_getGS                IPT0();
#define           getGS                a3_getGS 
#endif

#ifdef CCPU
IMPORT   IU16   c_getVM                IPT0();
#define           getVM                c_getVM 
#else
IMPORT   IU16  a3_getVM                IPT0();
#define           getVM                a3_getVM 
#endif

#ifdef CCPU
IMPORT   IU16   c_getPG                IPT0();
#define           getPG                c_getPG 
#else
IMPORT   IU16  a3_getPG                IPT0();
#define           getPG                a3_getPG 
#endif

#ifdef CCPU
IMPORT   VOID   c_setEAX               IPT1(IU32, val);
#define           setEAX               c_setEAX
#else
IMPORT   IU32  a3_setEAX               IPT1(IU32, val);
#define           setEAX               a3_setEAX
#endif

#ifdef CCPU
IMPORT   VOID   c_setEBX               IPT1(IU32, val);
#define           setEBX               c_setEBX
#else
IMPORT   IU32  a3_setEBX               IPT1(IU32, val);
#define           setEBX               a3_setEBX
#endif

#ifdef CCPU
IMPORT   VOID   c_setECX               IPT1(IU32, val);
#define           setECX               c_setECX
#else
IMPORT   IU32  a3_setECX               IPT1(IU32, val);
#define           setECX               a3_setECX
#endif

#ifdef CCPU
IMPORT   VOID   c_setEDX               IPT1(IU32, val);
#define           setEDX               c_setEDX
#else
IMPORT   IU32  a3_setEDX               IPT1(IU32, val);
#define           setEDX               a3_setEDX
#endif

#ifdef CCPU
IMPORT   VOID   c_setESP               IPT1(IU32, val);
#define           setESP               c_setESP
#else
IMPORT   IU32  a3_setESP               IPT1(IU32, val);
#define           setESP               a3_setESP
#endif

#ifdef CCPU
IMPORT   VOID   c_setEBP               IPT1(IU32, val);
#define           setEBP               c_setEBP
#else
IMPORT   IU32  a3_setEBP               IPT1(IU32, val);
#define           setEBP               a3_setEBP
#endif

#ifdef CCPU
IMPORT   VOID   c_setESI               IPT1(IU32, val);
#define           setESI               c_setESI
#else
IMPORT   IU32  a3_setESI               IPT1(IU32, val);
#define           setESI               a3_setESI
#endif

#ifdef CCPU
IMPORT   VOID   c_setEDI               IPT1(IU32, val);
#define           setEDI               c_setEDI
#else
IMPORT   IU32  a3_setEDI               IPT1(IU32, val);
#define           setEDI               a3_setEDI
#endif

#ifdef CCPU
IMPORT   VOID   c_setEIP               IPT1(IU32, val);
#define           setEIP               c_setEIP
#else
IMPORT   IU32  a3_setEIP               IPT1(IU32, val);
#define           setEIP               a3_setEIP
#endif

#ifdef CCPU
IMPORT   VOID   c_setFS                IPT1(IU16, val);
#define           setFS                c_setFS 
#else
IMPORT   IU32  a3_setFS                IPT1(IU16, val);
#define           setFS                a3_setFS 
#endif

#ifdef CCPU
IMPORT   VOID   c_setGS                IPT1(IU16, val);
#define           setGS                c_setGS 
#else
IMPORT   IU32  a3_setGS                IPT1(IU16, val);
#define           setGS                a3_setGS 
#endif

#ifdef IRET_HOOKS
#error Excluded unsupported IRET_HOOKS configuration






#endif /* IRET_HOOKS */

#ifdef CPU_PRIVATE

#ifdef CCPU
IMPORT	IU32	c_getFS_BASE		IPT0();
#define		getFS_BASE		c_getFS_BASE 
#else
IMPORT	IU32	a3_p_getFS_BASE		IPT0();
#define		getFS_BASE		a3_p_getFS_BASE 
#endif

#ifdef CCPU
IMPORT	IU16	c_getFS_LIMIT		IPT0();
#define		getFS_LIMIT		c_getFS_LIMIT 
#else
IMPORT	IU16	a3_p_getFS_LIMIT	IPT0();
#define		getFS_LIMIT		a3_p_getFS_LIMIT 
#endif

#ifdef CCPU
IMPORT	IU8	c_getFS_AR		IPT0();
#define		getFS_AR		c_getFS_AR 
#else
IMPORT	IU8	a3_p_getFS_AR		IPT0();
#define		getFS_AR		a3_p_getFS_AR 
#endif

#ifdef CCPU
IMPORT	IU32	c_getGS_BASE		IPT0();
#define		getGS_BASE		c_getGS_BASE 
#else
IMPORT	IU32	a3_p_getGS_BASE		IPT0();
#define		getGS_BASE		a3_p_getGS_BASE 
#endif

#ifdef CCPU
IMPORT	IU16	c_getGS_LIMIT		IPT0();
#define		getGS_LIMIT		c_getGS_LIMIT 
#else
IMPORT	IU16	a3_p_getGS_LIMIT	IPT0();
#define		getGS_LIMIT		a3_p_getGS_LIMIT 
#endif

#ifdef CCPU
IMPORT	IU8	c_getGS_AR		IPT0();
#define		getGS_AR		c_getGS_AR 
#else
IMPORT	IU8	a3_p_getGS_AR		IPT0();
#define		getGS_AR		a3_p_getGS_AR 
#endif

#ifdef CCPU
IMPORT	void	c_setFS_BASE		IPT1(IU32, base);
#define		setFS_BASE		c_setFS_BASE 
#else
IMPORT	void	a3_p_setFS_BASE		IPT1(IU32, base);
#define		setFS_BASE		a3_p_setFS_BASE 
#endif

#ifdef CCPU
IMPORT	void	c_setFS_LIMIT		IPT1(IU16, limit);
#define		setFS_LIMIT		c_setFS_LIMIT 
#else
IMPORT	void	a3_p_setFS_LIMIT	IPT1(IU16, limit);
#define		setFS_LIMIT		a3_p_setFS_LIMIT 
#endif

#ifdef CCPU
IMPORT	void	c_setFS_AR		IPT1(IU8, ar);
#define		setFS_AR		c_setFS_AR 
#else
IMPORT	void	a3_p_setFS_AR		IPT1(IU8, ar);
#define		setFS_AR		a3_p_setFS_AR 
#endif


#ifdef CCPU
IMPORT	void	c_setGS_BASE		IPT1(IU32, base);
#define		setGS_BASE		c_setGS_BASE 
#else
IMPORT	void	a3_p_setGS_BASE		IPT1(IU32, base);
#define		setGS_BASE		a3_p_setGS_BASE 
#endif

#ifdef CCPU
IMPORT	void	c_setGS_LIMIT		IPT1(IU16, limit);
#define		setGS_LIMIT		c_setGS_LIMIT 
#else
IMPORT	void	a3_p_setGS_LIMIT	IPT1(IU16, limit);
#define		setGS_LIMIT		a3_p_setGS_LIMIT 
#endif

#ifdef CCPU
IMPORT	void	c_setGS_AR		IPT1(IU8, ar);
#define		setGS_AR		c_setGS_AR 
#else
IMPORT	void	a3_p_setGS_AR		IPT1(IU8, ar);
#define		setGS_AR		a3_p_setGS_AR 
#endif

#ifndef CCPU
/*
 * These functions get the (E)IP or (E)SP correctly whatever size stack
 * or code segment is in use.
 */
extern IU32 GetInstructionPointer IPT0();
extern IU32 GetStackPointer IPT0();
#endif /* CCPU */
#endif /* CPU_PRIVATE */
#endif /* SPC386 */

#ifdef IRET_HOOKS
#error Excluded unsupported IRET_HOOKS configuration












#endif /* IRET_HOOKS */

/*
 * No non-386 cpu can run in VM mode, so getVM is always zero.
 * The same is true for GISP, but it has its own version of this interface.
 *
 * We also have definition of the GetInstructionPointer and GetStackPointer
 * interfaces, which on a non-386 can only be the 16 bit versions.
 */

#ifndef SPC386
#define getVM() 0
#define GetInstructionPointer() ((IU32)getIP())
#define GetStackPointer() ((IU32getSP())
#endif

#ifdef SWIN_CPU_OPTS
#error DIVERGENCE MVDM-HOST-DIV-605: unsupported SWIN_CPU_OPTS profile
#endif
#line 1201

#ifdef	CPU_PRIVATE
/*
 * Map new "private" cpu interface -> old interface
 */

#define	setIDT_BASE_LIMIT(base,limit)	{ setIDT_BASE(base); setIDT_LIMIT(limit); }
#define	setGDT_BASE_LIMIT(base,limit)	{ setGDT_BASE(base); setGDT_LIMIT(limit); }
#define	setLDT_BASE_LIMIT(base,limit)	{ setLDT_BASE(base); setLDT_LIMIT(limit); }
#define	setTR_BASE_LIMIT(base,limit)	{ setTR_BASE(base); setTR_LIMIT(limit); }

#define	setCS_BASE_LIMIT_AR(base,limit,ar)	{ setCS_BASE(base); setCS_LIMIT(limit); setCS_AR(ar); }
#define	setES_BASE_LIMIT_AR(base,limit,ar)	{ setES_BASE(base); setES_LIMIT(limit); setES_AR(ar); }
#define	setSS_BASE_LIMIT_AR(base,limit,ar)	{ setSS_BASE(base); setSS_LIMIT(limit); setSS_AR(ar); }
#define	setDS_BASE_LIMIT_AR(base,limit,ar)	{ setDS_BASE(base); setDS_LIMIT(limit); setDS_AR(ar); }
#define	setFS_BASE_LIMIT_AR(base,limit,ar)	{ setFS_BASE(base); setFS_LIMIT(limit); setFS_AR(ar); }
#define	setGS_BASE_LIMIT_AR(base,limit,ar)	{ setGS_BASE(base); setGS_LIMIT(limit); setGS_AR(ar); }

#endif	/* CPU_PRIVATE */

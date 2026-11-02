/*
	SccsID = @(#)sun4_cpuregs.c	1.12 3/27/91 Copyright Insignia Solutions Ltd.
*/
#include "host_def.h"
#include "insignia.h"
#include "xt.h"
#include CpuH

#include <stdio.h>


/*
 *  --- SUN_VA  CPU Interface Functions ---
 *
 *  There are two sections here. Compile-time define
 *  the one that is right for you!
 *
 *  CCPU   - use the C         CPU when HW is not available
 *  A2CPU  - use the assembler CPU when HW is not available
 *
 */


/* HW and SW HOST_SIMULATE functions. */
extern void sw_host_simulate ();		/* in ccpu.o AND a2cpu.o  */
extern void hw_host_simulate ();		/* in hcpu/host/hostsim.c */

/*
 *	The function pointers...
 */
#ifdef CCPU
GLOBAL   VOID		(*setLDTR_func ) ();
GLOBAL   word		(*getTR_func ) ();
GLOBAL	 INT		(*getTS_func ) ();
GLOBAL   VOID		(*setEM_func ) ();
GLOBAL   VOID		(*setGDTR_limit_func ) ();
GLOBAL   VOID		(*setIDTR_limit_func ) ();
GLOBAL   VOID		(*setIOPL_func ) ();
GLOBAL   VOID		(*setGDTR_base_func ) ();
GLOBAL   word		(*getLDTR_func ) ();
GLOBAL   VOID		(*setTR_func ) ();
GLOBAL   VOID		(*setTS_func ) ();
GLOBAL	 INT		(*getPE_func ) ();
GLOBAL	 sys_addr	(*getGDTR_base_func ) ();
GLOBAL   word		(*getMSW_reserved_func ) ();
GLOBAL   int		(*getCPL_func ) ();
GLOBAL	 INT		(*getMP_func ) ();
GLOBAL   VOID		(*setPE_func ) ();
GLOBAL   word		(*getGDTR_limit_func ) ();
GLOBAL   word		(*getIDTR_limit_func ) ();
GLOBAL   VOID		(*setMSW_reserved_func) ();
GLOBAL   VOID		(*setMP_func) ();
GLOBAL   VOID		(*setIDTR_base_func) ();
GLOBAL   word		(*getNT_func) ();
GLOBAL   VOID		(*setCPL_func) ();
GLOBAL	 INT		(*getEM_func) ();
GLOBAL   VOID		(*setNT_func ) ();
GLOBAL   sys_addr	(*getIDTR_base_func) ();
#endif

GLOBAL word		(*getAX_func) ();
GLOBAL half_word	(*getAH_func) ();
GLOBAL half_word	(*getAL_func) ();
GLOBAL word		(*getBX_func) ();
GLOBAL half_word	(*getBH_func) ();
GLOBAL half_word	(*getBL_func) ();
GLOBAL word		(*getCX_func) ();
GLOBAL half_word	(*getCH_func) ();
GLOBAL half_word	(*getCL_func) ();
GLOBAL word		(*getDX_func) ();
GLOBAL half_word	(*getDH_func) ();
GLOBAL half_word	(*getDL_func) ();
GLOBAL word		(*getSP_func) ();
GLOBAL word		(*getBP_func) ();
GLOBAL word		(*getSI_func) ();
GLOBAL word		(*getDI_func) ();
GLOBAL word		(*getIP_func) ();
GLOBAL word		(*getCS_func) ();
GLOBAL word		(*getDS_func) ();
GLOBAL word		(*getES_func) ();
GLOBAL word		(*getSS_func) ();
GLOBAL word		(*getMSW_func) ();

#ifdef CCPU
GLOBAL INT		(*getDF_func) ();
GLOBAL INT		(*getIF_func) ();
GLOBAL INT		(*getTF_func) ();
GLOBAL INT		(*getPF_func) ();
GLOBAL INT		(*getAF_func) ();
GLOBAL INT		(*getSF_func) ();
GLOBAL INT		(*getZF_func) ();
GLOBAL INT		(*getOF_func) ();
GLOBAL INT		(*getCF_func) ();
GLOBAL INT		(*getIOPL_func ) ();
#endif

#ifdef A3CPU
#error DIVERGENCE MVDM-HOST-DIV-522: A3CPU platform branch is not selected; see mirror register.
#endif
#line 109

GLOBAL word		(*getSTATUS_func) ();

GLOBAL double_word	(*getOPA_func) ();
GLOBAL double_word	(*getOPB_func) ();
GLOBAL double_word	(*getOPR_func) ();
GLOBAL sys_addr		(*getSSD_func) ();
GLOBAL sys_addr		(*getDSD_func) ();

GLOBAL VOID		(*setAX_func) ();
GLOBAL VOID		(*setAH_func) ();
GLOBAL VOID		(*setAL_func) ();
GLOBAL VOID		(*setBX_func) ();
GLOBAL VOID		(*setBH_func) ();
GLOBAL VOID		(*setBL_func) ();
GLOBAL VOID		(*setCX_func) ();
GLOBAL VOID		(*setCH_func) ();
GLOBAL VOID		(*setCL_func) ();
GLOBAL VOID		(*setDX_func) ();
GLOBAL VOID		(*setDH_func) ();
GLOBAL VOID		(*setDL_func) ();
GLOBAL VOID		(*setSP_func) ();
GLOBAL VOID		(*setBP_func) ();
GLOBAL VOID		(*setSI_func) ();
GLOBAL VOID		(*setDI_func) ();
GLOBAL VOID		(*setIP_func) ();
GLOBAL INT		(*setCS_func) ();
GLOBAL INT		(*setDS_func) ();
GLOBAL INT		(*setES_func) ();
GLOBAL INT		(*setSS_func) ();
GLOBAL VOID		(*setMSW_func) ();
GLOBAL VOID		(*setDF_func) ();
GLOBAL VOID		(*setIF_func) ();
GLOBAL VOID		(*setTF_func) ();
GLOBAL VOID		(*setPF_func) ();
GLOBAL VOID		(*setAF_func) ();
GLOBAL VOID		(*setSF_func) ();
GLOBAL VOID		(*setZF_func) ();
GLOBAL VOID		(*setOF_func) ();
GLOBAL VOID		(*setCF_func) ();

GLOBAL VOID		(*setOPLEN_func) ();
GLOBAL VOID		(*setOPA_func) ();
GLOBAL VOID		(*setOPB_func) ();
GLOBAL VOID		(*setOPR_func) ();

GLOBAL VOID		(*host_simulate_func) ();

/* DIVERGENCE(MVDM-HOST-DIV-151): the first CCPU branch is the original
 * generated-CCPU vector initializer (the c_get, c_set and c_cpu_simulate
 * providers).  The
 * historical CPU_30_STYLE gate accidentally selects the unrelated
 * soft_ccpu register-state implementation when the supported CPU_40_STYLE
 * profile is built without CPU_30_STYLE.  Select the same CCPU vector
 * branch explicitly for CPU40; CPU30 is neither defined nor linked. */
#ifdef CPU_30_STYLE
#error Excluded original CPU_30_STYLE branch






#else
#line 155
#if defined(CPU_40_STYLE)

#ifdef CCPU

GLOBAL VOID load_sw_cpu_access_functions()
{
  IMPORT VOID	c_cpu_simulate();

  fprintf (stderr,"[load_sw_cpu_access_functions] init READ/WRITE functions.\n");

  /* READ functions */
  getAX_func     = c_getAX;
  getAH_func     = c_getAH;
  getAL_func     = c_getAL;
  getBX_func     = c_getBX;
  getBH_func     = c_getBH;
  getBL_func     = c_getBL;
  getCX_func     = c_getCX;
  getCH_func     = c_getCH;
  getCL_func     = c_getCL;
  getDX_func     = c_getDX;
  getDH_func     = c_getDH;
  getDL_func     = c_getDL;
  getSP_func     = c_getSP;
  getBP_func     = c_getBP;
  getSI_func     = c_getSI;
  getDI_func     = c_getDI;
  getIP_func     = c_getIP;
  getCS_func     = c_getCS;
  getDS_func     = c_getDS;
  getES_func     = c_getES;
  getSS_func     = c_getSS;
  getMSW_func    = c_getMSW;
  getDF_func     = c_getDF;
  getIF_func     = c_getIF;
  getTF_func     = c_getTF;
  getPF_func     = c_getPF;
  getAF_func     = c_getAF;
  getSF_func     = c_getSF;
  getZF_func     = c_getZF;
  getOF_func     = c_getOF;
  getCF_func     = c_getCF;

  
  /* WRITE functions */  
  setAX_func     = c_setAX;
  setAH_func     = c_setAH;
  setAL_func     = c_setAL;
  setBX_func     = c_setBX;
  setBH_func     = c_setBH;
  setBL_func     = c_setBL;
  setCX_func     = c_setCX;
  setCH_func     = c_setCH;
  setCL_func     = c_setCL;
  setDX_func     = c_setDX;
  setDH_func     = c_setDH;
  setDL_func     = c_setDL;
  setSP_func     = c_setSP;
  setBP_func     = c_setBP;
  setSI_func     = c_setSI;
  setDI_func     = c_setDI;
  setIP_func     = c_setIP;
  setDF_func     = c_setDF;
  setIF_func     = c_setIF;
  setTF_func     = c_setTF;
  setPF_func     = c_setPF;
  setAF_func     = c_setAF;
  setSF_func     = c_setSF;
  setZF_func     = c_setZF;
  setOF_func     = c_setOF;
  setCF_func     = c_setCF;
  setSS_func	 = c_setSS;
  setDS_func     = c_setDS;
  setES_func     = c_setES;
  setCS_func     = c_setCS;

  /* SW HOST_SIMULATE function */
  host_simulate_func = c_cpu_simulate;
}
#endif /* CCPU */


/* Temporary 3.0 stubs... */
#ifdef A3CPU
#error DIVERGENCE MVDM-HOST-DIV-522: A3CPU platform branch is not selected; see mirror register.
#endif /* A3CPU */

#else /* CPU_40_STYLE */
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.

































































































































































































































#endif /* CPU_40_STYLE */
#endif
#line 244










#ifdef SUN_VA
#error DIVERGENCE MVDM-HOST-DIV-527: SUN_VA branch is not selected; see mirror register.
#endif SUN_VA

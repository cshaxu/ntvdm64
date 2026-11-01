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
/* DIVERGENCE MVDM-HOST-DIV-522: A3CPU platform branch is not selected; see mirror register. */
#endif

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
/* DIVERGENCE MVDM-HOST-DIV-522: A3CPU platform branch is not selected; see mirror register. */
#endif /* A3CPU */

#else /* CPU_40_STYLE */
/* DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register. */
#endif /* CPU_40_STYLE */










#ifdef SUN_VA
/*
 *     ---- SUN_VA   HARDWARE   SUPPORT ----
 *  needed regardless which softCPU is being used.
 *     ----                             ----
 */

 
extern union SDOS_XTSS *sdos_xtss_ptr;

/*
 *  The XTSS fields are stored in i486 format. SWAP the fields
 *  when accessed from Host-side before writes and after reads.
 */

#define FLAGS_CF   0x00000001
#define FLAGS_PF   0x00000004
#define FLAGS_AF   0x00000010
#define FLAGS_ZF   0x00000040
#define FLAGS_SF   0x00000080
#define FLAGS_TF   0x00000100
#define FLAGS_IF   0x00000200
#define FLAGS_DF   0x00000400
#define FLAGS_OF   0x00000800
#define FLAGS_IOPL 0x00003000
#define FLAGS_NT   0x00004000
#define FLAGS_RF   0x00010000
#define FLAGS_VM   0x00020000


static word       word_reg;
static half_word hword_reg;

word hard_cpu_getAX ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.ax), &word_reg);
   return (word_reg);
}

half_word hard_cpu_getAH ()
{
   sas_load       (&(sdos_xtss_ptr->h.ah), &hword_reg);
   return (hword_reg);
}

half_word hard_cpu_getAL ()
{
   sas_load       (&(sdos_xtss_ptr->h.al), &hword_reg);
   return (hword_reg);
}

word hard_cpu_getBX ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.bx), &word_reg);
   return (word_reg);
}

half_word hard_cpu_getBH ()
{
   sas_load       (&(sdos_xtss_ptr->h.bh), &hword_reg);
   return (hword_reg);
}

half_word hard_cpu_getBL ()
{
   sas_load       (&(sdos_xtss_ptr->h.bl), &hword_reg);
   return (hword_reg);
}

word hard_cpu_getCX ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.cx), &word_reg);
   return (word_reg);
}

half_word hard_cpu_getCH ()
{
   sas_load       (&(sdos_xtss_ptr->h.ch), &hword_reg);
   return (hword_reg);
}

half_word hard_cpu_getCL ()
{
   sas_load       (&(sdos_xtss_ptr->h.cl), &hword_reg);
   return (hword_reg);
}

word hard_cpu_getDX ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.dx), &word_reg);
   return (word_reg);
}

half_word hard_cpu_getDH ()
{
   sas_load       (&(sdos_xtss_ptr->h.dh), &hword_reg);
   return (hword_reg);
}

half_word hard_cpu_getDL ()
{
   sas_load       (&(sdos_xtss_ptr->h.dl), &hword_reg);
   return (hword_reg);
}

word hard_cpu_getSP ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.sp), &word_reg);
   return (word_reg);
}

word hard_cpu_getBP ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.bp), &word_reg);
   return (word_reg);
}

word hard_cpu_getSI ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.si), &word_reg);
   return (word_reg);
}

word hard_cpu_getDI ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.di), &word_reg);
   return (word_reg);
}

word hard_cpu_getIP ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.ip), &word_reg);
   return (word_reg);
}

word hard_cpu_getCS ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.cs), &word_reg);
   return (word_reg);
}

word hard_cpu_getDS ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.ds), &word_reg);
   return (word_reg);
}

word hard_cpu_getES ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.es), &word_reg);
   return (word_reg);
}

word hard_cpu_getSS ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.ss), &word_reg);
   return (word_reg);
}

word hard_cpu_getMSW ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.flags), &word_reg);
   return (word_reg);
}

word hard_cpu_getDF ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.flags), &word_reg);
   if (word_reg & FLAGS_DF)
     return (1);
   else
     return (0);
}

word hard_cpu_getIF ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.flags), &word_reg);
   if (word_reg & FLAGS_IF)
     return (1);
   else
     return (0);
}

word hard_cpu_getTF ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.flags), &word_reg);
   if (word_reg & FLAGS_TF)
     return (1);
   else
     return (0);
}

word hard_cpu_getPF ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.flags), &word_reg);
   if (word_reg & FLAGS_PF)
     return (1);
   else
     return (0);
}

word hard_cpu_getAF ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.flags), &word_reg);
   if (word_reg & FLAGS_AF)
     return (1);
   else
     return (0);
}

word hard_cpu_getSF ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.flags), &word_reg);
   if (word_reg & FLAGS_SF)
     return (1);
   else
     return (0);
}

word hard_cpu_getZF ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.flags), &word_reg);
   if (word_reg & FLAGS_ZF)
     return (1);
   else
     return (0);
}

word hard_cpu_getOF ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.flags), &word_reg);
   if (word_reg & FLAGS_OF)
     return (1);
   else
     return (0);
}

word hard_cpu_getCF ()
{
   sas_loadw_swap (&(sdos_xtss_ptr->x.flags), &word_reg);
   if (word_reg & FLAGS_CF)
     return (1);
   else
     return (0);
}


 
  
void hard_cpu_setAX (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.ax), val);
}

void hard_cpu_setAH (val)
unsigned int val;
{
  sas_store		(&(sdos_xtss_ptr->h.ah), val);
}

void hard_cpu_setAL (val)
unsigned int val;
{
  sas_store		(&(sdos_xtss_ptr->h.al), val);
}

void hard_cpu_setBX (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.bx), val);
}

void hard_cpu_setBH (val)
unsigned int val;
{
  sas_store		(&(sdos_xtss_ptr->h.bh), val);
}

void hard_cpu_setBL (val)
unsigned int val;
{
  sas_store		(&(sdos_xtss_ptr->h.bl), val);
}

void hard_cpu_setCX (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.cx), val);
}

void hard_cpu_setCH (val)
unsigned int val;
{
  sas_store		(&(sdos_xtss_ptr->h.ch), val);
}

void hard_cpu_setCL (val)
unsigned int val;
{
  sas_store		(&(sdos_xtss_ptr->h.cl), val);
}

void hard_cpu_setDX (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.dx), val);
}

void hard_cpu_setDH (val)
unsigned int val;
{
  sas_store		(&(sdos_xtss_ptr->h.dh), val);
}

void hard_cpu_setDL (val)
unsigned int val;
{
  sas_store		(&(sdos_xtss_ptr->h.dl), val);
}

void hard_cpu_setSP (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.sp), val);
}

void hard_cpu_setBP (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.bp), val);
}

void hard_cpu_setSI (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.si), val);
}

void hard_cpu_setDI (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.di), val);
}

void hard_cpu_setIP (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.ip), val);
}

void hard_cpu_setCS (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.cs), val);
}

void hard_cpu_setDS (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.ds), val);
}

void hard_cpu_setES (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.es), val);
}

void hard_cpu_setSS (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.ss), val);
}

void hard_cpu_setMSW (val)
unsigned int val;
{
  sas_storew_swap	(&(sdos_xtss_ptr->x.flags), val);
}

void hard_cpu_setDF (val)
unsigned int val;
{
unsigned short temp;

  sas_loadw_swap	(&(sdos_xtss_ptr->x.flags), &temp);
  if (val)
    sas_storew_swap	(&(sdos_xtss_ptr->x.flags), (temp | FLAGS_DF));
  else
    sas_storew_swap     (&(sdos_xtss_ptr->x.flags), (temp & (~FLAGS_DF)));
}

void hard_cpu_setIF (val)
unsigned int val;
{
unsigned short temp;

  sas_loadw_swap	(&(sdos_xtss_ptr->x.flags), &temp);
  if (val)
    sas_storew_swap	(&(sdos_xtss_ptr->x.flags), (temp | FLAGS_IF));
  else
    sas_storew_swap     (&(sdos_xtss_ptr->x.flags), (temp & (~FLAGS_IF)));
}

void hard_cpu_setTF (val)
unsigned int val;
{
unsigned short temp;

  sas_loadw_swap	(&(sdos_xtss_ptr->x.flags), &temp);
  if (val)
    sas_storew_swap	(&(sdos_xtss_ptr->x.flags), (temp | FLAGS_TF));
  else
    sas_storew_swap     (&(sdos_xtss_ptr->x.flags), (temp & (~FLAGS_TF)));
}

void hard_cpu_setPF (val)
unsigned int val;
{
unsigned short temp;

  sas_loadw_swap	(&(sdos_xtss_ptr->x.flags), &temp);
  if (val)
    sas_storew_swap	(&(sdos_xtss_ptr->x.flags), (temp | FLAGS_PF));
  else
    sas_storew_swap     (&(sdos_xtss_ptr->x.flags), (temp & (~FLAGS_PF)));
}

void hard_cpu_setAF (val)
unsigned int val;
{
unsigned short temp;

  sas_loadw_swap	(&(sdos_xtss_ptr->x.flags), &temp);
  if (val)
    sas_storew_swap	(&(sdos_xtss_ptr->x.flags), (temp | FLAGS_AF));
  else
    sas_storew_swap     (&(sdos_xtss_ptr->x.flags), (temp & (~FLAGS_AF)));
}

void hard_cpu_setSF (val)
unsigned int val;
{
unsigned short temp;

  sas_loadw_swap	(&(sdos_xtss_ptr->x.flags), &temp);
  if (val)
    sas_storew_swap	(&(sdos_xtss_ptr->x.flags), (temp | FLAGS_SF));
  else
    sas_storew_swap     (&(sdos_xtss_ptr->x.flags), (temp & (~FLAGS_SF)));
}

void hard_cpu_setZF (val)
unsigned int val;
{
unsigned short temp;

  sas_loadw_swap	(&(sdos_xtss_ptr->x.flags), &temp);
  if (val)
    sas_storew_swap	(&(sdos_xtss_ptr->x.flags), (temp | FLAGS_ZF));
  else
    sas_storew_swap     (&(sdos_xtss_ptr->x.flags), (temp & (~FLAGS_ZF)));
}

void hard_cpu_setOF (val)
unsigned int val;
{
unsigned short temp;

  sas_loadw_swap	(&(sdos_xtss_ptr->x.flags), &temp);
  if (val)
    sas_storew_swap	(&(sdos_xtss_ptr->x.flags), (temp | FLAGS_OF));
  else
    sas_storew_swap     (&(sdos_xtss_ptr->x.flags), (temp & (~FLAGS_OF)));
}

void hard_cpu_setCF (val)
unsigned int val;
{
unsigned short temp;

  sas_loadw_swap	(&(sdos_xtss_ptr->x.flags), &temp);
  if (val)
    sas_storew_swap	(&(sdos_xtss_ptr->x.flags), (temp | FLAGS_CF));
  else
    sas_storew_swap     (&(sdos_xtss_ptr->x.flags), (temp & (~FLAGS_CF)));
}



/*  these routines load the CPU access functions pointers for HW and SW. */
void load_hw_cpu_access_functions ()
{
  /* READ functions */
  getAX_func     = hard_cpu_getAX;
  getAH_func     = hard_cpu_getAH;
  getAL_func     = hard_cpu_getAL;
  getBX_func     = hard_cpu_getBX;
  getBH_func     = hard_cpu_getBH;
  getBL_func     = hard_cpu_getBL;
  getCX_func     = hard_cpu_getCX;
  getCH_func     = hard_cpu_getCH;
  getCL_func     = hard_cpu_getCL;
  getDX_func     = hard_cpu_getDX;
  getDH_func     = hard_cpu_getDH;
  getDL_func     = hard_cpu_getDL;
  getSP_func     = hard_cpu_getSP;
  getBP_func     = hard_cpu_getBP;
  getSI_func     = hard_cpu_getSI;
  getDI_func     = hard_cpu_getDI;
  getIP_func     = hard_cpu_getIP;
  getCS_func     = hard_cpu_getCS;
  getDS_func     = hard_cpu_getDS;
  getES_func     = hard_cpu_getES;
  getSS_func     = hard_cpu_getSS;
  getMSW_func    = hard_cpu_getMSW;
  getDF_func     = hard_cpu_getDF;
  getIF_func     = hard_cpu_getIF;
  getTF_func     = hard_cpu_getTF;
  getPF_func     = hard_cpu_getPF;
  getAF_func     = hard_cpu_getAF;
  getSF_func     = hard_cpu_getSF;
  getZF_func     = hard_cpu_getZF;
  getOF_func     = hard_cpu_getOF;
  getCF_func     = hard_cpu_getCF;
  
  /* WRITE functions */  
  setAX_func     = hard_cpu_setAX;
  setAH_func     = hard_cpu_setAH;
  setAL_func     = hard_cpu_setAL;
  setBX_func     = hard_cpu_setBX;
  setBH_func     = hard_cpu_setBH;
  setBL_func     = hard_cpu_setBL;
  setCX_func     = hard_cpu_setCX;
  setCH_func     = hard_cpu_setCH;
  setCL_func     = hard_cpu_setCL;
  setDX_func     = hard_cpu_setDX;
  setDH_func     = hard_cpu_setDH;
  setDL_func     = hard_cpu_setDL;
  setSP_func     = hard_cpu_setSP;
  setBP_func     = hard_cpu_setBP;
  setSI_func     = hard_cpu_setSI;
  setDI_func     = hard_cpu_setDI;
  setIP_func     = hard_cpu_setIP;
  setCS_func	 = hard_cpu_setCS;
  setDS_func     = hard_cpu_setDS;
  setES_func     = hard_cpu_setES;
  setSS_func     = hard_cpu_setSS;
  setMSW_func    = hard_cpu_setMSW;
  setDF_func     = hard_cpu_setDF;
  setIF_func     = hard_cpu_setIF;
  setTF_func     = hard_cpu_setTF;
  setPF_func     = hard_cpu_setPF;
  setAF_func     = hard_cpu_setAF;
  setSF_func     = hard_cpu_setSF;
  setZF_func     = hard_cpu_setZF;
  setOF_func     = hard_cpu_setOF;
  setCF_func     = hard_cpu_setCF;

  /*  HW HOST_SIMULATE function */
  host_simulate_func = hw_host_simulate;
}

#endif SUN_VA

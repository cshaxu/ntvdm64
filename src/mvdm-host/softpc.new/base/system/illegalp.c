#include "insignia.h"
#include "host_def.h"
/*[
	Name:		illegal_op.c
	Derived From:	Base 2.0
	Author:		William Gulland
	Created On:	Unknown
	Sccs ID:	@(#)illegal_op.c	1.19 07/04/95
	Notes:		Called from the CPU.
	Purpose:	The CPU has encountered an illegal op code.

	(c)Copyright Insignia Solutions Ltd., 1990. All rights reserved.

]*/

#ifdef SEGMENTATION
/*
 * The following #include specifies the code segment into which this
 * module will by placed by the MPW C compiler on the Mac II running
 * MultiFinder.
 */
#include "SOFTPC_ERROR.seg"
#endif


/*
 *    O/S include files.
 */
#include <stdio.h>
#include TypesH

/*
 * SoftPC include files
 */
#include "xt.h"
#include "sas.h"
#include CpuH
#include "bios.h"
#include "error.h"
#include "config.h"
#include "debug.h"
#include "yoda.h"
#ifdef NTVDM
#include "mvdm_softpc_termination.h"
#endif

#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif

/* Routine to produce human readable form of where an illegal instruction occured */
LOCAL VOID where IFN3(CHAR *, string, word, cs, LIN_ADDR, ip)
{
	double_word ea = effective_addr(cs, ip);

	sprintf(string,
#ifdef	PROD
		"CS:%04x IP:%04x OP:%02x %02x %02x %02x %02x",
#else	/* PROD */
#error DIVERGENCE MVDM-HOST-DIV-503: non-PROD alternative is not selected; see mirror register.
#endif	/* PROD */
		cs, ip,
		sas_hw_at(ea), sas_hw_at(ea+1), sas_hw_at(ea+2),
		sas_hw_at(ea+3),sas_hw_at(ea+4));
}

#if defined(NTVDM) && defined(MONITOR)
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif


void illegal_op()
{
#ifndef	PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
}

void illegal_op_int()
{
	CHAR string[100];
	word cs, ip;

#ifdef NTVDM
        UCHAR opcode;
        double_word ea;
#endif

	/* the cs and ip of the faulting instruction should be on the top of the stack */
	sys_addr stack;

	stack=effective_addr(getSS(),getESP());

	ip = sas_hw_at(stack) + (sas_hw_at(stack+1)<<8);
	cs = sas_hw_at(stack+2) + (sas_hw_at(stack+3)<<8);

#ifdef NTVDM
	/* DIVERGENCE(MVDM-HOST-DIV-328): report only the frame values already
	 * consumed by the original handler; never sample additional guest bytes. */
	mvdm_softpc_report_setup_illegal_opcode(cs, ip, getSS(), getESP(), getEFLAGS());
#endif

	where(string, cs, ip);

#ifndef NTVDM
#ifdef PROD
	host_error(EG_BAD_OP, ERR_QU_CO_RE, string);
#else  /* PROD */
#error DIVERGENCE MVDM-HOST-DIV-503: non-PROD alternative is not selected; see mirror register.
#endif /* PROD */

#else /* NTVDM */
#ifdef PROD
#if define(MONITOR) || define(CPU_40_STYLE)
        host_error(EG_BAD_OP, ERR_QU_CO_RE, string);
#else
        ea = effective_addr(cs, ip);
        opcode = sas_hw_at(ea);
        if (opcode == 0x66 || opcode == 0x67)
            host_error(EG_BAD_OP386, ERR_QU_CO_RE, string);
        else
            host_error(EG_BAD_OP, ERR_QU_CO_RE, string);
#endif /* MONITOR */
#endif /* PROD */
#endif /* NTVDM */



	/* the user has requested a `continue` */
	/* we don't know how many bytes this instr should be, so guess 1 */
	if (ip == 0xffff) {
		cs ++;
		sas_store (stack+2, cs & 0xff);
		sas_store (stack+3, (cs >> 8) & 0xff);
	}
	ip ++;
	sas_store (stack , ip & 0xff);
	sas_store (stack+1, (ip >> 8) & 0xff);
	unexpected_int();
}


void illegal_dvr_bop IFN0()
{
#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* ! NTVDM */
}


#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* ! PROD */
#line 154

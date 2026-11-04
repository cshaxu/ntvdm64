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
#line 17
#error Excluded unsupported SEGMENTATION configuration
#endif
#line 24


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

#line 44
#ifndef PROD
#line 45
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 47

/* Routine to produce human readable form of where an illegal instruction occured */
LOCAL VOID where IFN3(CHAR *, string, word, cs, LIN_ADDR, ip)
{
	double_word ea = effective_addr(cs, ip);

	sprintf(string,
#ifdef	PROD
		"CS:%04x IP:%04x OP:%02x %02x %02x %02x %02x",
#else	/* PROD */
#line 57
#error DIVERGENCE MVDM-HOST-DIV-503: non-PROD alternative is not selected; see mirror register.
#endif	/* PROD */
#line 59
		cs, ip,
		sas_hw_at(ea), sas_hw_at(ea+1), sas_hw_at(ea+2),
		sas_hw_at(ea+3),sas_hw_at(ea+4));
}

#line 64
#if defined(NTVDM) && defined(MONITOR)
#line 65
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif
#line 67


void illegal_op()
{
#ifndef	PROD
#line 72
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 77
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

#line 97
	where(string, cs, ip);

#ifndef NTVDM
#line 100
#error DIVERGENCE MVDM-HOST-DIV-591: non-NTVDM branch is not selected
#else /* NTVDM */
#line 113
#ifdef PROD
#if define(MONITOR) || define(CPU_40_STYLE)
#line 110
#error Unsupported excluded original configuration
#else
#line 112
        ea = effective_addr(cs, ip);
        opcode = sas_hw_at(ea);
        if (opcode == 0x66 || opcode == 0x67)
            host_error(EG_BAD_OP386, ERR_QU_CO_RE, string);
        else
            host_error(EG_BAD_OP, ERR_QU_CO_RE, string);
#endif /* MONITOR */
#line 119
#endif /* PROD */
#endif /* NTVDM */
#line 121



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
#line 141
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* ! NTVDM */
#line 187
}


#ifndef PROD
#line 191
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* ! PROD */
#line 396

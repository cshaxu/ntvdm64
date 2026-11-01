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
/* DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register. */
#endif

/* Routine to produce human readable form of where an illegal instruction occured */
LOCAL VOID where IFN3(CHAR *, string, word, cs, LIN_ADDR, ip)
{
	double_word ea = effective_addr(cs, ip);

	sprintf(string,
#ifdef	PROD
		"CS:%04x IP:%04x OP:%02x %02x %02x %02x %02x",
#else	/* PROD */
/* DIVERGENCE MVDM-HOST-DIV-503: non-PROD alternative is not selected; see mirror register. */
#endif	/* PROD */
		cs, ip,
		sas_hw_at(ea), sas_hw_at(ea+1), sas_hw_at(ea+2),
		sas_hw_at(ea+3),sas_hw_at(ea+4));
}

#if defined(NTVDM) && defined(MONITOR)
#define GetInstructionPointer()     getEIP()
#endif


void illegal_op()
{
#ifndef	PROD
/* DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register. */
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
/* DIVERGENCE MVDM-HOST-DIV-503: non-PROD alternative is not selected; see mirror register. */
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
	sys_addr bop_addr;
	CHAR buf[256];

	/* This is called when an Insignia Intel driver decides that
	 * this (old) SoftWindows is not compatible.
	 *
	 * We should:
	 * a) Put up a localised panel complaining that
	 *    the named driver CS:[eIP] with decimal
	 *    version AX is incompatible with the SoftWindows.
	 *    N.B. The most compatible way to pass the
	 *    name of the driver is by embedded bytes just
	 *    after the BOP. The problem is caused by the
	 *    fact that the driver may be either 16-bit RM
	 *    or a 32-bit flat VxD so the address of the
	 *    string can be either 16/32 bits, and we
	 *    need to be able to execute (and do nothing)
	 *    on the shipping SoftPC 1.xx which prevents
	 *    us doing anything with 32-bit registers!
	 *
	 *	BOP	driver_incompat
	 *	jmp	SHORT over_name
	 *	db	'somename.drv', 0
	 * over_name:
	 *
	 * b) setCF(0)
	 */

	buf[0] = '\0';
	bop_addr = effective_addr(getCS(), GetInstructionPointer());
	if (sas_hw_at(bop_addr) == 0xEB)
	{
		IU8 data;
		char *p;

		p = buf;
		bop_addr += 2;	/* Skip the xEB xXX */
		do {
			data = sas_hw_at(bop_addr++);
			*p++ = data;
		} while (data != 0);
		sprintf(p-1, " v%d.%02d", getAX() / 100, getAX() % 100);
	}
	host_error(EG_DRIVER_MISMATCH, ERR_CONT, buf);
	setCF(0);
#endif /* ! NTVDM */
}


#ifndef PROD
/* DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register. */
#endif /* ! PROD */

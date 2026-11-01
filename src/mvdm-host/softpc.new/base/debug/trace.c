#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 3.0
 *
 * Title	: Trace function
 *
 * Description	: This function will output a trace to the log device.
 *		  The device is set up in the main function module.  Options
 *		  are provided to VPC memory/register data.
 *
 * Author	: Henry Nash
 *
 * Notes	: None
 *
 * SccsID	: @(#)trace.c	1.36 06/02/95
 *
 * (c)Copyright Insignia Solutions Ltd., 1990-1994. All rights reserved.
 */


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
#include <stdlib.h>
#include <stdio.h>
#include TypesH

/*
 * SoftPC include files
 */
#include "xt.h"
#define CPU_PRIVATE	/* Request the CPU private interface as well */
#include CpuH
#undef CPU_PRIVATE
#include "sas.h"
#include "gvi.h"
#include "trace.h"

#ifdef CPU_40_STYLE
FORWARD CHAR *host_get_287_reg_as_string IPT2(IU32, reg_num, BOOL, in_hex);
FORWARD IU32 get_287_tag_word IPT0();
FORWARD IU32 get_287_control_word IPT0();
FORWARD IU32 get_287_status_word IPT0();
FORWARD IU32 get_287_sp IPT0();
#endif

FILE *trace_file;

#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* nPROD */
#line 62

static int trace_start = 0;

GLOBAL IU32 io_verbose = 0;
GLOBAL IU32 sub_io_verbose = 0;

#ifdef RDCHK
#include "egacpu.h"

void get_lar()

{
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
}

#endif

#if !defined(PROD) && defined(SPC386)

LOCAL IS32 read_from_la IFN1(LIN_ADDR, addr)
{
	return (IS32)sas_hw_at(addr);
}

/*
 *********************   dump386Registers  **********************************
 *
 * This functions dumps the CPU registers to the indicated file, taking
 * account of code and stack segment sizes.
 */
LOCAL void
dump386Registers IFN2(FILE *, fp, IUM32, dump_info)
{
	IBOOL is32BitCS = FALSE;
	IBOOL is32BitSS = FALSE;
	IU32 offset;
	IUM32 i;
	sys_addr desc_addr;	/* the descriptor's address */
	IU16 temp;

	if (getPE() && !getVM()) {
		/*
		 * Is it a 16 or 32 bit code segment?
		 */
	
		is32BitCS = CsIsBig(getCS());	
	
		/*
		 * Is it a 16 or 32 bit stack segment? (i.e do we use ESP or SP)
		 * (The test is the same as for a big CS!)
		 */
	
		is32BitSS = CsIsBig(getSS());	
	}

	if (dump_info & DUMP_REG)
	{
		if (is32BitSS || is32BitCS
		    || (getEAX() & 0xFFFF0000)
		    || (getEBX() & 0xFFFF0000)
		    || (getECX() & 0xFFFF0000)
		    || (getEDX() & 0xFFFF0000)
		    || (getEIP() & 0xFFFF0000)
		    || (getEDI() & 0xFFFF0000)
		    || (getESI() & 0xFFFF0000)
		    || (getEBP() & 0xFFFF0000)
		    || (getESP() & 0xFFFF0000)) {
			fprintf(fp, "EAX:%000008x EBX:%000008x ECX:%000008x EDX:%000008x\n",
			  		getEAX(), getEBX(), getECX(), getEDX());
			fprintf(fp, "ESP:%000008x EBP:%000008x ESI:%000008x EDI:%000008x\n",
			   		getESP(), getEBP(), getESI(), getEDI());
			fprintf(fp, "DS:%04x ES:%04x FS:%04x GS:%04x %sSS:%04x %sCS:%04x EIP:%08x\n",
				getDS(), getES(), getFS(), getGS(),
				is32BitSS ? "Big-" : "",  getSS(),
				is32BitCS ? "32-"  : "",  getCS(), getEIP());
		} else {
			fprintf(fp,"AX:%04x BX:%04x CX:%04x DX:%04x",
			  		getAX(), getBX(), getCX(), getDX());
			fprintf(fp, " SP:%04x", getSP());
			fprintf(fp, " BP:%04x SI:%04x DI:%04x\n",
			   		getBP(), getSI(), getDI());
			fprintf(fp,"DS:%04x ES:%04x FS:%04x GS:%04x SS:%04x CS:%04x IP:%04x\n",
				getDS(), getES(), getFS(), getGS(), getSS(), getCS(), getIP());
		}
	}

	if (dump_info & DUMP_INST)
	{
		char buff[256];
		char *fmt, *newline;
		IU32 eip = GetInstructionPointer();

		/* We use the internal dasm so that we can disassemble
		 * instructions at (CS_BASE+eip) rather than
		 * effective_addr(getCS(), getEIP()), since the latter
		 * produces garbage just after changing the PE bit.
		 */
		if ( eip & 0xffff0000 )
		{
			fmt = "%04x:%08x ";
			newline = "\n              ";
		}
		else
		{
			fmt = "%04x:%04x ";
			newline = "\n          ";
		}
		(void)dasm_internal(buff,
			    getCS(),
			    eip,
			    is32BitCS ? THIRTY_TWO_BIT : SIXTEEN_BIT,
			    getCS_BASE() + eip,
			    read_from_la,
			    fmt,
			    newline);
		fprintf (fp, "%s", buff);
	}

	if (dump_info & DUMP_CODE) {
		IU32 cs_base = getCS_BASE();

		fprintf(fp,"Code dump: Last 16 words\n\n");
	 	i = GetInstructionPointer() - 31;
		if (is32BitCS)
	   		fprintf(fp, "%08x:  ", i);
		else
	   		fprintf(fp, "%04x:  ", i);
		for(; i < getIP() - 15; i+=2)
			{
			sas_loadw((cs_base + i), &temp);
			fprintf(fp, "  %04x", temp);
		}
	   	fprintf(fp, "\n%x:  ", i);
		for(; i <= GetInstructionPointer(); i+=2)
		{
			sas_loadw((cs_base + i), &temp);
			fprintf(fp, " %04x", temp);
		}
		fprintf(fp,"\n\n");
	}


#ifdef	SPC486
	if (dump_info & DUMP_FLAGS)
	{
		fprintf(fp, "C:%1d P:%1d A:%1d Z:%1d S:%1d T:%1d I:%1d D:%1d O:%1d\n",
		getCF(), getPF(), getAF(), getZF(), getSF(),
		getTF(), getIF(), getDF(), getOF());

		fprintf(fp, "NT:%1d IOPL:%1d WP:%1d NE:%1d ET:%1d TS:%1d EM:%1d MP:%1d PE:%1d CPL:%1d PG:%1d VM:%1d\n",
		getNT(), getIOPL(), getWP(), getNE(), getET(), getTS(), getEM(), getMP(), getPE(), 
		getCPL(), getPG(),
		getVM());
	}
#else	/* SPC486 */
	if (dump_info & DUMP_FLAGS)
	{
		fprintf(fp,
		"C:%1d P:%1d A:%1d Z:%1d S:%1d T:%1d I:%1d D:%1d O:%1d\nNT:%1d IOPL:%1d TS:%1d EM:%1d MP:%1d PE:%1d CPL:%1d PG:%1d VM:%1d\n",
		getCF(), getPF(), getAF(), getZF(), getSF(),
		getTF(), getIF(), getDF(), getOF(),
		getNT(), getIOPL(), getTS(), getEM(), getMP(), getPE(), 
		getCPL(), getPG(),
		getVM()
		);
	}
#endif	/* SPC486 */
}
#endif /* !PROD && SPC386 */

void trace(error_msg, dump_info)
char *error_msg;
int  dump_info;
{
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.





























#else	/* PROD */
#line 241
	UNUSED(error_msg);
	UNUSED(dump_info);
#endif	/* PROD */
#line 244
}

void trace_init()
{
#if !defined(PROD) || defined(HUNTER)

  char *trace_env, *start;

  trace_env = host_getenv("TRACE");

/*
 * Set up the trace file
 *------------------------*/

  if (trace_env == NULL)
    trace_file = stdout;
  else
  {
    trace_file = fopen(trace_env, "w");
    if (trace_file == NULL)
      trace_file = stdout;

    start = host_getenv("TRACE_START");
    if(start == NULL)
      trace_start = 0;
    else 
      trace_start = atoi(start);
  }
#endif /* !PROD || HUNTER */
}

/* Get the code for 4.0 style CPUs */
#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* !PROD */
#line 279


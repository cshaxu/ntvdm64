#include "insignia.h"
#include "host_def.h"



/*                      INSIGNIA MODULE SPECIFICATION
                        -----------------------------

MODULE NAME     : nt_yoda
FILE NAME       : nt_yoda.c

        THIS  PROGRAM SOURCE FILE IS SUPPLIED IN CONFIDENCE TO THE
        CUSTOMER, THE CONTENTS  OR  DETAILS  OF ITS OPERATION MUST
        NOT  BE DISCLOSED TO ANY  OTHER  PARTIES  WITHOUT  EXPRESS
        AUTHORISATION FROM THE DIRECTORS OF INSIGNIA SOLUTIONS LTD.


DESIGNER        : Wayne Plummer
DATE            : 21st July 1989

PURPOSE         : Provides host specific extensions to YODA


The Following External Routines are defined:
                1. host_force_yoda_extensions
                2. host_yoda_check_I_extensions
                3. host_yoda_help_extensions


=========================================================================

AMMENDMENTS     :

        Version         Date            Author          Reason

=========================================================================
*/

#ifdef YODA

/******INCLUDES**********/
#include <stdio.h>
#include "xt.h"
#include CpuH
#include "hunter.h"
#include "nt_getXX.h"

/******DEFINES***********/
#define EXPORT

/* Get the size of a table. */
#define sizeoftable(tab)	(sizeof(tab)/sizeof(tab[0]))

/* Different types of CALL instruction. */
#define CT_IMM	0
#define CT_EA	1
#define CT_REG	2

/* Mod-rm table flags. */
#define MR_BX	0x01
#define MR_BP	0x02
#define MR_SI	0x04
#define MR_DI	0x08
#define MR_D8	0x10
#define MR_D16	0x20

/* Segment defines, correspond to entries in get_seg table. */
#define NO_OVERRIDE	(-1)
#define SEG_ES		(0)
#define SEG_CS		(1)
#define SEG_SS		(2)
#define SEG_DS		(3)

/* Maximum size of the call stack. */
#define MAX_CALL_STACK	128

/******TYPEDEFS**********/

/* Effective address call type additional data. */
typedef	struct
{
    word	seg;		/* The segment of the effective address. */
    word	off;		/* The offset of the effective address. */
    sys_addr	addr;		/* 20-bit effective address. */
    IS8		seg_override;	/* Segment override if any. */
    IBOOL	disp_present;	/* Is there a displacement present? */
    word	disp;		/* The value of the displacement.*/
    IU8		modrm_index;	/* Index into mod-rm look-up tables. */
} CALL_EA_DATA;

/* Register call type additional data. */
typedef IU8 CALL_REG_DATA;      /* Index into register look-up tables. */

/* Data structure which holds call stack entries. */
typedef struct
{
    IU8		type;		/* CALL instruction type one of
					CT_IMM - immediate
					CT_EA  - effective address in mod-rm
					CT_REG - register in mod-rm. */
    word	cs;		/* Code segment of call instruction. */
    word	ip;		/* Instruction pointer of call instruction. */
    sys_addr	inst_addr;	/* 20-bit address of call instruction. */
    IU8		nbytes;		/* Length of op-code. */
    IU8		opcode[5];	/* Op-code bytes. */
    IBOOL	cfar;		/* Is it a far CALL? */
    word	seg;		/* Target segment for CALL. */
    word	off;		/* Target offset for CALL. */
    word	ss;		/* Stack segment when call is executed. */
    word	sp;		/* Stack offset when call is executed. */
    union
    {
	CALL_EA_DATA	ea;	/* EA call specific data. */
	CALL_REG_DATA	regind;	/* Register call specific data. */
    } extra;
} CALL_STACK_ENTRY;

/******IMPORTS***********/
extern struct HOST_COM host_com[];
extern struct HOST_LPT host_lpt[];
extern char *nt_fgets(char *buffer, int len, void *input_stream);
extern char *nt_gets(char *buffer);
extern int vader;

/******LOCAL FUNCTIONS********/
LOCAL int do_ecbt	IPT5(char *, str, char *, com, long, cs,
			     long, ip, long, len);
LOCAL int do_dcbt	IPT5(char *, str, char *, com, long, cs,
			     long, ip, long, len);
LOCAL int do_pcbt	IPT5(char *, str, char *, com, long, cs,
			     long, ip, long, len);
LOCAL void check_stack	IPT2(word, ss, word, sp);
LOCAL int check_for_overflow	IPT0();
LOCAL void get_ea_from_modrm	IPT4(CALL_STACK_ENTRY *, cs_ptr,
				     IU8, mod,
				     IU8, rm,
				     sys_addr, op_addr);
LOCAL IS8 do_prefixes	IPT1(sys_addr *, opcode_ptr);
LOCAL int do_ntsd	IPT5(char *, str, char *, com, long, cs,
			     long, ip, long, len);

/******LOCAL VARS********/

/* Table of host yoda commands. */
LOCAL struct                                                                   
{                                                                               
	char *name;
	int (*function)	IPT5(char *, str, char *, com, long, cs,
			     long, ip, long, len);
	char *comment;
} host_yoda_command[] =
{
{ "ecbt", do_ecbt,	"                        - enable call-back-tracing" },
{ "dcbt", do_dcbt,	"                        - disable call-back-tracing" },
{ "pcbt", do_pcbt,	"                        - print call-back-trace" },
{ "ntsd", do_ntsd,	"                        - break to ntsd" }
};

/* Variable to enable call-back-tracing. */
LOCAL IBOOL call_back_tracing_enabled = FALSE;

/* Mod-rm byte effective address look-up table. */
LOCAL IU8 EA_table[] =
{
    MR_BX | MR_SI,
    MR_BX | MR_DI,
    MR_BP | MR_SI,
    MR_BP | MR_DI,
    MR_SI,
    MR_DI,
    MR_D16,
    MR_BX,
    MR_BX | MR_SI | MR_D8,
    MR_BX | MR_DI | MR_D8,
    MR_BP | MR_SI | MR_D8,
    MR_BP | MR_DI | MR_D8,
    MR_SI | MR_D8,
    MR_DI | MR_D8,
    MR_BP | MR_D8,
    MR_BX | MR_D8,
    MR_BX | MR_SI | MR_D16,
    MR_BX | MR_DI | MR_D16,
    MR_BP | MR_SI | MR_D16,
    MR_BP | MR_DI | MR_D16,
    MR_SI | MR_D16,
    MR_DI | MR_D16,
    MR_BP | MR_D16,
    MR_BX | MR_D16
};

/* Mod-rm byte string look-up table. */
LOCAL CHAR *EA_strings[] =
{
    "[BX + SI]",
    "[BX + DI]",
    "[BP + SI]",
    "[BP + DI]",
    "[SI]",
    "[DI]",
    "[%hX]",
    "[BX]",
    "[BX + SI + %hX]",
    "[BX + DI + %hX]",
    "[BP + SI + %hX]",
    "[BP + DI + %hX]",
    "[SI + %hX]",
    "[DI + %hX]",
    "[BP + %hX]",
    "[BX + %hX]",
    "[BX + SI + %hX]",
    "[BX + DI + %hX]",
    "[BP + SI + %hX]",
    "[BP + DI + %hX]",
    "[SI + %hX]",
    "[DI + %hX]",
    "[BP + %hX]",
    "[BX + %hX]"
};

/* Table of functions corresponding to register rm fields. */
LOCAL word (*EA_reg_func[])() =
{
    getAX,
    getCX,
    getDX,
    getBX,
    getSP,
    getBP,
    getSI,
    getDI
};

/* Table of names of register rm fields. */
LOCAL CHAR *EA_reg_strings[] =
{
    "AX",
    "CX",
    "DX",
    "BX",
    "SP",
    "BP",
    "SI",
    "DI"
};

/* Table of functions for getting segment values. */
LOCAL word (*get_seg[])() =
{
    getES,
    getCS,
    getSS,
    getDS
};

/* Table of segment names. */
LOCAL CHAR *seg_strings[] =
{
    "ES",
    "CS",
    "SS",
    "DS"
};

LOCAL CALL_STACK_ENTRY call_stack[MAX_CALL_STACK];
LOCAL CALL_STACK_ENTRY *call_next_free = call_stack;

/******EXPORT VARS*******/

#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* ndef PROD */
#line 272

#endif /* YODA */
#line 274

/* This stub exported as called from main() */
void    host_set_yoda_ints()
{
}

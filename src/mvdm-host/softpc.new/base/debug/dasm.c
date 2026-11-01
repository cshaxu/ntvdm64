#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 3.0
 *
 * Title        : CPU disassembler
 *
 * Description  : This dissasembler is called from the debugging
 *                software (trace + yoda).
 *
 * Author       : Paul Huckle / Henry Nash
 *
 * Notes        : There are some dependencies between this and the CPU
 *                module - unfortunately exactly what these are lie
 *                hidden in thrown together code and Super Supremes.
 */

/*
 * static char SccsID[]="@(#)dasm.c	1.24 05/16/94 Copyright Insignia Solutions Ltd.";
 */


#ifdef SEGMENTATION
/*
 * The following #include specifies the code segment into which this
 * module will by placed by the MPW C compiler on the Mac II running
 * MultiFinder.
 */
#include "DASM1.seg"
#endif



#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-502: non-PROD debug branch is not selected; see mirror register.
#endif /* PROD */
#line 37

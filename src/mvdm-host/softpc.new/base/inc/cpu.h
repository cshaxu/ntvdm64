
/*
 * SoftPC Revision 3.0
 *
 * Title	: Definitions for the CPU 
 *
 * Description	: Structures, macros and definitions for access to the 
 *		  CPU registers
 *
 * Author	: Henry Nash
 *
 * Notes	: This file should be portable - but includes a file
 *		  host_cpu.h which contains machine specific definitions
 *		  of CPU register mappings etc.
 */

/* SccsID[]="@(#)cpu.h	1.28 12/10/92 Copyright Insignia Solutions Ltd."; */

#if defined(NTVDM) && defined(MONITOR)
#error Excluded unsupported monitor CPU header profile
#endif /* NTVDM && MONITOR */

/*
 * VPC-XT Revision 1.0
 *
 * Title        : Sparc 2.0 Definitions for the CPU
 *
 * Description  : Structures, macros and definitions for access to the
 *                CPU registers
 *
 * Author       : Andrew Guthrie
 *
 * Notes        : This file is included by cpu.h and should NOT
 *                be included directly by any other module.
 */

/* SccsID[]="@(#)host_cpu.h     1.22 4/15/91 Copyright Insignia Solutions Ltd."; */


/*
 * ============================================================================
 * Structure/Data definitions
 * ============================================================================
 */

/*
 * Union representing different ways of accessing a register
 */

typedef union
    {
        word    X;
    struct
        {                               /* as two bytes */
        /* DIVERGENCE: word port I/O overlays X with these host-order bytes. */
#ifdef LITTLEND
        half_word low;
        half_word high;
#else
#error DIVERGENCE MVDM-HOST-DIV-620: unsupported non-little-endian host register layout
#endif
#line 131
        word pad;
        } byte;
    struct
        {                               /* as 4 nibbles */
        word n3:4;
        word n2:4;
        word n1:4;
        word n0:4;
        word pad;
        } nibble;
    struct
        {                               /* as 16 bits   */
        word b15:1;
        word b14:1;
        word b13:1;
        word b12:1;
        word b11:1;
        word b10:1;
        word b9:1;
        word b8:1;
        word b7:1;
        word b6:1;
        word b5:1;
        word b4:1;
        word b3:1;
        word b2:1;
        word b1:1;
        word b0:1;
        word pad;
        } bit;
    } reg;
#ifdef CPU_30_STYLE /* DIVERGENCE(MVDM-HOST-DIV-552) */
#error Excluded original conditional branch
#endif /* CPU_30_STYLE */

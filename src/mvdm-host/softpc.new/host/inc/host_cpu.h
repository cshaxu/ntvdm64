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
#line 17
#ifdef CPU_30_STYLE
#line 18
#error Excluded original conditional branch
#line 112
#else /* CPU_30_STYLE */
#line 113


/*
 * ============================================================================
 * Structure/Data definitions
 * ============================================================================
 */

/*
 * Union representing different ways of accessing a register
 */

#line 124
typedef union
    {
        word    X;
    struct
        {                               /* as two bytes */
        /* DIVERGENCE: word port I/O overlays X with these host-order bytes. */
#ifdef LITTLEND
#line 130
        half_word low;
#line 129
        half_word high;
#else
#line 129
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

#line 163
#ifdef A2CPU
#line 164
#error Excluded unsupported original A2CPU register carrier
#line 178
#endif
#line 179

#line 187
#ifndef EGATEST
#line 188
#line 188
#ifdef A2CPU
#line 189
#error Excluded unsupported original A2CPU register carrier
#line 193
#endif
#line 194
#line 229
#ifdef CCPU
#line 230
#line 246
#endif
#line 247
#line 248
#ifdef A2CPU
#line 249
#error Excluded unsupported original A2CPU register carrier
#line 255
#endif
#line 256
#line 290
#ifdef CCPU
#line 291
#line 306
#endif
#line 307
#line 308
#ifdef A2CPU
#line 309
#error Excluded unsupported original A2CPU register carrier
#line 314
#endif
#line 315
#endif /* EGATEST */
#line 316

#line 357
#ifdef CCPU
#line 358
#line 373
#endif
#line 374

#line 375
#ifdef A2CPU
#line 376
#error Excluded unsupported original A2CPU register carrier
#line 382
#endif
#line 383

#line 419
#ifdef CCPU
#line 420
#line 435
#endif
#line 436

#line 437
#ifdef A2CPU
#line 438
#error Excluded unsupported original A2CPU register carrier
#line 443
#endif
#line 444

#line 456
#ifdef A2CPU
#line 457
#error Excluded unsupported original A2CPU register carrier
#line 504
#endif
#line 505
#line 506
#endif /* CPU_30_STYLE */
#line 507

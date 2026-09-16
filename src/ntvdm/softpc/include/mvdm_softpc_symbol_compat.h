#ifndef MVDM_SOFTPC_SYMBOL_COMPAT_H
#define MVDM_SOFTPC_SYMBOL_COMPAT_H

/* OpenNT SoftPC's internal VDM shutdown routine predates the ISO/C++ CRT
 * symbol of the same spelling.  Keep every original call and definition in
 * one namespace at compile time; this header is forced only for the original
 * SoftPC/DPMI composition, never for the product entry or CRT. */
#define terminate mvdm_softpc_terminate

#endif

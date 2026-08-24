/*
 * Declaration-only bridge for the original non-CPU_40 Sim32 source when it
 * is compiled against the CCPU software monitor rather than the V86 monitor.
 */
#ifndef NTDOS64_OPENNT_CCPU_SIM32_X86_ABI_COMPAT_H
#define NTDOS64_OPENNT_CCPU_SIM32_X86_ABI_COMPAT_H

/* ccpusas4.c owns this CCPU linear-address translation. */
extern unsigned char *c_GetLinAdd(unsigned long lin_addr);

/* Sim32's historical V86 import has the same pointer-returning contract. */
#define NtGetPtrToLinAddrByte(lin_addr) c_GetLinAdd(lin_addr)

#endif

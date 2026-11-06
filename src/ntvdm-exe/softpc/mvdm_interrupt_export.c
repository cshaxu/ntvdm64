/* The public x86 VDD interrupt entry is callee-cleanup. CPU40's internal
 * PIC entry remains cdecl; retain its original queue, locking and ordering. */
#include <windows.h>

extern void __cdecl ica_hw_interrupt(unsigned int, unsigned int, int);

void WINAPI mvdm_public_ica_interrupt(int adapter, int line, int count)
{
    ica_hw_interrupt((unsigned int)adapter, (unsigned int)line, count);
}

/*
 * WOW32 mapped-pointer completion. This local provider returns success without
 * transferring memory; it is not the original SIM32 scratch-video writeback.
 * The selected CCPU cache notification is empty, but sim32.c also owns a paired
 * video scratch-buffer contract. Ownership remains under C0456 investigation;
 * see s14-p64-sim32-contract.json. The formal WOW build uses its default stdcall.
 */

#include <windows.h>

BOOL Sim32FlushVDMPointer(ULONG address, USHORT size, PBYTE buffer, BOOL mode)
{
    (void)address;
    (void)size;
    (void)buffer;
    (void)mode;
    return TRUE;
}

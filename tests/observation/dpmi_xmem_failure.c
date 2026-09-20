/* Test-only failures of two explicit probe sizes; original owner unchanged. */
#define DpmiAllocateVirtualMemory s38_allocate
#define DpmiReallocateVirtualMemory s38_reallocate
#define DpmiReallocateXmem s38_original_reallocate_xmem
#include "../../src/mvdm/dpmi32/xmem.c"
#undef DpmiAllocateVirtualMemory
#undef DpmiReallocateVirtualMemory
#undef DpmiReallocateXmem
#include <stdio.h>
NTSTATUS DpmiAllocateVirtualMemory(PVOID *, PULONG);
BOOL DpmiReallocateVirtualMemory(PVOID, ULONG, PVOID *, PULONG);
BOOL s38_grow_failed;
static void record_failure(const char *name, ULONG size)
{
    FILE *file = fopen(DPMI_TRACE_LOG, "a");
    if (file) { fprintf(file, "%s size=%lu\n", name, size); fclose(file); }
}
NTSTATUS s38_allocate(PVOID *address, PULONG size)
{
    if (*size == 0x34570) {
        record_failure("S38_ALLOCATE_INJECTED", *size);
        return (NTSTATUS)0xC0000017L;
    }
    return DpmiAllocateVirtualMemory(address, size);
}
BOOL s38_reallocate(PVOID oldAddress, ULONG oldSize, PVOID *address, PULONG size)
{
    if (*size == 0x4570) {
        record_failure("S38_GROW_INJECTED", *size);
        s38_grow_failed = TRUE;
        return FALSE;
    }
    return DpmiReallocateVirtualMemory(oldAddress, oldSize, address, size);
}
VOID DpmiReallocateXmem(VOID)
{
    PXMEM_BLOCK block = (PXMEM_BLOCK)(((ULONG)getSI() << 16) | getDI());
    BOOL observed = (((ULONG)getBX() << 16) | getCX()) == 0x4570;
    if (observed) {
        record_failure("S38_GROW_OLD_ADDRESS", (ULONG)block->Address);
        record_failure("S38_GROW_OLD_LENGTH", block->Length);
    }
    s38_original_reallocate_xmem();
    if (observed) {
        record_failure("S38_GROW_RETURN_CF", getCF());
        record_failure("S38_GROW_RETURN_BXCX", ((ULONG)getBX() << 16) | getCX());
        record_failure("S38_GROW_RETAINED_ADDRESS", (ULONG)block->Address);
        record_failure("S38_GROW_RETAINED_LENGTH", block->Length);
    }
}

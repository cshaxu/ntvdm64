#ifndef OPENNT_HOST_NTOS_RTL_ZWAPI_H
#define OPENNT_HOST_NTOS_RTL_ZWAPI_H

/* Finite user-mode NT VM boundary required by the selected original
 * environ.c.  Do not include the unrelated historical Zw declaration set. */
#ifndef MemoryBasicInformation
#define MemoryBasicInformation 0
#endif
NTSYSAPI NTSTATUS NTAPI ZwAllocateVirtualMemory(HANDLE ProcessHandle,
    PVOID *BaseAddress, ULONG ZeroBits, PULONG RegionSize,
    ULONG AllocationType, ULONG Protect);
NTSYSAPI NTSTATUS NTAPI ZwFreeVirtualMemory(HANDLE ProcessHandle,
    PVOID *BaseAddress, PULONG RegionSize, ULONG FreeType);
NTSYSAPI NTSTATUS NTAPI ZwQueryVirtualMemory(HANDLE ProcessHandle,
    PVOID BaseAddress, ULONG MemoryInformationClass, PVOID MemoryInformation,
    ULONG MemoryInformationLength, PULONG ReturnLength);

#endif

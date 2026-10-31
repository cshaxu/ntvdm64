#ifndef OPENNT_HOST_NTOS_RTL_ZWAPI_H
#define OPENNT_HOST_NTOS_RTL_ZWAPI_H

/* Finite user-mode NT VM boundary required by the selected original
 * environ.c.  Do not include the unrelated historical Zw declaration set.
 * DIVERGENCE(OPENNT-HOST-067): this standalone declaration carrier binds the
 * modern native-size API parameters; SIZE_T remains ULONG on x86. */
#ifndef MemoryBasicInformation
#define MemoryBasicInformation 0
#endif
NTSYSAPI NTSTATUS NTAPI ZwAllocateVirtualMemory(HANDLE ProcessHandle,
    PVOID *BaseAddress, ULONG_PTR ZeroBits, PSIZE_T RegionSize,
    ULONG AllocationType, ULONG Protect);
NTSYSAPI NTSTATUS NTAPI ZwFreeVirtualMemory(HANDLE ProcessHandle,
    PVOID *BaseAddress, PSIZE_T RegionSize, ULONG FreeType);
NTSYSAPI NTSTATUS NTAPI ZwQueryVirtualMemory(HANDLE ProcessHandle,
    PVOID BaseAddress, ULONG MemoryInformationClass, PVOID MemoryInformation,
    SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);

#endif

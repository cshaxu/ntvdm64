#ifndef WOW_USER_PROBE_BINDINGS_H
#define WOW_USER_PROBE_BINDINGS_H
#include <windows.h>
#include <winternl.h>
VOID NTAPI ProbeForRead(CONST VOID *, ULONG, ULONG);
#ifdef WOW_ORIGINAL_USER_PROBE
/* ADAPTER-WOW-046: original range/alignment policy; current process address
 * ceiling and native RtlRaiseStatus replace kernel environment only. */
NTSYSAPI VOID NTAPI RtlRaiseStatus(NTSTATUS);
static __inline ULONG_PTR wow_user_probe_address(void)
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (ULONG_PTR)info.lpMaximumApplicationAddress + 1;
}
#define NTKERNELAPI
#define PAGED_CODE() ((void)0)
#undef ASSERT
#define ASSERT(condition) ((void)0)
#define MM_USER_PROBE_ADDRESS wow_user_probe_address()
#define ExRaiseDatatypeMisalignment() RtlRaiseStatus((NTSTATUS)0x80000002)
#define ExRaiseAccessViolation() RtlRaiseStatus((NTSTATUS)0xc0000005)
#endif
#endif

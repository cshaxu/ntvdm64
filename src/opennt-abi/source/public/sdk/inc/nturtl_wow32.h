#ifndef OPENNT_ABI_NTURTL_WOW32_H
#define OPENNT_ABI_NTURTL_WOW32_H

/* Selected from OpenNT public/sdk/inc/nturtl.h (SHA-256
 * C6B7844E2E1248AC38EE90DD8173F95AB0F7997D7A7CFBAFDA851CC07CC80921). The current adapter-owned
 * TEB is intentionally not a cast of the host TEB; it carries this one
 * historical per-thread association explicitly. */
typedef struct _RTL_PERTHREAD_CURDIR {
    PVOID CurrentDirectories;
    PUNICODE_STRING ImageName;
    PVOID Environment;
} RTL_PERTHREAD_CURDIR, *PRTL_PERTHREAD_CURDIR;

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteCriticalSection(PRTL_CRITICAL_SECTION CriticalSection);

#define RtlAssociatePerThreadCurdir(BLOCK,CURRENTDIRECTORIES,IMAGENAME,ENVIRONMENT) \
    (BLOCK)->CurrentDirectories = (CURRENTDIRECTORIES); \
    (BLOCK)->ImageName = (IMAGENAME); \
    (BLOCK)->Environment = (ENVIRONMENT); \
    NtCurrentTeb()->NtTib.SubSystemTib = (PVOID)(BLOCK)

#endif /* OPENNT_ABI_NTURTL_WOW32_H */

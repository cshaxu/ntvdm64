#pragma once

#include <windows.h>
#include <winternl.h>

typedef struct _OPENNT_SUPPORT_CURDIR {
    UNICODE_STRING DosPath;
    HANDLE Handle;
} OPENNT_SUPPORT_CURDIR;

typedef struct _OPENNT_SUPPORT_PROCESS_PARAMETERS {
    OPENNT_SUPPORT_CURDIR CurrentDirectory;
    PVOID Environment;
    HANDLE ConsoleHandle;
    /* DIVERGENCE(ADAPTER-WIN32-059): BaseCheckVDM reads only these original
     * process-start fields; values come from public startup/std-handle APIs. */
    HANDLE StandardInput, StandardOutput, StandardError;
    ULONG WindowFlags;
} OPENNT_SUPPORT_PROCESS_PARAMETERS, *POPENNT_SUPPORT_PROCESS_PARAMETERS;

typedef struct _OPENNT_SUPPORT_PEB {
    POPENNT_SUPPORT_PROCESS_PARAMETERS ProcessParameters;
    PVOID ProcessHeap;
    PRTL_CRITICAL_SECTION FastPebLock;
    ULONG EnvironmentUpdateCount;
    /* DIVERGENCE(ADAPTER-WIN32-058): original BaseClient classifier compares
     * the process image's machine field. This is a local public-API value,
     * not a cast of the modern PEB or a guest-visible address. */
    PVOID ImageBaseAddress;
} OPENNT_SUPPORT_PEB, *POPENNT_SUPPORT_PEB;

typedef struct _OPENNT_SUPPORT_TEB {
    NT_TIB NtTib;
    UNICODE_STRING StaticUnicodeString;
    PWSTR StaticUnicodeBuffer;
    CLIENT_ID ClientId;
    /* DIVERGENCE(ADAPTER-WIN32-057): selected original RTL error.c records
     * the converted NTSTATUS in the historical per-thread LastStatusValue.
     * This private TLS field is the finite observable carrier; it does not
     * expose or reinterpret the modern host TEB. */
    NTSTATUS LastStatusValue;
    /* Reached historical TEB field: imported x86 DPMI sources locate their
     * per-thread VDM TIB through this spelling. The value is assigned only
     * by the monitor's bound-thread carrier and is never a guest value. */
    PVOID Vdm;
    /* DIVERGENCE(ADAPTER-WIN32-041): the selected original WOW32 source
     * stores its per-thread TD through the NT4 TEB's WOW32Reserved slot.
     * A modern public TEB does not expose that retired product field.  This
     * adapter-private TLS carrier preserves the source spelling and its
     * thread-local lifetime; it is host-private and is never a guest value. */
    PVOID WOW32Reserved;
} OPENNT_SUPPORT_TEB, *POPENNT_SUPPORT_TEB;

POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(VOID);
POPENNT_SUPPORT_PEB NTAPI NtCurrentPeb(VOID);

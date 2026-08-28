/*
 * Same-shaped subset of the historical OpenNT nt.h surface reached by the
 * original MVDM support libraries.  This is an adapter-owned binding header,
 * not a replacement for the OpenNT platform ABI mirror.
 */
#ifndef OPENNT_SUPPORT_NT_H
#define OPENNT_SUPPORT_NT_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <devioctl.h>
#include <winternl.h>
/* `ntpsapi.h` is a selected original OpenNT declaration subset under
 * opennt-host.  The adapter's type binding is deliberately established
 * first, then the original historical declaration shape is made visible. */
#include <ntpsapi.h>

/* DIVERGENCE: The selected historical netlibnt.h declaration carrier uses
 * NT_PRODUCT_TYPE. The modern SDK's winnt.h exposes its own foundational
 * definitions but not this retired enum, while including ntdef.h after
 * windows.h duplicates SDK structures. Preserve the exact three-value NT4
 * declaration shape locally; it declares no service or product policy. */
typedef enum _NT_PRODUCT_TYPE {
    NtProductWinNt = 1,
    NtProductLanManNt,
    NtProductServer
} NT_PRODUCT_TYPE, *PNT_PRODUCT_TYPE;

/* DIVERGENCE: Original DEM storage declarations require historical
 * devioctl.h and own GetDiskSpaceInformation as an unrelated BOOL helper.
 * Keep the storage carrier visible and remove only the conflicting modern
 * SDK convenience alias; ntioapi.h awaits its own complete ABI binding. */
#ifdef GetDiskSpaceInformation
#undef GetDiskSpaceInformation
#endif

#ifdef NtCurrentTeb
#undef NtCurrentTeb
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif
#ifndef STATUS_NO_MEMORY
#define STATUS_NO_MEMORY ((NTSTATUS)0xC0000017L)
#endif
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif
#ifndef STATUS_BUFFER_OVERFLOW
#define STATUS_BUFFER_OVERFLOW ((NTSTATUS)0x80000005L)
#endif
#ifndef STATUS_INVALID_PARAMETER
#define STATUS_INVALID_PARAMETER ((NTSTATUS)0xC000000DL)
#endif
#ifndef STATUS_NOT_IMPLEMENTED
#define STATUS_NOT_IMPLEMENTED ((NTSTATUS)0xC0000002L)
#endif
#ifndef STATUS_ALERTED
#define STATUS_ALERTED ((NTSTATUS)0x00000101L)
#endif
/* The modern SDK leaves these historical NTSTATUS spellings out of the
 * user-mode declaration set, although the values and their failure contract
 * are unchanged.  Keep the original SoftPC callers source-shaped. */
#ifndef STATUS_UNABLE_TO_DECOMMIT_VM
#define STATUS_UNABLE_TO_DECOMMIT_VM ((NTSTATUS)0xC000002CL)
#endif
#ifndef STATUS_NOT_COMMITTED
#define STATUS_NOT_COMMITTED ((NTSTATUS)0xC000002DL)
#endif
#ifndef STATUS_MEMORY_NOT_ALLOCATED
#define STATUS_MEMORY_NOT_ALLOCATED ((NTSTATUS)0xC00000A0L)
#endif

typedef STRING OEM_STRING, *POEM_STRING;
typedef const OEM_STRING *PCOEM_STRING;

typedef struct _OPENNT_SUPPORT_CURDIR {
    UNICODE_STRING DosPath;
    HANDLE Handle;
} OPENNT_SUPPORT_CURDIR, *POPENNT_SUPPORT_CURDIR;
typedef OPENNT_SUPPORT_CURDIR CURDIR, *PCURDIR;

typedef struct _OPENNT_SUPPORT_PROCESS_PARAMETERS {
    OPENNT_SUPPORT_CURDIR CurrentDirectory;
    HANDLE ConsoleHandle;
} OPENNT_SUPPORT_PROCESS_PARAMETERS, *POPENNT_SUPPORT_PROCESS_PARAMETERS;

typedef struct _OPENNT_SUPPORT_PEB {
    POPENNT_SUPPORT_PROCESS_PARAMETERS ProcessParameters;
    PVOID ProcessHeap;
} OPENNT_SUPPORT_PEB, *POPENNT_SUPPORT_PEB;

typedef struct _OPENNT_SUPPORT_TEB {
    UNICODE_STRING StaticUnicodeString;
    /* Reached historical TEB field: imported x86 DPMI sources locate their
     * per-thread VDM TIB through this spelling. The value is assigned only
     * by the monitor's bound-thread carrier and is never a guest value. */
    PVOID Vdm;
} OPENNT_SUPPORT_TEB, *POPENNT_SUPPORT_TEB;

POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(VOID);
POPENNT_SUPPORT_PEB NTAPI NtCurrentPeb(VOID);

/* winnt.h declares the real process TEB accessor.  OpenNT's reached source
 * needs only its historical StaticUnicodeString member, so route that source
 * spelling to adapter-private TLS rather than casting a modern TEB. */
#define NtCurrentTeb() opennt_support_current_teb()

#ifndef ARGUMENT_PRESENT
#define ARGUMENT_PRESENT(ArgumentPointer) ((ArgumentPointer) != NULL)
#endif
#ifndef ASSERT
#define ASSERT(Expression) ((void)0)
#endif
#ifndef try
#define try __try
#endif
#ifndef finally
#define finally __finally
#endif
#ifndef except
#define except __except
#endif
#ifndef RtlMoveMemory
#define RtlMoveMemory(Destination, Source, Length) \
    MoveMemory((Destination), (Source), (Length))
#endif
#ifndef RtlZeroMemory
#define RtlZeroMemory(Destination, Length) ZeroMemory((Destination), (Length))
#endif

#include <ntioapi.h>

#endif

/*
 * Same-shaped subset of the historical OpenNT nt.h surface reached by the
 * original MVDM support libraries.  This is an adapter-owned binding header,
 * not a replacement for the OpenNT platform ABI mirror.
 */
#ifndef OPENNT_SUPPORT_NT_H
#define OPENNT_SUPPORT_NT_H

/* DIVERGENCE(ADAPTER-WIN32-025): selected original user-mode VDM headers
 * expose their contract declarations when the historical NT base header's
 * `_NTDEF_` marker is present.  The modern SDK supplies the underlying
 * types but not that retired marker.  Preserve the declaration gate only;
 * this does not opt into a kernel VDM implementation. */
#ifndef _NTDEF_
#define _NTDEF_
#endif
#ifndef NOTHING
#define NOTHING
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <devioctl.h>
#include <winternl.h>
#include "wow_user_declarations.h"
/* `ntpsapi.h` is a selected original OpenNT declaration subset under
 * opennt-host.  The adapter's type binding is deliberately established
 * first, then the original historical declaration shape is made visible. */
#include <ntpsapi.h>
/* Finite original ntdef/ntexapi/ntnls declarations absent from the public
 * Win32 headers. Private NLS/process information is exposed only as opaque
 * pointers; fixed-width strings retain the original ABI layout. */
typedef enum _NT_PRODUCT_TYPE {
    NtProductWinNt = 1,
    NtProductLanManNt,
    NtProductServer
} NT_PRODUCT_TYPE, *PNT_PRODUCT_TYPE;
typedef ULONG CLONG;
typedef const STRING *PCSTRING;
typedef USHORT RTL_ATOM, *PRTL_ATOM;
typedef struct _CPTABLEINFO *PCPTABLEINFO;
typedef struct _NLSTABLEINFO *PNLSTABLEINFO;
typedef struct _RTL_USER_PROCESS_INFORMATION *PRTL_USER_PROCESS_INFORMATION;
/* Original ntldr.h pointer-only dependency of RTL_DEBUG_INFORMATION. */
typedef struct _RTL_PROCESS_MODULES *PRTL_PROCESS_MODULES;
#define FASTCALL __fastcall
typedef struct _STRING32 {
    USHORT   Length;
    USHORT   MaximumLength;
    ULONG  Buffer;
} STRING32;
typedef STRING32 UNICODE_STRING32;
typedef struct _STRING64 {
    USHORT   Length;
    USHORT   MaximumLength;
    ULONGLONG  Buffer;
} STRING64;
typedef STRING64 UNICODE_STRING64;
typedef short CSHORT;
/* This closure links original RTL locally and native RTL through import
 * libraries. Ordinary external declarations cover both without per-function
 * mirror omissions; restore the SDK decoration immediately afterwards. */
#pragma push_macro("NTSYSAPI")
#undef NTSYSAPI
#define NTSYSAPI
#include <ntrtl.h>
#include <nturtl.h>
#pragma pop_macro("NTSYSAPI")

/* Original registry declarations own the query layouts, selectors and calls. */
#include <ntregapi.h>

/* DIVERGENCE: Original DEM storage declarations require historical
 * devioctl.h and own GetDiskSpaceInformation as an unrelated BOOL helper.
 * Keep the storage carrier visible and remove only the conflicting modern
 * SDK convenience alias; ntioapi.h awaits its own complete ABI binding. */
#undef GetDiskSpaceInformation

#undef NtCurrentTeb

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
#ifndef STATUS_INVALID_LDT_DESCRIPTOR
#define STATUS_INVALID_LDT_DESCRIPTOR ((NTSTATUS)0xC000011AL)
#endif
#ifndef STATUS_NOT_IMPLEMENTED
#define STATUS_NOT_IMPLEMENTED ((NTSTATUS)0xC0000002L)
#endif
#ifndef STATUS_ALERTED
#define STATUS_ALERTED ((NTSTATUS)0x00000101L)
#endif
#ifndef STATUS_USER_APC
#define STATUS_USER_APC ((NTSTATUS)0x000000C0L)
#endif
/* DIVERGENCE(ADAPTER-WIN32-023): The original SoftPC host sources call the
 * NTDLL wait interface with the NT4 `WAIT_TYPE` enumerator spelling.  Modern
 * public user-mode headers omit that kernel-shaped declaration even though
 * the reachable wait-any value and its observable ordering are unchanged.
 * Retain the original numeric selector only; the actual wait call remains a
 * separately bound NTDLL/public-host interface. */
#ifndef WaitAll
#define WaitAll 0
#endif
#ifndef WaitAny
#define WaitAny 1
#endif
/* DIVERGENCE(ADAPTER-WIN32-029): declaration only. Retain the original NT
 * wait, including pending STATUS_ALERTED; Win32 waits consume that status. */
NTSTATUS NTAPI NtWaitForMultipleObjects(
    ULONG Count, HANDLE *Handles, ULONG WaitType, BOOLEAN Alertable,
    PLARGE_INTEGER Timeout);
/* DIVERGENCE(ADAPTER-WIN32-021): Original SoftPC fprt.c uses the retired
 * CRT calling-convention macro in declarations of its original diagnostic
 * wrappers.  Modern MSVC omits that spelling.  Preserve the original ABI as
 * cdecl without changing the source body or diagnostic behavior. */
#ifndef _CRTAPI1
#define _CRTAPI1 __cdecl
#endif
/* DIVERGENCE(ADAPTER-WIN32-024): Original SoftPC fprt.c declares both
 * historical CRT wrapper calling-convention macros.  Current MSVC omits the
 * second spelling as well.  Keep the source body's cdecl ABI unchanged. */
#ifndef _CRTAPI2
#define _CRTAPI2 __cdecl
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
#ifndef STATUS_VDM_HARD_ERROR
/* The selected original WOW caller uses this native status as the exact
 * private-CSRSS request selector. Modern winternl.h omits the historical
 * spelling; retain only the fixed ABI value, not a hard-error service. */
#define STATUS_VDM_HARD_ERROR ((NTSTATUS)0xC000021DL)
#endif
#ifndef STATUS_HANDLE_NOT_CLOSABLE
#define STATUS_HANDLE_NOT_CLOSABLE ((NTSTATUS)0xC0000235L)
#endif
#ifndef STATUS_NO_TOKEN
#define STATUS_NO_TOKEN ((NTSTATUS)0xC000007CL)
#endif
#ifndef MAXULONG
#define MAXULONG ((ULONG)0xFFFFFFFFUL)
#endif

typedef CURDIR OPENNT_SUPPORT_CURDIR, *POPENNT_SUPPORT_CURDIR;

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

/* winnt.h declares the real process TEB accessor.  Reached original sources
 * need only selected historical fields, so route their spelling to
 * adapter-private TLS rather than casting a modern TEB. */
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
#ifndef leave
#define leave __leave
#endif
#ifndef STATUS_VARIABLE_NOT_FOUND
#define STATUS_VARIABLE_NOT_FOUND ((NTSTATUS)0xC0000100L)
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

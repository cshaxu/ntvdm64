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
#include <winternl.h>

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

typedef STRING OEM_STRING, *POEM_STRING;
typedef const OEM_STRING *PCOEM_STRING;

typedef struct _OPENNT_SUPPORT_CURDIR {
    UNICODE_STRING DosPath;
    HANDLE Handle;
} OPENNT_SUPPORT_CURDIR, *POPENNT_SUPPORT_CURDIR;
typedef OPENNT_SUPPORT_CURDIR CURDIR, *PCURDIR;

typedef struct _OPENNT_SUPPORT_PROCESS_PARAMETERS {
    OPENNT_SUPPORT_CURDIR CurrentDirectory;
} OPENNT_SUPPORT_PROCESS_PARAMETERS, *POPENNT_SUPPORT_PROCESS_PARAMETERS;

typedef struct _OPENNT_SUPPORT_PEB {
    POPENNT_SUPPORT_PROCESS_PARAMETERS ProcessParameters;
    PVOID ProcessHeap;
} OPENNT_SUPPORT_PEB, *POPENNT_SUPPORT_PEB;

typedef struct _OPENNT_SUPPORT_TEB {
    UNICODE_STRING StaticUnicodeString;
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
#ifndef RtlMoveMemory
#define RtlMoveMemory(Destination, Source, Length) \
    MoveMemory((Destination), (Source), (Length))
#endif
#ifndef RtlZeroMemory
#define RtlZeroMemory(Destination, Length) ZeroMemory((Destination), (Length))
#endif

#endif

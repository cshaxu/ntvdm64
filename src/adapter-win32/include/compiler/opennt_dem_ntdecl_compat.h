/*
 * Historical DEM declaration bridge for the standalone object gate.
 *
 * The fixed OpenNT build assumed a PCH that made the selected NT user-mode
 * declarations visible. Current SDK winternl.h supplies the scalar and RTL
 * declarations but omits the older volume/object information layouts below.
 * Each layout and value is copied from the retained OpenNT public SDK headers:
 * ntioapi.h, ntobapi.h, and ntpsapi.h. This header supplies declarations only;
 * it implements no host operation and is never part of a runtime target.
 */
#ifndef DEM_NTDECL_COMPAT_H
#define DEM_NTDECL_COMPAT_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#elif _WIN32_WINNT < 0x0500
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#ifndef __kernel_entry
#define __kernel_entry
#endif
#include <winternl.h>

/* Retained OpenNT headers: status values, historical SEH spellings, and the
 * VdmQueryDir request layout used by demsrch.c. */
#pragma warning(push)
#pragma warning(disable:4005)
#include <ntstatus.h>
#pragma warning(pop)
/* OpenNT's crt/excpt.h supplied only the two legacy spellings below.  The
 * modern standalone SDK closure has no such header, so retain the spelling
 * adaptation locally instead of making this imported DEM declaration bridge
 * depend on a missing CRT include. */
#define STAGE_VDM_TYPES 1
#include "vdm.h"

/* OpenNT crt/excpt.h maps these legacy spellings to MS SEH keywords. */
#ifndef try
#define try __try
#endif
#ifndef finally
#define finally __finally
#endif

typedef enum _FS_INFORMATION_CLASS {
    FileFsVolumeInformation = 1,
    FileFsLabelInformation,
    FileFsSizeInformation,
    FileFsDeviceInformation
} FS_INFORMATION_CLASS;

typedef struct _FILE_FS_DEVICE_INFORMATION {
    ULONG DeviceType;
    ULONG Characteristics;
} FILE_FS_DEVICE_INFORMATION, *PFILE_FS_DEVICE_INFORMATION;

typedef struct _FILE_BOTH_DIR_INFORMATION {
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    ULONG EaSize;
    CCHAR ShortNameLength;
    WCHAR ShortName[12];
    WCHAR FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

/* FILE_INFORMATION_CLASS::FileBothDirectoryInformation from ntioapi.h. */
#ifndef FileBothDirectoryInformation
#define FileBothDirectoryInformation ((FILE_INFORMATION_CLASS)3)
#endif

#ifndef FILE_REMOVABLE_MEDIA
#define FILE_REMOVABLE_MEDIA 0x00000001
#endif
#ifndef FILE_REMOTE_DEVICE
#define FILE_REMOTE_DEVICE 0x00000010
#endif

typedef struct _OBJECT_NAME_INFORMATION {
    UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

#ifndef ObjectNameInformation
#define ObjectNameInformation ((OBJECT_INFORMATION_CLASS)1)
#endif
#ifndef SYMBOLIC_LINK_QUERY
#define SYMBOLIC_LINK_QUERY 0x0001
#endif
#ifndef MAXIMUM_FILENAME_LENGTH
#define MAXIMUM_FILENAME_LENGTH 256
#endif
#ifndef STATUS_BUFFER_OVERFLOW
#define STATUS_BUFFER_OVERFLOW ((NTSTATUS)0x80000005L)
#endif
#ifndef DOS_QM
#define DOS_QM (L'>')
#endif
#ifndef DOS_DOT
#define DOS_DOT (L'"')
#endif

NTSYSAPI PVOID NTAPI RtlAllocateHeap(
    PVOID heap_handle,
    ULONG flags,
    SIZE_T size);

/* The modern x86 ntdll import library no longer exports this historic entry.
 * A caller-proven host compatibility definition is supplied only by the S5
 * trace target that reaches it. */
PVOID NTAPI RtlProcessHeap(VOID);

NTSYSAPI BOOLEAN NTAPI RtlFreeHeap(
    PVOID heap_handle,
    ULONG flags,
    PVOID base_address);

NTSYSAPI NTSTATUS NTAPI RtlOemStringToUnicodeString(
    PUNICODE_STRING destination_string,
    PCOEM_STRING source_string,
    BOOLEAN allocate_destination_string);

/* demgset.c uses this retained NT user-mode conversion entry, but its
 * standalone historical include set does not expose the nturtl.h prototype.
 * The fourth parameter is PRTL_RELATIVE_NAME in the original header; this
 * profile passes NULL and preserves its pointer ABI without importing that
 * otherwise-unused private structure. */
NTSYSAPI BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(
    PCWSTR dos_file_name,
    PUNICODE_STRING nt_file_name,
    PWSTR *file_part,
    PVOID relative_name);

NTSYSAPI NTSTATUS NTAPI NtQueryVolumeInformationFile(
    HANDLE file_handle,
    PIO_STATUS_BLOCK io_status_block,
    PVOID fs_information,
    ULONG length,
    FS_INFORMATION_CLASS fs_information_class);

/* Preserve the OpenNT pseudo-handle value with the modern HANDLE type. */
#ifdef NtCurrentProcess
#undef NtCurrentProcess
#endif
#define NtCurrentProcess() ((HANDLE)(LONG_PTR)-1)

#endif

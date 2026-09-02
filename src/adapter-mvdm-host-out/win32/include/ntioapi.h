/*
 * Reached same-shaped subset of OpenNT ntioapi.h.
 *
 * The byte-exact historical carrier remains in opennt-abi. This
 * facade exposes only declarations reached by the original DEM source, where
 * current winternl.h has no compatible public declaration.
 */
#ifndef ADAPTER_WIN32_NTIOAPI_H
#define ADAPTER_WIN32_NTIOAPI_H

typedef struct _OPENNT_IO_STATUS_BLOCK {
    NTSTATUS Status;
    ULONG Information;
} OPENNT_IO_STATUS_BLOCK, *POPENNT_IO_STATUS_BLOCK;

typedef struct _OPENNT_FILE_BOTH_DIR_INFORMATION {
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
} OPENNT_FILE_BOTH_DIR_INFORMATION, *POPENNT_FILE_BOTH_DIR_INFORMATION;

typedef struct _OPENNT_FILE_FS_DEVICE_INFORMATION {
    DEVICE_TYPE DeviceType;
    ULONG Characteristics;
} OPENNT_FILE_FS_DEVICE_INFORMATION, *POPENNT_FILE_FS_DEVICE_INFORMATION;

typedef struct _OPENNT_OBJECT_NAME_INFORMATION {
    UNICODE_STRING Name;
} OPENNT_OBJECT_NAME_INFORMATION, *POPENNT_OBJECT_NAME_INFORMATION;

typedef enum _ADAPTER_FILE_INFORMATION_CLASS {
    AdapterFileBothDirectoryInformation = 3,
    AdapterFileAlignmentInformation = 17
} ADAPTER_FILE_INFORMATION_CLASS;

typedef enum _ADAPTER_FS_INFORMATION_CLASS {
    AdapterFileFsDeviceInformation = 4
} ADAPTER_FS_INFORMATION_CLASS;

#define FileBothDirectoryInformation AdapterFileBothDirectoryInformation
#define FileAlignmentInformation AdapterFileAlignmentInformation
#define FileFsDeviceInformation AdapterFileFsDeviceInformation

/* Exact reached OpenNT ntioapi.h constants. */
#ifndef MAXIMUM_FILENAME_LENGTH
#define MAXIMUM_FILENAME_LENGTH 256
#endif
#ifndef FILE_REMOVABLE_MEDIA
#define FILE_REMOVABLE_MEDIA 0x00000001
#endif
#ifndef FILE_REMOTE_DEVICE
#define FILE_REMOTE_DEVICE 0x00000010
#endif
#ifndef FSCTL_QUERY_FAT_BPB
#define FSCTL_QUERY_FAT_BPB CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif
#ifndef SYMBOLIC_LINK_QUERY
#define SYMBOLIC_LINK_QUERY 0x0001
#endif
#ifndef ObjectNameInformation
#define ObjectNameInformation 1
#endif
#ifndef DOS_QM
#define DOS_QM (L'>')
#endif
#ifndef DOS_DOT
#define DOS_DOT (L'"')
#endif

/* Current public winioctl.h already provides OpenNT's exact
 * FSCTL_QUERY_FAT_BPB_BUFFER name and 0x24-byte layout.  Do not redeclare it
 * here: this source-facing adapter must bind the SDK carrier rather than
 * shadow it with an identical private type.  Some original host units include
 * only ntioapi.h, so expose the public declaration at this boundary. */
#include <winioctl.h>

typedef struct _FILE_ALIGNMENT_INFORMATION {
    ULONG AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION;

#ifndef STATUS_UNSUCCESSFUL
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#endif
#ifndef STATUS_NO_MORE_FILES
#define STATUS_NO_MORE_FILES ((NTSTATUS)0x80000006L)
#endif
#ifndef STATUS_NO_SUCH_FILE
#define STATUS_NO_SUCH_FILE ((NTSTATUS)0xC000000FL)
#endif
#ifndef STATUS_BUFFER_TOO_SMALL
#define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
#endif
#ifndef STATUS_OBJECT_TYPE_MISMATCH
#define STATUS_OBJECT_TYPE_MISMATCH ((NTSTATUS)0xC0000024L)
#endif
#ifndef STATUS_OBJECT_NAME_NOT_FOUND
#define STATUS_OBJECT_NAME_NOT_FOUND ((NTSTATUS)0xC0000034L)
#endif
#ifndef STATUS_OBJECT_PATH_NOT_FOUND
#define STATUS_OBJECT_PATH_NOT_FOUND ((NTSTATUS)0xC000003AL)
#endif

/* DIVERGENCE: modern public headers omit these declarations even though
 * ntdll exports the historical call shapes.  Resolve at runtime so absence
 * returns STATUS_NOT_IMPLEMENTED rather than creating an unreviewed import. */
NTSTATUS NTAPI opennt_NtQueryDirectoryFile(
    HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext, POPENNT_IO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation,
    ULONG Length, ADAPTER_FILE_INFORMATION_CLASS FileInformationClass,
    BOOLEAN ReturnSingleEntry, PUNICODE_STRING FileName, BOOLEAN RestartScan);

NTSTATUS NTAPI opennt_NtQueryVolumeInformationFile(
    HANDLE FileHandle, POPENNT_IO_STATUS_BLOCK IoStatusBlock, PVOID FsInformation,
    ULONG Length, ADAPTER_FS_INFORMATION_CLASS FsInformationClass);

NTSTATUS NTAPI opennt_NtOpenSymbolicLinkObject(
    PHANDLE LinkHandle, ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes);

NTSTATUS NTAPI opennt_NtQuerySymbolicLinkObject(
    HANDLE LinkHandle, PUNICODE_STRING LinkTarget, PULONG ReturnedLength);

NTSTATUS NTAPI opennt_NtOpenFile(
    PHANDLE FileHandle, ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes, POPENNT_IO_STATUS_BLOCK IoStatusBlock,
    ULONG ShareAccess, ULONG OpenOptions);

NTSTATUS NTAPI opennt_NtCreateFile(
    PHANDLE FileHandle, ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes, POPENNT_IO_STATUS_BLOCK IoStatusBlock,
    PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess,
    ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer,
    ULONG EaLength);

NTSTATUS NTAPI opennt_NtQueryObject(
    HANDLE Handle, ULONG ObjectInformationClass, PVOID ObjectInformation,
    ULONG ObjectInformationLength, PULONG ReturnLength);

NTSTATUS NTAPI opennt_NtQueryInformationFile(
    HANDLE FileHandle, POPENNT_IO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation, ULONG Length,
    ADAPTER_FILE_INFORMATION_CLASS FileInformationClass);

NTSTATUS NTAPI opennt_NtFsControlFile(
    HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext, POPENNT_IO_STATUS_BLOCK IoStatusBlock,
    ULONG FsControlCode, PVOID InputBuffer, ULONG InputBufferLength,
    PVOID OutputBuffer, ULONG OutputBufferLength);

NTSTATUS NTAPI opennt_NtDeviceIoControlFile(
    HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext, POPENNT_IO_STATUS_BLOCK IoStatusBlock,
    ULONG IoControlCode, PVOID InputBuffer, ULONG InputBufferLength,
    PVOID OutputBuffer, ULONG OutputBufferLength);

/* DIVERGENCE: current winternl.h defines pointer-sized IO_STATUS_BLOCK on
 * x64, while the imported NT4 DEM contract has a 32-bit Information member.
 * Source uses the original spelling below; wrappers marshal only the
 * host-local transient record at the modern ntdll boundary. */
#define IO_STATUS_BLOCK OPENNT_IO_STATUS_BLOCK
#define PIO_STATUS_BLOCK POPENNT_IO_STATUS_BLOCK
#define FILE_BOTH_DIR_INFORMATION OPENNT_FILE_BOTH_DIR_INFORMATION
#define PFILE_BOTH_DIR_INFORMATION POPENNT_FILE_BOTH_DIR_INFORMATION
#define FILE_FS_DEVICE_INFORMATION OPENNT_FILE_FS_DEVICE_INFORMATION
#define PFILE_FS_DEVICE_INFORMATION POPENNT_FILE_FS_DEVICE_INFORMATION
#define OBJECT_NAME_INFORMATION OPENNT_OBJECT_NAME_INFORMATION
#define POBJECT_NAME_INFORMATION POPENNT_OBJECT_NAME_INFORMATION
#define NtOpenFile opennt_NtOpenFile
#define NtCreateFile opennt_NtCreateFile
#define NtQueryObject opennt_NtQueryObject
#define NtQueryDirectoryFile opennt_NtQueryDirectoryFile
#define NtQueryVolumeInformationFile opennt_NtQueryVolumeInformationFile
#define NtOpenSymbolicLinkObject opennt_NtOpenSymbolicLinkObject
#define NtQuerySymbolicLinkObject opennt_NtQuerySymbolicLinkObject
#define NtQueryInformationFile opennt_NtQueryInformationFile
#define NtFsControlFile opennt_NtFsControlFile
#define NtDeviceIoControlFile opennt_NtDeviceIoControlFile

#endif

#ifndef BX_NTVDM_BOP_SHIM_DEMSRCH_FCB_SHIM_H
#define BX_NTVDM_BOP_SHIM_DEMSRCH_FCB_SHIM_H

/* Common compatibility entrypoint for directly imported OpenNT demsrch.c and
 * demfcb.c.  This deliberately starts with the already-proven DEM mechanical
 * ABI only; DTA/FCB/PDB and NT-directory-query layouts are admitted from the
 * original headers one dependency at a time, never invented here. */
/* The former standalone-object compatibility header pulls a historical CRT
 * exception header which is neither present nor part of this Direct package.
 * The imported sources use the public Win32/NT user-mode declaration surface;
 * later declarations are admitted here only when their exact original use is
 * reached. */
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
/* winternl.h needs the normal SDK architecture selection made by windows.h.
 * This is an include-order repair only; no historical service is replaced. */
#include <windows.h>
#include <winternl.h>

/* Source declaration bridge: OpenNT's public NT/VDM types and constants,
 * with its historical crt/excpt.h dependency normalized above.  This is a
 * declaration-only compatibility seam, not a legacy BOP provider. */
#include "adapter-win32/include/compiler/opennt_dem_ntdecl_compat.h"

/* winternl.h must precede demfile_shim.h: the retained OpenNT ACCESS_MASK
 * value macro is source-local DOS open-mode vocabulary, whereas winternl.h
 * uses ACCESS_MASK as an NT declaration type. */
#include "demfile_shim.h"
#include "adapter-win32/include/legacy-api/opennt_dem_search_facade.h"

/* Verbatim layout source: base/mvdm/dos/dem/dosdef.h. */
#define ATTR_READ_ONLY 0x1
#define ATTR_HIDDEN 0x2
#define ATTR_SYSTEM 0x4
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_DEVICE 0x40
#define ATTR_ALL (ATTR_HIDDEN | ATTR_SYSTEM | ATTR_DIRECTORY)
#ifndef DOS_VOLUME_NAME_SIZE
#define DOS_VOLUME_NAME_SIZE 11
#endif
#ifndef NT_VOLUME_NAME_SIZE
#define NT_VOLUME_NAME_SIZE 255
#endif
#define FETCHWORD(value) ((WORD)(value))
#define FETCHDWORD(value) ((DWORD)(value))
#ifndef ASSERT
#define ASSERT(value) ((void)0)
#endif
#define MSG_INVALID_HFIND 7u
#define MSG_FILEINFO 8u
#define MSG_FILESIZE_TOOBIG 9u
#pragma pack(push, 1)
/* OpenNT's 32-bit SRCHDTA and DIRENT reserve exactly four bytes for the
 * find-entry reference.  On an x64 host that field cannot be a PVOID: it is
 * guest memory, not host storage.  The imported search code already obtains
 * the live record by FFindId; FFindRef is the same stable 32-bit validation
 * cookie and never transports a host address. */
typedef struct _SRCHDTA { ULONG FFindRef; ULONG FFindId; BYTE bReserved[13]; UCHAR uchFileAttr; USHORT usTimeLastWrite; USHORT usDateLastWrite; USHORT usLowSize; USHORT usHighSize; CHAR achFileName[13]; } SRCHDTA, *PSRCHDTA;
typedef struct _DIRENT { CHAR FileName[8]; CHAR FileExt[3]; UCHAR uchAttributes; ULONG FFindRef; ULONG FFindId; USHORT usDummy; USHORT usTime; USHORT usDate; USHORT usReserved2; ULONG ulFileSize; } DIRENT, *PDIRENT;
typedef struct _SRCHBUF { UCHAR uchDriveNumber; CHAR FileName[8]; CHAR FileExt[3]; USHORT usCurBlkNumber; USHORT usRecordSize; ULONG ulFileSize; DIRENT DirEnt; } SRCHBUF, *PSRCHBUF;
#pragma pack(pop)
#ifndef STOREDWORD
#define STOREDWORD(destination, source) ((destination) = (DWORD)(source))
#endif
#define BX_NTVDM_DEMSEARCH_REFERENCE(find_id) ((DWORD)(find_id))
#define InitializeListHead(list_head) do { (list_head)->Flink = (list_head); (list_head)->Blink = (list_head); } while (0)
#define IsListEmpty(list_head) ((list_head)->Flink == (list_head))
#define InsertHeadList(list_head, entry) do { PLIST_ENTRY const bx_next = (list_head)->Flink; (entry)->Flink = bx_next; (entry)->Blink = (list_head); bx_next->Blink = (entry); (list_head)->Flink = (entry); } while (0)
#define RemoveEntryList(entry) (((entry)->Blink->Flink = (entry)->Flink), ((entry)->Flink->Blink = (entry)->Blink), TRUE)
extern ULONG UNALIGNED *pulDTALocation;

USHORT demDeleteLabel(BYTE drive);
VOID demFCBCommon(ULONG create_directive);
BOOL demGetMiscInfo(HANDLE file, LPWORD time_out, LPWORD date_out,
    LPDWORD size_out);

/* C11 needs declarations for OpenNT's forward-called K&R-era bodies; their
 * definitions and order remain in the directly imported files. */
DWORD demFileFindFirst(PVOID dta, LPSTR file, USHORT attributes);
DWORD demFileFindNext(PVOID dta);

NTSYSAPI BOOLEAN NTAPI RtlCreateUnicodeString(PUNICODE_STRING destination,
    PCWSTR source);
NTSYSAPI LONG NTAPI RtlCompareUnicodeString(PCUNICODE_STRING first,
    PCUNICODE_STRING second, BOOLEAN case_insensitive);
NTSYSAPI NTSTATUS NTAPI RtlUpcaseUnicodeStringToCountedOemString(
    POEM_STRING destination, PCUNICODE_STRING source, BOOLEAN allocate);
NTSYSAPI ULONG NTAPI RtlIsDosDeviceName_U(PWSTR path);
NTSYSAPI NTSTATUS NTAPI NtQueryDirectoryFile(HANDLE file, HANDLE event,
    PIO_APC_ROUTINE apc, PVOID context, PIO_STATUS_BLOCK status,
    PVOID information, ULONG length, FILE_INFORMATION_CLASS info_class,
    BOOLEAN return_single, PUNICODE_STRING name, BOOLEAN restart);

/* These warnings arise from source-preserved 1990s identifiers and a
 * deliberately unused historical parameter; they are not semantic changes. */
#pragma warning(disable:4100 4459)

#endif

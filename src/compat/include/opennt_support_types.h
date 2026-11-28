/*
 * SDK bindings used by the selected OpenNT host declarations.
 */
#ifndef OPENNT_SUPPORT_TYPES_H
#define OPENNT_SUPPORT_TYPES_H

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
#include <winternl.h>
#include "section_image_information.h"
/* Native SSPI status declaration, shared/sspi.h. */
typedef LONG SECURITY_STATUS;
/* XP pebteb.h frame identity; RTL declarations need only its pointer. */
typedef struct _TEB_ACTIVE_FRAME *PTEB_ACTIVE_FRAME;
/* XP ntddk.h FAST_MUTEX tag: pointer-only prerequisite of WIN32HEAP. */
typedef struct _FAST_MUTEX FAST_MUTEX, *PFAST_MUTEX;
/* XP base/ntos/inc/ex.h:885-889; declaration only, no allocator. */
typedef VOID (*PFREE_FUNCTION)(PVOID Buffer);

/* XP ntdef.h:1019; native declaration prerequisite. */
typedef enum _TIMER_TYPE {
    NotificationTimer,
    SynchronizationTimer
    } TIMER_TYPE;

/* XP ntkeapi.h:142; native declaration prerequisite. */
typedef enum _KPROFILE_SOURCE {
    ProfileTime,
    ProfileAlignmentFixup,
    ProfileTotalIssues,
    ProfilePipelineDry,
    ProfileLoadInstructions,
    ProfilePipelineFrozen,
    ProfileBranchInstructions,
    ProfileTotalNonissues,
    ProfileDcacheMisses,
    ProfileIcacheMisses,
    ProfileCacheMisses,
    ProfileBranchMispredictions,
    ProfileStoreInstructions,
    ProfileFpInstructions,
    ProfileIntegerInstructions,
    Profile2Issue,
    Profile3Issue,
    Profile4Issue,
    ProfileSpecialInstructions,
    ProfileTotalCycles,
    ProfileIcacheIssues,
    ProfileDcacheAccesses,
    ProfileMemoryBarrierCycles,
    ProfileLoadLinkedIssues,
    ProfileMaximum
} KPROFILE_SOURCE;

/* XP ntioapi.h:2979; native declaration prerequisite. */
typedef enum _INTERFACE_TYPE {
    InterfaceTypeUndefined = -1,
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMEBus,
    NuBus,
    PCMCIABus,
    CBus,
    MPIBus,
    MPSABus,
    ProcessorInternal,
    InternalPowerBus,
    PNPISABus,
    PNPBus,
    MaximumInterfaceType
}INTERFACE_TYPE, *PINTERFACE_TYPE;

/* XP ntioapi.h:3038; native declaration prerequisite. */
typedef enum _BUS_DATA_TYPE {
    ConfigurationSpaceUndefined = -1,
    Cmos,
    EisaConfiguration,
    Pos,
    CbusConfiguration,
    PCIConfiguration,
    VMEConfiguration,
    NuBusConfiguration,
    PCMCIAConfiguration,
    MPIConfiguration,
    MPSAConfiguration,
    PNPISAConfiguration,
    SgiInternalConfiguration,
    MaximumBusDataType
} BUS_DATA_TYPE, *PBUS_DATA_TYPE;

/* XP ntdef.h:761; native physical-address scalar ABI. */
typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

/* XP public/sdk/inc/ntkeapi.h:53-57; public shared-time ABI only. */
typedef struct _KSYSTEM_TIME {
    ULONG LowPart;
    LONG High1Time;
    LONG High2Time;
} KSYSTEM_TIME, *PKSYSTEM_TIME;

/* XP ntpsapi.h and ntexapi.h process drive-map query. */
#define ProcessDeviceMap ((PROCESSINFOCLASS)23)
#define DOSDEVICE_DRIVE_REMOVABLE 2
#define DOSDEVICE_DRIVE_FIXED 3
#define DOSDEVICE_DRIVE_CDROM 5
#define DOSDEVICE_DRIVE_RAMDISK 6
#include "wow_user_declarations.h"
/* Finite original ntdef/ntexapi/ntnls declarations absent from the public
 * Win32 headers. Private NLS/process information is exposed only as opaque
 * pointers; fixed-width strings retain the original ABI layout. */
typedef enum _NT_PRODUCT_TYPE {
    NtProductWinNt = 1,
    NtProductLanManNt,
    NtProductServer
} NT_PRODUCT_TYPE, *PNT_PRODUCT_TYPE;
typedef ULONG CLONG;

/* Original ntddk.h:81,204,9260-9264. Types required by retained USER
 * declarations only; no kernel object or system-thread provider is imported. */
typedef struct _OBJECT_TYPE *POBJECT_TYPE;
/* XP ntconfig.h: opaque resource-list pointer used by ntpnpapi.h. */
typedef struct _CM_RESOURCE_LIST *PCM_RESOURCE_LIST;
typedef CCHAR KPROCESSOR_MODE;
/* XP base/ntos/inc/ntosdef.h:105-109 and ke.h:320. */

typedef struct _KWAIT_BLOCK *PKWAIT_BLOCK;
typedef struct _HARDERROR_MSG *PHARDERROR_MSG;

/* Original ke.h:318-326 selected x86/x64 declaration and w32p.h:47-54.
 * Declaration-only prefix; no kernel table, ETHREAD access or provider. */
typedef struct _ETHREAD *PETHREAD;
typedef struct _KSERVICE_TABLE_DESCRIPTOR {
    PULONG_PTR Base;
    PULONG Count;
    ULONG Limit;
    PUCHAR Number;
} KSERVICE_TABLE_DESCRIPTOR, *PKSERVICE_TABLE_DESCRIPTOR;
typedef struct _W32THREAD {
    KSERVICE_TABLE_DESCRIPTOR ServiceDescriptorTable[2];
    PETHREAD    Thread;
    CLIENT_ID   RealClientId;
    HANDLE      GdiCachedProcessHandle;
    PVOID       pgdiDcattr;
    PVOID       pgdiBrushAttr;
} W32THREAD;

/* Original ntddk.h:77,12498-12520; pointer-only object dependencies. */
typedef struct _EPROCESS *PEPROCESS;
typedef struct _ACCESS_STATE *PACCESS_STATE;
/* Original public/ifskit/inc/ntifs.h:17965-17971. */
typedef enum _OB_OPEN_REASON {
    ObCreateHandle,
    ObOpenHandle,
    ObDuplicateHandle,
    ObInheritHandle,
    ObMaxOpenReason
} OB_OPEN_REASON;

/* Original public/ifskit/inc/ntifs.h:1410-1417; declaration only. */
typedef struct _SECURITY_CLIENT_CONTEXT {
    SECURITY_QUALITY_OF_SERVICE SecurityQos;
    PACCESS_TOKEN ClientToken;
    BOOLEAN DirectlyAccessClientToken;
    BOOLEAN DirectAccessEffectiveOnly;
    BOOLEAN ServerIsRemote;
    TOKEN_CONTROL ClientTokenControl;
    } SECURITY_CLIENT_CONTEXT, *PSECURITY_CLIENT_CONTEXT;
/* Original event tag is pointer-only here;its kernel body is not imported. */
typedef struct _KEVENT *PKEVENT;

/* Original w32p.h:82,88-108.
 * Declaration-only PROCESSINFO dependencies; no kernel process state.
 * NextStart spells the original tag directly so PW32PROCESS remains
 * available to the existing host binding. */
typedef USHORT W32PID;
typedef struct _PID_HANDLE_TRACK
{
    ULONG       Pid;
    LONG        HandleCount;
    struct _PID_HANDLE_TRACK *pNext;
    struct _PID_HANDLE_TRACK *pPrev;
}PID_HANDLE_TRACK,*PPID_HANDLE_TRACK;
typedef struct _W32PROCESS {
    PEPROCESS   Process;
    ULONG       W32PF_Flags;
    PKEVENT     InputIdleEvent;
    ULONG       StartCursorHideTime;
    struct _W32PROCESS *NextStart;
    PVOID       pDCAttrList;
    PVOID       pBrushAttrList;
    W32PID      W32Pid;
    PID_HANDLE_TRACK pidHandleTrack;
} W32PROCESS;
/* Original ntddk.h:11204-11221; resource object remains opaque. */
typedef struct _ERESOURCE *PERESOURCE;
#include <nt_event_declarations.h>
typedef
VOID
(*PKSTART_ROUTINE) (
    IN PVOID StartContext
    );
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
/* DIVERGENCE: Original DEM storage declarations require historical
 * devioctl.h and own GetDiskSpaceInformation as an unrelated BOOL helper.
 * Keep the storage carrier visible and remove only the conflicting modern
 * SDK convenience alias; ntioapi.h awaits its own complete ABI binding. */
#undef GetDiskSpaceInformation

#undef NtCurrentTeb

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif
#ifndef STATUS_UNMAPPABLE_CHARACTER
#define STATUS_UNMAPPABLE_CHARACTER ((NTSTATUS)0xC0000162L)
#endif
#ifndef STATUS_SEVERITY_WARNING
#define STATUS_SEVERITY_WARNING 2
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

#include "opennt_support_thread.h"

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

#endif

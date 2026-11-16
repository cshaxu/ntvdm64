
/*++ BUILD Version: 0008    // Increment this if a change has global effects

Copyright (c) 1989-1999  Microsoft Corporation

Module Name:

    ntexapi.h

Abstract:

    This module is the header file for the all the system services that
    are contained in the "ex" directory.

Author:

    David N. Cutler (davec) 5-May-1989

Revision History:

--*/

#line 23
#ifndef _NTEXAPI_
#define _NTEXAPI_

#line 26
#if _MSC_VER > 1000
#pragma once
#endif
#line 30
#ifdef __cplusplus
#line 31
#error Excluded unsupported C++ NT executive declaration context
#endif
#line 615
//
// Performance Counter Definitions
//

NTSYSAPI
NTSTATUS
NTAPI
NtQueryPerformanceCounter (
    OUT PLARGE_INTEGER PerformanceCounter,
    OUT PLARGE_INTEGER PerformanceFrequency OPTIONAL
    );

#line 627

//
// Nt Api Profile Definitions
//

//
// Nt Api Profiling data structure
//

typedef struct _NAPDATA {
    ULONG NapLock;
    ULONG Calls;
    ULONG TimingErrors;
    LARGE_INTEGER TotalTime;
    LARGE_INTEGER FirstTime;
    LARGE_INTEGER MaxTime;
    LARGE_INTEGER MinTime;
} NAPDATA, *PNAPDATA;

NTSTATUS
NapClearData (
    VOID
    );

NTSTATUS
NapRetrieveData (
    OUT NAPDATA *NapApiData,
    OUT PCHAR **NapApiNames,
    OUT PLARGE_INTEGER *NapCounterFrequency
    );

NTSTATUS
NapGetApiCount (
    OUT PULONG NapApiCount
    );

NTSTATUS
NapPause (
    VOID
    );

NTSTATUS
NapResume (
    VOID
    );


//
//  Driver Verifier Definitions
//

typedef ULONG_PTR (*PDRIVER_VERIFIER_THUNK_ROUTINE) (
    IN PVOID Context
    );

//
//  This structure is passed in by drivers that want to thunk callers of
//  their exports.
//

typedef struct _DRIVER_VERIFIER_THUNK_PAIRS {
    PDRIVER_VERIFIER_THUNK_ROUTINE  PristineRoutine;
    PDRIVER_VERIFIER_THUNK_ROUTINE  NewRoutine;
} DRIVER_VERIFIER_THUNK_PAIRS, *PDRIVER_VERIFIER_THUNK_PAIRS;

//
//  Driver Verifier flags.
//

#define DRIVER_VERIFIER_SPECIAL_POOLING             0x0001
#define DRIVER_VERIFIER_FORCE_IRQL_CHECKING         0x0002
#define DRIVER_VERIFIER_INJECT_ALLOCATION_FAILURES  0x0004
#define DRIVER_VERIFIER_TRACK_POOL_ALLOCATIONS      0x0008
#define DRIVER_VERIFIER_IO_CHECKING                 0x0010


//
// System Information Classes.
//

#line 764

//
// System Information Structures.
//

// begin_winnt
#define TIME_ZONE_ID_UNKNOWN  0
#define TIME_ZONE_ID_STANDARD 1
#define TIME_ZONE_ID_DAYLIGHT 2
// end_winnt

typedef struct _SYSTEM_VDM_INSTEMUL_INFO {
    ULONG SegmentNotPresent ;
    ULONG VdmOpcode0F       ;
    ULONG OpcodeESPrefix    ;
    ULONG OpcodeCSPrefix    ;
    ULONG OpcodeSSPrefix    ;
    ULONG OpcodeDSPrefix    ;
    ULONG OpcodeFSPrefix    ;
    ULONG OpcodeGSPrefix    ;
    ULONG OpcodeOPER32Prefix;
    ULONG OpcodeADDR32Prefix;
    ULONG OpcodeINSB        ;
    ULONG OpcodeINSW        ;
    ULONG OpcodeOUTSB       ;
    ULONG OpcodeOUTSW       ;
    ULONG OpcodePUSHF       ;
    ULONG OpcodePOPF        ;
    ULONG OpcodeINTnn       ;
    ULONG OpcodeINTO        ;
    ULONG OpcodeIRET        ;
    ULONG OpcodeINBimm      ;
    ULONG OpcodeINWimm      ;
    ULONG OpcodeOUTBimm     ;
    ULONG OpcodeOUTWimm     ;
    ULONG OpcodeINB         ;
    ULONG OpcodeINW         ;
    ULONG OpcodeOUTB        ;
    ULONG OpcodeOUTW        ;
    ULONG OpcodeLOCKPrefix  ;
    ULONG OpcodeREPNEPrefix ;
    ULONG OpcodeREPPrefix   ;
    ULONG OpcodeHLT         ;
    ULONG OpcodeCLI         ;
    ULONG OpcodeSTI         ;
    ULONG BopCount          ;
} SYSTEM_VDM_INSTEMUL_INFO, *PSYSTEM_VDM_INSTEMUL_INFO;

#line 821

#line 835

typedef struct _SYSTEM_PROCESSOR_INFORMATION {
    USHORT ProcessorArchitecture;
    USHORT ProcessorLevel;
    USHORT ProcessorRevision;
    USHORT Reserved;
    ULONG ProcessorFeatureBits;
} SYSTEM_PROCESSOR_INFORMATION, *PSYSTEM_PROCESSOR_INFORMATION;

#line 852

typedef struct _SYSTEM_QUERY_TIME_ADJUST_INFORMATION {
    ULONG TimeAdjustment;
    ULONG TimeIncrement;
    BOOLEAN Enable;
} SYSTEM_QUERY_TIME_ADJUST_INFORMATION, *PSYSTEM_QUERY_TIME_ADJUST_INFORMATION;

typedef struct _SYSTEM_SET_TIME_ADJUST_INFORMATION {
    ULONG TimeAdjustment;
    BOOLEAN Enable;
} SYSTEM_SET_TIME_ADJUST_INFORMATION, *PSYSTEM_SET_TIME_ADJUST_INFORMATION;

#line 940

#line 976

typedef struct _SYSTEM_SESSION_PROCESS_INFORMATION {
    ULONG SessionId;
    ULONG SizeOfBuf;
    PVOID Buffer;
} SYSTEM_SESSION_PROCESS_INFORMATION, *PSYSTEM_SESSION_PROCESS_INFORMATION;

#line 996

typedef struct _SYSTEM_MEMORY_INFO {
    PUCHAR StringOffset;
    USHORT ValidCount;
    USHORT TransitionCount;
    USHORT ModifiedCount;
    USHORT PageTableCount;
} SYSTEM_MEMORY_INFO, *PSYSTEM_MEMORY_INFO;

typedef struct _SYSTEM_MEMORY_INFORMATION {
    ULONG InfoSize;
    ULONG StringStart;
    SYSTEM_MEMORY_INFO Memory[1];
} SYSTEM_MEMORY_INFORMATION, *PSYSTEM_MEMORY_INFORMATION;

typedef struct _SYSTEM_CALL_COUNT_INFORMATION {
    ULONG Length;
    ULONG NumberOfTables;
    //ULONG NumberOfEntries[NumberOfTables];
    //ULONG CallCounts[NumberOfTables][NumberOfEntries];
} SYSTEM_CALL_COUNT_INFORMATION, *PSYSTEM_CALL_COUNT_INFORMATION;

typedef struct _SYSTEM_DEVICE_INFORMATION {
    ULONG NumberOfDisks;
    ULONG NumberOfFloppies;
    ULONG NumberOfCdRoms;
    ULONG NumberOfTapes;
    ULONG NumberOfSerialPorts;
    ULONG NumberOfParallelPorts;
} SYSTEM_DEVICE_INFORMATION, *PSYSTEM_DEVICE_INFORMATION;

typedef struct _SYSTEM_CRASH_DUMP_INFORMATION {
    HANDLE CrashDumpSection;
    HANDLE hDumpSection;
} SYSTEM_CRASH_DUMP_INFORMATION, *PSYSTEM_CRASH_DUMP_INFORMATION;

#line 1038

typedef struct _SYSTEM_CRASH_STATE_INFORMATION {
    ULONG ValidCrashDump;
    ULONG ValidDirectDump;
} SYSTEM_CRASH_STATE_INFORMATION, *PSYSTEM_CRASH_STATE_INFORMATION;

typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION {
    BOOLEAN KernelDebuggerEnabled;
    BOOLEAN KernelDebuggerNotPresent;
} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

#line 1054

typedef struct _SYSTEM_GDI_DRIVER_INFORMATION {
    UNICODE_STRING DriverName;
    PVOID ImageAddress;
    PVOID SectionPointer;
    PVOID EntryPoint;
    PIMAGE_EXPORT_DIRECTORY ExportSectionPointer;
} SYSTEM_GDI_DRIVER_INFORMATION, *PSYSTEM_GDI_DRIVER_INFORMATION;

#if DEVL
#line 1064
#error Excluded unsupported DEVL NT executive declarations
#line 1245
#endif // DEVL
#line 1246

#line 1258

typedef struct _SYSTEM_LEGACY_DRIVER_INFORMATION {
    ULONG VetoType;
    UNICODE_STRING VetoList;
} SYSTEM_LEGACY_DRIVER_INFORMATION, *PSYSTEM_LEGACY_DRIVER_INFORMATION;

// begin_winnt

#define PROCESSOR_INTEL_386     386
#define PROCESSOR_INTEL_486     486
#define PROCESSOR_INTEL_PENTIUM 586
#define PROCESSOR_MIPS_R4000    4000
#define PROCESSOR_ALPHA_21064   21064

#define PROCESSOR_ARCHITECTURE_INTEL 0
#define PROCESSOR_ARCHITECTURE_MIPS  1
#define PROCESSOR_ARCHITECTURE_ALPHA 2
#define PROCESSOR_ARCHITECTURE_PPC   3

#define PROCESSOR_ARCHITECTURE_UNKNOWN 0xFFFF

// end_winnt

#line 1290

NTSYSAPI
NTSTATUS
NTAPI
NtSetSystemInformation (
    IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
    IN PVOID SystemInformation,
    IN ULONG SystemInformationLength
    );


//
// SysDbg APIs are available to user-mode processes via
// NtSystemDebugControl.
//

typedef enum _SYSDBG_COMMAND {
    SysDbgQueryModuleInformation,
    SysDbgQueryTraceInformation,
    SysDbgSetTracepoint,
    SysDbgSetSpecialCall,
    SysDbgClearSpecialCalls,
    SysDbgQuerySpecialCalls,
    SysDbgBreakPoint
} SYSDBG_COMMAND, *PSYSDBG_COMMAND;

NTSYSAPI
NTSTATUS
NTAPI
NtSystemDebugControl (
    IN SYSDBG_COMMAND Command,
    IN PVOID InputBuffer,
    IN ULONG InputBufferLength,
    OUT PVOID OutputBuffer,
    IN ULONG OutputBufferLength,
    OUT PULONG ReturnLength
    );

typedef enum _HARDERROR_RESPONSE_OPTION {
        OptionAbortRetryIgnore,
        OptionOk,
        OptionOkCancel,
        OptionRetryCancel,
        OptionYesNo,
        OptionYesNoCancel,
        OptionShutdownSystem,
        OptionOkNoWait,
        OptionCancelTryContinue
} HARDERROR_RESPONSE_OPTION;

typedef enum _HARDERROR_RESPONSE {
        ResponseReturnToCaller,
        ResponseNotHandled,
        ResponseAbort,
        ResponseCancel,
        ResponseIgnore,
        ResponseNo,
        ResponseOk,
        ResponseRetry,
        ResponseYes,
        ResponseTryAgain,
        ResponseContinue
} HARDERROR_RESPONSE;

#define HARDERROR_PARAMETERS_FLAGSPOS   4
#define HARDERROR_FLAGS_DEFDESKTOPONLY  0x00020000

#define MAXIMUM_HARDERROR_PARAMETERS    5

#define HARDERROR_OVERRIDE_ERRORMODE    0x10000000

#line 1371

NTSYSAPI
NTSTATUS
NTAPI
NtRaiseHardError(
    IN NTSTATUS ErrorStatus,
    IN ULONG NumberOfParameters,
    IN ULONG UnicodeStringParameterMask,
    IN PULONG_PTR Parameters,
    IN ULONG ValidResponseOptions,
    OUT PULONG Response
    );

// begin_ntddk begin_nthal begin_ntifs

//
// Defined processor features
//

#define PF_FLOATING_POINT_PRECISION_ERRATA  0   // winnt
#define PF_FLOATING_POINT_EMULATED          1   // winnt
#define PF_COMPARE_EXCHANGE_DOUBLE          2   // winnt
#define PF_MMX_INSTRUCTIONS_AVAILABLE       3   // winnt
#define PF_PPC_MOVEMEM_64BIT_OK             4   // winnt
#define PF_ALPHA_BYTE_INSTRUCTIONS          5   // winnt
#define PF_XMMI_INSTRUCTIONS_AVAILABLE      6   // winnt
#define PF_3DNOW_INSTRUCTIONS_AVAILABLE     7   // winnt
#define PF_RDTSC_INSTRUCTION_AVAILABLE      8   // winnt
#define PF_PAE_ENABLED                      9   // winnt

typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE {
    StandardDesign,                 // None == 0 == standard design
    NEC98x86,                       // NEC PC98xx series on X86
    EndAlternatives                 // past end of known alternatives
} ALTERNATIVE_ARCHITECTURE_TYPE;

// correctly define these run-time definitions for non X86 machines

#ifndef _X86_

#ifndef IsNEC_98
#define IsNEC_98 (FALSE)
#endif

#ifndef IsNotNEC_98
#define IsNotNEC_98 (TRUE)
#endif

#ifndef SetNEC_98
#define SetNEC_98
#endif

#ifndef SetNotNEC_98
#define SetNotNEC_98
#endif

#endif

// end_ntddk end_nthal end_ntifs

#define PROCESSOR_FEATURE_MAX 64

//
// Define data shared between kernel and user mode.
//
// N.B. User mode has read only access to this data
//

#ifdef _MAC
#line 1440
#error Excluded unsupported Mac NT executive profile
#line 1441
#endif
#line 1442
#line 1459
#if defined(_WIN64)
#line 1460
#error Excluded unsupported 64-bit NT executive shared data profile
#line 1461
#endif
#line 1462
#line 1462

    //
    // Current 64-bit system time in 100ns units.
    //

#if defined(_WIN64)
#line 1468
#error Excluded unsupported 64-bit NT executive shared data profile
#line 1471
#else
#line 1472
#line 1473
#endif
#line 1474

    //
    // Current 64-bit time zone bias.
    //

#line 1480
#if defined(_WIN64)
#line 1481
#error Excluded unsupported 64-bit NT executive shared data profile
#line 1482
#endif
#line 1483
#line 1573
#if defined(REMOTE_BOOT)
#line 1574
#error Excluded unsupported NT executive remote boot declarations
#line 1585
#endif // defined(REMOTE_BOOT)
#line 1586
#line 1594
#define DOSDEVICE_DRIVE_UNKNOWN     0
#define DOSDEVICE_DRIVE_CALCULATE   1
#define DOSDEVICE_DRIVE_REMOVABLE   2
#define DOSDEVICE_DRIVE_FIXED       3
#define DOSDEVICE_DRIVE_REMOTE      4
#define DOSDEVICE_DRIVE_CDROM       5
#define DOSDEVICE_DRIVE_RAMDISK     6

#if defined(USER_SHARED_DATA)
#line 1603
#error Excluded unsupported NT executive shared-data tick implementation
#line 1630
#else
#line 1631

NTSYSAPI
ULONG
NTAPI
NtGetTickCount(
    VOID
    );

#endif

NTSTATUS
NTAPI
NtQueryDefaultLocale(
    IN BOOLEAN UserProfile,
    OUT PLCID DefaultLocaleId
    );

NTSYSAPI
NTSTATUS
NTAPI
NtSetDefaultLocale(
    IN BOOLEAN UserProfile,
    IN LCID DefaultLocaleId
    );

NTSYSAPI
NTSTATUS
NTAPI
NtQueryInstallUILanguage(
    OUT LANGID *InstallUILanguageId
    );

NTSYSAPI
NTSTATUS
NTAPI
NtQueryDefaultUILanguage(
    OUT LANGID *DefaultUILanguageId
    );

NTSYSAPI
NTSTATUS
NTAPI
NtSetDefaultUILanguage(
    IN LANGID DefaultUILanguageId
    );

NTSYSAPI
NTSTATUS
NTAPI
NtSetDefaultHardErrorPort(
    IN HANDLE DefaultHardErrorPort
    );

typedef enum _SHUTDOWN_ACTION {
    ShutdownNoReboot,
    ShutdownReboot,
    ShutdownPowerOff
} SHUTDOWN_ACTION;

NTSYSAPI
NTSTATUS
NTAPI
NtShutdownSystem(
    IN SHUTDOWN_ACTION Action
    );

NTSYSAPI
NTSTATUS
NTAPI
NtDisplayString(
    IN PUNICODE_STRING String
    );


//
// Global flags that can be set to control system behaviour
// Flag word is 32 bits.
//

#define FLG_STOP_ON_EXCEPTION           0x00000001      // user and kernel mode
#define FLG_SHOW_LDR_SNAPS              0x00000002      // user and kernel mode
#define FLG_DEBUG_INITIAL_COMMAND       0x00000004      // kernel mode only up until WINLOGON started
#define FLG_STOP_ON_HUNG_GUI            0x00000008      // kernel mode only while running

#define FLG_HEAP_ENABLE_TAIL_CHECK      0x00000010      // user mode only
#define FLG_HEAP_ENABLE_FREE_CHECK      0x00000020      // user mode only
#define FLG_HEAP_VALIDATE_PARAMETERS    0x00000040      // user mode only
#define FLG_HEAP_VALIDATE_ALL           0x00000080      // user mode only

#define FLG_POOL_ENABLE_TAGGING         0x00000400      // kernel mode only
#define FLG_HEAP_ENABLE_TAGGING         0x00000800      // user mode only

#define FLG_USER_STACK_TRACE_DB         0x00001000      // x86 user mode only
#define FLG_KERNEL_STACK_TRACE_DB       0x00002000      // x86 kernel mode only at boot time
#define FLG_MAINTAIN_OBJECT_TYPELIST    0x00004000      // kernel mode only at boot time
#define FLG_HEAP_ENABLE_TAG_BY_DLL      0x00008000      // user mode only

#define FLG_ENABLE_CSRDEBUG             0x00020000      // kernel mode only at boot time
#define FLG_ENABLE_KDEBUG_SYMBOL_LOAD   0x00040000      // kernel mode only
#define FLG_DISABLE_PAGE_KERNEL_STACKS  0x00080000      // kernel mode only at boot time

#define FLG_HEAP_DISABLE_COALESCING     0x00200000      // user mode only

#define FLG_ENABLE_CLOSE_EXCEPTIONS     0x00400000      // kernel mode only
#define FLG_ENABLE_EXCEPTION_LOGGING    0x00800000      // kernel mode only

#define FLG_ENABLE_HANDLE_TYPE_TAGGING  0x01000000      // kernel mode only

#define FLG_HEAP_PAGE_ALLOCS            0x02000000      // user mode only
#define FLG_DEBUG_INITIAL_COMMAND_EX    0x04000000      // kernel mode only up until WINLOGON started

#define FLG_DISABLE_DBGPRINT            0x08000000      // kernel mode only

#define FLG_CRITSEC_EVENT_CREATION      0x10000000      // user mode only, Force early creation of
                                                        // resource events for critical system processes

#define FLG_DISABLE_PROTDLLS            0x80000000      // user mode only (smss/winlogon)

#define FLG_VALID_BITS                  0x9FEEFFFF

#define FLG_USERMODE_VALID_BITS        (FLG_STOP_ON_EXCEPTION           | \
                                        FLG_SHOW_LDR_SNAPS              | \
                                        FLG_HEAP_ENABLE_TAIL_CHECK      | \
                                        FLG_HEAP_ENABLE_FREE_CHECK      | \
                                        FLG_HEAP_VALIDATE_PARAMETERS    | \
                                        FLG_HEAP_VALIDATE_ALL           | \
                                        FLG_HEAP_ENABLE_TAGGING         | \
                                        FLG_USER_STACK_TRACE_DB         | \
                                        FLG_HEAP_ENABLE_TAG_BY_DLL      | \
                                        FLG_HEAP_DISABLE_COALESCING     | \
                                        FLG_DISABLE_PROTDLLS            | \
                                        FLG_HEAP_PAGE_ALLOCS            | \
                                        FLG_CRITSEC_EVENT_CREATION)

#define FLG_BOOTONLY_VALID_BITS        (FLG_KERNEL_STACK_TRACE_DB       | \
                                        FLG_MAINTAIN_OBJECT_TYPELIST    | \
                                        FLG_ENABLE_CSRDEBUG             | \
                                        FLG_DEBUG_INITIAL_COMMAND       | \
                                        FLG_DEBUG_INITIAL_COMMAND_EX    | \
                                        FLG_DISABLE_PAGE_KERNEL_STACKS)

#define FLG_KERNELMODE_VALID_BITS      (FLG_STOP_ON_EXCEPTION           | \
                                        FLG_SHOW_LDR_SNAPS              | \
                                        FLG_STOP_ON_HUNG_GUI            | \
                                        FLG_POOL_ENABLE_TAGGING         | \
                                        FLG_ENABLE_KDEBUG_SYMBOL_LOAD   | \
                                        FLG_ENABLE_CLOSE_EXCEPTIONS     | \
                                        FLG_ENABLE_EXCEPTION_LOGGING    | \
                                        FLG_ENABLE_HANDLE_TYPE_TAGGING  | \
                                        FLG_DISABLE_DBGPRINT              \
                                       )

//
// Routines for manipulating global atoms stored in kernel space
//

typedef USHORT RTL_ATOM, *PRTL_ATOM;

NTSYSAPI
NTSTATUS
NTAPI
NtAddAtom(
    IN PWSTR AtomName,
    IN ULONG Length,
    OUT PRTL_ATOM Atom OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
NtFindAtom(
    IN PWSTR AtomName,
    IN ULONG Length,
    OUT PRTL_ATOM Atom OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
NtDeleteAtom(
    IN RTL_ATOM Atom
    );

typedef enum _ATOM_INFORMATION_CLASS {
    AtomBasicInformation,
    AtomTableInformation
} ATOM_INFORMATION_CLASS;

typedef struct _ATOM_BASIC_INFORMATION {
    USHORT UsageCount;
    USHORT Flags;
    USHORT NameLength;
    WCHAR Name[ 1 ];
} ATOM_BASIC_INFORMATION, *PATOM_BASIC_INFORMATION;

typedef struct _ATOM_TABLE_INFORMATION {
    ULONG NumberOfAtoms;
    RTL_ATOM Atoms[ 1 ];
} ATOM_TABLE_INFORMATION, *PATOM_TABLE_INFORMATION;

NTSYSAPI
NTSTATUS
NTAPI
NtQueryInformationAtom(
    IN RTL_ATOM Atom,
    IN ATOM_INFORMATION_CLASS AtomInformationClass,
    OUT PVOID AtomInformation,
    IN ULONG AtomInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

#ifdef __cplusplus
#line 1843
#error Excluded unsupported C++ NT executive declaration context
#line 1844
#endif
#line 1845

#endif // _NTEXAPI_

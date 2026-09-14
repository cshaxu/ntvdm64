/* Finite declarations for original base/win32/client/vdm.c classification.
 * Original structures/prototypes: OpenNT public/sdk/inc/{nturtl,ntmmapi}.h.
 * No classifier policy or PE parser is implemented here. */
#ifndef OPENNT_BASE_CLASSIFIER_H
#define OPENNT_BASE_CLASSIFIER_H
#include "opennt-host/public/sdk/inc/nturtl.h"
#include "opennt-host/base/win32/inc/basevdm.h"
/* Private symbol avoids colliding with Kernel32's imported GetBinaryTypeW. */
#define GetBinaryTypeW OpenNtBaseGetBinaryTypeW
/* This owner consumes an NT path; the DEM DOS-path adapter is not its edge. */
#undef NtOpenFile
#define STATUS_OBJECT_NAME_INVALID ((NTSTATUS)0xC0000033L)
#define STATUS_INVALID_IMAGE_NE_FORMAT ((NTSTATUS)0xC000011BL)
#define STATUS_INVALID_IMAGE_NOT_MZ ((NTSTATUS)0xC000012FL)
#define STATUS_INVALID_IMAGE_PROTECT ((NTSTATUS)0xC0000130L)
#define STATUS_INVALID_IMAGE_WIN_16 ((NTSTATUS)0xC0000131L)
typedef struct _RTL_RELATIVE_NAME {
    STRING RelativeName;
    HANDLE ContainingDirectory;
} RTL_RELATIVE_NAME;
BOOLEAN NTAPI OpenNtBaseDosPathNameToNtPathName_U(PCWSTR, PUNICODE_STRING, PWSTR *, RTL_RELATIVE_NAME *);
#define RtlDosPathNameToNtPathName_U OpenNtBaseDosPathNameToNtPathName_U
typedef struct _SECTION_IMAGE_INFORMATION {
    PVOID TransferAddress;
    ULONG ZeroBits, MaximumStackSize, CommittedStackSize, SubSystemType;
    union {
        struct { USHORT SubSystemMinorVersion, SubSystemMajorVersion; };
        ULONG SubSystemVersion;
    };
    ULONG GpValue;
    USHORT ImageCharacteristics, DllCharacteristics, Machine;
    BOOLEAN ImageContainsCode, Spare1;
    ULONG LoaderFlags, Reserved[2];
} SECTION_IMAGE_INFORMATION;
#define SectionImageInformation 1
NTSTATUS NTAPI NtCreateSection(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PLARGE_INTEGER, ULONG, ULONG, HANDLE);
NTSTATUS NTAPI NtQuerySection(HANDLE, ULONG, PVOID, ULONG, PULONG);
PIMAGE_NT_HEADERS NTAPI RtlImageNtHeader(PVOID);
extern UNICODE_STRING BaseDotComSuffixName, BaseDotPifSuffixName, BaseDotExeSuffixName;
ULONG BaseIsDosApplication(PUNICODE_STRING, NTSTATUS);
BOOL WINAPI OpenNtBaseGetBinaryTypeW(LPCWSTR, LPDWORD);
#endif

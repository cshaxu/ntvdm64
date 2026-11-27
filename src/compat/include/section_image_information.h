/* XP SP1 public/sdk/inc/ntmmapi.h:185; shared native image declaration. */
#ifndef COMPAT_SECTION_IMAGE_INFORMATION_H
#define COMPAT_SECTION_IMAGE_INFORMATION_H
typedef struct _SECTION_IMAGE_INFORMATION {
    PVOID TransferAddress;
    ULONG ZeroBits;
    SIZE_T MaximumStackSize;
    SIZE_T CommittedStackSize;
    ULONG SubSystemType;
    union {
        struct {
            USHORT SubSystemMinorVersion;
            USHORT SubSystemMajorVersion;
        };
        ULONG SubSystemVersion;
    };
    ULONG GpValue;
    USHORT ImageCharacteristics;
    USHORT DllCharacteristics;
    USHORT Machine;
    BOOLEAN ImageContainsCode;
    BOOLEAN Spare1;
    ULONG LoaderFlags;
    ULONG Reserved[ 2 ];
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;
#endif

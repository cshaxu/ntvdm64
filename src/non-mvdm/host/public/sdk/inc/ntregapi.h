/* Copyright Microsoft Corporation. Original ntregapi.h true subset.
 * DIVERGENCE(OPENNT-HOST-056): only the native value-query ABI reached by
 * the admitted USER compatibility reader, no registry service implementation. */
#ifndef WOW_PROFILE_NTREGAPI_H
#define WOW_PROFILE_NTREGAPI_H
typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;
typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64
} KEY_VALUE_INFORMATION_CLASS;
#endif

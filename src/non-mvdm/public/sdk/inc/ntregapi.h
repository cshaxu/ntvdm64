/* Copyright Microsoft Corporation. Original ntregapi.h true subset.
 * DIVERGENCE(OPENNT-HOST-056): only the native value-query ABI reached by
 * the admitted USER compatibility reader, no registry service implementation. */
#line 22
#ifndef _NTREGAPI_
#define _NTREGAPI_

#line 25
#if _MSC_VER > 1000
#pragma once
#endif

#line 29
#ifdef __cplusplus
#line 30
#error Excluded unsupported C++ NT registry declaration context
#endif
#line 233
typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;
#line 253
typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64
} KEY_VALUE_INFORMATION_CLASS;
#line 520
#ifdef __cplusplus
#line 521
#error Excluded unsupported C++ NT registry declaration context
#endif
#line 524
#endif // _NTREGAPI_

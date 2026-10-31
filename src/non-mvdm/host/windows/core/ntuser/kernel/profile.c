/****************************** Module Header ******************************\
* Module Name: profile.c
*
* Copyright (c) 1985-1996, Microsoft Corporation
*
* This module contains code to emulate ini file mapping.
*
* History:
* 30-Nov-1993 SanfordS  Created.
\***************************************************************************/
/* DIVERGENCE(OPENNT-HOST-055): exact string/integer reader subset.
 * ADAPTER-WOW-042 binds PMAP_COMPAT and EDIT's PMAP_DESKTOP read-only access. */
#define WOW_ORIGINAL_PROFILE
#include "wow_task_profile_bindings.h"

UINT FastGetProfileIntW(
    UINT    idSection,
    LPCWSTR lpKeyName,
    UINT    nDefault)
{
    WCHAR          ValueBuf[40];
    UNICODE_STRING Value;
    UINT           ReturnValue;

    UserAssert(idSection <= PMAP_LAST);

    Value.Length        = 0;
    Value.MaximumLength = sizeof(ValueBuf);
    Value.Buffer        = ValueBuf;

    RtlIntegerToUnicodeString(nDefault, 10, &Value);
    if (!FastGetProfileStringW(idSection,
                               lpKeyName,
                               (LPWSTR)Value.Buffer,
                               Value.Buffer,
                               sizeof(ValueBuf) / sizeof(WCHAR))) {
        return nDefault;
    }

    /*
     * Convert string to int.
     */
    Value.Length = wcslen(Value.Buffer) * sizeof(WCHAR);
    RtlUnicodeStringToInteger(&Value, 10, &ReturnValue);

    return ReturnValue;
}

DWORD FastGetProfileStringW(
    UINT    idSection,
    LPCWSTR lpKeyName,
    LPCWSTR lpDefault,
    LPWSTR  lpReturnedString,
    DWORD   cchBuf)
{
    HANDLE                         hKey;
    DWORD                          cbSize;
    LONG                           Status;
    UNICODE_STRING                 UnicodeString;
    PKEY_VALUE_PARTIAL_INFORMATION pKeyInfo;


    UserAssert(idSection <= PMAP_LAST);
    UserAssert(lpKeyName != NULL);

    if ((hKey = OpenCacheKeyEx(idSection, KEY_READ)) == NULL) {
        RIPMSG1(RIP_WARNING, "FastGetProfileStringW: Failed to open cache-key (%ws)", lpKeyName);
        goto DefExit;
    }

    cbSize = (cchBuf * sizeof(WCHAR)) +
            offsetof(KEY_VALUE_PARTIAL_INFORMATION, Data);

    if ((pKeyInfo = UserAllocPoolWithQuota(cbSize, TAG_PROFILE)) == NULL)
        goto DefExit;

    RtlInitUnicodeString(&UnicodeString, lpKeyName);
    Status = ZwQueryValueKey(hKey,
                             &UnicodeString,
                             KeyValuePartialInformation,
                             pKeyInfo,
                             cbSize,
                             &cbSize);

    if (Status == STATUS_BUFFER_OVERFLOW) {
        RIPMSG0(RIP_WARNING, "FastGetProfileStringW: Buffer overflow");
        Status = STATUS_SUCCESS;
    }

    UserAssert(NT_SUCCESS(Status) || (Status == STATUS_OBJECT_NAME_NOT_FOUND));

    if (NT_SUCCESS(Status)) {

        if (pKeyInfo->DataLength >= sizeof(WCHAR)) {

            ((LPWSTR)(pKeyInfo->Data))[cchBuf - 1] = L'\0';
            wcscpy(lpReturnedString, (LPWSTR)pKeyInfo->Data);

        } else {
            /*
             * Appears to be a bug with empty strings - only first
             * byte is set to NULL. (SAS)
             */
            lpReturnedString[0] = TEXT('\0');
        }

        cchBuf = pKeyInfo->DataLength;

        UserFreePool(pKeyInfo);

        ZwClose(hKey);

        /*
         * data length includes terminating zero [bodind]
         */
        return (cchBuf / sizeof(WCHAR));
    }

    UserFreePool(pKeyInfo);

DefExit:

    /*
     * Make sure the key is closed.
     */
    if (hKey)
        ZwClose(hKey);

    /*
     * wcscopy copies terminating zero, but the length returned by
     * wcslen does not, so add 1 to be consistent with success
     * return [bodind]
     */
    if (lpDefault != NULL) {
        wcscpy(lpReturnedString, lpDefault);
        return (wcslen(lpDefault) + 1);
    }

    return 0;
}



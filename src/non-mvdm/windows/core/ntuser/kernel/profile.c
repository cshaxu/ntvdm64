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
#define WOW_ORIGINAL_PROFILE
#line 11
#include "non-mvdm/public/sdk/inc/nt.h"
#include "wow_task_profile_bindings.h"

/* Preserve the task adapter's checked buffer contract at the original owner. */
UINT WINAPI wow_original_task_profile(UINT section, LPCWSTR key, LPCWSTR fallback,
    LPWSTR output, DWORD capacity)
{
    if (section != 14 || !key || !output || !capacity ||
        capacity > (MAXDWORD - sizeof(KEY_VALUE_PARTIAL_INFORMATION)) / sizeof(WCHAR) ||
        (fallback && wcslen(fallback) >= capacity)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    return FastGetProfileStringW(section, key, fallback, output, capacity);
}
#pragma hdrstop

#line 68
#ifdef FE_SB // PMAP_WINLOGON
#error
#endif // FE_SB
#line 235
/*****************************************************************************\
* FastGetProfileDwordW
*
* Reads a REG_DWORD type key from the registry.
*
* returns value read or default value on failure.
*
* History:
* 02-Dec-1993 SanfordS  Created.
\*****************************************************************************/
DWORD FastGetProfileDwordW(
    UINT    idSection,
    LPCWSTR lpKeyName,
    DWORD   dwDefault)
{
    HANDLE         hKey;
    DWORD          cbSize;
    DWORD          dwRet;
    LONG           Status;
    UNICODE_STRING UnicodeString;
    BYTE           Buf[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD)];

    UserAssert(idSection <= PMAP_LAST);

    if ((hKey = OpenCacheKeyEx(idSection, KEY_READ)) == NULL) {
        RIPMSG1(RIP_WARNING, "FastGetProfileDwordW: Failed to open cache-key (%ws)", lpKeyName);
        return dwDefault;
    }

    RtlInitUnicodeString(&UnicodeString, lpKeyName);
    Status = ZwQueryValueKey(hKey,
                             &UnicodeString,
                             KeyValuePartialInformation,
                             (PKEY_VALUE_PARTIAL_INFORMATION)Buf,
                             sizeof(Buf),
                             &cbSize);

    dwRet = dwDefault;

    if (NT_SUCCESS(Status)) {

        dwRet = *((PDWORD)((PKEY_VALUE_PARTIAL_INFORMATION)Buf)->Data);

    } else if (Status != STATUS_OBJECT_NAME_NOT_FOUND) {

        RIPMSG1(RIP_WARNING,
                "FastGetProfileDwordW: ObjectName not found: %ws",
                lpKeyName);
    }

    ZwClose(hKey);

    return dwRet;
}

#line 431
/*****************************************************************************\
* FastGetProfileStringW()
*
* Implements a fast version of the standard API using predefined registry
* section indecies (PMAP_) that reference lazy-opened, cached registry
* handles.  FastCloseProfileUserMapping() should be called to clean up
* cached entries when fast profile calls are completed.
*
* This api does NOT implement the NULL lpKeyName feature of the real API.
*
* History:
* 02-Dec-1993 SanfordS  Created.
\*****************************************************************************/
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

/*****************************************************************************\
* FastGetProfileIntW()
*
* Implements a fast version of the standard API using predefined registry
* section indecies (PMAP_) that reference lazy-opened, cached registry
* handles.  FastCloseProfileUserMapping() should be called to clean up
* cached entries when fast profile calls are completed.
*
* History:
* 02-Dec-1993 SanfordS  Created.
\*****************************************************************************/
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
#line 745
#ifdef DEBUG
#error
#line 750
#endif
#line 827
#ifdef DEBUG
#error
#line 832
#endif

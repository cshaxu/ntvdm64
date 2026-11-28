/****************************** Module Header ******************************\
* Module Name: profile.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* This module contains code to emulate ini file mapping.
*
* History:
* 30-Nov-1993 SanfordS  Created.
\***************************************************************************/
#define WOW_ORIGINAL_PROFILE
#include "non-mvdm/public/sdk/inc/nt.h"
#include "wow_task_profile_bindings.h"
#line 12
#pragma hdrstop

/***************************************************************************\
* aFastRegMap[]
*
* This array maps section ids (PMAP_) to cached registry keys and section
* addresses within the registry.  IF INI FILE MAPPING CHANGES ARE MADE,
* THIS TABLE MUST BE UPDATED.
*
* The first character of the szSection field indicates what root the
* section is in. (or locked open status)
*      M = LocalMachine
*      U = CurrentUser
*      L = Locked open - used only on M mappings.
*
* History:
\***************************************************************************/
#define PROOT_CPANEL     0
#define PROOT_ACCESS     1
#define PROOT_CURRENTM   2
#define PROOT_CURRENTU   3
#define PROOT_CONTROL    4
#define PROOT_SERVICES   5
#define PROOT_KEYBOARD   6
#define PROOT_SYSTEM     7

typedef struct tagFASTREGMAP {
    UINT idRoot;
    PCWSTR szSection;
} FASTREGMAP, *PFASTREGMAP;

CONST PCWSTR aFastRegRoot[] = {
    L"UControl Panel\\",                                    // PROOT_CPANEL
    L"UControl Panel\\Accessibility\\",                     // PROOT_ACCESS
    L"MSoftware\\Microsoft\\Windows NT\\CurrentVersion\\",  // PROOT_CURRENTM
    L"USoftware\\Microsoft\\Windows NT\\CurrentVersion\\",  // PROOT_CURRENTU
    L"MSystem\\CurrentControlSet\\Control\\",               // PROOT_CONTROL
    L"MSystem\\CurrentControlSet\\Services\\",              // PROOT_SERVICES
    L"UKeyboard Layout\\",                                  // PROOT_KEYBOARD
    L"MSystem\\",                                           // PROOT_SYSTEM   
};

CONST FASTREGMAP aFastRegMap[PMAP_LAST + 1] = {
    { PROOT_CPANEL,   L"Colors" },                            // PMAP_COLORS
    { PROOT_CPANEL,   L"Cursors" },                           // PMAP_CURSORS
    { PROOT_CURRENTM, L"Windows" },                           // PMAP_WINDOWSM
    { PROOT_CURRENTU, L"Windows" },                           // PMAP_WINDOWSU
    { PROOT_CPANEL,   L"Desktop" },                           // PMAP_DESKTOP
    { PROOT_CPANEL,   L"Icons" },                             // PMAP_ICONS
    { PROOT_CURRENTM, L"Fonts" },                             // PMAP_FONTS
    { PROOT_CURRENTU, L"TrueType" },                          // PMAP_TRUETYPE
    { PROOT_CONTROL,  L"Keyboard Layout" },                   // PMAP_KBDLAYOUT
    { PROOT_SERVICES, L"RIT" },                               // PMAP_INPUT
    { PROOT_CURRENTM, L"Compatibility" },                     // PMAP_COMPAT
    { PROOT_CONTROL,  L"Session Manager\\SubSystems" },       // PMAP_SUBSYSTEMS
    { PROOT_CPANEL,   L"Sound" },                             // PMAP_BEEP
    { PROOT_CPANEL,   L"Mouse" },                             // PMAP_MOUSE
    { PROOT_CPANEL,   L"Keyboard" },                          // PMAP_KEYBOARD
    { PROOT_ACCESS,   L"StickyKeys" },                        // PMAP_STICKYKEYS
    { PROOT_ACCESS,   L"Keyboard Response" },                 // PMAP_KEYBOARDRESPONSE
    { PROOT_ACCESS,   L"MouseKeys" },                         // PMAP_MOUSEKEYS
    { PROOT_ACCESS,   L"ToggleKeys" },                        // PMAP_TOGGLEKEYS
    { PROOT_ACCESS,   L"TimeOut" },                           // PMAP_TIMEOUT
    { PROOT_ACCESS,   L"SoundSentry" },                       // PMAP_SOUNDSENTRY
    { PROOT_ACCESS,   L"ShowSounds" },                        // PMAP_SHOWSOUNDS
    { PROOT_CURRENTM, L"AeDebug" },                           // PMAP_AEDEBUG
    { PROOT_CONTROL,  L"NetworkProvider" },                   // PMAP_NETWORK
    { PROOT_CPANEL,   L"Desktop\\WindowMetrics" },            // PMAP_METRICS
    { PROOT_KEYBOARD, L"" },                                  // PMAP_UKBDLAYOUT
    { PROOT_KEYBOARD, L"Toggle" },                            // PMAP_UKBDLAYOUTTOGGLE
    { PROOT_CURRENTM, L"Winlogon" },                          // PMAP_WINLOGON
    { PROOT_ACCESS,   L"Keyboard Preference" },               // PMAP_KEYBOARDPREF
    { PROOT_ACCESS,   L"Blind Access" },                      // PMAP_SCREENREADER
    { PROOT_ACCESS,   L"HighContrast" },                      // PMAP_HIGHCONTRAST
    { PROOT_CURRENTM, L"IME Compatibility" },                 // PMAP_IMECOMPAT
    { PROOT_CURRENTM, L"IMM" },                               // PMAP_IMM
    { PROOT_CONTROL,  L"Session Manager\\SubSystems\\Pool" }, // PMAP_POOLLIMITS
    { PROOT_CURRENTM, L"Compatibility32" },                   // PMAP_COMPAT32
    { PROOT_CURRENTM, L"WOW\\SetupPrograms" },                // PMAP_SETUPPROGRAMNAMES
    { PROOT_CPANEL,   L"Input Method" },                      // PMAP_INPUTMETHOD
    { PROOT_CURRENTM, L"Compatibility2" },                    // PMAP_COMPAT2
    { PROOT_SERVICES, L"Mouclass\\Parameters" },              // PMAP_MOUCLASS_PARAMS
    { PROOT_SERVICES, L"Kbdclass\\Parameters" },              // PMAP_KBDCLASS_PARAMS
    { PROOT_CONTROL,  L"ComputerName\\ComputerName" },        // PMAP_COMPUTERNAME
    { PROOT_CONTROL,  L"Terminal Server" },                   // PMAP_TS
    { PROOT_SYSTEM,   L"WPA\\TabletPC" },                     // PMAP_TABLETPC
    { PROOT_SYSTEM,   L"WPA\\MediaCenter" },                  // PMAP_MEDIACENTER
};

WCHAR PreviousUserStringBuf[256];
UNICODE_STRING PreviousUserString = {0, sizeof PreviousUserStringBuf, PreviousUserStringBuf};
LUID luidPrevious;

CONST WCHAR wszDefaultUser[] = L"\\Registry\\User\\.Default";
UNICODE_STRING DefaultUserString = {sizeof wszDefaultUser - sizeof(WCHAR), sizeof wszDefaultUser, (WCHAR *)wszDefaultUser};

#line 529


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
BOOL FastGetProfileDwordW(PUNICODE_STRING pProfileUserName OPTIONAL,
    UINT    idSection,
    LPCWSTR lpKeyName,
    DWORD   dwDefault,
    PDWORD  pdwReturn,
    DWORD   dwPolicyOnly
    )
{
    HANDLE         hKey;
    DWORD          cbSize;
    DWORD          dwRet;
    LONG           Status;
    UNICODE_STRING UnicodeString;
    BYTE           Buf[sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(DWORD)];
    DWORD          dwPolicyFlags = gdwPolicyFlags | dwPolicyOnly;

    UserAssert(idSection <= PMAP_LAST);

TryAgain:

    if ((hKey = OpenCacheKeyEx(pProfileUserName,
                               idSection,
                               KEY_READ,
                               &dwPolicyFlags)) == NULL) {
        RIPMSG1(RIP_WARNING | RIP_NONAME, "%ws", lpKeyName);

        if (dwPolicyOnly & POLICY_ONLY) {
            return FALSE;
        }

        if (pdwReturn) {
            *pdwReturn = dwDefault;
        }
        return TRUE;
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

    } else if (dwPolicyFlags) {

        ZwClose(hKey);
        goto TryAgain;

    } else if (Status != STATUS_OBJECT_NAME_NOT_FOUND) {

        RIPMSG1(RIP_WARNING,
                "FastGetProfileDwordW: ObjectName not found: %ws",
                lpKeyName);
    }

    ZwClose(hKey);

    if (pdwReturn) {
        *pdwReturn = dwRet;
    }
    return TRUE;
}

/*****************************************************************************\
* FastGetProfileKeysW()
*
* Reads all key names in the given section.
*
* History:
* 15-Dec-1994 JimA      Created.
\*****************************************************************************/
DWORD FastGetProfileKeysW(PUNICODE_STRING pProfileUserName OPTIONAL,
    UINT    idSection,
    LPCWSTR lpDefault,
    LPWSTR  *lpReturnedString
    )
{
    HANDLE                       hKey;
    DWORD                        cchSize;
    DWORD                        cchKey;
    LONG                         Status;
    WCHAR                        Buffer[256 + 6];
    PKEY_VALUE_BASIC_INFORMATION pKeyInfo;
    ULONG                        iValue;
    LPWSTR                       lpTmp;
    LPWSTR                       lpKeys = NULL;
    DWORD                        dwPoolSize;

    UserAssert(idSection <= PMAP_LAST);

    if ((hKey = OpenCacheKeyEx(pProfileUserName,
                               idSection,
                               KEY_READ,
                               NULL)) == NULL) {
        RIPMSG0(RIP_WARNING | RIP_NONAME, "");
        goto DefExit;
    }

    pKeyInfo          = (PKEY_VALUE_BASIC_INFORMATION)Buffer;
    cchSize           = 0;
    *lpReturnedString = NULL;
    iValue            = 0;

    while (TRUE) {

#if DBG
#error
#endif
#line 656
        Status = ZwEnumerateValueKey(hKey,
                                     iValue,
                                     KeyValueBasicInformation,
                                     pKeyInfo,
                                     sizeof(Buffer),
                                     &cchKey);

        UserAssert(_wcsicmp(Buffer + 256, L"DON'T") == 0);

        if (Status == STATUS_NO_MORE_ENTRIES) {

            break;

        } else if (!NT_SUCCESS(Status)) {

            if (lpKeys) {
                UserFreePool(lpKeys);
                lpKeys = NULL;
            }
            goto DefExit;
        }

        UserAssert(pKeyInfo->NameLength * sizeof(WCHAR) <=
                   sizeof(Buffer) - sizeof(KEY_VALUE_BASIC_INFORMATION));

        UserAssert(cchKey <= sizeof(Buffer));

        /*
         * A key was found.  Allocate space for it.  Note that
         * NameLength is in bytes.
         */
        cchKey   = cchSize;
        cchSize += pKeyInfo->NameLength + sizeof(WCHAR);

        if (lpKeys == NULL) {

            dwPoolSize = cchSize + sizeof(WCHAR);
            lpKeys = UserAllocPoolWithQuota(dwPoolSize, TAG_PROFILE);

        } else {

            lpTmp = lpKeys;
            lpKeys = UserReAllocPoolWithQuota(lpTmp,
                                              dwPoolSize,
                                              cchSize + sizeof(WCHAR),
                                              TAG_PROFILE);

            /*
             * Free the original buffer if the allocation fails
             */
            if (lpKeys == NULL) {
                UserFreePool(lpTmp);
            }
            dwPoolSize = cchSize + sizeof(WCHAR);
        }

        /*
         * Check for out of memory.
         */
        if (lpKeys == NULL)
            goto DefExit;

        /*
         * NULL terminate the string and append it to
         * the key list.
         */
        UserAssert(pKeyInfo->NameLength < sizeof(Buffer) - sizeof(KEY_VALUE_BASIC_INFORMATION));

        RtlCopyMemory(&lpKeys[cchKey / sizeof(WCHAR)], pKeyInfo->Name, pKeyInfo->NameLength);
        lpKeys[(cchKey + pKeyInfo->NameLength) / sizeof(WCHAR)] = 0;

        iValue++;
    }

    /*
     * If no keys were found, return the default.
     */
    if (iValue == 0) {

DefExit:

        cchSize = wcslen(lpDefault)+1;
        lpKeys  = UserAllocPoolWithQuota((cchSize+1) * sizeof(WCHAR), TAG_PROFILE);

        if (lpKeys)
            wcscpy(lpKeys, lpDefault);
        else
            cchSize = 0;

    } else {

        /*
         * Turn the byte count into a char count.
         */
        cchSize /= sizeof(WCHAR);
    }

    /*
     * Make sure hKey is closed.
     */
    if (hKey)
        ZwClose(hKey);

    /*
     * Append the ending NULL.
     */
    if (lpKeys)
        lpKeys[cchSize] = 0;

    *lpReturnedString = lpKeys;

    return cchSize;
}

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
DWORD FastGetProfileStringW(PUNICODE_STRING pProfileUserName OPTIONAL,
    UINT    idSection,
    LPCWSTR lpKeyName,
    LPCWSTR lpDefault,
    LPWSTR  lpReturnedString,
    DWORD   cchBuf,
    DWORD   dwPolicyOnly
    )
{
    HANDLE                         hKey = NULL;
    DWORD                          cbSize;
    LONG                           Status;
    UNICODE_STRING                 UnicodeString;
    PKEY_VALUE_PARTIAL_INFORMATION pKeyInfo;
    DWORD                          dwPolicyFlags = gdwPolicyFlags | dwPolicyOnly;


    UserAssert(idSection <= PMAP_LAST);
    UserAssert(lpKeyName != NULL);

TryAgain:

    if ((hKey = OpenCacheKeyEx(pProfileUserName,
                               idSection,
                               KEY_READ,
                               &dwPolicyFlags)) == NULL) {
#if DBG
#error
#line 813
#endif
#line 814
        if (dwPolicyOnly & POLICY_ONLY) {
            return 0;
        }
        goto DefExit;
    }

    cbSize = (cchBuf * sizeof(WCHAR)) +
            FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data);

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

    } else if (dwPolicyFlags) {
        UserFreePool(pKeyInfo);
        ZwClose(hKey);
        goto TryAgain;
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
        cchBuf = wcslen(lpDefault) + 1;
        RtlCopyMemory(lpReturnedString, lpDefault, cchBuf * sizeof(WCHAR));
        return cchBuf;
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
BOOL FastGetProfileIntW(PUNICODE_STRING pProfileUserName OPTIONAL,
    UINT    idSection,
    LPCWSTR lpKeyName,
    UINT    nDefault,
    PUINT   puiReturn,
    DWORD   dwPolicyOnly
    )
{
    WCHAR          ValueBuf[40];
    UNICODE_STRING Value;
    UINT           ReturnValue;

    UserAssert(idSection <= PMAP_LAST);
    UserAssert(puiReturn);

    if (!FastGetProfileStringW(pProfileUserName,
                               idSection,
                               lpKeyName,
                               NULL,
                               ValueBuf,
                               sizeof(ValueBuf) / sizeof(WCHAR),
                               dwPolicyOnly
                               )) {
        if (dwPolicyOnly & POLICY_ONLY) {
            return FALSE;
        }

        *puiReturn = nDefault; 
        return TRUE;
    }

    /*
     * Convert string to int.
     */
    RtlInitUnicodeString(&Value, ValueBuf);
    RtlUnicodeStringToInteger(&Value, 10, &ReturnValue);

    *puiReturn = ReturnValue; 
    return TRUE;
}

/*****************************************************************************\
* FastWriteProfileStringW
*
* Implements a fast version of the standard API using predefined registry
* section indecies (PMAP_) that reference lazy-opened, cached registry
* handles.  FastCloseProfileUserMapping() should be called to clean up
* cached entries when fast profile calls are completed.
*
* History:
* 02-Dec-1993 SanfordS  Created.
\*****************************************************************************/
BOOL FastWriteProfileStringW(PUNICODE_STRING pProfileUserName OPTIONAL,
    UINT    idSection,
    LPCWSTR lpKeyName,
    LPCWSTR lpString
    )
{
    HANDLE         hKey;
    LONG           Status;
    UNICODE_STRING UnicodeString;

    UserAssert(idSection <= PMAP_LAST);

    /*
     * We shouldn't be writing values that are controlled by policy.
     */
    if (idSection == PMAP_DESKTOP) {
        UserAssert(!CheckDesktopPolicy(pProfileUserName, lpKeyName));
    }

    if ((hKey = OpenCacheKeyEx(pProfileUserName,
                               idSection,
                               KEY_WRITE,
                               NULL)) == NULL) {
        RIPMSG1(RIP_WARNING | RIP_NONAME, "%ws", lpKeyName);
        return FALSE;
    }

    RtlInitUnicodeString(&UnicodeString, lpKeyName);
    Status = ZwSetValueKey(hKey,
                           &UnicodeString,
                           0,
                           REG_SZ,
                           (PVOID)lpString,
                           (wcslen(lpString) + 1) * sizeof(WCHAR));

    ZwClose(hKey);

    return (NT_SUCCESS(Status));
}


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
    return FastGetProfileStringW(NULL, section, key, fallback, output, capacity, 0);
}

UINT WINAPI wow_original_profile_int(UINT section, LPCWSTR key, UINT fallback)
{
    UINT value = fallback;
    FastGetProfileIntW(NULL, section, key, fallback, &value, 0);
    return value;
}

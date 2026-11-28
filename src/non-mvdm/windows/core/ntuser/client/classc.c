/****************************** Module Header ******************************\
* Module Name: classc.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* This module contains
*
* History:
* 15-Dec-1993 JohnC      Pulled functions from user\server.
\***************************************************************************/

#line 344
/***************************************************************************\
* ClassNameToVersion
*
* Map class name to class name+version.
* lpClassName   : Class name to be mapped, it may be ANSI, Unicode or an Atom.
* pClassVerName : Buffer to receive the class name+version.
* lpDllName     : if it is not NULL it will point to the DLL owns this class 
                  name.
* bIsANSI       : True of lpClassName is ANSI, FALSE if it is Unicode.
*
* Return: If it succeed it returns lpClassName or lpClassName.
*         if it failed it returns NULL.
*
* History:
* 08-01-00 MHamid      Wrote.
\***************************************************************************/
LPWSTR ClassNameToVersion (LPCWSTR lpClassName, LPWSTR pClassVerName, LPWSTR *lpDllName, BOOL bIsANSI)
{
    int cbSrc;
    int cbDst;
    UNICODE_STRING UnicodeClassName;
    ACTIVATION_CONTEXT_SECTION_KEYED_DATA acskd;
    ACTIVATION_CONTEXT_DATA_WINDOW_CLASS_REDIRECTION UNALIGNED * pRedirEntry;
    LPWSTR lpClassNameRet; 
    LPWSTR pwstr;
    ULONG strLength;
    LPWSTR Buffer;
    NTSTATUS Status;
    
    /*
     * Allocate local buffer.
     */
    Buffer = UserLocalAlloc(0, MAX_ATOM_LEN * sizeof(WCHAR));
    if (Buffer == NULL) {
        return NULL;
    }

    /*
     * Capture lpClassName into a local buffer.
     */
    if (IS_PTR(lpClassName)) {
        /*
         * lpClassName is string.
         */
        if (bIsANSI) {
            /*
             * it is ANSI then convert it to unicode.
             */
            cbSrc = strlen((LPSTR)lpClassName) + 1;
            RtlMultiByteToUnicodeN(Buffer,
                    MAX_ATOM_LEN * sizeof(WCHAR), &cbDst,
                    (LPSTR)lpClassName, cbSrc);
        } else {
            /*
             * It is already unicode, then just copy it.
             */
            cbSrc = min (wcslen(lpClassName) + 1, MAX_ATOM_LEN);
            cbSrc *= sizeof(WCHAR);
            RtlCopyMemory(Buffer, lpClassName, cbSrc);
        }
        /*
         * Build the UNICODE_STRING
         */
        RtlInitUnicodeString(&UnicodeClassName, Buffer);
    } else {
        /*
         * lpClassName is an atom, get its name and build the UNICODE_STRING
         */
        UnicodeClassName.MaximumLength = (USHORT)(MAX_ATOM_LEN * sizeof(WCHAR));
        UnicodeClassName.Buffer = Buffer;
        UnicodeClassName.Length = (USHORT)NtUserGetAtomName((ATOM)lpClassName, &UnicodeClassName) * sizeof(WCHAR);

        if (!UnicodeClassName.Length) {
            lpClassNameRet = NULL;
            goto Free_Buffer;
        }
    }

    /*
     * Call Fusion to map the class name.
     */
    RtlZeroMemory(&acskd, sizeof(acskd));
    acskd.Size = sizeof(acskd);

    Status = RtlFindActivationContextSectionString(
        0,
        NULL,
        ACTIVATION_CONTEXT_SECTION_WINDOW_CLASS_REDIRECTION,
        &UnicodeClassName,
        &acskd);
    /*
     * If there is no Activation Section we will use the plain class name.
     */
    if ((Status == STATUS_SXS_SECTION_NOT_FOUND) ||
        (Status == STATUS_SXS_KEY_NOT_FOUND)) {
        lpClassNameRet = (LPWSTR)lpClassName;
        goto Free_Buffer;
    }

    /*
     * Case of failure return NULL.
     */
    if (!NT_SUCCESS(Status) || 
        acskd.DataFormatVersion != ACTIVATION_CONTEXT_DATA_WINDOW_CLASS_REDIRECTION_FORMAT_WHISTLER) {

        lpClassNameRet = NULL;
        goto Free_Buffer;
    }

    pRedirEntry = (PACTIVATION_CONTEXT_DATA_WINDOW_CLASS_REDIRECTION) acskd.Data;

    UserAssert(pRedirEntry);

    pwstr = (LPWSTR)(((ULONG_PTR) pRedirEntry) + pRedirEntry->VersionSpecificClassNameOffset);
    strLength = pRedirEntry->VersionSpecificClassNameLength + sizeof(WCHAR);
    if (lpDllName) {
        *lpDllName = (LPWSTR)(((ULONG_PTR) acskd.SectionBase) + pRedirEntry->DllNameOffset);
    }

    UserAssert(pwstr);
    UserAssert(strLength <= MAX_ATOM_LEN * sizeof(WCHAR));
    /*
     * if the call is ANSI then convert the class name+version to ANSI string.
     */
    if (bIsANSI) {
        RtlUnicodeToMultiByteN((LPSTR)pClassVerName,
                MAX_ATOM_LEN, &cbDst,
                pwstr, strLength);
    } else {
        /*
         * if it is unicode then just copy the class name+version to the caller's buffer.
         */
        RtlCopyMemory(pClassVerName, pwstr, strLength);
    }
    /*
     * And return it.
     */
    lpClassNameRet = pClassVerName;

Free_Buffer: 
    /*
     * Don't forget to free the local memory.
     */
    UserLocalFree(Buffer);
    return lpClassNameRet;
}



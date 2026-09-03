/*++
Copyright (c) 1990  Microsoft Corporation
Module Name: vdm.c
Abstract: Selected Base-client VDM environment routines.
--*/

/* DIVERGENCE(OPENNT-HOST-014): true source subset of original
 * base/win32/client/vdm.c. Only these three routines are selected; the rest
 * needs the excluded Base/CSR process-creation shell. Names, parameters,
 * algorithm, call order and failure directions are retained. */
#include <nt.h>
#include <ntrtl.h>
#include <wchar.h>

#define ENV_NAME_PATH L"PATH"
#define ENV_NAME_WINDIR L"WINDIR"
#define ENV_NAME_SYSTEMROOT L"SYSTEMROOT"
#define ENV_NAME_TYPE_NO_PATH 1
#define ENV_NAME_TYPE_SINGLE_PATH 2
#define ENV_NAME_TYPE_MULTIPLE_PATH 3
#define STD_ENV_NAME_COUNT 3
typedef struct _ENV_INFO { UINT NameType; UINT NameLength; WCHAR *Name; } ENV_INFO, *PENV_INFO;
/* Heap tags are source metadata; modern public HeapAlloc has no tag input. */
#define VDM_TAG 0
#define MAKE_TAG(Tag) 0

static VOID BaseSetLastNTError(NTSTATUS Status)
{
    SetLastError(RtlNtStatusToDosError(Status));
}

UINT BaseGetEnvNameType_U(WCHAR *Name, DWORD NameLength);

BOOL BaseCreateVDMEnvironment(
    PWCHAR lpEnvironment,
    ANSI_STRING * pAStringEnv,
    UNICODE_STRING *pUStringEnv
    )
{
    WCHAR *pEnv, *pDst, *EnvStrings, *pTmp, *pNewEnv;
    DWORD cchEnv, dw, Length, dwRemain;
    NTSTATUS Status;
    UINT NameType;
    BOOL bRet = FALSE;

    if (!ARGUMENT_PRESENT(pAStringEnv) || !ARGUMENT_PRESENT(pUStringEnv)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    try {
        if (lpEnvironment == NULL) {
            Status = RtlCreateEnvironment(TRUE, (PVOID *)&EnvStrings);
            if (!NT_SUCCESS(Status)) goto bveTryExit;
        } else EnvStrings = lpEnvironment;
        if (EnvStrings == NULL) {
            SetLastError(ERROR_BAD_ENVIRONMENT);
            goto bveTryExit;
        }
        cchEnv = 0;
        pEnv = EnvStrings;
        while (!(*pEnv++ == UNICODE_NULL && *pEnv == UNICODE_NULL)) cchEnv++;
        cchEnv += 2;
        pNewEnv = (LPWSTR)RtlAllocateHeap(RtlProcessHeap(), MAKE_TAG(VDM_TAG),
            (cchEnv + MAX_PATH) * sizeof(WCHAR));
        if (pNewEnv == NULL) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto bveTryExit;
        }
        dwRemain = MAX_PATH - 2;
        pEnv = EnvStrings;
        pDst = pNewEnv;
        while (*pEnv != UNICODE_NULL) {
            pTmp = pEnv;
            if (pEnv[0] == (WCHAR)'=') {
                if (((pEnv[1] >= (WCHAR)'A' && pEnv[1] <= (WCHAR)'Z') ||
                     (pEnv[1] >= (WCHAR)'a' && pEnv[1] <= (WCHAR)'z')) &&
                    pEnv[2] == (WCHAR)':' && pEnv[3] == (WCHAR)'=') {
                    *pDst++ = *pEnv++; *pDst++ = *pEnv++;
                    *pDst++ = *pEnv++; *pDst++ = *pEnv++;
                    NameType = ENV_NAME_TYPE_SINGLE_PATH;
                } else NameType = ENV_NAME_TYPE_NO_PATH;
            } else {
                while (*pEnv && (*pDst++ = *pEnv++) != (WCHAR)'=') ;
                NameType = BaseGetEnvNameType_U(pTmp, (DWORD)(pEnv - pTmp) - 1);
            }
            if (NameType == ENV_NAME_TYPE_NO_PATH) {
                while ((*pDst++ = *pEnv++) != UNICODE_NULL) ;
            } else if (NameType == ENV_NAME_TYPE_SINGLE_PATH) {
                Length = wcslen(pEnv) + 1;
                dw = GetShortPathNameW(pEnv, pDst, Length + dwRemain);
                if (dw == 0 || dw >= Length + dwRemain) {
                    RtlMoveMemory(pDst, pEnv, Length * sizeof(WCHAR));
                    dw = Length - 1;
                }
                pDst += dw + 1;
                pEnv += Length;
                if (dw > Length) dwRemain -= dw - Length;
            } else {
                pTmp = pEnv;
                while (*pEnv != UNICODE_NULL) {
                    if (*pEnv == (WCHAR)';') {
                        Length = (DWORD)(pEnv - pTmp);
                        if (Length > 0) {
                            *pEnv = UNICODE_NULL;
                            dw = GetShortPathNameW(pTmp, pDst, Length + 1 + dwRemain);
                            if (dw == 0 || dw > Length + dwRemain) {
                                RtlMoveMemory(pDst, pTmp, Length * sizeof(WCHAR));
                                dw = Length;
                            }
                            pDst += dw;
                            *pDst++ = *pEnv++ = (WCHAR)';';
                            if (dw > Length) dwRemain -= dw - Length;
                        }
                        while (*pEnv == (WCHAR)';') *pDst++ = *pEnv++;
                        pTmp = pEnv;
                    } else pEnv++;
                }
                if ((Length = (DWORD)(pEnv - pTmp)) != 0) {
                    dw = GetShortPathNameW(pTmp, pDst, Length + 1 + dwRemain);
                    if (dw == 0 || dw > Length) {
                        RtlMoveMemory(pDst, pTmp, Length * sizeof(WCHAR));
                        dw = Length;
                    }
                    pDst += dw;
                    if (dw > Length) dwRemain -= dw - Length;
                }
                *pDst++ = *pEnv++;
            }
        }
        *pDst++ = UNICODE_NULL;
        cchEnv = (DWORD)pDst - (DWORD)pNewEnv;
        pUStringEnv->MaximumLength = pUStringEnv->Length = (USHORT)cchEnv;
        pUStringEnv->Buffer = pNewEnv;
        Status = RtlUnicodeStringToAnsiString(pAStringEnv, pUStringEnv, TRUE);
        if (!NT_SUCCESS(Status)) BaseSetLastNTError(Status); else bRet = TRUE;
bveTryExit:;
    }
    finally {
        if (lpEnvironment == NULL && EnvStrings != NULL) RtlDestroyEnvironment(EnvStrings);
    }
    return bRet;
}

BOOL BaseDestroyVDMEnvironment(ANSI_STRING *pAStringEnv, UNICODE_STRING *pUStringEnv)
{
    if (pAStringEnv->Buffer) RtlFreeAnsiString(pAStringEnv);
    if (pUStringEnv->Buffer) RtlFreeHeap(RtlProcessHeap(), 0, pUStringEnv->Buffer);
    return TRUE;
}

UINT BaseGetEnvNameType_U(WCHAR *Name, DWORD NameLength)
{
    static ENV_INFO EnvInfoTable[STD_ENV_NAME_COUNT] = {
        {ENV_NAME_TYPE_MULTIPLE_PATH, 4, ENV_NAME_PATH},
        {ENV_NAME_TYPE_SINGLE_PATH, 6, ENV_NAME_WINDIR},
        {ENV_NAME_TYPE_SINGLE_PATH, 10, ENV_NAME_SYSTEMROOT}
    };
    UINT NameType = ENV_NAME_TYPE_NO_PATH;
    int i;
    for (i = 0; i < STD_ENV_NAME_COUNT; i++) {
        if (EnvInfoTable[i].NameLength == NameLength &&
            !_wcsnicmp(EnvInfoTable[i].Name, Name, NameLength)) {
            NameType = EnvInfoTable[i].NameType;
            break;
        }
    }
    return NameType;
}

/*++
Copyright (c) 1990  Microsoft Corporation
Module Name: vdm.c
Abstract: Selected Base-client VDM environment and command routines.
--*/

/* DIVERGENCE(OPENNT-HOST-014): true source subset of original
 * base/win32/client/vdm.c. Environment routines are selected by the current
 * product; OPENNT_BASE_CLIENT_VDM_COMMANDS admits the original command API
 * for T412 source-owned integration. The remainder
 * needs the excluded Base/CSR process-creation shell. Names, parameters,
 * algorithm, call order and failure directions are retained. */
#include <nt.h>
#include "opennt-host/base/ntos/rtl/environapi.h"
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

#if !defined(OPENNT_BASE_CLIENT_VDM_COMMANDS)
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

#endif

#if defined(OPENNT_BASE_CLIENT_VDM_COMMANDS)
#include "adapter-opennt-host/basesrv/include/base_client.h"

BOOL
APIENTRY
GetNextVDMCommand(
    PVDMINFO lpVDMInfo
    )

/*++

Routine Description:
    This routine is used by MVDM to get a new command to execute. The
    VDM is blocked untill a DOS/WOW binary is encountered.


Arguments:
    lpVDMInfo - pointer to VDMINFO where new DOS command and other
		enviornment information is returned.

    if lpVDMInfo is NULL, then the caller is
    asking whether its the first VDM in the system.

Return Value:

    TRUE - The operation was successful. lpVDMInfo is filled in.

    FALSE/NULL - The operation failed.

--*/

{

    NTSTATUS Status;
    BASE_API_MSG m;
    PBASE_GET_NEXT_VDM_COMMAND_MSG a = (PBASE_GET_NEXT_VDM_COMMAND_MSG)&m.u.GetNextVDMCommand;
    PBASE_EXIT_VDM_MSG c= (PBASE_EXIT_VDM_MSG)&m.u.ExitVDM;
    PBASE_IS_FIRST_VDM_MSG d= (PBASE_IS_FIRST_VDM_MSG)&m.u.IsFirstVDM;
    PBASE_SET_REENTER_COUNT_MSG e = (PBASE_SET_REENTER_COUNT_MSG)&m.u.SetReenterCount;
    PCSR_CAPTURE_HEADER CaptureBuffer;
    ULONG Len,nPointers;
    USHORT VDMStateSave;

    // Special case to query the first VDM In the system.
    if(lpVDMInfo == NULL){
        Status = CsrClientCallServer(
                          (PCSR_API_MSG)&m,
                          NULL,
                          CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,
                                              BasepIsFirstVDM
                                              ),
                          sizeof( *d )
                          );

        if (NT_SUCCESS(Status)) {
            return(d->FirstVDM);
            }
        else {
            BaseSetLastNTError(Status);
            return FALSE;
            }
	}

    // Special case to increment/decrement the re-enterancy count

    if (lpVDMInfo->VDMState == INCREMENT_REENTER_COUNT ||
	lpVDMInfo->VDMState == DECREMENT_REENTER_COUNT) {

	e->ConsoleHandle = NtCurrentPeb()->ProcessParameters->ConsoleHandle;
        e->fIncDec = lpVDMInfo->VDMState;
        Status = CsrClientCallServer(
                        (PCSR_API_MSG)&m,
                        NULL,
			CSR_MAKE_API_NUMBER( BASESRV_SERVERDLL_INDEX,
					     BasepSetReenterCount
					   ),
			sizeof( *e )
                       );
        if (NT_SUCCESS(Status)) {
            return TRUE;
            }
        else {
            BaseSetLastNTError(Status);
            return FALSE;
            }
    }

    VDMStateSave = lpVDMInfo->VDMState;

    if(VDMStateSave & ASKING_FOR_WOW_BINARY)
	a->ConsoleHandle = (HANDLE)-1;
    else
        a->ConsoleHandle = NtCurrentPeb()->ProcessParameters->ConsoleHandle;

    if (lpVDMInfo->VDMState & ASKING_FOR_PIF)
        a->iTask = lpVDMInfo->iTask;

    a->AppLen = lpVDMInfo->AppLen;
    a->PifLen = lpVDMInfo->PifLen;
    a->CmdLen = lpVDMInfo->CmdSize;
    a->EnvLen = lpVDMInfo->EnviornmentSize;
    a->ExitCode = lpVDMInfo->ErrorCode;
    a->VDMState = VDMStateSave;
    a->WaitObjectForVDM = 0;
    a->DesktopLen = lpVDMInfo->DesktopLen;
    a->TitleLen = lpVDMInfo->TitleLen;
    a->ReservedLen = lpVDMInfo->ReservedLen;
    a->CurDirectoryLen = lpVDMInfo->CurDirectoryLen;

    // Find the total space for capture buffer

      // startup info
    Len = ROUND_UP(sizeof(STARTUPINFOA),4);
    nPointers = 1;

    if (lpVDMInfo->CmdSize) {
        Len += ROUND_UP(a->CmdLen,4);
        nPointers++;
        }

    if (lpVDMInfo->AppLen) {
        Len +=ROUND_UP(a->AppLen,4);
        nPointers++;
        }

    if (lpVDMInfo->PifLen) {
        Len +=ROUND_UP(a->PifLen,4);
        nPointers++;
        }

    if (lpVDMInfo->Enviornment) {
	nPointers++;
	Len+= (lpVDMInfo->EnviornmentSize) ?
		     ROUND_UP(lpVDMInfo->EnviornmentSize, 4) : 4;
        }

    if (lpVDMInfo->CurDirectoryLen == 0)
	a->CurDirectory = NULL;
    else{
	Len += ROUND_UP(lpVDMInfo->CurDirectoryLen,4);
	nPointers++;
	}

    if (lpVDMInfo->DesktopLen == 0)
	a->Desktop = NULL;
    else {
	Len += ROUND_UP(lpVDMInfo->DesktopLen,4);
	nPointers++;
	}

    if (lpVDMInfo->TitleLen == 0)
	a->Title = NULL;
    else {
	Len += ROUND_UP(lpVDMInfo->TitleLen,4);
	nPointers++;
        }

    if (lpVDMInfo->ReservedLen == 0)
        a->Reserved = NULL;
    else {
        Len += ROUND_UP(lpVDMInfo->ReservedLen,4);
	nPointers++;
	}

    CaptureBuffer = CsrAllocateCaptureBuffer(nPointers, 0, Len);
    if (CaptureBuffer == NULL) {
        BaseSetLastNTError( STATUS_NO_MEMORY );
        return FALSE;
        }

    if (lpVDMInfo->CmdLine) {
        CsrAllocateMessagePointer( CaptureBuffer,
                                   lpVDMInfo->CmdSize,
                                   (PVOID *)&a->CmdLine
                                 );
        }
    else {
        a->CmdLine = NULL;
        }


    if (lpVDMInfo->AppLen) {
	CsrAllocateMessagePointer( CaptureBuffer,
                                   lpVDMInfo->AppLen,
                                   (PVOID *)&a->AppName
				 );
        }
    else {
        a->AppName = NULL;
        }

    if (lpVDMInfo->PifLen) {
	CsrAllocateMessagePointer( CaptureBuffer,
                                   lpVDMInfo->PifLen,
                                   (PVOID *)&a->PifFile
				 );
        }
    else {
        a->PifFile = NULL;
        }


    if (lpVDMInfo->EnviornmentSize) {
        CsrAllocateMessagePointer( CaptureBuffer,
                                   lpVDMInfo->EnviornmentSize,
				   (PVOID *)&a->Env
				 );
        }
    else {
        a->Env = NULL;
        }

    if (lpVDMInfo->CurDirectoryLen)
	CsrAllocateMessagePointer( CaptureBuffer,
				   lpVDMInfo->CurDirectoryLen,
				   (PVOID *)&a->CurDirectory
				 );
    else
	a->CurDirectory = NULL;


    CsrAllocateMessagePointer( CaptureBuffer,
			       sizeof(STARTUPINFOA),
			       (PVOID *)&a->StartupInfo
			     );

    if (lpVDMInfo->DesktopLen)
	CsrAllocateMessagePointer( CaptureBuffer,
				   lpVDMInfo->DesktopLen,
				   (PVOID *)&a->Desktop
				 );
    else
	a->Desktop = NULL;

    if (lpVDMInfo->TitleLen)
	CsrAllocateMessagePointer( CaptureBuffer,
				   lpVDMInfo->TitleLen,
				   (PVOID *)&a->Title
				 );
    else
	a->Title = NULL;

    if (lpVDMInfo->ReservedLen)
	CsrAllocateMessagePointer( CaptureBuffer,
                                   lpVDMInfo->ReservedLen,
                                   (PVOID *)&a->Reserved
				 );
    else
        a->Reserved = NULL;

retry:
    Status = CsrClientCallServer(
                        (PCSR_API_MSG)&m,
			CaptureBuffer,
			CSR_MAKE_API_NUMBER( BASESRV_SERVERDLL_INDEX,
					    BasepGetNextVDMCommand
					   ),
			sizeof( *a )
                        );

    if (a->WaitObjectForVDM) {
	Status = NtWaitForSingleObject(a->WaitObjectForVDM,FALSE,NULL);
	if (Status != STATUS_SUCCESS){
	    BaseSetLastNTError(Status);
	    return FALSE;
	    }
	else {
	    a->VDMState |= ASKING_FOR_SECOND_TIME;
	    a->ExitCode = 0;
	    goto retry;
	    }
	}

    if (NT_SUCCESS(Status)) {
        Status = (NTSTATUS)m.ReturnValue;
        }


    if (!NT_SUCCESS( Status )) {
        if (Status == STATUS_INVALID_PARAMETER) {
	    //This means one of the buffer size is less than required.
            lpVDMInfo->CmdSize = a->CmdLen;
            lpVDMInfo->AppLen = a->AppLen;
            lpVDMInfo->PifLen = a->PifLen;
	    lpVDMInfo->EnviornmentSize = a->EnvLen;
	    lpVDMInfo->CurDirectoryLen = a->CurDirectoryLen;
	    lpVDMInfo->DesktopLen      = a->DesktopLen;
	    lpVDMInfo->TitleLen        = a->TitleLen;
            lpVDMInfo->ReservedLen     = a->ReservedLen;
            }
        else {
            lpVDMInfo->CmdSize = 0;
            lpVDMInfo->AppLen = 0;
            lpVDMInfo->PifLen = 0;
	    lpVDMInfo->EnviornmentSize = 0;
	    lpVDMInfo->CurDirectoryLen = 0;
	    lpVDMInfo->DesktopLen      = 0;
	    lpVDMInfo->TitleLen        = 0;
            lpVDMInfo->ReservedLen     = 0;
	    }
	CsrFreeCaptureBuffer( CaptureBuffer );
	BaseSetLastNTError(Status);
	return FALSE;
    }


    try {

        if (lpVDMInfo->CmdSize)
            RtlMoveMemory(lpVDMInfo->CmdLine,
                          a->CmdLine,
                          a->CmdLen);


        if (lpVDMInfo->AppLen)
            RtlMoveMemory(lpVDMInfo->AppName,
                          a->AppName,
                          a->AppLen);

        if (lpVDMInfo->PifLen)
            RtlMoveMemory(lpVDMInfo->PifFile,
                          a->PifFile,
                          a->PifLen);


        if (lpVDMInfo->Enviornment)
	    RtlMoveMemory(lpVDMInfo->Enviornment,
			  a->Env,
                          a->EnvLen);


	if (lpVDMInfo->CurDirectoryLen)
	    RtlMoveMemory(lpVDMInfo->CurDirectory,
			  a->CurDirectory,
			  a->CurDirectoryLen);

	if (a->VDMState & STARTUP_INFO_RETURNED)
	    RtlMoveMemory(&lpVDMInfo->StartupInfo,
			  a->StartupInfo,
                          sizeof(STARTUPINFOA));

	if (lpVDMInfo->DesktopLen){
	    RtlMoveMemory(lpVDMInfo->Desktop,
			  a->Desktop,
			  a->DesktopLen);
	    lpVDMInfo->StartupInfo.lpDesktop = lpVDMInfo->Desktop;
	}


	if (lpVDMInfo->TitleLen){
	    RtlMoveMemory(lpVDMInfo->Title,
			  a->Title,
			  a->TitleLen);
	    lpVDMInfo->StartupInfo.lpTitle = lpVDMInfo->Title;
	}

        if (lpVDMInfo->ReservedLen){
            RtlMoveMemory(lpVDMInfo->Reserved,
                          a->Reserved,
                          a->ReservedLen);
            lpVDMInfo->StartupInfo.lpReserved = lpVDMInfo->Reserved;
        }

        lpVDMInfo->CmdSize = a->CmdLen;
        lpVDMInfo->AppLen = a->AppLen;
        lpVDMInfo->PifLen = a->PifLen;
        lpVDMInfo->EnviornmentSize = a->EnvLen;
	if (a->VDMState & STARTUP_INFO_RETURNED)
	    lpVDMInfo->VDMState = STARTUP_INFO_RETURNED;
	else
	    lpVDMInfo->VDMState = 0;
	lpVDMInfo->CurDrive = a->CurrentDrive;
	lpVDMInfo->StdIn  = a->StdIn;
	lpVDMInfo->StdOut = a->StdOut;
	lpVDMInfo->StdErr = a->StdErr;
	lpVDMInfo->iTask = a->iTask;
	lpVDMInfo->CodePage = a->CodePage;
	lpVDMInfo->CurDirectoryLen = a->CurDirectoryLen;
	lpVDMInfo->DesktopLen = a->DesktopLen;
	lpVDMInfo->TitleLen = a->TitleLen;
        lpVDMInfo->ReservedLen = a->ReservedLen;
        lpVDMInfo->dwCreationFlags = a->dwCreationFlags;
        lpVDMInfo->fComingFromBat = a->fComingFromBat;

	CsrFreeCaptureBuffer( CaptureBuffer );
	return TRUE;
	}
    except ( EXCEPTION_EXECUTE_HANDLER ) {
        BaseSetLastNTError(GetExceptionCode());
	CsrFreeCaptureBuffer( CaptureBuffer );
	return FALSE;
	}
}

VOID
APIENTRY
ExitVDM(
    BOOL IsWowCaller,
    ULONG iWowTask
    )

/*++

Routine Description:
    This routine is used by MVDM to exit.


Arguments:
    IsWowCaller - TRUE if the caller is WOWVDM.
                  FALSE if the caller is DOSVDM

    iWowTask - if IsWowCaller == FALSE then Dont Care
	     - if IsWowCaller == TRUE && iWowTask != -1 kill iWowTask task
	     - if IsWowCaller == TRUE && iWowTask == -1 kill all wow task

Return Value:
    None

--*/

{

    NTSTATUS Status;
    BASE_API_MSG m;
    PBASE_EXIT_VDM_MSG c= (PBASE_EXIT_VDM_MSG)&m.u.ExitVDM;


    if(IsWowCaller){
	c->ConsoleHandle = (HANDLE)-1;
	c->iWowTask = iWowTask;
	}
    else {
        c->ConsoleHandle = NtCurrentPeb()->ProcessParameters->ConsoleHandle;
        }

    c->WaitObjectForVDM =0;

    Status = CsrClientCallServer(
                      (PCSR_API_MSG)&m,
                      NULL,
                      CSR_MAKE_API_NUMBER( BASESRV_SERVERDLL_INDEX,
                                           BasepExitVDM
                                         ),
                      sizeof( *c )
                      );
    if (NT_SUCCESS(Status) && c->WaitObjectForVDM) {
        NtClose (c->WaitObjectForVDM);
        }

    return;
}

/*++

Routine Description:
    Set new VDM current directories

Arguments:
    cchCurDir - length of buffer in bytes
    lpszCurDir - buffer to return the current director of NTVDM

Return Value:
    TRUE if function succeed
    FALSE if function failed, GetLastError() has the error code
--*/


BOOL
APIENTRY
SetVDMCurrentDirectories(
    IN ULONG  cchCurDirs,
    IN LPSTR  lpszzCurDirs
    )
{
    NTSTATUS Status;
    PCSR_CAPTURE_HEADER CaptureBuffer;
    BASE_API_MSG m;
    PBASE_GET_SET_VDM_CUR_DIRS_MSG a = (PBASE_GET_SET_VDM_CUR_DIRS_MSG)&m.u.GetSetVDMCurDirs;

    a->ConsoleHandle = NtCurrentPeb()->ProcessParameters->ConsoleHandle;
    // caller must have a valid console(WOW will fail)
    if (a->ConsoleHandle == (HANDLE) -1) {
	BaseSetLastNTError(STATUS_INVALID_PARAMETER);
	return FALSE;
    }
    if (cchCurDirs && lpszzCurDirs) {
	// get capture buffer, one pointer in the message

	CaptureBuffer = CsrAllocateCaptureBuffer(1, 0, cchCurDirs);
        if (CaptureBuffer == NULL) {
            BaseSetLastNTError( STATUS_NO_MEMORY );
            return FALSE;
            }

	CsrAllocateMessagePointer( CaptureBuffer,
				   cchCurDirs,
				   (PVOID *)&a->lpszzCurDirs
				   );

	a->cchCurDirs = cchCurDirs;
	try {
	    RtlMoveMemory(a->lpszzCurDirs, lpszzCurDirs, cchCurDirs);
	}
	except (EXCEPTION_EXECUTE_HANDLER) {
	    BaseSetLastNTError(GetExceptionCode());
	    CsrFreeCaptureBuffer(CaptureBuffer);
	    return FALSE;
	}
        Status = CsrClientCallServer(
                            (PCSR_API_MSG)&m,
			    CaptureBuffer,
			    CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,
						BasepSetVDMCurDirs
						),
			    sizeof( *a )
			    );
	CsrFreeCaptureBuffer(CaptureBuffer);

        if (!NT_SUCCESS(Status) || !NT_SUCCESS((NTSTATUS)m.ReturnValue)) {
	    BaseSetLastNTError(Status);
	    return FALSE;
	}
    }
    return TRUE;
}




/*++

Routine Description:
    To return current directory of NTVDM.
    This allows the parent process(CMD.EXE in most cases) to keep track the
    current directory after each VDM execution.
    NOTE: this function doesn't apply to wow

Arguments:
    cchCurDir - length of buffer in bytes
    lpszCurDir - buffer to return the current director of NTVDM

    Note: We don't require the process id to the running VDM because
	  current directories are global to every VDMs under a single NTVDM
	  control -- each console handle has its own current directories
Return Value:
    ULONG - (1). number of bytes written to the given buffer if succeed
	    (2). lentgh of the current directory including NULL
		 if the provided buffer is not large enough
	    (3). 0  then GetLastError() has the error code
--*/


ULONG
APIENTRY
GetVDMCurrentDirectories(
    IN ULONG  cchCurDirs,
    IN LPSTR  lpszzCurDirs
    )
{
    NTSTATUS Status;
    PCSR_CAPTURE_HEADER CaptureBuffer;
    BASE_API_MSG m;
    PBASE_GET_SET_VDM_CUR_DIRS_MSG a = (PBASE_GET_SET_VDM_CUR_DIRS_MSG)&m.u.GetSetVDMCurDirs;


    a->ConsoleHandle = NtCurrentPeb()->ProcessParameters->ConsoleHandle;
    if (a->ConsoleHandle == (HANDLE) -1) {
	BaseSetLastNTError(STATUS_INVALID_PARAMETER);
	return 0L;
    }
    if (cchCurDirs && lpszzCurDirs) {
	CaptureBuffer = CsrAllocateCaptureBuffer(1, 0, cchCurDirs);
        if (CaptureBuffer == NULL) {
            BaseSetLastNTError( STATUS_NO_MEMORY );
            return FALSE;
            }

	CsrAllocateMessagePointer( CaptureBuffer,
				   cchCurDirs,
				   (PVOID *)&a->lpszzCurDirs
				   );

	a->cchCurDirs = cchCurDirs;
    }
    else {
	a->cchCurDirs = 0;
	a->lpszzCurDirs = NULL;
	CaptureBuffer = NULL;
    }

    m.ReturnValue = 0xffffffff;

    Status = CsrClientCallServer(
                         (PCSR_API_MSG)&m,
			 CaptureBuffer,
			 CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,
					     BasepGetVDMCurDirs
                                             ),
                         sizeof( *a )
                         );

    if (m.ReturnValue == 0xffffffff) {
        a->cchCurDirs = 0;
        }

    if (NT_SUCCESS(Status)) {
        Status = m.ReturnValue;
        }

    if (NT_SUCCESS(Status)) {

        try {
            RtlMoveMemory(lpszzCurDirs, a->lpszzCurDirs, a->cchCurDirs);
            }
        except(EXCEPTION_EXECUTE_HANDLER) {
            Status = GetExceptionCode();
            a->cchCurDirs = 0;
            }
        }
    else {
        BaseSetLastNTError(Status);
        }

    if (CaptureBuffer) {
        CsrFreeCaptureBuffer(CaptureBuffer);
        }

    return a->cchCurDirs;
}


VOID
APIENTRY
CmdBatNotification(
    IN  ULONG   fBeginEnd
    )

/*++

Routine Description:
    This API lets base know about .bat processing from cmd. This is
    required by VDM, so that it can decided correctly when to  put
    command.com prompt on TSRs. If the command came from .bat file
    then VDM should'nt put its prompt. This is important for
    ventura publisher and civilization apps.

Arguments:
    fBeginEnd - CMD_BAT_OPERATION_STARTING  -> .BAT processing is starting
                CMD_BAT_OPERATION_TERMINATING -> .BAT processing is ending

Return Value:
    None
--*/

{
    BASE_API_MSG m;
    PBASE_BAT_NOTIFICATION_MSG a = (PBASE_BAT_NOTIFICATION_MSG)&m.u.BatNotification;

    a->ConsoleHandle = NtCurrentPeb()->ProcessParameters->ConsoleHandle;

    if (a->ConsoleHandle == (HANDLE) -1)
        return;

    a->fBeginEnd = fBeginEnd;

    CsrClientCallServer((PCSR_API_MSG)&m,
                         NULL,
			 CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,
                                             BasepBatNotification
					     ),
                         sizeof( *a )
                         );

    return;
}


VOID
APIENTRY
RegisterWowExec(
    IN  HANDLE   hwndWowExec
    )

/*++

Routine Description:
    This API gives basesrv the window handle for the shared WowExec so
    it can send WM_WOWEXECSTARTAPP messages to WowExec.  This
    saves having a thread in WOW dedicated to GetNextVDMCommand.

Arguments:
    hwndWowExec - Win32 window handle for WowExec in shared WOW VDM.
                  Separate WOW VDMs don't register their WowExec handle
                  because they never get commands from base.

Return Value:
    None.
--*/

{
    BASE_API_MSG m;
    PBASE_REGISTER_WOWEXEC_MSG a = &m.u.RegisterWowExec;

    a->hwndWowExec = hwndWowExec;

    CsrClientCallServer((PCSR_API_MSG)&m,
                         NULL,
			 CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,
                                             BasepRegisterWowExec
					     ),
                         sizeof( *a )
                        );
    return;
}

BOOL
BaseUpdateVDMEntry(
    IN ULONG UpdateIndex,
    IN OUT HANDLE *WaitHandle,
    IN ULONG IndexInfo,
    IN ULONG BinaryType
    )
{
    NTSTATUS Status;
    BASE_API_MSG m;
    PBASE_UPDATE_VDM_ENTRY_MSG c= (PBASE_UPDATE_VDM_ENTRY_MSG)&m.u.UpdateVDMEntry;

    switch (UpdateIndex) {
	case UPDATE_VDM_UNDO_CREATION:
	    c->iTask = (ULONG)*WaitHandle;
	    c->VDMCreationState = (USHORT)IndexInfo;
	    break;
	case UPDATE_VDM_PROCESS_HANDLE:
	    c->VDMProcessHandle = *WaitHandle;	// Actually this is VDM handle
	    c->iTask = IndexInfo;
	    break;
        }

    if(BinaryType == BINARY_TYPE_WIN16)
        c->ConsoleHandle = (HANDLE)-1;
    else if (c->iTask)
        c->ConsoleHandle = 0;
    else
	c->ConsoleHandle = NtCurrentPeb()->ProcessParameters->ConsoleHandle;

    c->EntryIndex = (WORD)UpdateIndex;
    c->BinaryType = BinaryType;


    Status = CsrClientCallServer(
                      (PCSR_API_MSG)&m,
                      NULL,
                      CSR_MAKE_API_NUMBER( BASESRV_SERVERDLL_INDEX,
                                           BasepUpdateVDMEntry
                                         ),
                      sizeof( *c )
                      );

    if (!NT_SUCCESS(Status) || !NT_SUCCESS((NTSTATUS)m.ReturnValue)) {
	BaseSetLastNTError((NTSTATUS)m.ReturnValue);
	return FALSE;
	}

    switch (UpdateIndex) {
	case UPDATE_VDM_UNDO_CREATION:
	    break;
	case UPDATE_VDM_PROCESS_HANDLE:
	    *WaitHandle = c->WaitObjectForParent;
	    break;
    }
    return TRUE;
}
#endif

/*  process.c
 *  OemUnicode win32 thunks
 *  - process\env stuff
 *
 *  14-Jan-1993 Jonle
 */
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <oemuni.h>
#include "oem.h"
#include <vdmapi.h>
UINT
APIENTRY
GetSystemDirectoryOem(
    LPSTR lpBuffer,
    UINT uSize
    )

/*++

Routine Description:

    OEM thunk to GetSystemDirectoryW

--*/

{
    OEM_STRING OemString;
    UNICODE_STRING Unicode;
    NTSTATUS Status;

    DWORD UnicodeCapacity = GetSystemDirectoryW(NULL,0);
    if ( !UnicodeCapacity ) return 0;
    if ( UnicodeCapacity > 32767 ) { BaseSetLastNTError(STATUS_BUFFER_OVERFLOW); return 0; }
    Unicode.MaximumLength = (USHORT)(UnicodeCapacity*sizeof(WCHAR));
#line 35
    Unicode.Buffer = RtlAllocateHeap(
                                RtlProcessHeap(), 0,
                                Unicode.MaximumLength
                                );
    if ( !Unicode.Buffer ) {
        BaseSetLastNTError(STATUS_NO_MEMORY);
        return 0;
        }

    UnicodeCapacity = GetSystemDirectoryW(Unicode.Buffer,UnicodeCapacity);

    if ( !UnicodeCapacity || UnicodeCapacity >= Unicode.MaximumLength/sizeof(WCHAR) ) {
#line 49
        RtlFreeHeap(RtlProcessHeap(), 0,Unicode.Buffer);
        if ( UnicodeCapacity ) BaseSetLastNTError(STATUS_BUFFER_OVERFLOW);
        return 0;
#line 51
        }
    Unicode.Length = (USHORT)(UnicodeCapacity*sizeof(WCHAR));
#line 52
    OemString.Buffer = lpBuffer;
    OemString.MaximumLength = (USHORT)min(uSize,65535);
    if ( RtlUnicodeStringToOemSize(&Unicode) > uSize ) {
        uSize = RtlUnicodeStringToOemSize(&Unicode);
        RtlFreeHeap(RtlProcessHeap(), 0,Unicode.Buffer);
        return uSize;
        }
#line 54
    Status = RtlUnicodeStringToOemString(&OemString,&Unicode,FALSE);
    RtlFreeHeap(RtlProcessHeap(), 0,Unicode.Buffer);
    if ( !NT_SUCCESS(Status) ) {
        BaseSetLastNTError(Status);
        return 0;
        }
    return OemString.Length;
}



UINT
APIENTRY
GetWindowsDirectoryOem(
    LPSTR lpBuffer,
    UINT uSize
    )

/*++

Routine Description:

    OEM thunk to GetWindowsDirectoryW

--*/

{
    OEM_STRING OemString;
    UNICODE_STRING Unicode;
    NTSTATUS Status;

    DWORD UnicodeCapacity = GetWindowsDirectoryW(NULL,0);
    if ( !UnicodeCapacity ) return 0;
    if ( UnicodeCapacity > 32767 ) { BaseSetLastNTError(STATUS_BUFFER_OVERFLOW); return 0; }
    Unicode.MaximumLength = (USHORT)(UnicodeCapacity*sizeof(WCHAR));
#line 86
    Unicode.Buffer = RtlAllocateHeap(
                                RtlProcessHeap(), 0,
                                Unicode.MaximumLength
                                );
    if ( !Unicode.Buffer ) {
        BaseSetLastNTError(STATUS_NO_MEMORY);
        return 0;
        }

    UnicodeCapacity = GetWindowsDirectoryW(Unicode.Buffer,UnicodeCapacity);

    if ( !UnicodeCapacity || UnicodeCapacity >= Unicode.MaximumLength/sizeof(WCHAR) ) {
#line 100
        RtlFreeHeap(RtlProcessHeap(), 0,Unicode.Buffer);
        if ( UnicodeCapacity ) BaseSetLastNTError(STATUS_BUFFER_OVERFLOW);
        return 0;
#line 102
        }
    Unicode.Length = (USHORT)(UnicodeCapacity*sizeof(WCHAR));
#line 103
    OemString.Buffer = lpBuffer;
    OemString.MaximumLength = (USHORT)min(uSize,65535);
    if ( RtlUnicodeStringToOemSize(&Unicode) > uSize ) {
        uSize = RtlUnicodeStringToOemSize(&Unicode);
        RtlFreeHeap(RtlProcessHeap(), 0,Unicode.Buffer);
        return uSize;
        }
#line 105
    Status = RtlUnicodeStringToOemString(&OemString,&Unicode,FALSE);
    RtlFreeHeap(RtlProcessHeap(), 0,Unicode.Buffer);
    if ( !NT_SUCCESS(Status) ) {
        BaseSetLastNTError(Status);
        return 0;
        }
    return OemString.Length;
}


DWORD
APIENTRY
SearchPathOem (
    LPCSTR lpPath,
    LPCSTR lpFileName,
    LPCSTR lpExtension,
    DWORD nBufferLength,
    LPSTR lpBuffer,
    LPSTR *lpFilePart
    )

/*++

Routine Description:

    Oem thunk to SearchPathW

--*/

{

    UNICODE_STRING xlpPath;
    PUNICODE_STRING Unicode;
    UNICODE_STRING xlpExtension;
    PWSTR xlpBuffer;
    DWORD UnicodeCapacity, ReturnValue = 0;
#line 141
    OEM_STRING OemString;
    UNICODE_STRING UnicodeString;
    NTSTATUS Status;
    PWSTR FilePart;
    PWSTR *FilePartPtr;

    if ( ARGUMENT_PRESENT(lpFilePart) ) {
        FilePartPtr = &FilePart;
        }
    else {
        FilePartPtr = NULL;
        }

    Unicode = &NtCurrentTeb()->StaticUnicodeString;
    InitOemString(&OemString,lpFileName);

    Status = RtlOemStringToUnicodeString(Unicode,&OemString,FALSE);
    if ( !NT_SUCCESS(Status) ) {
        if ( Status == STATUS_BUFFER_OVERFLOW ) {
            SetLastError(ERROR_FILENAME_EXCED_RANGE);
            }
        else {
            BaseSetLastNTError(Status);
            }
        return 0;
        }

    if ( ARGUMENT_PRESENT(lpExtension) ) {
        InitOemString(&OemString,lpExtension);
        Status = RtlOemStringToUnicodeString(&xlpExtension,&OemString,TRUE);
        if ( !NT_SUCCESS(Status) ) {
            BaseSetLastNTError(Status);
            return 0;
            }
        }
    else {
        xlpExtension.Buffer = NULL;
        }

    if ( ARGUMENT_PRESENT(lpPath) ) {
        InitOemString(&OemString,lpPath);
        Status = RtlOemStringToUnicodeString(&xlpPath,&OemString,TRUE);
        if ( !NT_SUCCESS(Status) ) {
            if ( ARGUMENT_PRESENT(lpExtension) ) {
                RtlFreeUnicodeString(&xlpExtension);
                }
            BaseSetLastNTError(Status);
            return 0;
            }
        }
    else {
        xlpPath.Buffer = NULL;
        }

    UnicodeCapacity = SearchPathW(xlpPath.Buffer,Unicode->Buffer,xlpExtension.Buffer,0,NULL,NULL);
    if ( !UnicodeCapacity ) goto bail0;
    if ( UnicodeCapacity > 32767 ) { BaseSetLastNTError(STATUS_BUFFER_OVERFLOW); goto bail0; }
    xlpBuffer = RtlAllocateHeap(RtlProcessHeap(), 0,UnicodeCapacity*sizeof(WCHAR));
#line 196
    if ( !xlpBuffer ) {
        BaseSetLastNTError(STATUS_NO_MEMORY);
        goto bail0;
        }
    ReturnValue = SearchPathW(
                    xlpPath.Buffer,
                    Unicode->Buffer,
                    xlpExtension.Buffer,
                    UnicodeCapacity,
                    xlpBuffer,
                    FilePartPtr
                    );
    if (ReturnValue && ReturnValue < UnicodeCapacity ) {
#line 209
        RtlInitUnicodeString(&UnicodeString,xlpBuffer);
        ReturnValue = RtlUnicodeStringToOemSize(&UnicodeString);
        if ( ReturnValue > nBufferLength ) {
            RtlFreeHeap(RtlProcessHeap(), 0,xlpBuffer);
            goto bail0;
            }
        OemString.MaximumLength = (USHORT)min(nBufferLength,65535);
#line 211
        OemString.Buffer = lpBuffer;
        Status = RtlUnicodeStringToOemString(&OemString,&UnicodeString,FALSE);
        if ( !NT_SUCCESS(Status) ) {
            BaseSetLastNTError(Status);
            ReturnValue = 0;
            }
        else {
            ReturnValue = OemString.Length;
#line 218
            if ( ARGUMENT_PRESENT(lpFilePart) ) {
                if ( FilePart == NULL ) {
                    *lpFilePart = NULL;
                    }
                else {
                    UnicodeString.Length = (USHORT)((FilePart - xlpBuffer)*sizeof(WCHAR));
                    *lpFilePart = lpBuffer + RtlUnicodeStringToOemSize(&UnicodeString)-1;
#line 225
                    }
                }
            }
        }

    else if ( ReturnValue ) {
        BaseSetLastNTError(STATUS_BUFFER_OVERFLOW);
        ReturnValue = 0;
        }
#line 230
    RtlFreeHeap(RtlProcessHeap(), 0,xlpBuffer);
bail0:
    if ( ARGUMENT_PRESENT(lpExtension) ) {
        RtlFreeUnicodeString(&xlpExtension);
        }

    if ( ARGUMENT_PRESENT(lpPath) ) {
        RtlFreeUnicodeString(&xlpPath);
        }
    return ReturnValue;
}


DWORD
APIENTRY
GetTempPathOem(
    DWORD nBufferLength,
    LPSTR lpBuffer
    )

/*++

Routine Description:

    OEM thunk to GetTempPathW

--*/

{
    OEM_STRING OemString;
    UNICODE_STRING UnicodeString;
    NTSTATUS Status;

    DWORD UnicodeCapacity = GetTempPathW(0,NULL);
    if ( !UnicodeCapacity ) return 0;
    if ( UnicodeCapacity > 32767 ) { BaseSetLastNTError(STATUS_BUFFER_OVERFLOW); return 0; }
    UnicodeString.MaximumLength = (USHORT)(UnicodeCapacity*sizeof(WCHAR));
#line 264
    UnicodeString.Buffer = RtlAllocateHeap(
                                RtlProcessHeap(), 0,
                                UnicodeString.MaximumLength
                                );
    if ( !UnicodeString.Buffer ) {
        BaseSetLastNTError(STATUS_NO_MEMORY);
        return 0;
        }
    UnicodeCapacity = GetTempPathW(UnicodeCapacity,UnicodeString.Buffer);
    if ( !UnicodeCapacity || UnicodeCapacity >= UnicodeString.MaximumLength/sizeof(WCHAR) ) {
#line 277
        RtlFreeHeap(RtlProcessHeap(), 0,UnicodeString.Buffer);
        if ( UnicodeCapacity ) BaseSetLastNTError(STATUS_BUFFER_OVERFLOW);
        return 0;
#line 279
        }
    UnicodeString.Length = (USHORT)(UnicodeCapacity*sizeof(WCHAR));
#line 280
    OemString.Buffer = lpBuffer;
    OemString.MaximumLength = (USHORT)min(nBufferLength,65535);
    if ( RtlUnicodeStringToOemSize(&UnicodeString) > nBufferLength ) {
        nBufferLength = RtlUnicodeStringToOemSize(&UnicodeString);
        RtlFreeHeap(RtlProcessHeap(), 0,UnicodeString.Buffer);
        return nBufferLength;
        }
#line 282
    Status = RtlUnicodeStringToOemString(&OemString,&UnicodeString,FALSE);
    RtlFreeHeap(RtlProcessHeap(), 0,UnicodeString.Buffer);
    if ( !NT_SUCCESS(Status) ) {
        BaseSetLastNTError(Status);
        return 0;
        }
    return OemString.Length;
}



UINT
APIENTRY
GetTempFileNameOem(
    LPCSTR lpPathName,
    LPCSTR lpPrefixString,
    UINT uUnique,
    LPSTR lpTempFileName
    )

/*++

Routine Description:

    Oem thunk to GetTempFileNameW

--*/

{
    PUNICODE_STRING Unicode;
    UNICODE_STRING UnicodePrefix;
    OEM_STRING OemString;
    NTSTATUS Status;
    UINT ReturnValue;
    UNICODE_STRING UnicodeResult;

    Unicode = &NtCurrentTeb()->StaticUnicodeString;

    InitOemString(&OemString,lpPathName);
    Status = RtlOemStringToUnicodeString(Unicode,&OemString,FALSE);
    if ( !NT_SUCCESS(Status) ) {
        if ( Status == STATUS_BUFFER_OVERFLOW ) {
            SetLastError(ERROR_FILENAME_EXCED_RANGE);
            }
        else {
            BaseSetLastNTError(Status);
            }
        return 0;
        }

    InitOemString(&OemString,lpPrefixString);
    Status = RtlOemStringToUnicodeString(&UnicodePrefix,&OemString,TRUE);
    if ( !NT_SUCCESS(Status) ) {
        BaseSetLastNTError(Status);
        return 0;
        }
    UnicodeResult.MaximumLength = (USHORT)((MAX_PATH<<1)+sizeof(UNICODE_NULL));
    UnicodeResult.Buffer = RtlAllocateHeap(RtlProcessHeap(), 0,UnicodeResult.MaximumLength);
    if ( !UnicodeResult.Buffer ) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        RtlFreeUnicodeString(&UnicodePrefix);
        return 0;
        }

    ReturnValue = GetTempFileNameW(
                    Unicode->Buffer,
                    UnicodePrefix.Buffer,
                    uUnique,
                    UnicodeResult.Buffer
                    );
    if ( ReturnValue ) {
        RtlInitUnicodeString(&UnicodeResult,UnicodeResult.Buffer);
        OemString.Buffer = lpTempFileName;
        OemString.MaximumLength = MAX_PATH+1;
        Status = RtlUnicodeStringToOemString(&OemString,&UnicodeResult,FALSE);
        if ( !NT_SUCCESS(Status) ) {
            BaseSetLastNTError(Status);
            ReturnValue = 0;
            }
        }
    RtlFreeUnicodeString(&UnicodePrefix);
    RtlFreeHeap(RtlProcessHeap(), 0,UnicodeResult.Buffer);

    return ReturnValue;
}


#line 369
#if 0  // unused
#line 370
#error DIVERGENCE MVDM-HOST-DIV-496: original literal #if 0 excludes this legacy branch; see mirror register.
#endif
#line 559



DWORD
WINAPI
GetEnvironmentVariableOem(
    LPSTR lpName,
    LPSTR lpBuffer,
    DWORD nSize
    )
/*++

    OEM thunk to GetEnvironmentVariableW

--*/
{
    NTSTATUS Status;
    UNICODE_STRING Unicode;
    OEM_STRING     OemString;
    ANSI_STRING Name, Buffer;
    DWORD ReturnValue;

    Unicode.Buffer = NULL;
    Name.Buffer = NULL;
    Buffer.Buffer = NULL;
    ReturnValue = 0;
    try {

        InitOemString(&OemString,lpName);
        Status = RtlOemStringToUnicodeString(&Unicode, &OemString, TRUE);
        if ( !NT_SUCCESS(Status) ) {
            if ( Status == STATUS_BUFFER_OVERFLOW ) {
                SetLastError(ERROR_FILENAME_EXCED_RANGE);
                }
            else {
                BaseSetLastNTError(Status);
                }
            return 0;
            }

        Status = RtlUnicodeStringToAnsiString( &Name, &Unicode, TRUE );
        if (!NT_SUCCESS( Status )) {
            BaseSetLastNTError( Status );
            goto try_exit;
            }

        ReturnValue = GetEnvironmentVariableA(Name.Buffer, NULL, 0);
        if (ReturnValue == 0)
            goto try_exit;
        if (ReturnValue > 65535) {
            BaseSetLastNTError(STATUS_BUFFER_OVERFLOW);
            ReturnValue = 0;
            goto try_exit;
            }
        Buffer.MaximumLength = (USHORT)ReturnValue;
#line 606
        Buffer.Buffer = (PCHAR)
            RtlAllocateHeap( RtlProcessHeap(), 0, Buffer.MaximumLength );
        if (Buffer.Buffer == NULL) {
            BaseSetLastNTError( STATUS_NO_MEMORY );
            ReturnValue = 0;
#line 610
            goto try_exit;
            }

        SetLastError(ERROR_SUCCESS);
#line 613
        ReturnValue = GetEnvironmentVariableA( Name.Buffer,
                                               Buffer.Buffer,
                                               Buffer.MaximumLength
                                             );
        if (ReturnValue == 0 && GetLastError() == ERROR_SUCCESS) {
            if (nSize != 0) lpBuffer[0] = '\0';
            else ReturnValue = 1;
            goto try_exit;
            }
#line 617
        if (ReturnValue != 0) {
            if ( ReturnValue < Buffer.MaximumLength ) {
                Buffer.Length = (USHORT)ReturnValue;
                RtlFreeUnicodeString( &Unicode );
                Unicode.Buffer = NULL;
                Status = RtlAnsiStringToUnicodeString( &Unicode, &Buffer, TRUE );
                if (!NT_SUCCESS( Status )) {
                    BaseSetLastNTError( Status );
                    ReturnValue = 0;
                    goto try_exit;
#line 626
                    }

                OemString.Buffer        = lpBuffer;
                OemString.MaximumLength = (USHORT)(nSize > 65535 ? 65535 : nSize);
                ReturnValue = RtlUnicodeStringToOemSize(&Unicode);
                if (ReturnValue > nSize)
                    goto try_exit;
#line 630
                Status = RtlUnicodeStringToOemString( &OemString, &Unicode, FALSE );
                if (!NT_SUCCESS( Status )) {
                    BaseSetLastNTError( Status );
                    ReturnValue = 0;
                    }
                else {
                    ReturnValue = OemString.Length;
                    }
                }
            else {
                BaseSetLastNTError(STATUS_BUFFER_OVERFLOW);
                ReturnValue = 0;
#line 635
                }
            }
try_exit:;
        }
    finally {
        if (Unicode.Buffer != NULL) {
            RtlFreeUnicodeString( &Unicode );
            }

        if (Name.Buffer != NULL) {
            RtlFreeAnsiString( &Name );
            }

        if (Buffer.Buffer != NULL) {
            RtlFreeHeap( RtlProcessHeap(), 0, Buffer.Buffer );
            }
        }

    return( ReturnValue );
}




BOOL
WINAPI
SetEnvironmentVariableOem(
    LPSTR lpName,
    LPSTR lpValue
    )
/*++

    OEM thunk to SetEnvironmentVariableW

--*/

{
    NTSTATUS Status;
    UNICODE_STRING Unicode;
    OEM_STRING     OemString;
    ANSI_STRING Name, Value;
    DWORD ReturnValue;

    Unicode.Buffer = NULL;
    Name.Buffer = NULL;
    Value.Buffer = NULL;
    ReturnValue = 0;
    try {
        InitOemString(&OemString, lpName);
        Status = RtlOemStringToUnicodeString(&Unicode, &OemString, TRUE);
        if ( !NT_SUCCESS(Status) ) {
            if ( Status == STATUS_BUFFER_OVERFLOW ) {
                SetLastError(ERROR_FILENAME_EXCED_RANGE);
                }
            else {
                BaseSetLastNTError(Status);
                }
            return 0;
            }

        Status = RtlUnicodeStringToAnsiString( &Name, &Unicode, TRUE );
        if (!NT_SUCCESS( Status )) {
            BaseSetLastNTError( Status );
            goto try_exit;
            }
        RtlFreeUnicodeString( &Unicode );
        Unicode.Buffer = NULL;

        if (ARGUMENT_PRESENT( lpValue )) {
            InitOemString(&OemString, lpValue);
            Status = RtlOemStringToUnicodeString(&Unicode, &OemString, TRUE);
            if ( !NT_SUCCESS(Status) ) {
                if ( Status == STATUS_BUFFER_OVERFLOW ) {
                    SetLastError(ERROR_FILENAME_EXCED_RANGE);
                    }
                else {
                    BaseSetLastNTError(Status);
                    }
                return 0;
                }

            Status = RtlUnicodeStringToAnsiString( &Value, &Unicode, TRUE );
            if (!NT_SUCCESS( Status )) {
                BaseSetLastNTError( Status );
                goto try_exit;
                }

            }

        ReturnValue = SetEnvironmentVariableA( Name.Buffer,
                                               Value.Buffer
                                             );
try_exit:;
        }
    finally {
        if (Unicode.Buffer != NULL) {
            RtlFreeUnicodeString( &Unicode );
            }

        if (Name.Buffer != NULL) {
            RtlFreeAnsiString( &Name );
            }

        if (Value.Buffer != NULL) {
            RtlFreeAnsiString( &Value );
            }
        }

    return( ReturnValue );
}



DWORD
WINAPI
ExpandEnvironmentStringsOem(
    LPSTR lpSrc,
    LPSTR lpDst,
    DWORD cchDst
    )
/*++

    OEM thunk to ExpandEnvironmentStrings

--*/

{

    NTSTATUS Status;
    UNICODE_STRING Unicode;
    OEM_STRING     OemString;
    ANSI_STRING Name, Value;
    DWORD ReturnValue;

    if (!ARGUMENT_PRESENT(lpSrc)){
	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
    }

    Unicode.Buffer = NULL;
    Name.Buffer = NULL;
    Value.Buffer = NULL;
    ReturnValue = 0;
    try {
	InitOemString(&OemString, lpSrc);
        Status = RtlOemStringToUnicodeString(&Unicode, &OemString, TRUE);
        if ( !NT_SUCCESS(Status) ) {
            if ( Status == STATUS_BUFFER_OVERFLOW ) {
                SetLastError(ERROR_FILENAME_EXCED_RANGE);
		}
            else {
                BaseSetLastNTError(Status);
                }
            return 0;
            }

        Status = RtlUnicodeStringToAnsiString( &Name, &Unicode, TRUE );
        if (!NT_SUCCESS( Status )) {
            BaseSetLastNTError( Status );
            goto try_exit;
            }
        RtlFreeUnicodeString( &Unicode );
        Unicode.Buffer = NULL;

	ReturnValue = ExpandEnvironmentStrings( Name.Buffer,
						lpDst,
						cchDst
					      );
	if (ReturnValue != 0 && ReturnValue <= cchDst) {
	    ReturnValue = 0;
#line 804
	    RtlInitString(&Value, lpDst);
	    Status = RtlAnsiStringToUnicodeString(&Unicode, &Value, TRUE);
            Value.Buffer = NULL;
#line 806
            if ( !NT_SUCCESS(Status) ) {
                if ( Status == STATUS_BUFFER_OVERFLOW ) {
                    SetLastError(ERROR_FILENAME_EXCED_RANGE);
                    }
                else {
                    BaseSetLastNTError(Status);
                    }
		goto try_exit;
                }
	    Status = RtlUnicodeStringToOemString( &Value, &Unicode, TRUE );
            if (!NT_SUCCESS( Status )) {
                BaseSetLastNTError( Status );
                goto try_exit;
                }

            ReturnValue = Value.Length + 1;
            if (ReturnValue <= cchDst)
                RtlMoveMemory(lpDst, Value.Buffer, ReturnValue);

#line 821
            }
try_exit:;
        }
    finally {
        if (Unicode.Buffer != NULL) {
            RtlFreeUnicodeString( &Unicode );
            }

        if (Name.Buffer != NULL) {
            RtlFreeAnsiString( &Name );
            }

        if (Value.Buffer != NULL) {
            RtlFreeAnsiString( &Value );
            }
        }

    return( ReturnValue );
}


UINT
WINAPI
GetShortPathNameOem(
    LPSTR lpSrc,
    LPSTR lpDst,
    DWORD cchDst
    )
/*++

    OEM thunk to GetShortPathNameW

--*/

{

	UNICODE_STRING	UString = { 0 }, UStringRet;
    OEM_STRING	   OemString;
    NTSTATUS	    Status;
	LPWSTR	    lpDstW = NULL;
	DWORD	    ReturnValue = 0;
    DWORD UnicodeCapacity;

#line 863
    if (lpSrc == NULL) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return 0;
	}
    try {
	InitOemString(&OemString, lpSrc);
	Status = RtlOemStringToUnicodeString(&UString,
					     &OemString,
					     TRUE
					     );
	if (!NT_SUCCESS(Status)){
	    BaseSetLastNTError(Status);
	    return 0;
	    }
	UnicodeCapacity = GetShortPathNameW(UString.Buffer,NULL,0);
    if (!UnicodeCapacity) return 0;
    if (UnicodeCapacity > 32767) { BaseSetLastNTError(STATUS_BUFFER_OVERFLOW); return 0; }
#line 878
	    lpDstW = RtlAllocateHeap(RtlProcessHeap(), 0,
					UnicodeCapacity * sizeof(WCHAR)
					);
	    if (lpDstW == NULL) {
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return 0;
		}
#line 890
	ReturnValue = GetShortPathNameW(UString.Buffer,
					lpDstW,
					UnicodeCapacity
					);
	if (ReturnValue != 0 && ReturnValue < UnicodeCapacity) {
        RtlInitUnicodeString(&UStringRet,lpDstW);
        ReturnValue = RtlUnicodeStringToOemSize(&UStringRet);
	    if (ARGUMENT_PRESENT(lpDst) && ReturnValue <= cchDst) {
#line 896
		OemString.Buffer = lpDst;
		OemString.MaximumLength = (USHORT)min(cchDst,65535);
#line 900
		Status = RtlUnicodeStringToOemString(&OemString,
						     &UStringRet,
						     FALSE
						     );
		if (!NT_SUCCESS(Status)) {
		    BaseSetLastNTError(Status);
		    ReturnValue = 0;
#line 906
		    return 0;
		    }
        ReturnValue = OemString.Length;
#line 908
		}
	    }
    else if (ReturnValue) {
        BaseSetLastNTError(STATUS_BUFFER_OVERFLOW);
        ReturnValue = 0;
        }
#line 910
	}
    finally {
	    RtlFreeUnicodeString(&UString);
	    RtlFreeHeap(RtlProcessHeap(), 0, lpDstW);
	    return ReturnValue;
	}
}

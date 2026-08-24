/*
 * Re-rooted from OpenNT base/mvdm/vdmredir/vrnmpipe.c.
 *
 * The reached DEM interception surface retains the OpenNT function names,
 * return values, and named-pipe record ordering.  VDD/ICA async completion
 * and network/DLC entries remain out of this synchronous helper unit; their
 * BOP delivery composition is owned by opennt-bop ingress.
 */
#include "vrnmpipe_compat.h"

#include <stdlib.h>
#include <string.h>

/* OpenNT imported these private product-header spellings.  They are local to
 * the recovered source unit, rather than being exported into other OpenNT
 * translation units. */
#define IS_ASCII_PATH_SEPARATOR(ch) (((ch) == '/') || ((ch) == '\\'))
#define LOCAL_DEVICE_PREFIX "\\\\."
#define ARGUMENT_PRESENT(value) ((value) != NULL)
#define ASSERT(value) ((void)0)
#define ROUND_UP_COUNT(count, alignment) \
    (((count) + ((alignment) - 1u)) & ~((alignment) - 1u))

typedef struct _OPEN_NAMED_PIPE_INFO {
    struct _OPEN_NAMED_PIPE_INFO *Next;
    HANDLE Handle;
    DWORD NameLength;
    WORD DosPdb;
    CHAR Name[2];
} OPEN_NAMED_PIPE_INFO, *POPEN_NAMED_PIPE_INFO;

/* Kept in the original local shape so the synchronous read/write bodies below
 * retain their historical call and failure order. */
typedef struct _OVERLAPPED_PIPE_IO {
    struct _OVERLAPPED_PIPE_IO *Next;
    DWORD Thread;
    BOOL Cancelled;
    OVERLAPPED Overlapped;
} OVERLAPPED_PIPE_IO, *POVERLAPPED_PIPE_IO;

static POPEN_NAMED_PIPE_INFO OpenNamedPipeInfoList;
static POPEN_NAMED_PIPE_INFO LastOpenNamedPipeInfo;

static VOID RememberPipeIo(POVERLAPPED_PIPE_IO PipeIo)
{
    /* DIVERGENCE(HOST-DIV-015): the original body links this stack record to
     * NTVDM's VDD cancellation list.  The standalone session cannot expose a
     * raw native pointer to a product-global VDD list, so this bounded,
     * synchronous path retains the record shape and starts un-cancelled. */
    PipeIo->Cancelled = FALSE;
}

static VOID ForgetPipeIo(POVERLAPPED_PIPE_IO PipeIo)
{
    /* See HOST-DIV-015 at RememberPipeIo.  There is no VDD list to unlink
     * from; keeping this source-order call makes the missing lifecycle
     * explicit rather than silently changing the I/O control flow. */
    (void)PipeIo;
}

static POPEN_NAMED_PIPE_INFO VrpGetOpenNamedPipeInfo(HANDLE Handle)
{
    POPEN_NAMED_PIPE_INFO ptr;
    for (ptr = OpenNamedPipeInfoList; ptr != NULL; ptr = ptr->Next) {
        if (ptr->Handle == Handle) {
            break;
        }
    }
    return ptr;
}

static BOOL VrpAddOpenNamedPipeInfo(HANDLE Handle, LPSTR Name)
{
    POPEN_NAMED_PIPE_INFO PipeInfo;
    DWORD NameLength;

    NameLength = (DWORD)strlen(Name) + 1u;
    PipeInfo = (POPEN_NAMED_PIPE_INFO)
                LocalAlloc(LMEM_FIXED,
                    ROUND_UP_COUNT((sizeof(OPEN_NAMED_PIPE_INFO) + NameLength),
                        sizeof(DWORD)
                        )
                    );
    if (PipeInfo == NULL) {
        return FALSE;
    }

    PipeInfo->Next = NULL;
    PipeInfo->Handle = Handle;
    PipeInfo->NameLength = NameLength;
    strcpy(PipeInfo->Name, Name);
    if (LastOpenNamedPipeInfo == NULL) {
        OpenNamedPipeInfoList = PipeInfo;
    } else {
        LastOpenNamedPipeInfo->Next = PipeInfo;
    }
    LastOpenNamedPipeInfo = PipeInfo;
    return TRUE;
}

static BOOL VrpRemoveOpenNamedPipeInfo(HANDLE Handle)
{
    POPEN_NAMED_PIPE_INFO ptr, prev = NULL;
    for (ptr = OpenNamedPipeInfoList; ptr; ) {
        if (ptr->Handle == Handle) {
            if (!prev) {
                OpenNamedPipeInfoList = ptr->Next;
            } else {
                prev->Next = ptr->Next;
            }
            if (LastOpenNamedPipeInfo == ptr) {
                LastOpenNamedPipeInfo = prev;
            }
            LocalFree(ptr);
            return TRUE;
        }
        prev = ptr;
        ptr = ptr->Next;
    }
    return FALSE;
}

VOID VrTerminateNamedPipes(VOID)
{
    /* DIVERGENCE(HOST-DIV-021): the original
     * VrTerminateNamedPipes(DosPdb) source body is empty.  This static
     * single-session boundary still must retire its local metadata at session
     * teardown; it is not a claimed recovery of the historical DOS-PDB hook. */
    while (OpenNamedPipeInfoList != NULL)
        (void)VrpRemoveOpenNamedPipeInfo(OpenNamedPipeInfoList->Handle);
}

BOOL VrIsNamedPipeName(LPSTR Name)
{
    int CharCount;

    if (IS_ASCII_PATH_SEPARATOR(*Name)) {
        ++Name;
        if (IS_ASCII_PATH_SEPARATOR(*Name)) {
            ++Name;
            CharCount = 0;
            while (*Name && !IS_ASCII_PATH_SEPARATOR(*Name)) {
                ++Name;
                ++CharCount;
            }
            if (!CharCount || !*Name) {
                return FALSE;
            }
            ++Name;
        } else {
            return FALSE;
        }

        if (!_strnicmp(Name, "PIPE", 4)) {
            Name += 4;
            if (IS_ASCII_PATH_SEPARATOR(*Name)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL VrAddOpenNamedPipeInfo(HANDLE Handle, LPSTR lpFileName)
{
    BOOL ok;
    if (VrIsNamedPipeName(lpFileName)) {
        ok = VrpAddOpenNamedPipeInfo(Handle, lpFileName);
    } else {
        ok = FALSE;
    }
    return ok;
}

BOOL VrRemoveOpenNamedPipeInfo(HANDLE Handle)
{
    /* The non-DBG OpenNT body removes opportunistically and returns TRUE;
     * close of an ordinary DOS handle is therefore not redirected into a
     * new failure path. */
    (void)VrpRemoveOpenNamedPipeInfo(Handle);
    return TRUE;
}
BOOL VrIsNamedPipeHandle(HANDLE Handle) { return VrpGetOpenNamedPipeInfo(Handle) != NULL; }

BOOL VrReadNamedPipe(HANDLE Handle, LPBYTE Buffer, DWORD Buflen,
    LPDWORD BytesRead, LPDWORD Error)
{
    OVERLAPPED_PIPE_IO pipeio;
    BOOL success;
    DWORD error;
    /* DIVERGENCE(HOST-DIV-023): OpenNT leaves this local uninitialized until
     * GetOverlappedResult.  Current /W4 /WX correctly rejects the possible
     * later read on a failed provider; zero is not observable on a successful
     * transfer and preserves the original error/result ordering. */
    DWORD dwBytesRead = 0u;

    ZeroMemory(&pipeio, sizeof(pipeio));
    if ((pipeio.Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL) {
        *Error = ERROR_NOT_ENOUGH_MEMORY;
        return FALSE;
    }

    RememberPipeIo(&pipeio);
    success = ReadFile(Handle, Buffer, Buflen, BytesRead, &pipeio.Overlapped);
    if (!success) {
        error = GetLastError();
        if (error == ERROR_IO_PENDING) {
            error = WaitForSingleObject(pipeio.Overlapped.hEvent, INFINITE);
            if (error == 0xffffffffu) {
                error = GetLastError();
            } else {
                success = (error == WAIT_OBJECT_0);
            }
        } else if (error == ERROR_MORE_DATA) {
            success = TRUE;
        }
    } else {
        error = NO_ERROR;
    }

    ForgetPipeIo(&pipeio);
    if (pipeio.Cancelled) {
        error = WAIT_TIMEOUT;
        success = FALSE;
    }

    if (success) {
        success = GetOverlappedResult(Handle, &pipeio.Overlapped, &dwBytesRead, FALSE);
        error = success ? NO_ERROR : GetLastError();
        if (error == ERROR_MORE_DATA) {
            success = TRUE;
        }
    } else if (error == WAIT_TIMEOUT) {
        /* DIVERGENCE(HOST-DIV-016): original VDMREDIR owns the raw handle and
         * removes its private record here.  In this composition the session
         * owns the handle; only bounded session teardown may close/retire it. */
    }

    CloseHandle(pipeio.Overlapped.hEvent);

    if (error == NO_ERROR && dwBytesRead == 0u) {
        error = ERROR_NO_DATA;
        success = FALSE;
    }

    if (!success) {
        SetLastError(error);
    } else {
        *BytesRead = dwBytesRead;
    }

    *Error = error;
    return success;
}

BOOL VrWriteNamedPipe(HANDLE Handle, LPBYTE Buffer, DWORD Buflen,
    LPDWORD BytesWritten)
{
    OVERLAPPED_PIPE_IO pipeio;
    BOOL success;
    DWORD error;

    ZeroMemory(&pipeio, sizeof(pipeio));
    if ((pipeio.Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL) {
        error = ERROR_NOT_ENOUGH_MEMORY;
        success = FALSE;
    } else {
        RememberPipeIo(&pipeio);
        success = WriteFile(Handle, Buffer, Buflen, BytesWritten, &pipeio.Overlapped);
        error = success ? NO_ERROR : GetLastError();
        if (error == ERROR_IO_PENDING) {
            error = WaitForSingleObject(pipeio.Overlapped.hEvent, INFINITE);
            if (error == 0xffffffffu) {
                error = GetLastError();
            } else {
                success = (error == WAIT_OBJECT_0);
            }
        }
        ForgetPipeIo(&pipeio);
        if (pipeio.Cancelled) {
            error = WAIT_TIMEOUT;
            success = FALSE;
        }
    }
    if (success) {
        (void)GetOverlappedResult(Handle, &pipeio.Overlapped, BytesWritten, FALSE);
    }
    CloseHandle(pipeio.Overlapped.hEvent);
    if (!success) SetLastError(error);
    return success;
}

LPSTR VrConvertLocalNtPipeName(LPSTR Buffer, LPSTR Name)
{
    DWORD prefixLength;
    DWORD pipeLength;
    LPSTR pipeName;
    static char ThisComputerName[MAX_COMPUTERNAME_LENGTH + 1] = {0};
    static DWORD ThisComputerNameLength = 0xffffffff;
    BOOLEAN mapped = FALSE;

    ASSERT(Name);
    ASSERT(IS_ASCII_PATH_SEPARATOR(Name[0]) && IS_ASCII_PATH_SEPARATOR(Name[1]));

    if (ThisComputerNameLength == 0xffffffff) {
        ThisComputerNameLength = sizeof(ThisComputerName);
        if (!GetComputerName((LPTSTR)&ThisComputerName, &ThisComputerNameLength)) {
            ThisComputerNameLength = 0;
        }
    }

    if (!ARGUMENT_PRESENT(Buffer)) {
        Buffer = (LPSTR)LocalAlloc(LMEM_FIXED, strlen(Name) + 1);
    }

    if (Buffer) {
        pipeName = strchr(Name + 2, '\\');
        if (!pipeName) {
            pipeName = strchr(Name + 2, '/');
        }
        ASSERT(pipeName);
        pipeLength = (DWORD)strlen(pipeName);
        /* DIVERGENCE(HOST-DIV-020): OpenNT narrowed two flat pointers to
         * DWORD before subtracting, assigned `strlen` directly to DWORD, and
         * used assignment-as-condition. Preserve their values while using
         * defined x86/x64 arithmetic and warning-clean source spelling. */
        prefixLength = (DWORD)(pipeName - Name);
        if (ThisComputerNameLength && (prefixLength - 2 == ThisComputerNameLength)) {
            if (!_strnicmp(ThisComputerName, &Name[2], ThisComputerNameLength)) {
                strcpy(Buffer, LOCAL_DEVICE_PREFIX);
                mapped = TRUE;
            }
        }
        if (!mapped) {
            strncpy(Buffer, Name, prefixLength);
            Buffer[prefixLength] = 0;
        }
        strcat(Buffer, pipeName);

        do {
            if ((pipeName = strchr(Buffer, '/')) != NULL) {
                *pipeName++ = '\\';
            }
        } while (pipeName);
        (void)pipeLength;
    }

    return Buffer;
}

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

static POPEN_NAMED_PIPE_INFO OpenNamedPipeInfoList;
static POPEN_NAMED_PIPE_INFO LastOpenNamedPipeInfo;

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
    /* DIVERGENCE(HOST-DIV-015): OpenNT also registers this OVERLAPPED record with the
     * NTVDM VDD cancellation list.  That product-global list has no public
     * modern counterpart; the bounded session ingress owns cancellation and
     * never exposes this native pointer beyond the synchronous call. */
    OVERLAPPED overlap;
    DWORD transferred = 0u, error = NO_ERROR;
    BOOL success;
    if (BytesRead != NULL) *BytesRead = 0u;
    if (Error != NULL) *Error = ERROR_INVALID_HANDLE;
    if (!VrIsNamedPipeHandle(Handle)) { SetLastError(ERROR_INVALID_HANDLE); return FALSE; }
    memset(&overlap, 0, sizeof(overlap));
    overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (overlap.hEvent == NULL) { if (Error) *Error = ERROR_NOT_ENOUGH_MEMORY; return FALSE; }
    success = ReadFile(Handle, Buffer, Buflen, &transferred, &overlap);
    if (!success) {
        error = GetLastError();
        if (error == ERROR_IO_PENDING) {
            error = WaitForSingleObject(overlap.hEvent, INFINITE);
            if (error == 0xffffffffu) {
                error = GetLastError();
            } else {
                success = error == WAIT_OBJECT_0;
            }
        } else if (error == ERROR_MORE_DATA) success = TRUE;
    }
    if (success) {
        success = GetOverlappedResult(Handle, &overlap, &transferred, FALSE);
        error = success ? NO_ERROR : GetLastError();
        if (error == ERROR_MORE_DATA) success = TRUE;
    }
    CloseHandle(overlap.hEvent);
    /* DIVERGENCE(HOST-DIV-016): the original WAIT_TIMEOUT branch closes the native handle
     * and removes its VDMREDIR record.  This handle is session-manager owned
     * here, so only its owner may close it during bounded teardown. */
    if (success && error == NO_ERROR && transferred == 0u) { error = ERROR_NO_DATA; success = FALSE; }
    if (BytesRead) *BytesRead = transferred;
    if (Error) *Error = error;
    if (!success) SetLastError(error);
    return success;
}

BOOL VrWriteNamedPipe(HANDLE Handle, LPBYTE Buffer, DWORD Buflen,
    LPDWORD BytesWritten)
{
    OVERLAPPED overlap;
    DWORD transferred = 0u, error;
    BOOL success;
    if (BytesWritten) *BytesWritten = 0u;
    if (!VrIsNamedPipeHandle(Handle)) { SetLastError(ERROR_INVALID_HANDLE); return FALSE; }
    memset(&overlap, 0, sizeof(overlap));
    overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (overlap.hEvent == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    success = WriteFile(Handle, Buffer, Buflen, &transferred, &overlap);
    error = success ? NO_ERROR : GetLastError();
    if (!success && error == ERROR_IO_PENDING) {
        success = WaitForSingleObject(overlap.hEvent, INFINITE) == WAIT_OBJECT_0;
        if (success) success = GetOverlappedResult(Handle, &overlap, &transferred, FALSE);
        error = success ? NO_ERROR : GetLastError();
    }
    CloseHandle(overlap.hEvent);
    if (BytesWritten) *BytesWritten = transferred;
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

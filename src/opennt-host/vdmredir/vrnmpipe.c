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

typedef struct _OPEN_NAMED_PIPE_INFO {
    struct _OPEN_NAMED_PIPE_INFO *Next;
    HANDLE Handle;
    LPSTR Name;
} OPEN_NAMED_PIPE_INFO, *POPEN_NAMED_PIPE_INFO;

static BOOL IsVrInitialized;
static POPEN_NAMED_PIPE_INFO OpenNamedPipeInfo;

static POPEN_NAMED_PIPE_INFO VrpGetOpenNamedPipeInfo(HANDLE Handle)
{
    POPEN_NAMED_PIPE_INFO ptr;
    for (ptr = OpenNamedPipeInfo; ptr != NULL; ptr = ptr->Next) {
        if (ptr->Handle == Handle) return ptr;
    }
    return NULL;
}

static BOOL VrpAddOpenNamedPipeInfo(HANDLE Handle, LPSTR Name)
{
    size_t bytes;
    POPEN_NAMED_PIPE_INFO ptr;
    if (VrpGetOpenNamedPipeInfo(Handle) != NULL) return TRUE;
    ptr = (POPEN_NAMED_PIPE_INFO)calloc(1u, sizeof(*ptr));
    if (ptr == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    bytes = strlen(Name) + 1u;
    ptr->Name = (LPSTR)LocalAlloc(LMEM_FIXED, bytes);
    if (ptr->Name == NULL) { free(ptr); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    memcpy(ptr->Name, Name, bytes);
    ptr->Handle = Handle;
    ptr->Next = OpenNamedPipeInfo;
    OpenNamedPipeInfo = ptr;
    return TRUE;
}

static BOOL VrpRemoveOpenNamedPipeInfo(HANDLE Handle)
{
    POPEN_NAMED_PIPE_INFO *link = &OpenNamedPipeInfo;
    while (*link != NULL) {
        POPEN_NAMED_PIPE_INFO ptr = *link;
        if (ptr->Handle == Handle) {
            *link = ptr->Next;
            LocalFree(ptr->Name);
            free(ptr);
            return TRUE;
        }
        link = &ptr->Next;
    }
    return FALSE;
}

BOOL VrInitialized(void) { return IsVrInitialized; }
BOOL VrInitialize(void) { IsVrInitialized = TRUE; return TRUE; }
VOID VrUninitialize(void)
{
    while (OpenNamedPipeInfo != NULL) (void)VrpRemoveOpenNamedPipeInfo(OpenNamedPipeInfo->Handle);
    IsVrInitialized = FALSE;
}

BOOL VrIsNamedPipeName(LPSTR Name)
{
    int CharCount = 0;
    if (Name == NULL || (*Name != '\\' && *Name != '/')) return FALSE;
    ++Name;
    if (*Name != '\\' && *Name != '/') return FALSE;
    ++Name;
    while (*Name && *Name != '\\' && *Name != '/') { ++Name; ++CharCount; }
    if (!CharCount || !*Name) return FALSE;
    ++Name;
    return _strnicmp(Name, "PIPE", 4) == 0 && (Name[4] == '\\' || Name[4] == '/');
}

BOOL VrAddOpenNamedPipeInfo(HANDLE Handle, LPSTR lpFileName)
{
    if (!VrInitialized() || !VrIsNamedPipeName(lpFileName)) return FALSE;
    return VrpAddOpenNamedPipeInfo(Handle, lpFileName);
}

BOOL VrRemoveOpenNamedPipeInfo(HANDLE Handle) { return VrpRemoveOpenNamedPipeInfo(Handle); }
BOOL VrIsNamedPipeHandle(HANDLE Handle) { return VrpGetOpenNamedPipeInfo(Handle) != NULL; }

BOOL VrReadNamedPipe(HANDLE Handle, LPBYTE Buffer, DWORD Buflen,
    LPDWORD BytesRead, LPDWORD Error)
{
    /* DIVERGENCE: OpenNT also registers this OVERLAPPED record with the
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
            success = WaitForSingleObject(overlap.hEvent, INFINITE) == WAIT_OBJECT_0;
            if (success) success = GetOverlappedResult(Handle, &overlap, &transferred, FALSE);
            error = success ? NO_ERROR : GetLastError();
        } else if (error == ERROR_MORE_DATA) success = TRUE;
    }
    CloseHandle(overlap.hEvent);
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
    DWORD computer_bytes = MAX_COMPUTERNAME_LENGTH + 1u;
    CHAR computer[MAX_COMPUTERNAME_LENGTH + 1u];
    LPSTR pipe, result;
    size_t bytes;
    if (Name == NULL || !VrInitialized()) { SetLastError(ERROR_INVALID_FUNCTION); return NULL; }
    pipe = strchr(Name + 2, '\\');
    if (pipe == NULL) pipe = strchr(Name + 2, '/');
    if (pipe == NULL) { SetLastError(ERROR_BAD_PATHNAME); return NULL; }
    if (!GetComputerNameA(computer, &computer_bytes) ||
        _strnicmp(Name + 2, computer, (size_t)(pipe - (Name + 2))) != 0) return Name;
    bytes = strlen(pipe) + 4u;
    result = Buffer != NULL ? Buffer : (LPSTR)LocalAlloc(LMEM_FIXED, bytes);
    if (result == NULL) return NULL;
    memcpy(result, "\\\\.", 3u);
    strcpy(result + 3u, pipe);
    return result;
}

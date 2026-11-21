/* Modern capability binding for BaseCheckVDM's original CONSOLE_HANDLE
 * predicate (OpenNT public/internal/base/inc/conroute.h).  Retain the
 * original low-bit pseudo-handle identity as well as recognizing a modern
 * Console object at the adapter resource boundary. */
#include "include/base_console_handle.h"
BOOL OpenNtBaseIsConsoleHandle(HANDLE handle)
{
    enum { opennt_console_handle_signature = 0x00000003u };
    DWORD mode, saved=GetLastError();
    BOOL result=((ULONG_PTR)handle & opennt_console_handle_signature) ==
        opennt_console_handle_signature || GetConsoleMode(handle,&mode);
    SetLastError(saved);
    return result;
}

DWORD OpenNtBaseAdoptStandardHandles(HANDLE streams[3],
    ULONG pipe_mask, ULONG pipe_count, HANDLE *pipes,
    ULONG file_mask, ULONG file_count, HANDLE *files)
{
    HANDLE selected[3] = {0};
    ULONG p = 0, f = 0, i;
    ULONG mask = pipe_mask | file_mask;

    if ((mask & ~7u) || (pipe_mask & file_mask))
        return ERROR_INVALID_DATA;
    for (i = 0; i < 3; ++i) {
        ULONG bit = 1u << i;
        if (!!streams[i] != !!(mask & bit))
            return ERROR_INVALID_DATA;
        if (pipe_mask & bit) {
            if (!pipes || p >= pipe_count) return ERROR_INVALID_DATA;
            selected[i] = pipes[p++];
        } else if (file_mask & bit) {
            if (!files || f >= file_count) return ERROR_INVALID_DATA;
            selected[i] = files[f++];
        }
    }
    if (p != pipe_count || f != file_count) return ERROR_INVALID_DATA;
    for (i = 0; i < 3; ++i) {
        if (selected[i] && !SetHandleInformation(selected[i],
                HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
            return GetLastError();
    }
    CopyMemory(streams, selected, sizeof(selected));
    if (p) ZeroMemory(pipes, p * sizeof(*pipes));
    if (f) ZeroMemory(files, f * sizeof(*files));
    return ERROR_SUCCESS;
}

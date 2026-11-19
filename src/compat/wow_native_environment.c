#include "include/wow_native_environment.h"

BOOL WINAPI wow_edit_native_ensure_gui_thread(void)
{
    DWORD saved = GetLastError(), error;
    BOOL converted;
    /* Original ConnectIfNecessary establishes the native USER thread, not
     * a WOW task or our metadata domain. Use its public host owner rather
     * than NT4 TEB offsets or NtUserGetThreadState's private -1 operation. */
    if (IsGUIThread(FALSE)) { SetLastError(saved); return TRUE; }
    SetLastError(ERROR_SUCCESS);
    converted = IsGUIThread(TRUE);
    error = GetLastError();
    /* The documented conversion failure can be the nonzero error value 8.
     * Query actual GUI-thread state instead of treating any nonzero result
     * as success. Do not keep another cached connection flag. */
    if (!IsGUIThread(FALSE)) {
        SetLastError(converted == ERROR_NOT_ENOUGH_MEMORY ?
            ERROR_NOT_ENOUGH_MEMORY : (error ? error : ERROR_NOT_READY));
        return FALSE;
    }
    SetLastError(saved);
    return TRUE;
}

PVOID WINAPI wow_edit_native_highest_user_address(void)
{
    DWORD error = GetLastError();
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    SetLastError(error);
    return info.lpMaximumApplicationAddress;
}


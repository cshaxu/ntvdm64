#include "include/wow_icon_size.h"
#include <limits.h>

BOOL WINAPI wow_control_icon_size(HICON icon, UINT step, int *width, int *height)
{
    ICONINFO info = {0};
    BITMAP bitmap;
    BOOL result = FALSE;
    DWORD error;
    /* All recovered consumers request frame zero; no private cursor layout. */
    if (step || !width || !height) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!GetIconInfo(icon, &info)) return FALSE;
    if (GetObjectW(info.hbmColor ? info.hbmColor : info.hbmMask,
            sizeof(bitmap), &bitmap) == sizeof(bitmap) && bitmap.bmWidth > 0 &&
            bitmap.bmHeight > 0 && (!info.hbmColor || bitmap.bmHeight <= INT_MAX / 2)) {
        *width = bitmap.bmWidth;
        *height = info.hbmColor ? bitmap.bmHeight * 2 : bitmap.bmHeight;
        result = TRUE;
    }
    error = GetLastError();
    if (info.hbmColor) DeleteObject(info.hbmColor);
    if (info.hbmMask) DeleteObject(info.hbmMask);
    SetLastError(error);
    return result;
}

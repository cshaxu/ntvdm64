#include "include/pif_path.h"
#include <windows.h>
#include <string.h>

int mvdm_softpc_config_path_to_ansi(char *path, uint32_t capacity)
{
    WCHAR unicode[MAX_PATH + 12];
    int chars;

    /* GetPIFConfigFiles feeds ANSI CreateFile callers in cmdconf/nt_msscs.
     * Preserve the original OEM expansion; cross encoding only here. */
    if (path == NULL || capacity == 0 || capacity > MAX_PATH + 12 ||
        memchr(path, '\0', capacity) == NULL) return 0;
    chars = MultiByteToWideChar(CP_OEMCP, 0, path, -1, unicode,
        sizeof(unicode) / sizeof(unicode[0]));
    if (!chars) return 0;
    return WideCharToMultiByte(CP_ACP, 0, unicode, chars, path,
        (int)capacity, NULL, NULL) != 0;
}

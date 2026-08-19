/* OpenNT source mirror: base/mvdm/dos/dem/demmsg.c. */

#include <windows.h>

#include "dem_shared.h"

static const char *const aMsg[] = {
    "DOS Location Not Found. Using Default.\n",
    "Read On NTDOS.SYS Failed.\n",
    "Open On NTDOS.SYS Failed.\n",
    "EAs Not Supported\n",
    "Letter mismatch in Set_Default_Drive\n",
    "Volume ID support is missing\n",
    "Invalid Date Time Format for NT\n",
    "DTA has an Invalid Find Handle for FINDNEXT\n",
    "Unexpected failure to get file information\n",
    "File Size is too big for DOS\n"
};

void demPrintMsg(uint32_t iMsg)
{
    if (fShowSVCMsg != 0u && iMsg < (uint32_t)(sizeof(aMsg) / sizeof(aMsg[0]))) {
        /* Divergence: the historical debug buffer was consumed by NTVDM's
         * debug host.  Direct CLI diagnostic output uses the supported Win32
         * debug sink and preserves the original message selection. */
        OutputDebugStringA(aMsg[iMsg]);
    }
}

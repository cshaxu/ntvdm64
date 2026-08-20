/* Compatibility globals for directly imported OpenNT cmdenv.c.
 * They replace only the non-composable command.lib/SoftPC global closure;
 * cmdGetInitEnvironment itself remains in the original translation unit. */

#include "command_misc_shim.h"

BOOL fSeparateWow;
CHAR comspec[] = "COMSPEC=";
CHAR *lpszzInitEnvironment;
WORD cchInitEnvironment;
CHAR *lpszzVDMEnv32;
DWORD cchVDMEnv32;

void RcMessageBox(UINT error, PVOID first, PVOID second, UINT flags)
{
    (void)error; (void)first; (void)second; (void)flags;
}

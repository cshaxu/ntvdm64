/* cmd.c - Main Module of Command.lib
 *
 * True source subset of OpenNT base/mvdm/dos/command/cmd.c.
 */

/* DIVERGENCE(BOP-DIV-101): the source's cmd.h/cmdsvc.h include closure
 * reached the NT4 host GetSystemDirectory entry. Keep the exact CMDInit
 * algorithm and call shape through the declared same-shaped facade. */
#include "opennt_command_composition.h"

BOOL CMDInit (int argc, char *argv[])
{
CHAR  RootDir [MAX_PATH];
UINT  Len;

    (void)argc;
    (void)argv;
    Len = GetSystemDirectory (RootDir,MAX_PATH);
    if (Len <= MAX_PATH && Len > 0)
        cmdHomeDirectory[0] = RootDir[0];
    return TRUE;
}

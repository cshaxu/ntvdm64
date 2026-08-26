/* cmddata.c - Misc. SCS global data
 *
 * True source subset of OpenNT base/mvdm/dos/command/cmddata.c.
 */

/* DIVERGENCE(BOP-DIV-100): cmd.h and mvdm.h carried the historical product
 * include closure. The declared composition header supplies the same source
 * types and external interface without changing the original global names,
 * storage, initialization or ownership. */
#include "opennt_command_composition.h"

CHAR     lpszComSpec[64+8];
USHORT   cbComSpec=0;
BOOL     IsFirstCall = TRUE;
BOOL     IsRepeatCall = FALSE;
BOOL     IsFirstWOWCheckBinary = TRUE;
BOOL     IsFirstVDMInSystem = FALSE;
BOOL     SaveWorldCreated;
PCHAR    pSCS_ToSync;
PSCSINFO pSCSInfo;
BOOL     fBlock = FALSE;
PCHAR    pCommand32;
PCHAR    pEnv32;
DWORD    dwExitCode32;
CHAR     cmdHomeDirectory [] = "C:\\";
CHAR     chDefaultDrive;
CHAR     comspec[]="COMSPEC=";
BOOL     fSoftpcRedirection;
BOOL     fSoftpcRedirectionOnShellOut;
CHAR     ShortCutInfo[MAX_SHORTCUT_SIZE];
BOOL     DosEnvCreated = FALSE;

BOOL     IsFirstVDM = TRUE;
BOOL     DontCheckDosBinaryType = FALSE;
WORD     Exe32ActiveCount = 0;

VDMINFO  VDMInfo;
CHAR     *lpszzInitEnvironment = NULL;
WORD     cchInitEnvironment = 0;
CHAR     *lpszzCurrentDirectories = NULL;
DWORD    cchCurrentDirectories = 0;
BYTE     * pIsDosBinary;
CHAR     *lpszzcmdEnv16 = NULL;
CHAR     *lpszzVDMEnv32 = NULL;
DWORD    cchVDMEnv32;
VDMENVBLK cmdVDMEnvBlk;

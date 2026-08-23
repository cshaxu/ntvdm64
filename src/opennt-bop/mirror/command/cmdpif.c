/*  cmdpif.c - PIF Handling Routines For SCS
 *
 *
 *  Modification History:
 *
 *  Sudeepb 19-Aug-1992 Created
 *  williamh 10-Nov-1992 (1). get params from w386 extention if exist
 *			 (2). null terminate caption
 *  williamh 27-May-1993 almost rewrote for better pif support
 */

/* OpenNT source: base/mvdm/dos/command/cmdpif.c.  The original owner body
 * below is compiled through the smallest COMMAND/PIF host capability seam;
 * the paired original nt_pif.c parser is included by command_opennt_pif_parser.c. */
#include "../../../opennt-host/capability/command_pif_shim.h"
#include <ctype.h>

VOID cmdCheckForPIF (PVDMINFO pvi)
{
PCHAR   pCmdLine = pvi->CmdLine;
PCHAR   pDot;
ULONG   size;
UCHAR   ch;
DWORD   dw;
CHAR	lpszEnvDir [] = "=?:";
CHAR	FullPathName[MAX_PATH + 1];
CHAR	* pFilePart;
BOOL	IsPIFFile, IsFromForceDos;
CHAR	AppFullPathName[MAX_PATH + 1];

    /* DIVERGENCE: these historical locals belonged to removed ForceDos-era
     * branches. Keep source declaration order while making the retained body
     * warning-clean under the formal /W4 /WX build. */
    (void)size;
    (void)ch;
    (void)IsFromForceDos;

    //
    // Advance CmdLine pointer to beg of command tail
    //
    while (*pCmdLine && !isgraph(*pCmdLine)) { // skip to first nonwhite
        pCmdLine++;
        }

    pDot = strrchr(pvi->AppName, '.');
    if (pDot)
	IsPIFFile = pDot && !_strnicmp(pDot, ".pif", 4);
    else
       IsPIFFile = FALSE;


    // if the command is not a pif file and it is not
    // running on a new console
    if (!IsPIFFile && !DosSessionId)
	goto CleanUpAndReturn;

    if (IsPIFFile) {
	if (!IsFirstVDM) {
            //
            // Get the pif data. If no pif data, or not from forcedos
            // just return -- command.com will receive the pif file
            // name and fail.
            //
            pfdata.AppHasPIFFile =
	    pfdata.IgnoreStartDirInPIF =
	    pfdata.IgnoreTitleInPIF =
	    pfdata.IgnoreCmdLineInPIF =
	    pfdata.IgnoreConfigAutoexec = 1;
            if (!GetPIFData(&pfdata, pvi->AppName))
		goto CleanUpAndReturn;
        }

	// we only run a pif file on two occasions:
	// (1). it is from a new console
	// (2). it is from forcedos.
        if (!DosSessionId && pfdata.SubSysId != SUBSYS_DOS)
            goto CleanUpAndReturn;

    }
    if (IsFirstVDM) {
	// if this is the first vdm, take cmdline, startupdir and title
	// if they are there
	if (pfdata.StartDir){
	    dw = cmdExpandEnvironmentStrings(NULL,
					     pfdata.StartDir,
					     FullPathName,
					     MAX_PATH + 1
					     );
            if (dw != 0 &&      dw <= MAX_PATH) {
		dw = GetFullPathNameOem(FullPathName,
					MAX_PATH + 1,
					pfdata.StartDir,
					&pFilePart
                                        );
            }

	    if (dw != 0 && dw <= MAX_PATH)
		dw = GetFileAttributesOem(pfdata.StartDir);
	    else
                dw = 0;

	    if (dw == 0 || dw == 0xFFFFFFFF || !(dw & FILE_ATTRIBUTE_DIRECTORY))
		{
		RcMessageBox(EG_PIF_STARTDIR_ERR,
			     NULL,
			     NULL,
			     RMB_ICON_BANG | RMB_ABORT);
		goto CleanUpAndReturn;
            }

            dw = GetShortPathNameOem(pfdata.StartDir,
                                     pfdata.StartDir,
                                     MAX_PATH + 1
                                     );
	    if (dw == 0 || dw > MAX_PATH || dw > 64) {
		RcMessageBox(EG_PIF_STARTDIR_ERR,
			     NULL,
			     NULL,
			     RMB_ICON_BANG | RMB_ABORT);
		goto CleanUpAndReturn;
            }

	    lpszEnvDir[1] = pfdata.StartDir[0];
	    SetEnvironmentVariableOem(lpszEnvDir, pfdata.StartDir);
	    SetCurrentDirectoryOem(pfdata.StartDir);
	    pvi->CurDrive = (USHORT)(toupper((unsigned char)pfdata.StartDir[0]) - 'A');
        }

	if (pfdata.WinTitle) {
	    strcpy(FullPathName, pfdata.WinTitle);
	    dw = cmdExpandEnvironmentStrings(NULL,
					     FullPathName,
					     pfdata.WinTitle,
					     MAX_PATH + 1
					     );
	    pfdata.WinTitle[MAX_PATH] = '\0';
        }

        if (!*pCmdLine && pfdata.CmdLine) {

	    // if the optinal parameter is '?'
	    // prompt the user
	    pDot = pfdata.CmdLine;
	    while (*pDot && *pDot <= ' ')
                pDot++;

	    if (*pDot == '?') {
		pfdata.CmdLine[0] = '\0';
		RcMessageBox(EG_PIF_ASK_CMDLINE,
			     NULL,
			     pfdata.CmdLine,
			     RMB_EDIT | RMB_ICON_INFO | (128 << 16)
			     );
            }

	    if (*pfdata.CmdLine) {
		strcpy(FullPathName, pfdata.CmdLine);
		dw = cmdExpandEnvironmentStrings(NULL,
						 FullPathName,
						 pfdata.CmdLine,
						 MAX_PATH + 1
						);
	    }
	}
    }


    if(IsPIFFile) {
        dw = cmdExpandEnvironmentStrings(NULL,
                                         pfdata.StartFile,
					 FullPathName,
                                         MAX_PATH + 1
                                        );
        if (!dw || dw > MAX_PATH) {
            RcMessageBox(EG_PIF_STARTFILE_ERR,
                         NULL, NULL, RMB_ICON_BANG | RMB_ABORT);
            goto CleanUpAndReturn;
        }



        // search from the current directory
        // note that the startup directory specified in
        // the pif file has been set as our current directory
        // when we got here
        dw = SearchPathOem(".",
                           FullPathName,
                           NULL,
                           MAX_PATH + 1,
			   AppFullPathName,
                           &pFilePart
                           );
        // if couldn't find the file from the current dir
        // ask win32api help
        if (dw == 0 || dw > MAX_PATH) {
            dw = SearchPathOem(NULL,
                               FullPathName,
                               NULL,
                               MAX_PATH + 1,
			       AppFullPathName,
                               &pFilePart
                               );
        }

        // couldn't find the file, give up
        if (dw == 0 || dw > MAX_PATH) {
            RcMessageBox(EG_PIF_STARTFILE_ERR,
                         NULL, NULL, RMB_ICON_BANG | RMB_ABORT);
            goto CleanUpAndReturn;
        }

	dw = GetFileAttributesOem(AppFullPathName);
        if (dw == (DWORD)(-1) || (dw & FILE_ATTRIBUTE_DIRECTORY)) {
            RcMessageBox(EG_PIF_STARTFILE_ERR, NULL, NULL,
                         RMB_ICON_BANG | RMB_ABORT
                         );
            goto CleanUpAndReturn;
        }

        // convert to shortfilename
	dw = GetShortPathNameOem(AppFullPathName, pvi->AppName,
                                 MAX_PATH + 1);
        if (dw == 0 || dw > MAX_PATH) {

            RcMessageBox(EG_PIF_STARTFILE_ERR, NULL, NULL,
                         RMB_ICON_BANG | RMB_ABORT
                         );
            goto CleanUpAndReturn;
        }
	// update the application path name length(including the terminate NULL)
	/* DIVERGENCE: AppName is bounded by the original MAX_PATH buffer; make
	 * the historical 16-bit CMDINFO field conversion explicit for /W4 /WX. */
	pvi->AppLen = (USHORT)(strlen(pvi->AppName) + 1u);

	// pvi->AppName contains the application short name.
	// verify that it has the correct extension(.EXE, .COM or .BAT).
	pDot = (PCHAR)pvi->AppName + pvi->AppLen - 5;
	if (pvi->AppLen < 5 ||
	    (_strnicmp(pDot, EXE_EXTENTION_STRING, EXTENTION_STRING_LEN) &&
	     _strnicmp(pDot, COM_EXTENTION_STRING, EXTENTION_STRING_LEN) &&
	     _strnicmp(pDot, BAT_EXTENTION_STRING, EXTENTION_STRING_LEN)))
	{

	    RcMessageBox(EG_DOS_PROG_EXTENSION,AppFullPathName, NULL, RMB_ICON_BANG | RMB_ABORT);
	    goto CleanUpAndReturn;

	}
    }


    //
    // Copy in pif command tail if original command tail is empty
    //
    if (!*pCmdLine && pfdata.CmdLine) {
        strcpy(FullPathName, pfdata.CmdLine);
        strcat(FullPathName, "\x0d\x0a");
        if (strlen(FullPathName) >= 128 - 13) {
	    // too bad, the command line is too long
            RcMessageBox(EG_PIF_CMDLINE_ERR,NULL,NULL,RMB_ICON_BANG | RMB_ABORT);
	    goto CleanUpAndReturn;

	}
	strcpy(pvi->CmdLine, FullPathName);
	/* DIVERGENCE: FullPathName is the original bounded MAX_PATH local. */
	pvi->CmdSize = (USHORT)(strlen(FullPathName) + 1u);
    }

    if (IsPIFFile)
	// we don't know the binary type at this point.
	*pIsDosBinary = 0;

    if (pfdata.WinTitle)
	SetConsoleTitle(pfdata.WinTitle);

    DontCheckDosBinaryType = (pfdata.SubSysId == SUBSYS_DOS);

CleanUpAndReturn:
    if (pfdata.CmdLine) {
	free(pfdata.CmdLine);
	pfdata.CmdLine = NULL;
    }
    if (pfdata.StartDir) {
	free(pfdata.StartDir);
	pfdata.StartDir = NULL;
    }
    if (pfdata.StartFile) {
	free(pfdata.StartFile);
	pfdata.StartFile = NULL;
    }
    if (pfdata.WinTitle) {
	free(pfdata.WinTitle);
	pfdata.WinTitle = NULL;
    }
    return;

}

/*  cmdmisc.c - Misc. SVC routines of Command.lib
 *
 *
 *  Modification History:
 *
 *  Sudeepb 17-Sep-1991 Created
 */

#include "cmd.h"

#include <stddef.h>

/* DIVERGENCE(MVDM-HOST-DIV-111): SCSINFO and SCS_ToSync were retained as
 * process-address aliases in NT4.  Preserve their 16:16 source positions and
 * obtain session-mapped guest leases only at the original access points. */
#include "adapter-mvdm-host-out/softpc/include/mvdm_command_guest_state.h"
/* DIVERGENCE(MVDM-HOST-DIV-177): diagnostic-only COMMAND call recorder. */
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_termination.h"

#include <cmdsvc.h>
#include <demexp.h>
#include <softpc.h>
#include <mvdm.h>
#include <ctype.h>
#include <memory.h>
#include "oemuni.h"
#include "nt_pif.h"
#include "nt_uis.h"	  // For resource id
/* DIVERGENCE(MVDM-HOST-DIV-009): GetVDMAddr has no bounded x64 lifetime.
 * Retain cmdGetCurrentDir ordering and register contract through the
 * source-shaped adapter-mvdm-host-out/softpc numeric location/lease boundary. */
#include "mvdm_guest_location.h"
/* DIVERGENCE(MVDM-HOST-DIV-108): the original SCS command record stores a
 * host redirection-record pointer in a fixed DWORD.  Publish the same
 * source record through the session host-resource mapping instead; the
 * original record allocation, use and free ordering remain in cmdredir.c. */
#include "adapter-mvdm-host-out/softpc/include/mvdm_command_redirection.h"


VOID GetWowKernelCmdLine(VOID);
extern ULONG fSeparateWow;

VOID cmdGetNextCmd (VOID)
{
LPSTR   lpszCmd;
PCMDINFO pCMDInfo;
/* DIVERGENCE(MVDM-HOST-DIV-194): the original CMDINFO and each pointed
 * command payload are guest-memory records, not process aliases.  Snapshot
 * the fixed CMDINFO only for this synchronous host call, then commit its
 * original fields through a fresh checked lease before returning to COMMAND. */
CMDINFO CmdInfo;
mvdm_guest_location cmdinfo_location;
mvdm_guest_location cmdline_location;
mvdm_guest_location environment_location;
mvdm_guest_location_lease command_lease;
/* DIVERGENCE(MVDM-HOST-DIV-115): retain local title and command-line
 * lengths at their native width.  The existing guest byte write remains at
 * the source's already asserted 127-byte boundary. */
size_t  cb;
PREDIRCOMPLETE_INFO pRdrInfo;
VDMINFO MyVDMInfo;

char    *pSrc, *pDst;
/* DIVERGENCE(MVDM-HOST-DIV-160): the original request advertises
 * `MAX_PATH + 1` bytes to BaseSrv but allocates only the guest DOS
 * current-directory limit.  The modern package root is a valid host path
 * longer than that guest-only limit.  Keep the original VDMINFO request and
 * SetCurrentDirectory semantics, but make the receiving host buffer match
 * the stated ABI capacity. */
char    achCurDirectory[MAX_PATH + 1];
char    CmdLine[MAX_PATH];
/* COMMAND.COM's original EXECPATHLEN is MAX_PATH + 13 (comequ.asm). */
char    AppName[MAX_PATH + 13];

    //
    // This routine is called once for WOW VDMs, to retrieve the
    // "krnl386" command line.
    //                                         5
    if (VDMForWOW) {
        GetWowKernelCmdLine();
        return;
    }

    VDMInfo.VDMState = 0;
    if (!mvdm_guest_location_set_real_mode(&cmdinfo_location,
            (USHORT)getDS(), (USHORT)getDX()) ||
        !mvdm_guest_location_acquire(&cmdinfo_location, sizeof(CmdInfo),
            GUEST_MEMORY_ACCESS_READ, &command_lease)) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }
    RtlMoveMemory(&CmdInfo, command_lease.bytes, sizeof(CmdInfo));
    if (!mvdm_guest_location_release(&command_lease, 0)) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }
    pCMDInfo = &CmdInfo;

    VDMInfo.ErrorCode = FETCHWORD(pCMDInfo->ReturnCode);
    VDMInfo.CmdSize = sizeof(CmdLine);
    VDMInfo.CmdLine = CmdLine;
    AppName[0] = '\0';
    VDMInfo.AppName = AppName;
    VDMInfo.AppLen = FETCHWORD(pCMDInfo->ExecPathSize);
    VDMInfo.PifLen = 0;
    VDMInfo.EnviornmentSize = 0;
    VDMInfo.Enviornment = NULL;
    VDMInfo.CurDrive = 0;
    VDMInfo.TitleLen = 0;
    VDMInfo.ReservedLen = 0;
    VDMInfo.DesktopLen = 0;
    VDMInfo.CurDirectoryLen = MAX_PATH + 1;
    VDMInfo.CurDirectory = achCurDirectory;

    if(IsFirstCall){
        VDMInfo.VDMState = ASKING_FOR_FIRST_COMMAND;
        VDMInfo.ErrorCode = 0;

        DeleteConfigFiles();   // get rid of the temp boot files

	// When COMMAND.COM issues first cmdGetNextCmd, it has
	// a completed environment already(cmdGetInitEnvironment),
	// Therefore, we don't have to ask environment from BASE
	cmdVDMEnvBlk.lpszzEnv = NULL;
        cmdVDMEnvBlk.cchEnv = FETCHWORD(pCMDInfo->EnvSize);

	//clear bits that track printer flushing
        host_lpt_flush_initialize();
    }
    else {

	// program has terminated. If the termiation was issued from
	// second(or later) instance of command.com(cmd.exe), don't
	// reset the flag.
	if (Exe32ActiveCount == 0)
            DontCheckDosBinaryType = FALSE;

	// tell the base our new current directories (in ANSI)
	// we don't do it on repeat call(the shell out case is handled in
	// return exit code
        if (!IsRepeatCall) {
            cmdUpdateCurrentDirectories((BYTE)pCMDInfo->CurDrive);
	}

        VDMInfo.VDMState = 0;
        if(!IsRepeatCall){
            demCloseAllPSPRecords ();

            if (!pfdata.CloseOnExit && DosSessionId)
                nt_block_event_thread(1);
            else
                nt_block_event_thread(0);

            if (DosSessionId) {
		if (!pfdata.CloseOnExit){
		    char  achTitle[MAX_PATH];
                    char  achInactive[60];     //should be plenty for 'inactive'
		    strcpy (achTitle, "[");
                    if (!LoadString(GetModuleHandle(NULL), EXIT_NO_CLOSE,
                                                              achInactive, 60))
		        strcat (achTitle, "Inactive ");
                    else
                        strcat(achTitle, achInactive);
		    cb = strlen(achTitle);
		    // GetConsoleTitleA and SetConsoleTitleA
		    // are working on OEM character set.
		    GetConsoleTitleA(achTitle + cb, (DWORD)(MAX_PATH - cb - 1));
		    cb = strlen(achTitle);
		    achTitle[cb] = ']';
		    achTitle[cb + 1] = '\0';
		    SetConsoleTitleA(achTitle);
		    Sleep(INFINITE);
		}
		else {
		    VdmExitCode = VDMInfo.ErrorCode;
		    TerminateVDM();
		}
            }
            fBlock = TRUE;
	}
    }

    if(IsRepeatCall) {
        VDMInfo.VDMState |= ASKING_FOR_SECOND_TIME;
        if( VDMInfo.ErrorCode != 0 )
            IsRepeatCall = FALSE;
    }

    VDMInfo.VDMState |= ASKING_FOR_DOS_BINARY;

    if (!IsFirstCall && !(VDMInfo.VDMState & ASKING_FOR_SECOND_TIME)) {
        ULONG redirection_identity = FETCHDWORD(pCMDInfo->pRdrInfo);
        pRdrInfo = NULL;
        if (redirection_identity != 0 &&
            !mvdm_command_redirection_resolve(
                (USHORT)(redirection_identity >> 16),
                (USHORT)redirection_identity, (PVOID *)&pRdrInfo))
            VDMInfo.ErrorCode = ERROR_INVALID_HANDLE;
        if (cmdCheckCopyForRedirection (pRdrInfo) == FALSE)
            VDMInfo.ErrorCode = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Leave the current directory in a safe place, so that other 32bit
    // apps etc. can delnode this directory (and other such operations) later.
    if ( IsFirstCall == FALSE && IsRepeatCall == FALSE )
        SetCurrentDirectory (cmdHomeDirectory);

    // TSRExit will be set to 1, only if we are coming from command.com's
    // prompt and user typed an exit. We need to kill our parent also, so we
    // should write an exit in the console buffer.
    if (FETCHWORD(pCMDInfo->fTSRExit)) {
        cmdPushExitInConsoleBuffer ();
    }

    /**
	Merging environment is required if
	(1). Not the first comamnd &&
	(2). NTVDM is running on an existing console ||
	     NTVDM has been shelled out.
        Note that WOW doesn't need enviornment merging.
    **/
    if (!DosEnvCreated && !IsFirstCall && (!DosSessionId || Exe32ActiveCount)) {
	RtlZeroMemory(&MyVDMInfo, sizeof(VDMINFO));
	MyVDMInfo.VDMState = ASKING_FOR_ENVIRONMENT | ASKING_FOR_DOS_BINARY;
	if (IsRepeatCall) {
	    MyVDMInfo.VDMState |= ASKING_FOR_SECOND_TIME;
	    MyVDMInfo.ErrorCode = 0;
	}
	else
	    MyVDMInfo.ErrorCode = VDMInfo.ErrorCode;
        MyVDMInfo.Enviornment = lpszzVDMEnv32;
        MyVDMInfo.EnviornmentSize = (USHORT) cchVDMEnv32;
        if (!GetNextVDMCommand(&MyVDMInfo) && MyVDMInfo.EnviornmentSize > cchVDMEnv32) {
            MyVDMInfo.Enviornment = realloc(lpszzVDMEnv32, MyVDMInfo.EnviornmentSize);
            if (MyVDMInfo.Enviornment == NULL) {
		RcErrorDialogBox(EG_MALLOC_FAILURE, NULL, NULL);
		TerminateVDM();
	    }
            lpszzVDMEnv32 = MyVDMInfo.Enviornment;
            cchVDMEnv32 = MyVDMInfo.EnviornmentSize;
	    MyVDMInfo.VDMState = ASKING_FOR_DOS_BINARY | ASKING_FOR_ENVIRONMENT |
                                 ASKING_FOR_SECOND_TIME;

            MyVDMInfo.TitleLen =
	    MyVDMInfo.DesktopLen =
            MyVDMInfo.ReservedLen =
            MyVDMInfo.CmdSize =
            MyVDMInfo.AppLen =
            MyVDMInfo.PifLen =
            MyVDMInfo.CurDirectoryLen = 0;
            MyVDMInfo.ErrorCode = 0;
	    if (!GetNextVDMCommand(&MyVDMInfo)) {
		RcErrorDialogBox(EG_ENVIRONMENT_ERR, NULL, NULL);
		TerminateVDM();
	    }
	}
        if (!cmdCreateVDMEnvironment(&cmdVDMEnvBlk)) {
	    RcErrorDialogBox(EG_ENVIRONMENT_ERR, NULL, NULL);
	    TerminateVDM();
	}
	DosEnvCreated = TRUE;
        VDMInfo.ErrorCode = 0;
    }
    if (cmdVDMEnvBlk.cchEnv > FETCHWORD(pCMDInfo->EnvSize)) {
        setAX((USHORT)cmdVDMEnvBlk.cchEnv);
	setCF(1);
        IsFirstCall = FALSE;
	IsRepeatCall = TRUE;
	return;
    }
    if (DosEnvCreated)
	VDMInfo.VDMState |= ASKING_FOR_SECOND_TIME;

    if(!GetNextVDMCommand(&VDMInfo)){
       RcErrorDialogBox(EG_ENVIRONMENT_ERR, NULL, NULL);
       TerminateVDM();
    }


    IsRepeatCall = FALSE;
    IsFirstCall = FALSE;

    if(fBlock){
         nt_resume_event_thread();
         fBlock = FALSE;
    }

    // Sync VDMs enviornment variables for current directories
    cmdSetDirectories (lpszzVDMEnv32, &VDMInfo);

    // tell DOS that this is a dos executable and no further checking is
    // necessary
    /* DIVERGENCE MVDM-HOST-DIV-111: the original durable guest alias is
     * represented by a fresh session-scoped scalar lease. */
    if (!mvdm_command_guest_state_write_is_dos_binary(UINT8_C(1))) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }


    // Check for PIF files. If a pif file is being executed extract the
    // executable name, command line, current directory and title from the pif
    // file and place the stuff appropriately in VDMInfo. Note, if pif file
    // is invalid, we dont do any thing to vdminfo. In such a case we
    // pass the pif as it is to scs to execute which we know will fail and
    // will come back to cmdGettNextCmd with proper error code.

    cmdCheckForPIF (&VDMInfo);
    if (VDMInfo.ErrorCode == ERROR_INVALID_ADDRESS) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }


    //
    // if forcedos, then don't check binary type on int 21 exec process,
    // so that child spawns stay in dos land. Begining with NT 4.0 forcedos.exe
    // no longer uses pif files to force execution of a binary as a dos exe.
    // It now uses a bit in CreateProcess (dwCreationFlags).
    //

    DontCheckDosBinaryType = (VDMInfo.dwCreationFlags & CREATE_FORCEDOS) != 0;


    // convert exec path name to upper case. This is what command.com expect
    if (_strupr(VDMInfo.AppName) == NULL) {
       pSrc = VDMInfo.AppName;
       while (*pSrc)
	    *pSrc++ = (char)toupper((int)*pSrc);
    }
    // figure out the extention type
    // at least one char for the base name plus
    // EXTENTION_STRING_LEN for the extention
    // plus the NULL char
    if (VDMInfo.AppLen > 1 + EXTENTION_STRING_LEN  + 1) {
	pSrc = (PCHAR)VDMInfo.AppName + VDMInfo.AppLen - 5;
	if (!strncmp(pSrc, EXE_EXTENTION_STRING, EXTENTION_STRING_LEN))
	    STOREWORD(pCMDInfo->ExecExtType, EXE_EXTENTION);
	else if (!strncmp(pSrc, COM_EXTENTION_STRING, EXTENTION_STRING_LEN))
	    STOREWORD(pCMDInfo->ExecExtType, COM_EXTENTION);
	else if (!strncmp(pSrc, BAT_EXTENTION_STRING, EXTENTION_STRING_LEN))
	    STOREWORD(pCMDInfo->ExecExtType, BAT_EXTENTION);
	else
	    STOREWORD(pCMDInfo->ExecExtType, UNKNOWN_EXTENTION);
    }
    else
	STOREWORD(pCMDInfo->ExecExtType, UNKNOWN_EXTENTION);

    // tell command.com the length of the app full path name.
    STOREWORD(pCMDInfo->ExecPathSize, VDMInfo.AppLen);

    //
    // Prepare ccom's UCOMBUF
    //
    if (!mvdm_guest_location_set_real_mode(&cmdline_location,
            FETCHWORD(pCMDInfo->CmdLineSeg),
            FETCHWORD(pCMDInfo->CmdLineOff)) ||
        !mvdm_guest_location_acquire(&cmdline_location,
            FETCHWORD(pCMDInfo->CmdLineSize), GUEST_MEMORY_ACCESS_WRITE,
            &command_lease)) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }
    lpszCmd = (LPSTR)command_lease.bytes;

    // Copy filepart of AppName excluding extension to ccom's buffer
    pSrc = strrchr(VDMInfo.AppName, '\\');
    if (!pSrc) {
         pSrc = VDMInfo.AppName;
        }
    else {
         pSrc++;
        }
    pDst = lpszCmd + 2;
    while (*pSrc && *pSrc != '.') {
         *pDst++ = *pSrc++;
         }
    cb = strlen(CmdLine);

    // cmd line must be terminated with "\0xd\0xa\0". This is either done
    // by BASE or cmdCheckForPif function(cmdpif.c).

    ASSERT((cb >= 2) && (0xd == CmdLine[cb - 2]) && (0xa == CmdLine[cb - 1]));

    // if cmd line is not blank, separate program base name and
    // cmd line with a SPACE. If it IS blank, do not insert any white chars
    // or we end up passing white chars to the applications as cmd line
    // and some applications just can not live with that.
    // We do not strip leading white characters in the passed command line
    // so the application sees the original data.
    if (cb > 2)
	*pDst++ = ' ';

    // append the command tail(at least, "\0xd\0xa")
    strncpy(pDst, CmdLine, cb + 1);

    // set the count
    // cb has the cmd line length including the terminated 0xd and 0xa
    // It does NOT count the terminated NULL char.
    ASSERT((cb + pDst - lpszCmd - 2) <= 127);

    // minus 2 because the real data in lpszCmd start from lpszCmd[2]
    lpszCmd[1] = (CHAR)(cb + pDst - lpszCmd - 2);

    if (!mvdm_guest_location_release(&command_lease, 1)) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }



    if (DosEnvCreated) {
	if (!mvdm_guest_location_set_real_mode(&environment_location,
		FETCHWORD(pCMDInfo->EnvSeg), 0) ||
	    !mvdm_guest_location_copy_to_guest(&environment_location,
		(uint8_t const *)cmdVDMEnvBlk.lpszzEnv,
		cmdVDMEnvBlk.cchEnv)) {
	    setCF(1);
	    setAX((USHORT)ERROR_INVALID_ADDRESS);
	    return;
	}
	STOREWORD(pCMDInfo->EnvSize,cmdVDMEnvBlk.cchEnv);
	free(cmdVDMEnvBlk.lpszzEnv);
	DosEnvCreated = FALSE;
    }

    STOREWORD(pCMDInfo->fBatStatus,(USHORT)VDMInfo.fComingFromBat);
    STOREWORD(pCMDInfo->CurDrive,VDMInfo.CurDrive);
    STOREWORD(pCMDInfo->NumDrives,nDrives);
    VDMInfo.CodePage = (ULONG) cmdMapCodePage (VDMInfo.CodePage);
    STOREWORD(pCMDInfo->CodePage,(USHORT)VDMInfo.CodePage);

    cmdVDMEnvBlk.lpszzEnv = NULL;
    cmdVDMEnvBlk.cchEnv = 0;

    IsFirstVDM = FALSE;

    // Handle Standard IO redirection
    pRdrInfo = cmdCheckStandardHandles (&VDMInfo,&pCMDInfo->bStdHandles);
    {
        ULONG redirection_identity = 0;
        if (pRdrInfo != NULL && !mvdm_command_redirection_publish(pRdrInfo,
                &redirection_identity)) {
            (void)cmdCheckCopyForRedirection(pRdrInfo);
            VDMInfo.ErrorCode = ERROR_NOT_ENOUGH_MEMORY;
            setCF(1);
            setAX((USHORT)ERROR_NOT_ENOUGH_MEMORY);
            return;
        }
        STOREDWORD(pCMDInfo->pRdrInfo, redirection_identity);
    }

    // Tell DOS that it has to invalidate the CDSs
    if (!mvdm_command_guest_state_set_to_sync(UINT8_C(0xff))) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }
    if (!mvdm_guest_location_copy_to_guest(&cmdinfo_location,
            (uint8_t const *)&CmdInfo, sizeof(CmdInfo))) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }
    setCF(0);

    return;
}



VOID GetWowKernelCmdLine(VOID)
{
CMDINFO UNALIGNED *pCMDInfo;
PCHAR	 pch, pEnvStrings;
PCHAR    pSlash;
int      Len;
LPSTR    pszCmdLine;


    DeleteConfigFiles();   // get rid of the temp boot files
    host_lpt_flush_initialize();

    //
    // Only a few things need be set for WOW.
    //   1. NumDrives
    //   2. Environment (get from current 32-bit env.)
    //   3. Kernel CmdLine (get from ntvdm command tail)
    //   4. Current drive
    //

    pCMDInfo = (LPVOID) GetVDMAddr ((USHORT)getDS(),(USHORT)getDX());
    pCMDInfo->NumDrives = nDrives;

    //
    // Get the process's environment into lpszzVDMEnv32 and count
    // its size into cchVDMEnv32.
    //

    pEnvStrings = pch = lpszzVDMEnv32 = GetEnvironmentStrings();
    cchVDMEnv32 = 0;
    while (pch[0] || pch[1]) {
        cchVDMEnv32++;
        pch++;
    }
    cchVDMEnv32 += 2;  // two terminating nulls not counted in loop.

    //
    // Transform environment to suit VDM.  cmdCreateVDMEnvironment
    // uses lpszzVDMEnv32 and cchVDMEnv32 as the source.
    //

    if (!cmdCreateVDMEnvironment(&cmdVDMEnvBlk)) {
        RcErrorDialogBox(EG_ENVIRONMENT_ERR, NULL, NULL);
        TerminateVDM();
    }

    //
    // Copy the transformed environment to real mode mem and then free it.
    //

    pch = (PVOID)GetVDMAddr(FETCHWORD(pCMDInfo->EnvSeg),0);
    RtlMoveMemory(pch,
                  cmdVDMEnvBlk.lpszzEnv,
                  cmdVDMEnvBlk.cchEnv
                 );
    STOREWORD(pCMDInfo->EnvSize,cmdVDMEnvBlk.cchEnv);
    free(cmdVDMEnvBlk.lpszzEnv);
    // GetEnvironmentStrings needs us to call its corresponding function
    // to free the memory it allocated.
    FreeEnvironmentStrings(pEnvStrings);

    //
    // Get the command line parameter, which consists of a fully
    // qualified short path file name: "-a %SystemRoot%\system32\krnl386.exe".
    //
    // Note that the first token of cmdline is "%SystemRoot%\system32\ntvdm ",
    // and may be a long file name surrounded by quotes.
    //
    pszCmdLine = GetCommandLine();
    if (pszCmdLine) {

        // skip leading spaces
        while (*pszCmdLine && !isgraph(*pszCmdLine)) {
               pszCmdLine++;
               }

        // skip first token
        if (*pszCmdLine == '"') {
            pszCmdLine++;
            while (*pszCmdLine && *pszCmdLine++ != '"')
                   ;
            }
        else {
            while (isgraph(*pszCmdLine)) {
                   pszCmdLine++;
                   }
            }

        // mov to beg of WowKernelPathName
        pszCmdLine = strstr(pszCmdLine, " -a ");
        pszCmdLine += 4;
        while (*pszCmdLine && *pszCmdLine == ' ') {
               pszCmdLine++;
        }
    }

    if (!pszCmdLine || !*pszCmdLine) {
        RcErrorDialogBox(EG_ENVIRONMENT_ERR, NULL, NULL);
        TerminateVDM();
    }


    //
    // Copy first token to ExecPath, and find the beg of the file part.
    //
    Len = FETCHWORD(pCMDInfo->ExecPathSize);
    pch = (PVOID)GetVDMAddr(FETCHWORD(pCMDInfo->ExecPathSeg),
                            FETCHWORD(pCMDInfo->ExecPathOff));

    pSlash = pszCmdLine;
    while (--Len && isgraph(*pszCmdLine)) {
         if (*pszCmdLine == '\\') {
             pSlash = pszCmdLine + 1;
         }
         *pch++ = *pszCmdLine++;
    }
    *pch = '\0';
    pCMDInfo->ExecPathSize -= Len;
    pCMDInfo->ExecExtType = EXE_EXTENTION; // for WOW, use EXE extention

    pszCmdLine = pSlash;              // filepart begins here


    //
    // Copy filepart of first token and rest to CmdLine buffer
    //
    Len = FETCHWORD(pCMDInfo->CmdLineSize);
    pch = (PVOID)GetVDMAddr(FETCHWORD(pCMDInfo->CmdLineSeg),
                            FETCHWORD(pCMDInfo->CmdLineOff));

    while (--Len && *pszCmdLine) {
         *pch++ = *pszCmdLine++;
    }
    strcpy(pch, "\x0d\x0a");


    /* DIVERGENCE MVDM-HOST-DIV-111: do not retain the SCS binary byte as a
     * process pointer between this command BOP and its guest consumer. */
    if (!mvdm_command_guest_state_write_is_dos_binary(UINT8_C(1))) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }
    IsRepeatCall = FALSE;
    IsFirstCall = FALSE;

    // Tell DOS that it has to invalidate the CDSs
    if (!mvdm_command_guest_state_set_to_sync(UINT8_C(0xff))) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }
    setCF(0);

    return;
}


/* cmdGetCurrentDir - Return the current directory for a drive.
 *
 *
 *  Entry - Client (DS:SI) - buffer to return the directory
 *          Client (AL)   - drive being queried (0 = A)
 *
 *  EXIT  - SUCCESS Client (CY) clear
 *          FAILURE Client (CY) set
 *                         (AX) = 0 (Directory was bigger than 64)
 *                         (AX) = 1 (the drive is not valid)
 *
 */

VOID cmdGetCurrentDir (VOID)
{
CHAR  CurDir[MAXIMUM_VDM_CURRENT_DIR + 3];
mvdm_guest_location CurDirLocation;
UCHAR chDrive;
CHAR  EnvVar[] = "=?:";
CHAR  RootName[] = "?:\\";
DWORD EnvVarLen;
UINT  DriveType;


    /* DIVERGENCE(MVDM-HOST-DIV-009): retain the far address numerically;
     * no guest pointer is materialized before the known bounded result exists. */
    if (!mvdm_guest_location_set_real_mode(&CurDirLocation, (USHORT)getDS(),
        (USHORT)getSI())) {
        setCF(1);
        setAX(0);
        return;
    }
    chDrive = getAL();
    EnvVar[1] = chDrive + 'A';
    RootName[0] = chDrive + 'A';

    // if the drive doesn't exist, blindly clear the environment var
    // and return error
    DriveType = demGetPhysicalDriveType(chDrive);
    if (DriveType == DRIVE_UNKNOWN) {
        DriveType = GetDriveTypeOem(RootName);
        }

    if (DriveType == DRIVE_UNKNOWN || DriveType == DRIVE_NO_ROOT_DIR){
        SetEnvironmentVariableOem(EnvVar, NULL);
	setCF(1);
	setAX(0);
	return;
    }

    if((EnvVarLen = GetEnvironmentVariableOem (EnvVar,CurDir,
                                            MAXIMUM_VDM_CURRENT_DIR+3)) == 0){

	// if its not in env then and drive exist then we have'nt
	// yet touched it.
        strcpy(CurDir, RootName);
        /* DIVERGENCE(MVDM-HOST-DIV-009): exact synchronous write replaces
         * the source's direct guest alias before its environment mutation. */
        if (!mvdm_guest_location_copy_to_guest(&CurDirLocation,
            (PBYTE)CurDir, (ULONG)strlen(CurDir) + 1)) {
            setCF(1);
            setAX(0);
            return;
        }
        SetEnvironmentVariableOem (EnvVar,RootName);
	setCF(0);
	return;
    }
    if (EnvVarLen > MAXIMUM_VDM_CURRENT_DIR+3) {
        setCF(1);
        setAX(0);
    }
    else {
        if (!mvdm_guest_location_copy_to_guest(&CurDirLocation,
            (PBYTE)CurDir, EnvVarLen + 1)) {
            setCF(1);
            setAX(0);
            return;
        }
	setCF(0);
    }
    return;
}

/* cmdSetInfo -     Set the address of SCS_ToSync variable in DOSDATA.
 *                  This variable is set whenever SCS dispatches a new
 *                  command to command.com. Setting of this variable
 *                  indicates to dos to validate all the CDS structures
 *                  for local drives.
 *
 *
 *  Entry - Client (DS:DX) - pointer to SCSINFO.
 *
 *  EXIT  - None
 */

VOID cmdSetInfo (VOID)
{

    /* DIVERGENCE(MVDM-HOST-DIV-177): state-neutral selected-handler entry. */
    mvdm_softpc_record_command_call(5u, 2u, (unsigned int)getAX(),
        (unsigned int)getCF());
    /* DIVERGENCE MVDM-HOST-DIV-111: record all three original SCS scalar
     * locations numerically; their values are acquired only at each use. */
    if (!mvdm_command_guest_state_set_scs_scalars(getDS(), getDX(),
            (uint32_t)sizeof(SCSINFO),
            (uint32_t)offsetof(SCSINFO, SCS_ToSync),
            getDS(), getBX(), getDS(), getCX())) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        /* DIVERGENCE(MVDM-HOST-DIV-177): state-neutral failure return. */
        mvdm_softpc_record_command_call(5u, 3u, (unsigned int)getAX(),
            (unsigned int)getCF());
        return;
    }

    /* DIVERGENCE(MVDM-HOST-DIV-177): state-neutral success return. */
    mvdm_softpc_record_command_call(5u, 3u, (unsigned int)getAX(),
        (unsigned int)getCF());
    return;
}


VOID cmdSetDirectories (PCHAR lpszzEnv, VDMINFO * pVdmInfo)
{
LPSTR   lpszVal;
CHAR	ch, chDrive, achEnvDrive[] = "=?:";

    ch = pVdmInfo->CurDrive + 'A';
    if (pVdmInfo->CurDirectoryLen != 0){
	SetCurrentDirectory(pVdmInfo->CurDirectory);
	achEnvDrive[1] = ch;
	SetEnvironmentVariable(achEnvDrive, pVdmInfo->CurDirectory);
    }
    if (lpszzEnv) {
        while(*lpszzEnv) {
	    if(*lpszzEnv == '=' &&
		    (chDrive = toupper(*(lpszzEnv+1))) >= 'A' &&
		    chDrive <= 'Z' &&
		    /* DIVERGENCE(MVDM-HOST-DIV-091): the original 32-bit
		     * process-address casts only advanced inside the same
		     * native MULTI_SZ buffer. Keep that byte access without
		     * truncating the host pointer on x64. */
		    (lpszzEnv[2] == ':') &&
		    chDrive != ch) {
		    lpszVal = lpszzEnv + 4;
		    achEnvDrive[1] = chDrive;
		    SetEnvironmentVariable (achEnvDrive,lpszVal);
            }
            lpszzEnv = strchr(lpszzEnv,'\0');
            lpszzEnv++;
        }
    }
}

static BOOL fConOutput = FALSE;

VOID cmdComSpec (VOID)
{
LPSTR   lpszCS;
/* DIVERGENCE(MVDM-HOST-DIV-192): NT4 used GetVDMAddr as a durable
 * low-address host alias for the 16:16 COMSPEC source.  Keep the original
 * COMSPEC assembly and AL result, but copy that bounded guest string through
 * the session-owned mapping lease before the original host buffer consumes
 * it. */
mvdm_guest_location comspec_location;
CHAR    achComSpec[MAX_PATH];
uint32_t cbGuestComSpec;


    if(IsFirstCall == FALSE)
        return;

    if (!mvdm_guest_location_set_real_mode(&comspec_location,
            (USHORT)getDS(), (USHORT)getDX()) ||
        !mvdm_guest_location_copy_c_string(&comspec_location,
            (uint8_t *)achComSpec, sizeof(achComSpec), &cbGuestComSpec) ||
        cbGuestComSpec == 0u) {
        setCF(1);
        setAX((USHORT)ERROR_INVALID_ADDRESS);
        return;
    }
    lpszCS = achComSpec;
    strcpy(lpszComSpec,"COMSPEC=");
    strcpy(lpszComSpec+8,lpszCS);
    cbComSpec = strlen(lpszComSpec) +1;

    setAL((BYTE)(!fConOutput || VDMForWOW));

    return;
}


VOID cmdSaveWorld (VOID)
{
#ifdef CHECK_IT_LATER
SAVEWORLD VDMState;
HANDLE  hFile;
PCHAR   pVDM;
DWORD   dwBytesWritten;

    if(IsFirstVDMInSystem) {
        IsFirstVDMInSystem = FALSE;
        if ((hFile = CreateFile("c:\\nt\\bin86\\savevdm.wld",
                            GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_ALWAYS,
                            0,
                            NULL)) == (HANDLE)-1){
            SaveWorldCreated = FALSE;
            return;
        }
        VDMState.ax    =    getAX();
        VDMState.bx    =    getBX();
        VDMState.cx    =    getCX();
        VDMState.dx    =    getDX();
        VDMState.cs    =    getCS();
        VDMState.ss    =    getSS();
        VDMState.ds    =    getDS();
        VDMState.es    =    getES();
        VDMState.si    =    getSI();
        VDMState.di    =    getDI();
        VDMState.bp    =    getBP();
        VDMState.sp    =    getSP();
        VDMState.ip    =    getIP() + 1;
        VDMState.flag  =    0;
        VDMState.ImageSize = 1024*1024;

        pVDM = (PVOID)GetVDMAddr(0,0);

        if (WriteFile (hFile,
                       (LPVOID)&VDMState,
                       (DWORD)sizeof(VDMState),
                       &dwBytesWritten,
                       NULL) == FALSE){
            SaveWorldCreated = FALSE;
            CloseHandle(hFile);
            return;
        }

        if (WriteFile (hFile,
                       (LPVOID)pVDM,
                       (DWORD)VDMState.ImageSize,
                       &dwBytesWritten,
                       NULL) == FALSE){
            SaveWorldCreated = FALSE;
            CloseHandle(hFile);
            return;
        }
        CloseHandle(hFile);
    }
#endif
    return;
}


/* cmdInitConsole - Let Video VDD know that it can start console output
 *                  operations.
 *
 *
 *  Entry - None
 *
 *
 *  EXIT  - None
 *
 */

VOID cmdInitConsole (VOID)
{
    if (fConOutput == FALSE) {
        fConOutput = TRUE;
        nt_init_event_thread ();
        }
    return;
}


/* cmdMapCodePage - Map the Win32 Code page to DOS code page
 */

USHORT cmdMapCodePage (ULONG CodePage)
{
    // Currently We understand US code page only
    if (CodePage == 1252)
        return 437;
    else
        return ((USHORT)CodePage);
}



/* GetWOWShortCutInfo - returns the startupinf.reserved field of
 *                      vdminfo for the first wow task.
 *
 * Input - Bufsize - pointer to bufsize
 *         Buf     - buffer where the info is returned
 *
 * Output
 *        Success - returns TRUE, BufSize has the length of buffer filled in
 *        Failure - returns FALSE, Bufsize has the required buffer size.
 */

BOOL GetWOWShortCutInfo (PULONG Bufsize, PVOID Buf)
{
    if (*Bufsize >= VDMInfo.ReservedLen) {
        *Bufsize =  VDMInfo.ReservedLen;
        if (Bufsize)
            strncpy (Buf, VDMInfo.Reserved, VDMInfo.ReservedLen);
        return TRUE;
    }
    else {
        *Bufsize =  VDMInfo.ReservedLen;
        return FALSE;
    }
}

VOID cmdUpdateCurrentDirectories(BYTE CurDrive)
{
    DWORD cchRemain, cchCurDir;
    CHAR *lpszCurDir;
    BYTE Drive;
    DWORD DriveType;
    CHAR achName[] = "=?:";
    CHAR RootName[] = "?:\\";


    // allocate new space for the new current directories
    lpszzCurrentDirectories = (CHAR*) malloc(MAX_PATH);
    cchCurrentDirectories = 0;
    cchRemain = MAX_PATH;
    lpszCurDir = lpszzCurrentDirectories;
    if (lpszCurDir != NULL) {
	Drive = 0;
	// current directory is the first entry
        achName[1] = CurDrive + 'A';
        cchCurrentDirectories = GetEnvironmentVariable(
                                                        achName,
                                                        lpszCurDir,
                                                        cchRemain
                                                      );

	if (cchCurrentDirectories == 0 || cchCurrentDirectories > MAX_PATH) {
	    free(lpszzCurrentDirectories);
	    lpszzCurrentDirectories = NULL;
	    cchCurrentDirectories = 0;
	    return;
	}

	cchRemain -= ++cchCurrentDirectories;
	// we got current directory already. Keep the drive number
        lpszCurDir += cchCurrentDirectories;

        while (Drive < 26) {

            // ignore invalid drives and current drive
            if (Drive != CurDrive) {
                DriveType = demGetPhysicalDriveType(Drive);
                if (DriveType == DRIVE_UNKNOWN) {
                    RootName[0] = (CHAR)('A' + Drive);
                    DriveType = GetDriveTypeOem(RootName);
                }

                if (DriveType != DRIVE_UNKNOWN &&
                    DriveType != DRIVE_NO_ROOT_DIR )
                 {
                    achName[1] = Drive + 'A';
                    cchCurDir = GetEnvironmentVariable(
                                                       achName,
                                                       lpszCurDir,
                                                       cchRemain
                                                       );
                    if(cchCurDir > cchRemain) {
                        lpszCurDir = (CHAR *)realloc(lpszzCurrentDirectories,
                                                     cchRemain + MAX_PATH + cchCurrentDirectories
                                                     );
                        if (lpszCurDir == NULL) {
                            free(lpszzCurrentDirectories);
                            lpszzCurrentDirectories = NULL;
                            cchCurrentDirectories = 0;
                            return;
                        }
                        lpszzCurrentDirectories = lpszCurDir;
                        lpszCurDir += cchCurrentDirectories;
                        cchRemain += MAX_PATH;
                        cchCurDir = GetEnvironmentVariable(
                                                           achName,
                                                           lpszCurDir,
                                                           cchRemain
                                                           );
                    }
                    if (cchCurDir != 0) {
                        // GetEnvironmentVariable doesn't count the NULL char
                        lpszCurDir += ++cchCurDir;
                        cchRemain -= cchCurDir;
                        cchCurrentDirectories += cchCurDir;
                    }
                }
            }
	    // next drive
	    Drive++;
        }


	lpszCurDir = lpszzCurrentDirectories;
	// need space for the ending NULL and shrink the space if necessary
	lpszzCurrentDirectories = (CHAR *) realloc(lpszCurDir, cchCurrentDirectories + 1);
	if (lpszzCurrentDirectories != NULL && cchCurrentDirectories != 0){
	    lpszzCurrentDirectories[cchCurrentDirectories++] = '\0';
	    SetVDMCurrentDirectories(cchCurrentDirectories, lpszzCurrentDirectories);
	    free(lpszzCurrentDirectories);
	    lpszzCurrentDirectories = NULL;
	    cchCurrentDirectories = 0;
	}
	else {
	    free(lpszCurDir);
	    cchCurrentDirectories = 0;
	}

    }
}

/* This SVC function tells command.com, if the VDM was started without an
 * existing console. If so, on finding a TSR, command.com will return
 * back to GetNextVDMCommand, rather than putting its own popup.
 *
 * Entry - None
 *
 * Exit  - Client (AL) = 0 if started with an existing console
 *         Client (AL) = 1 if started with new console
 */

VOID cmdGetStartInfo (VOID)
{
    setAL((BYTE) (DosSessionId ? 1 : 0));
    return;
}

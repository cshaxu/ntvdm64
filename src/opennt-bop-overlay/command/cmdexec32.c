/* DIVERGENCE(BOP-DIV-102): private same-signature replacement for the
 * original cmdexec.c cmdExec32 body.  It retains the source worker/re-entry
 * order but replaces CCPU/BaseSrv and process-global standard handles with
 * the already admitted bounded child/session mechanics. */
VOID cmdExec32 (PCHAR pCmd32, PCHAR pEnv)
{
    runtime_command_misc_session *Session = runtime_command_misc_active_session();

    if (Session != NULL && runtime_command_worker_reentry_pending()) {
        if (!runtime_command_worker_complete()) {
            if (GetLastError() == ERROR_IO_INCOMPLETE)
                (void)runtime_command_misc_set_pending();
            else {
                setCF(0);
                setAL((UCHAR)GetLastError());
            }
            return;
        }
        goto WorkerComplete;
    }

    pCommand32 = pCmd32;
    pEnv32 = pEnv;
    CntrlHandlerState = (CntrlHandlerState & ~CNTRL_SHELLCOUNT) |
                         (((WORD)(CntrlHandlerState & CNTRL_SHELLCOUNT)) + 1);

    nt_block_event_thread(0);
    fSoftpcRedirectionOnShellOut = fSoftpcRedirection;
    fBlock = TRUE;

    if (!runtime_command_worker_begin(pCmd32, pEnv32)) {
        setCF(0);
        setAL((UCHAR)GetLastError());
        nt_resume_event_thread();
        nt_std_handle_notification(fSoftpcRedirectionOnShellOut);
        fBlock = FALSE;
        CntrlHandlerState = (CntrlHandlerState & ~CNTRL_SHELLCOUNT) |
                         (((WORD)(CntrlHandlerState & CNTRL_SHELLCOUNT)) - 1);
        return;
    }
    (void)runtime_command_misc_set_pending();
    return;

WorkerComplete:
    VDMInfo.VDMState = NO_PARENT_TO_WAKE | RETURN_ON_NO_COMMAND;
    VDMInfo.EnviornmentSize = 0;
    VDMInfo.ErrorCode = 0;
    VDMInfo.CmdSize = 0;
    VDMInfo.TitleLen = 0;
    VDMInfo.ReservedLen = 0;
    VDMInfo.DesktopLen = 0;
    VDMInfo.CurDirectoryLen = 0;
    GetNextVDMCommand (&VDMInfo);
    if (VDMInfo.CmdSize > 0) {
        setCF(1);
        IsRepeatCall = TRUE;
    }
    else {
        setCF(0);
        setAL((UCHAR)dwExitCode32);
        nt_resume_event_thread();
        nt_std_handle_notification(fSoftpcRedirectionOnShellOut);
        fBlock = FALSE;
    }

    CntrlHandlerState = (CntrlHandlerState & ~CNTRL_SHELLCOUNT) |
                     (((WORD)(CntrlHandlerState & CNTRL_SHELLCOUNT)) - 1);
}

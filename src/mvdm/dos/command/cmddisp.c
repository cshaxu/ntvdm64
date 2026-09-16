/*
 *  cmddisp.c - SVC dispatch module of command
 *
 *  Modification History:
 *
 *  Sudeepb 17-Sep-1991 Created
 */

#include "cmd.h"

#include <cmdsvc.h>
#include <softpc.h>
/* DIVERGENCE(MVDM-HOST-DIV-177): diagnostic-only COMMAND call recorder. */
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_termination.h"


PFNSVC	apfnSVCCmd [] = {
     cmdExitVDM,		//SVC_CMDEXITVDM
     cmdGetNextCmd,		//SVC_CMDGETNEXTCMD
     cmdComSpec,		//SVC_CMDCOMSPEC
     cmdSaveWorld,		//SVC_CMDSAVEWORLD
     cmdGetCurrentDir,		//SVC_CMDGETCURDIR
     cmdSetInfo,		//SVC_CMDSETINFO
     cmdGetStdHandle,		//SVC_GETSTDHANDLE
     cmdCheckBinary,		//SVC_CMDCHECKBINARY
     cmdExec,			//SVC_CMDEXEC
     cmdInitConsole,		//SVC_CMDINITCONSOLE
     cmdExecComspec32,		//SVC_EXECCOMSPEC32
     cmdReturnExitCode,         //SVC_RETURNEXITCODE
     cmdGetConfigSys,           //SVC_GETCONFIGSYS
     cmdGetAutoexecBat,		//SVC_GETAUTOEXECBAT
     cmdGetKbdLayout,		//SVC_GETKBDLAYOUT
     cmdGetInitEnvironment,     //SVC_GETINITENVIRONMENT
     cmdGetStartInfo            //SVC_GETSTARTINFO
};


/* cmdDispatch - Dispatch SVC call to right command handler.
 *
 * Entry - iSvc (SVC byte following SVCop)
 *
 * Exit  - None
 *
 */

BOOL CmdDispatch (ULONG iSvc)
{
#if DBG
    if (iSvc >= SVC_CMDLASTSVC){
	DbgPrint("Unimplemented SVC index for COMMAND %x\n",iSvc);
	setCF(1);
	return FALSE;
    }
#endif
    /* DIVERGENCE(MVDM-HOST-DIV-177): state-neutral table-call attribution. */
    mvdm_softpc_record_command_call((unsigned int)iSvc, 0u,
        (unsigned int)getAX(), (unsigned int)getCF());
    /* DIVERGENCE(MVDM-HOST-DIV-199): default-off scalar evidence for the
     * original 54:0F ES:0/BX contract.  It observes the existing table call,
     * does not read its buffer, and cannot alter the original provider. */
    if (iSvc == SVC_GETINITENVIRONMENT)
        mvdm_softpc_record_command_environment(0u, (unsigned int)getES(),
            (unsigned int)getBX(), (unsigned int)getAX(),
            (unsigned int)getCF(), (unsigned int)getDS(), (unsigned int)getSS(),
            (unsigned int)getSP());
    (apfnSVCCmd [iSvc])();
    if (iSvc == SVC_GETINITENVIRONMENT)
        mvdm_softpc_restore_child_report_paths();
    /* DIVERGENCE(MVDM-HOST-DIV-177): state-neutral table-return attribution. */
    mvdm_softpc_record_command_call((unsigned int)iSvc, 1u,
        (unsigned int)getAX(), (unsigned int)getCF());
    if (iSvc == SVC_GETINITENVIRONMENT)
        mvdm_softpc_record_command_environment(1u, (unsigned int)getES(),
            (unsigned int)getBX(), (unsigned int)getAX(),
            (unsigned int)getCF(), (unsigned int)getDS(), (unsigned int)getSS(),
            (unsigned int)getSP());
    /* DIVERGENCE(MVDM-HOST-DIV-202): a fixed-image, default-off read of the
     * original resident stub table after its unchanged 54:0F call.  It uses
     * short mapping-manager leases and cannot modify the table or any CPU
     * state. */
    if (iSvc == SVC_GETINITENVIRONMENT)
        mvdm_softpc_record_command_stub_table((uint16_t)getCS());

    return TRUE;
}

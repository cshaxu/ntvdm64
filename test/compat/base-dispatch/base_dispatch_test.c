#include <nt.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ntlpcapi.h>
#include <ntcsrmsg.h>
#include "non-mvdm/base/win32/inc/basemsg.h"
#include "non-mvdm/base/win32/inc/basevdm.h"
#include <vdmapi.h>
#include "compat/include/base_client.h"
#include "compat/include/base_transport.h"
#include "base-request-owner.inc"

static unsigned checks, selected, calls;
static DWORD caller;
static NTSTATUS transport_failure, get_result;
static HANDLE supplied_wait;
static ULONG release_wait;
static USHORT get_flags, get_length;
static ULONGLONG supplied_direct;
static unsigned activations;
static DWORD activation_error;
static DWORD activate(struct compat_console_activation *context, BOOL active)
{ (void)context; if(active)++activations; return activation_error; }
#define CHECK(x) do { ++checks; if (!(x)) { fprintf(stderr,"FAIL %u: %s\n",__LINE__,#x); exit(1); } } while (0)
static NTSTATUS hit(unsigned operation)
{
    CHECK(GetCurrentThreadId() == caller);
    selected = operation; ++calls;
    return STATUS_SUCCESS;
}
static NTSTATUS status(DWORD error)
{
    CHECK(GetCurrentThreadId() == caller);
    return error ? STATUS_UNSUCCESSFUL : transport_failure;
}
static NTSTATUS check(PBASE_CHECKVDM_MSG p) { CHECK(p); return hit(1); }
static NTSTATUS get(PBASE_GET_NEXT_VDM_COMMAND_MSG p,HANDLE *wait,ULONGLONG *direct)
{ CHECK(p && wait && direct); *wait=supplied_wait; *direct=supplied_direct;
  p->VDMState=get_flags; p->CmdLen=get_length; hit(2); return get_result; }
static NTSTATUS update(PBASE_UPDATE_VDM_ENTRY_MSG p) { CHECK(p); return hit(3); }
static NTSTATUS exit_vdm(BOOL wow,BOOL notify,ULONG task,ULONG *release)
{ CHECK(!wow && !notify && !task && release); *release=release_wait; return hit(4); }
static NTSTATUS code(HANDLE parent,ULONG *value)
{ CHECK(parent==(HANDLE)17 && value); *value=37; return hit(5); }
static NTSTATUS reenter(ULONG value)
{ CHECK(value==INCREMENT_REENTER_COUNT); return hit(6); }
static NTSTATUS register_wow(ULONG window) { CHECK(window==23); return hit(7); }
static NTSTATUS first(ULONG *value) { CHECK(value); *value=1; return hit(8); }
int main(void)
{
    const ntvdm_base_transport transport={status,check,get,update,exit_vdm,code,reenter,register_wow,first,BaseDecodeVdmRequest,BaseReadVdmCommandState};
    const ULONG operations[]={BasepCheckVDM,BasepGetNextVDMCommand,BasepUpdateVDMEntry,
        BasepExitVDM,BasepGetVDMExitCode,BasepSetReenterCount,BasepRegisterWowExec,BasepIsFirstVDM};
    BASE_API_MSG m={0};
    const ULONG sizes[]={sizeof(m.u.CheckVDM),sizeof(m.u.GetNextVDMCommand),sizeof(m.u.UpdateVDMEntry),
        sizeof(m.u.ExitVDM),sizeof(m.u.GetVDMExitCode),sizeof(m.u.SetReenterCount),sizeof(m.u.RegisterWowExec),sizeof(m.u.IsFirstVDM)};
    unsigned i;
    caller=GetCurrentThreadId();
    CHECK(!NT_SUCCESS(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,0,0)));
    ntvdm_base_bind_transport(&transport);
    for(i=0;i<8;++i) {
        CSR_API_NUMBER api=CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,operations[i]);
        ZeroMemory(&m,sizeof(m)); selected=calls=0;
        if(i==4)m.u.GetVDMExitCode.hParent=(HANDLE)17;
        if(i==5)m.u.SetReenterCount.fIncDec=INCREMENT_REENTER_COUNT;
        if(i==6)m.u.RegisterWowExec.hwndWowExec=(HANDLE)23;
        CHECK(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,api,sizes[i])==STATUS_SUCCESS);
        CHECK(m.ReturnValue==STATUS_SUCCESS && selected==i+1 && calls==1);
        if(i==4)CHECK(m.u.GetVDMExitCode.ExitCode==37);
        if(i==7)CHECK(m.u.IsFirstVDM.FirstVDM==1);
        CHECK(!NT_SUCCESS(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,api,sizes[i]-1)));
        CHECK(!NT_SUCCESS(m.ReturnValue) && calls==1);
        transport_failure=STATUS_PORT_DISCONNECTED;
        CHECK(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,api,sizes[i])==STATUS_PORT_DISCONNECTED);
        CHECK(m.ReturnValue==STATUS_PORT_DISCONNECTED && calls==1);
        transport_failure=STATUS_SUCCESS;
    }
    /* Actual C1 projection plus B duplicate/transfer/rollback ownership. */
    {
        HANDLE first_wait=CreateEventW(NULL,TRUE,FALSE,NULL), duplicate=NULL;
        DWORD flags;
        ULONG get_api=CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand);
        ULONG exit_api=CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepExitVDM);
        CHECK(first_wait && DuplicateHandle(GetCurrentProcess(),first_wait,GetCurrentProcess(),
            &duplicate,0,FALSE,DUPLICATE_SAME_ACCESS));
        ZeroMemory(&m,sizeof(m)); supplied_wait=first_wait;
        CHECK(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,get_api,sizeof(m.u.GetNextVDMCommand))==STATUS_SUCCESS);
        CHECK(m.u.GetNextVDMCommand.WaitObjectForVDM==first_wait);
        ZeroMemory(&m,sizeof(m)); supplied_wait=duplicate;
        CHECK(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,get_api,sizeof(m.u.GetNextVDMCommand))==STATUS_SUCCESS);
        CHECK(m.u.GetNextVDMCommand.WaitObjectForVDM==first_wait && !GetHandleInformation(duplicate,&flags));
        supplied_wait=NULL; get_length=1; supplied_direct=71;
        ntvdm_base_set_activation(activate,NULL);
        ZeroMemory(&m,sizeof(m)); m.u.GetNextVDMCommand.VDMState=ASKING_FOR_PIF;
        CHECK(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,get_api,sizeof(m.u.GetNextVDMCommand))==STATUS_SUCCESS);
        CHECK(!ntvdm_base_observation_direct() && activations==1);
        ZeroMemory(&m,sizeof(m)); get_length=0; get_flags=RETURN_ON_NO_COMMAND; get_result=STATUS_NO_MEMORY;
        CHECK(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,get_api,sizeof(m.u.GetNextVDMCommand))==STATUS_NO_MEMORY);
        CHECK(ntvdm_base_observation_direct()==71 && activations==1);
        ZeroMemory(&m,sizeof(m)); get_result=STATUS_SUCCESS; get_flags=0; get_length=1; activation_error=ERROR_ACCESS_DENIED;
        CHECK(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,get_api,sizeof(m.u.GetNextVDMCommand))==STATUS_UNSUCCESSFUL);
        CHECK(GetLastError()==ERROR_ACCESS_DENIED && activations==2);
        ZeroMemory(&m,sizeof(m)); release_wait=2;
        CHECK(!NT_SUCCESS(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,exit_api,sizeof(m.u.ExitVDM))));
        CHECK(GetHandleInformation(first_wait,&flags) && !m.u.ExitVDM.WaitObjectForVDM);
        release_wait=1;
        CHECK(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,exit_api,sizeof(m.u.ExitVDM))==STATUS_SUCCESS);
        CHECK(m.u.ExitVDM.WaitObjectForVDM==first_wait && CloseHandle(first_wait));
        CHECK(!NT_SUCCESS(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,exit_api,sizeof(m.u.ExitVDM))));
    }
    ntvdm_base_bind_transport(NULL);
    CHECK(!NT_SUCCESS(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,0,0)));
    puts("PASS eight same-thread BASE callbacks, malformed sizes, disconnected transport and unbind; transport mocked");
    printf("PASS %u assertions\n",checks);
    return 0;
}

#include <nt.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "compat/include/base_client.h"
#include "compat/include/base_transport.h"

static unsigned checks, selected, calls;
static DWORD caller;
static NTSTATUS transport_failure;
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
{ CHECK(p && wait && direct); *wait=NULL; *direct=0; return hit(2); }
static NTSTATUS update(PBASE_UPDATE_VDM_ENTRY_MSG p) { CHECK(p); return hit(3); }
static NTSTATUS exit_vdm(BOOL wow,BOOL notify,ULONG task,ULONG *release)
{ CHECK(!wow && !notify && !task && release); *release=0; return hit(4); }
static NTSTATUS code(HANDLE parent,ULONG *value)
{ CHECK(parent==(HANDLE)17 && value); *value=37; return hit(5); }
static NTSTATUS reenter(ULONG value)
{ CHECK(value==INCREMENT_REENTER_COUNT); return hit(6); }
static NTSTATUS register_wow(ULONG window) { CHECK(window==23); return hit(7); }
static NTSTATUS first(ULONG *value) { CHECK(value); *value=1; return hit(8); }
int main(void)
{
    const ntvdm_base_transport transport={status,check,get,update,exit_vdm,code,reenter,register_wow,first};
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
    ntvdm_base_bind_transport(NULL);
    CHECK(!NT_SUCCESS(NtvdmBaseClientCallServer((PCSR_API_MSG)&m,NULL,0,0)));
    puts("PASS eight same-thread BASE callbacks, malformed sizes, disconnected transport and unbind; transport mocked");
    printf("PASS %u assertions\n",checks);
    return 0;
}

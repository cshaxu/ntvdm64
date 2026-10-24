#include "session_service.h"
#include "session_arguments.h"
#include "common/protocol/frontend_protocol.h"
#include "common/protocol/frontend_lifecycle.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PVOID CsrPortHeap;
static DWORD session_entry(HANDLE caller,HANDLE notification,HANDLE retire,HANDLE restored,
    BOOL lease,BOOL borrowed)
{
    DWORD pid=0,error,ignored;
    frontend_session_service *service=NULL;
    /* NTSRV validates the exact admitted process and capability in the RPC
     * registration and startup-result calls. The inherited caller identifies
     * the real Console, not the broker's own Console. */
    error=OpenNtBaseClientConnectCurrent();
    if(error)goto done;
    if(WaitForSingleObject(caller,0)!=WAIT_TIMEOUT){error=ERROR_ACCESS_DENIED;goto respond;}
    pid=GetProcessId(caller);
    if(!pid || !AttachConsole(pid)){error=GetLastError();goto respond;}
    /* GetConsoleWindow is presentation metadata, not Console identity.  In
     * particular, a modern pseudoconsole may report distinct message-only
     * HWND values before and after AttachConsole.  NTSRV authenticates this
     * bootstrap with its suspended-process admission, the creator PID, and
     * the one-shot capability reservation; do not replace that authority
     * with an implementation-dependent HWND comparison. */
    error=OpenNtBaseClientRegisterFrontendRoot(notification);if(error)goto respond;
    error=OpenNtBaseClientReportCurrentConsoleMembers();
    if(error)goto respond;
    if(lease) {
        uint64_t actual_window=(uint64_t)(UINT_PTR)GetConsoleWindow();
        error=OpenNtBaseClientRegisterFrontendLease(actual_window,pid,borrowed,retire,restored);
        if(error)goto respond;
    }
    error=frontend_service_start_process(notification,caller,retire,&service);if(error)goto respond;
respond:
    {
        DWORD sent=OpenNtBaseClientFrontendStartupResult(notification,error);
        if(sent)error=sent;
    }
    if(error)goto done;
    /* The service waits for actual I/O-user retirement, not creator exit. */
    if(WaitForSingleObject(frontend_service_thread(service),INFINITE)==WAIT_OBJECT_0){
        if(!GetExitCodeThread(frontend_service_thread(service),&ignored))error=GetLastError();
        else error=ignored;
    }else error=GetLastError();
done:
    {
        DWORD close_error=frontend_service_close(service),ack_error=ERROR_SUCCESS;
        if(!error && close_error)error=close_error;
        /* Report to the broker only after canonical-buffer selection and
         * input-mode/cursor-shape restoration. The launcher has no direct
         * acknowledgement or shutdown channel to this frontend. */
        if(!close_error) {
            ack_error=OpenNtBaseClientFrontendConsoleRestored();
            if(ack_error && !error)error=ack_error;
        }
    }
    OpenNtBaseClientDisconnectCurrent();
    if(notification)CloseHandle(notification);
    CloseHandle(caller);CloseHandle(retire);CloseHandle(restored);
    return error;
}
int wmain(int argc,WCHAR **argv)
{
    UINT_PTR caller,notification,retire,restored;
    uint64_t console_window=0;
    BOOL lease=FALSE,borrowed=FALSE;
    DWORD result;
    if((argc!=6 && argc!=8) || wcscmp(argv[1],L"--session"))return ERROR_INVALID_PARAMETER;
    if(!frontend_session_resource(argv[2],&caller) ||
       !frontend_session_resource(argv[3],&notification) ||
       !frontend_session_resource(argv[4],&retire) ||
       !frontend_session_resource(argv[5],&restored))return ERROR_INVALID_PARAMETER;
    if(argc==8) {
        lease=TRUE;
        if(wcscmp(argv[6],L"0") && wcscmp(argv[6],L"1"))return ERROR_INVALID_PARAMETER;
        borrowed=!wcscmp(argv[6],L"1");
        if(!frontend_session_window(argv[7],&console_window))return ERROR_INVALID_PARAMETER;
    }
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return ERROR_NOT_ENOUGH_MEMORY;
    result=session_entry((HANDLE)caller,(HANDLE)notification,(HANDLE)retire,
        (HANDLE)restored,lease,borrowed);
    HeapDestroy(CsrPortHeap);return (int)result;
}

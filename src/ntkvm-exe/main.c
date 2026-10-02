#include "interface/frontend_bootstrap.h"
#include "session_service.h"
#include "interface/native_request_protocol.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PVOID CsrPortHeap;
static void bootstrap_trace(const char *stage,DWORD error)
{
    WCHAR path[MAX_PATH];char text[96];HANDLE file;DWORD count,length;
    length=GetEnvironmentVariableW(L"NTVDM_BOOTSTRAP_TRACE",path,ARRAYSIZE(path));
    if(!length || length>=ARRAYSIZE(path))return;
    file=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return;
    count=(DWORD)sprintf_s(text,sizeof(text),"ntkvm %s %lu\r\n",stage,error);
    if(count)WriteFile(file,text,count,&length,NULL);CloseHandle(file);
}
static DWORD session_entry(HANDLE pipe,HANDLE caller,HANDLE notification,HANDLE retire,HANDLE restored,
    BOOL lease,BOOL borrowed,uint64_t console_window)
{
    DWORD pid=0,error,ignored;
    HANDLE event=NULL,broker=NULL;
    frontend_session_service *service=NULL;
    frontend_bootstrap_reply reply={FRONTEND_BOOTSTRAP_VERSION,0,APP_VERSION};
    /* The pipe belongs to the authenticated broker, not to the launcher.
     * The separately inherited caller capability identifies the real Console. */
    error=OpenNtBaseClientConnectCurrent();if(error){bootstrap_trace("connect",error);goto done;}
    error=OpenNtBaseClientWatchBroker();if(error){bootstrap_trace("watch",error);goto done;}
    error=OpenNtBaseClientBrokerProcess(&broker);if(error)goto done;
    if(!GetNamedPipeServerProcessId(pipe,&pid) || !pid || pid!=GetProcessId(broker) ||
        WaitForSingleObject(caller,0)!=WAIT_TIMEOUT){error=ERROR_ACCESS_DENIED;bootstrap_trace("identity",error);goto done;}
    pid=GetProcessId(caller);
    if(!pid || !AttachConsole(pid)){error=GetLastError();bootstrap_trace("attach",error);goto done;}
    if(lease && (uint64_t)(UINT_PTR)GetConsoleWindow()!=console_window)
        {error=ERROR_ACCESS_DENIED;bootstrap_trace("console-match",error);goto done;}
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!event){error=GetLastError();goto done;}
    error=OpenNtBaseClientRegisterFrontendRoot(notification);if(error){bootstrap_trace("register",error);goto respond;}
    error=OpenNtBaseClientReportCurrentConsoleMembers();
    if(error){bootstrap_trace("console-identity",error);goto respond;}
    if(lease) {
        error=OpenNtBaseClientRegisterFrontendLease(console_window,pid,borrowed,retire,restored);
        if(error){bootstrap_trace("lease",error);goto respond;}
        error=frontend_service_start_process_lease(notification,notification,caller,retire,
            restored,borrowed,&service);
    }else error=frontend_service_start_process(notification,notification,caller,retire,&service);
    if(error){bootstrap_trace("service",error);goto respond;}
respond:
    reply.status=error;
    {
        DWORD sent=frontend_request_transfer(pipe,broker,NULL,event,TRUE,&reply,sizeof(reply));
        if(sent)error=sent;
    }
    if(error)goto done;
    /* The service waits for actual I/O-user retirement, not creator exit. */
    if(WaitForSingleObject(frontend_service_thread(service),INFINITE)==WAIT_OBJECT_0){
        if(!GetExitCodeThread(frontend_service_thread(service),&ignored))error=GetLastError();
        else error=ignored;
    }else error=GetLastError();
    bootstrap_trace("thread",error);
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
    if(event)CloseHandle(event);
    if(broker)CloseHandle(broker);
    CloseHandle(pipe);CloseHandle(caller);CloseHandle(retire);CloseHandle(restored);
    return error;
}
int wmain(int argc,WCHAR **argv)
{
    WCHAR *end;
    UINT_PTR pipe,caller,notification,retire,restored;
    uint64_t console_window=0;
    BOOL lease=FALSE,borrowed=FALSE;
    DWORD result;
    if((argc!=7 && argc!=9) || wcscmp(argv[1],L"--session"))return ERROR_INVALID_PARAMETER;
    pipe=(UINT_PTR)wcstoul(argv[2],&end,16);if(!pipe || *end)return ERROR_INVALID_PARAMETER;
    caller=(UINT_PTR)wcstoul(argv[3],&end,16);if(!caller || *end)return ERROR_INVALID_PARAMETER;
    notification=(UINT_PTR)wcstoul(argv[4],&end,16);if(!notification || *end)return ERROR_INVALID_PARAMETER;
    retire=(UINT_PTR)wcstoul(argv[5],&end,16);if(!retire || *end)return ERROR_INVALID_PARAMETER;
    restored=(UINT_PTR)wcstoul(argv[6],&end,16);if(!restored || *end)return ERROR_INVALID_PARAMETER;
    if(argc==9) {
        lease=TRUE;
        if(wcscmp(argv[7],L"0") && wcscmp(argv[7],L"1"))return ERROR_INVALID_PARAMETER;
        borrowed=!wcscmp(argv[7],L"1");
        console_window=_wcstoui64(argv[8],&end,16);
        if(!console_window || *end)return ERROR_INVALID_PARAMETER;
    }
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return ERROR_NOT_ENOUGH_MEMORY;
    result=session_entry((HANDLE)pipe,(HANDLE)caller,(HANDLE)notification,(HANDLE)retire,
        (HANDLE)restored,lease,borrowed,console_window);
    HeapDestroy(CsrPortHeap);return (int)result;
}

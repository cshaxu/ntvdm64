#include "bootstrap.h"
#include "session_service.h"
#include "native_console_host.h"
#include "native_request_protocol.h"
#include "basesrv-exe/opennt/include/base_rpc_client.h"
#include <shellapi.h>
#include <stdlib.h>
#include <string.h>

PVOID CsrPortHeap;
static DWORD session_entry(HANDLE pipe,HANDLE caller,HANDLE notification)
{
    DWORD pid=0,error,ignored;
    HANDLE event=NULL;
    frontend_session_service *service=NULL;
    frontend_bootstrap_reply reply={FRONTEND_BOOTSTRAP_VERSION,0,APP_VERSION};
    /* The inherited process capability pins the creator; the private pipe
     * must have been created by that same process. Arguments alone grant nothing. */
    if(!GetNamedPipeServerProcessId(pipe,&pid) || !pid || pid!=GetProcessId(caller) ||
        WaitForSingleObject(caller,0)!=WAIT_TIMEOUT)return ERROR_ACCESS_DENIED;
    if(!AttachConsole(pid))return GetLastError();
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!event){error=GetLastError();goto done;}
    error=OpenNtBaseClientConnectCurrent();if(error)goto respond;
    error=OpenNtBaseClientWatchBroker();if(error)goto respond;
    error=OpenNtBaseClientRegisterFrontendRoot(notification);if(error)goto respond;
    error=frontend_service_start_process(notification,notification,caller,&service);if(error)goto respond;
respond:
    reply.status=error;
    {
        DWORD sent=frontend_request_transfer(pipe,caller,NULL,event,TRUE,&reply,sizeof(reply));
        if(sent)error=sent;
    }
    if(error)goto done;
    /* The service waits for actual I/O-user retirement, not creator exit. */
    if(WaitForSingleObject(frontend_service_thread(service),INFINITE)==WAIT_OBJECT_0){
        if(!GetExitCodeThread(frontend_service_thread(service),&ignored))error=GetLastError();
        else error=ignored;
    }else error=GetLastError();
done:
    frontend_service_close(service);
    OpenNtBaseClientDisconnectCurrent();
    if(notification)CloseHandle(notification);
    if(event)CloseHandle(event);
    CloseHandle(pipe);CloseHandle(caller);
    return error;
}
int wmain(int argc,WCHAR **argv)
{
    WCHAR *end;
    UINT_PTR pipe,caller,notification;
    DWORD result;
    if(argc==2 && !wcscmp(argv[1],L"--internal-native-console"))return (int)run16_native_console_host();
    if(argc!=5 || wcscmp(argv[1],L"--session"))return ERROR_INVALID_PARAMETER;
    pipe=(UINT_PTR)wcstoul(argv[2],&end,16);if(!pipe || *end)return ERROR_INVALID_PARAMETER;
    caller=(UINT_PTR)wcstoul(argv[3],&end,16);if(!caller || *end)return ERROR_INVALID_PARAMETER;
    notification=(UINT_PTR)wcstoul(argv[4],&end,16);if(!notification || *end)return ERROR_INVALID_PARAMETER;
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return ERROR_NOT_ENOUGH_MEMORY;
    result=session_entry((HANDLE)pipe,(HANDLE)caller,(HANDLE)notification);
    HeapDestroy(CsrPortHeap);return (int)result;
}

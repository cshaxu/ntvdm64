#include "bootstrap.h"
#include "session_service.h"
#include "native_request_protocol.h"
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
static DWORD session_entry(HANDLE pipe,HANDLE caller,HANDLE notification,HANDLE retire,HANDLE restored)
{
    DWORD pid=0,error,ignored;
    HANDLE event=NULL;
    frontend_session_service *service=NULL;
    frontend_bootstrap_reply reply={FRONTEND_BOOTSTRAP_VERSION,0,APP_VERSION};
    /* The inherited process capability pins the creator; the private pipe
     * must have been created by that same process. Arguments alone grant nothing. */
    if(!GetNamedPipeServerProcessId(pipe,&pid) || !pid || pid!=GetProcessId(caller) ||
        WaitForSingleObject(caller,0)!=WAIT_TIMEOUT){bootstrap_trace("identity",ERROR_ACCESS_DENIED);return ERROR_ACCESS_DENIED;}
    if(!AttachConsole(pid)){error=GetLastError();bootstrap_trace("attach",error);return error;}
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!event){error=GetLastError();goto done;}
    error=OpenNtBaseClientConnectCurrent();if(error){bootstrap_trace("connect",error);goto respond;}
    error=OpenNtBaseClientWatchBroker();if(error){bootstrap_trace("watch",error);goto respond;}
    error=OpenNtBaseClientRegisterFrontendRoot(notification);if(error){bootstrap_trace("register",error);goto respond;}
    error=frontend_service_start_process(notification,notification,caller,retire,&service);if(error){bootstrap_trace("service",error);goto respond;}
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
    bootstrap_trace("thread",error);
done:
    {
        DWORD close_error=frontend_service_close(service),ack_error=ERROR_SUCCESS;
        BOOL acknowledged=FALSE;
        if(!error && close_error)error=close_error;
        /* The launcher holds only SYNCHRONIZE access to this private event.
         * Signal it after, never before, canonical-buffer selection and
         * input-mode/cursor-shape restoration. */
        if(!close_error) {
            acknowledged=SetEvent(restored);
            if(!acknowledged)ack_error=GetLastError();
            if(!acknowledged && !error)error=ack_error;
        }
    }
    OpenNtBaseClientDisconnectCurrent();
    if(notification)CloseHandle(notification);
    if(event)CloseHandle(event);
    CloseHandle(pipe);CloseHandle(caller);CloseHandle(retire);CloseHandle(restored);
    return error;
}
int wmain(int argc,WCHAR **argv)
{
    WCHAR *end;
    UINT_PTR pipe,caller,notification,retire,restored;
    DWORD result;
    if(argc!=7 || wcscmp(argv[1],L"--session"))return ERROR_INVALID_PARAMETER;
    pipe=(UINT_PTR)wcstoul(argv[2],&end,16);if(!pipe || *end)return ERROR_INVALID_PARAMETER;
    caller=(UINT_PTR)wcstoul(argv[3],&end,16);if(!caller || *end)return ERROR_INVALID_PARAMETER;
    notification=(UINT_PTR)wcstoul(argv[4],&end,16);if(!notification || *end)return ERROR_INVALID_PARAMETER;
    retire=(UINT_PTR)wcstoul(argv[5],&end,16);if(!retire || *end)return ERROR_INVALID_PARAMETER;
    restored=(UINT_PTR)wcstoul(argv[6],&end,16);if(!restored || *end)return ERROR_INVALID_PARAMETER;
    CsrPortHeap=HeapCreate(0,0,0);if(!CsrPortHeap)return ERROR_NOT_ENOUGH_MEMORY;
    result=session_entry((HANDLE)pipe,(HANDLE)caller,(HANDLE)notification,(HANDLE)retire,(HANDLE)restored);
    HeapDestroy(CsrPortHeap);return (int)result;
}

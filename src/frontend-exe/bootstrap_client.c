#include "bootstrap.h"
#include "native_request_protocol.h"
#include "basesrv-exe/opennt/include/base_rpc_client.h"
#include <stdio.h>
#include <string.h>

void frontend_bootstrap_release(frontend_connection *connection)
{
    if(!connection)return;
    if(connection->capability)CloseHandle(connection->capability);
    if(connection->channel)CloseHandle(connection->channel);
    if(connection->process)CloseHandle(connection->process);
    ZeroMemory(connection,sizeof(*connection));
}
DWORD frontend_bootstrap_start(PCWSTR image,frontend_connection *output)
{
    static LONG serial;
    WCHAR name[96],command[1024];
    HANDLE server=INVALID_HANDLE_VALUE,child_pipe=INVALID_HANDLE_VALUE,caller=NULL,event=NULL,verified=NULL;
    HANDLE inherited[3],deadline=NULL,notification=NULL;
    LARGE_INTEGER due;
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
    STARTUPINFOEXW startup={0};PROCESS_INFORMATION process={0};
    frontend_bootstrap_reply reply={0};
    const char expected_version[APP_VERSION_BYTES]=APP_VERSION;
    SIZE_T size=0;BOOL attributes=FALSE,handed_off=FALSE;
    DWORD error=ERROR_SUCCESS,generation;
    if(!image || !*image || !output)return ERROR_INVALID_PARAMETER;
    ZeroMemory(output,sizeof(*output));
    swprintf_s(name,96,L"\\\\.\\pipe\\ntvdm-frontend-start-%lu-%lu",GetCurrentProcessId(),(DWORD)InterlockedIncrement(&serial));
    server=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,4096,4096,0,NULL);
    if(server==INVALID_HANDLE_VALUE){error=GetLastError();goto done;}
    child_pipe=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,&security,OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED|SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION,NULL);
    if(child_pipe==INVALID_HANDLE_VALUE){error=GetLastError();goto done;}
    if(!DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),&caller,
        PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,TRUE,0)){error=GetLastError();goto done;}
    /* Own every local handle before accepting an untrusted reply. The child
     * receives the notification object; the launcher retains wait-only access.
     * No reply-supplied handle can be closed or leaked during rejection. */
    notification=CreateEventW(&security,TRUE,FALSE,NULL);
    if(!notification || !DuplicateHandle(GetCurrentProcess(),notification,GetCurrentProcess(),
        &output->capability,SYNCHRONIZE,FALSE,0)){error=GetLastError();goto done;}
    event=CreateEventW(NULL,TRUE,FALSE,NULL);if(!event){error=GetLastError();goto done;}
    {
        OVERLAPPED io={0};DWORD ignored;io.hEvent=event;
        if(!ConnectNamedPipe(server,&io) && GetLastError()!=ERROR_PIPE_CONNECTED){
            error=GetLastError();CancelIoEx(server,&io);GetOverlappedResult(server,&io,&ignored,TRUE);goto done;
        }
    }
    startup.StartupInfo.cb=sizeof(startup);
    /* DETACHED_PROCESS avoids a transient Console. Do not pass SW_HIDE:
     * it overrides the first ShowWindow of this process, including its
     * later user-requested KVM Window. Hidden backends have their own role. */
    InitializeProcThreadAttributeList(NULL,1,0,&size);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,size);
    if(!startup.lpAttributeList){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    if(!InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&size)){error=GetLastError();goto done;}
    attributes=TRUE;inherited[0]=child_pipe;inherited[1]=caller;inherited[2]=notification;
    if(!UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
        inherited,sizeof(inherited),NULL,NULL)){error=GetLastError();goto done;}
    if(swprintf_s(command,1024,L"\"%ls\" --session %Ix %Ix %Ix",image,
        (UINT_PTR)child_pipe,(UINT_PTR)caller,(UINT_PTR)notification)<0){error=ERROR_FILENAME_EXCED_RANGE;goto done;}
    if(!CreateProcessW(image,command,NULL,NULL,TRUE,EXTENDED_STARTUPINFO_PRESENT|DETACHED_PROCESS,
        NULL,NULL,&startup.StartupInfo,&process)){error=GetLastError();goto done;}
    CloseHandle(process.hThread);process.hThread=NULL;
    CloseHandle(child_pipe);child_pipe=INVALID_HANDLE_VALUE;
    CloseHandle(notification);notification=NULL;
    /* Bound only pre-handoff startup. A live but unresponsive peer must not
     * hold the launcher forever; this is not a running-target lifetime limit. */
    deadline=CreateWaitableTimerW(NULL,TRUE,NULL);
    if(!deadline){error=GetLastError();goto done;}
    due.QuadPart=-10LL*1000*10000;
    if(!SetWaitableTimer(deadline,&due,0,NULL,NULL,FALSE)){error=GetLastError();goto done;}
    error=frontend_request_transfer(server,process.hProcess,deadline,event,FALSE,&reply,sizeof(reply));
    if(error==ERROR_OPERATION_ABORTED && WaitForSingleObject(deadline,0)==WAIT_OBJECT_0)error=ERROR_TIMEOUT;
    if(error)goto done;
    if(reply.version!=FRONTEND_BOOTSTRAP_VERSION || memcmp(reply.application,expected_version,sizeof(expected_version))){error=ERROR_REVISION_MISMATCH;goto done;}
    if(reply.status){error=reply.status;goto done;}
    error=OpenNtBaseClientRetainFrontendRoot(output->capability,&verified,&generation);
    if(error)goto done;
    if(GetProcessId(verified)!=process.dwProcessId){error=ERROR_ACCESS_DENIED;goto done;}
    output->process=process.hProcess;process.hProcess=NULL;
    output->channel=server;server=INVALID_HANDLE_VALUE;handed_off=TRUE;
done:
    if(verified)CloseHandle(verified);
    if(process.hProcess){
        /* Only this unaccepted bootstrap, before any task can be submitted. */
        if(!handed_off)TerminateProcess(process.hProcess,error);
        CloseHandle(process.hProcess);
    }
    if(deadline)CloseHandle(deadline);
    if(event)CloseHandle(event);
    if(caller)CloseHandle(caller);
    if(notification)CloseHandle(notification);
    if(child_pipe!=INVALID_HANDLE_VALUE)CloseHandle(child_pipe);
    if(server!=INVALID_HANDLE_VALUE)CloseHandle(server);
    if(attributes)DeleteProcThreadAttributeList(startup.lpAttributeList);
    if(startup.lpAttributeList)HeapFree(GetProcessHeap(),0,startup.lpAttributeList);
    if(error)frontend_bootstrap_release(output);
    return error;
}

/* Client-only link: no renderer, input pump, helper or service implementation.
 * Broker admission is mocked here; real RPC identity tests remain mandatory. */
#include <windows.h>
#include <stdio.h>
#include "ntkvm-exe/native_request_client.h"
#include "ntkvm-exe/native_request_protocol.h"

static HANDLE peer_thread;
static HANDLE peer_process;
static DWORD scenario,peer_error;
static DWORD WINAPI peer(void *context)
{
    HANDLE pipe=context,event=CreateEventW(NULL,TRUE,FALSE,NULL);
    native_request_header header;
    native_request_reply reply={NATIVE_REQUEST_VERSION,0,0};
    BYTE *payload=NULL;
    DWORD error;
    PROCESS_INFORMATION process={0};
    error=frontend_request_transfer(pipe,peer_process,NULL,event,FALSE,&header,sizeof(header));
    if(error)goto done;
    if(header.version!=NATIVE_REQUEST_VERSION || header.bytes>65536){error=ERROR_INVALID_DATA;goto done;}
    payload=HeapAlloc(GetProcessHeap(),0,header.bytes);
    if(!payload){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    error=frontend_request_transfer(pipe,peer_process,NULL,event,FALSE,payload,header.bytes);
    if(error)goto done;
    if(scenario==0){
        error=run16_native_launch_start(payload,header.bytes,&process);
        if(error)goto done;
        CloseHandle(process.hThread);
        /* Same-process fixture recipient; ownership transfers to the client. */
        reply.target=(uint64_t)(ULONG_PTR)process.hProcess;
        reply.receipt=(uint64_t)(ULONG_PTR)CreateEventW(NULL,TRUE,TRUE,NULL);
        if(!reply.receipt){error=GetLastError();goto done;}
    }else if(scenario==1)reply.error=ERROR_ACCESS_DENIED;
    else if(scenario==2)reply.version++;
    else if(scenario==3){reply.error=ERROR_ACCESS_DENIED;reply.target=1;}
    else if(scenario==4)goto done; /* EOF is not target success. */
    else if(scenario==5){
        error=frontend_request_transfer(pipe,peer_process,NULL,event,TRUE,&reply,sizeof(reply)/2);
        goto done;
    }
    error=frontend_request_transfer(pipe,peer_process,NULL,event,TRUE,&reply,sizeof(reply));
done:
    peer_error=error;
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    CloseHandle(event);CloseHandle(pipe);
    return error;
}
DWORD OpenNtBaseClientSubmitFrontendChannel(HANDLE capability,HANDLE pipe)
{
    HANDLE copy=NULL;
    if(capability!=(HANDLE)1)return ERROR_ACCESS_DENIED;
    if(!DuplicateHandle(GetCurrentProcess(),pipe,GetCurrentProcess(),&copy,0,FALSE,DUPLICATE_SAME_ACCESS))return GetLastError();
    peer_thread=CreateThread(NULL,0,peer,copy,0,NULL);
    if(!peer_thread){DWORD error=GetLastError();CloseHandle(copy);return error;}
    return 0;
}
int main(void)
{
    WCHAR image[MAX_PATH],command[2*MAX_PATH],directory[MAX_PATH];
    LPWCH environment;
    run16_native_start start={0};
    DWORD expected[]={0,ERROR_ACCESS_DENIED,ERROR_INVALID_DATA,ERROR_INVALID_DATA,ERROR_BROKEN_PIPE,ERROR_BROKEN_PIPE};
    if(!GetEnvironmentVariableW(L"COMSPEC",image,MAX_PATH))return 1;
    if(!GetCurrentDirectoryW(MAX_PATH,directory))return 1;
    environment=GetEnvironmentStringsW();if(!environment)return 1;
    peer_process=OpenProcess(SYNCHRONIZE,FALSE,GetCurrentProcessId());if(!peer_process)return 1;
    swprintf_s(command,2*MAX_PATH,L"\"%ls\" /d /c exit 37",image);
    start.application=image;start.command=command;start.directory=directory;start.environment=environment;
    for(scenario=0;scenario<6;++scenario){
        HANDLE target=NULL;DWORD error,result=0;
        peer_error=0;peer_thread=NULL;
        error=run16_native_request_submit(peer_process,(HANDLE)1,&start,&target);
        if(!peer_thread || WaitForSingleObject(peer_thread,5000)!=WAIT_OBJECT_0){printf("FAIL admission case=%lu error=%lu\n",scenario,error);return 2;}
        CloseHandle(peer_thread);
        if(error!=expected[scenario] || peer_error){printf("FAIL case=%lu error=%lu peer=%lu\n",scenario,error,peer_error);return 3;}
        if(!error){
            if(!target || WaitForSingleObject(target,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(target,&result) || result!=37)return 4;
            CloseHandle(target);
        }else if(target)return 5;
    }
    FreeEnvironmentStringsW(environment);
    CloseHandle(peer_process);
    puts("PASS client-only link: actual target 37; rejection, version, contradictory reply, EOF and partial reply fail without UI ownership");
    return 0;
}

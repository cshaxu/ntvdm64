/* Client-only ownership plus broker-owned wire negatives. The broker test
 * substitute owns the worker channel and uses production reply validators;
 * actual RPC identity and process lifecycle tests remain mandatory. */
#include <windows.h>
#include <stdio.h>
#include "interface/native_request_client.h"
#include "interface/native_request_protocol.h"
#include "ntsrv-exe/transport/native_control.h"

static HANDLE peer_thread;
static HANDLE peer_process;
static DWORD scenario,peer_error;
static HANDLE broker_pipe;
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
    if(header.version!=NATIVE_REQUEST_VERSION || header.bytes>NATIVE_LAUNCH_MAX_BYTES){error=ERROR_INVALID_DATA;goto done;}
    if(scenario>=9) {
        if(header.bytes){error=ERROR_INVALID_DATA;goto done;}
        if(scenario==10)reply.error=ERROR_ACCESS_DENIED;
        if(scenario==11)reply.target=1; /* Resume must never export a target. */
        error=frontend_request_transfer(pipe,peer_process,NULL,event,TRUE,&reply,sizeof(reply));
        goto done;
    }
    payload=HeapAlloc(GetProcessHeap(),0,header.bytes);
    if(!payload){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    error=frontend_request_transfer(pipe,peer_process,NULL,event,FALSE,payload,header.bytes);
    if(error)goto done;
    if(scenario==0 || scenario>=6){
        error=run16_native_launch_start(payload,header.bytes,&process);
        if(error)goto done;
        CloseHandle(process.hThread);
        /* Same-process fixture recipient; ownership transfers to the client. */
        reply.target=(uint64_t)(ULONG_PTR)process.hProcess;
        reply.receipt=(uint64_t)(ULONG_PTR)CreateEventW(NULL,TRUE,TRUE,NULL);
        reply.request=91;
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
    if(!error && scenario>=6 && scenario!=8) {
        native_request_completion completion={NATIVE_REQUEST_VERSION,
            scenario==7 ? ERROR_WRITE_FAULT : 0};
        if(scenario==6)Sleep(2100); /* A completed target is not the frame barrier. */
        error=frontend_request_transfer(pipe,peer_process,NULL,event,TRUE,&completion,sizeof(completion));
    }
done:
    peer_error=error;
    if(payload)HeapFree(GetProcessHeap(),0,payload);
    CloseHandle(event);CloseHandle(pipe);
    return error;
}
DWORD OpenNtBaseClientSubmitNativeRequest(HANDLE capability,DWORD bytes,BYTE *payload,
    HANDLE *target,HANDLE *receipt,DWORD *request)
{
    static LONG serial;WCHAR name[96];DWORD error;
    HANDLE server=INVALID_HANDLE_VALUE,event=NULL;
    native_request_header header={NATIVE_REQUEST_VERSION,bytes};
    native_request_reply reply={0};
    *target=*receipt=NULL;*request=0;broker_pipe=INVALID_HANDLE_VALUE;
    if(capability!=(HANDLE)1)return ERROR_ACCESS_DENIED;
    swprintf_s(name,ARRAYSIZE(name),L"\\\\.\\pipe\\broker-native-fixture-%lu-%lu",
        GetCurrentProcessId(),(DWORD)InterlockedIncrement(&serial));
    server=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,65536,65536,0,NULL);
    if(server==INVALID_HANDLE_VALUE)return GetLastError();
    broker_pipe=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,NULL);
    if(broker_pipe==INVALID_HANDLE_VALUE){error=GetLastError();goto done;}
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!event){error=GetLastError();goto done;}
    {
        OVERLAPPED io={0};io.hEvent=event;
        if(!ConnectNamedPipe(server,&io) && GetLastError()!=ERROR_PIPE_CONNECTED) {
            DWORD ignored;error=GetLastError();CancelIoEx(server,&io);
            GetOverlappedResult(server,&io,&ignored,TRUE);goto done;
        }
    }
    peer_thread=CreateThread(NULL,0,peer,server,0,NULL);
    if(!peer_thread){error=GetLastError();goto done;}
    server=INVALID_HANDLE_VALUE; /* Peer owns its end, substitute broker owns ours. */
    error=frontend_request_transfer(broker_pipe,peer_process,NULL,event,TRUE,&header,sizeof(header));
    if(!error && bytes)error=frontend_request_transfer(broker_pipe,peer_process,NULL,event,TRUE,payload,bytes);
    if(!error)error=frontend_request_transfer(broker_pipe,peer_process,NULL,event,FALSE,&reply,sizeof(reply));
    if(!error)error=broker_native_reply_status(&reply,bytes!=0);
    if(!error && bytes) {
        *target=(HANDLE)(ULONG_PTR)reply.target;*receipt=(HANDLE)(ULONG_PTR)reply.receipt;
        *request=reply.request;
    }
done:
    if(server!=INVALID_HANDLE_VALUE)CloseHandle(server);
    if(event)CloseHandle(event);
    if((error || !bytes || scenario==0) && broker_pipe!=INVALID_HANDLE_VALUE) {
        CloseHandle(broker_pipe);broker_pipe=INVALID_HANDLE_VALUE;
    }
    return error;
}
DWORD OpenNtBaseClientFinishNativeRequest(DWORD request,DWORD *exit_code,DWORD *target_completed)
{
    HANDLE event;DWORD error;native_request_completion completion={0};
    if(request!=91 || !exit_code || !target_completed)return ERROR_INVALID_PARAMETER;
    if(scenario==12)return ERROR_PROCESS_ABORTED;
    *exit_code=37;
    *target_completed=TRUE;
    event=CreateEventW(NULL,TRUE,FALSE,NULL);if(!event)return GetLastError();
    error=frontend_request_transfer(broker_pipe,peer_process,NULL,event,FALSE,&completion,sizeof(completion));
    if(!error)error=broker_native_completion_status(&completion);
    CloseHandle(event);CloseHandle(broker_pipe);broker_pipe=INVALID_HANDLE_VALUE;
    return error;
}
int main(void)
{
    WCHAR image[MAX_PATH],command[2*MAX_PATH],directory[MAX_PATH];
    LPWCH environment;
    run16_native_start start={0};
    DWORD expected[]={0,ERROR_ACCESS_DENIED,ERROR_INVALID_DATA,ERROR_INVALID_DATA,ERROR_BROKEN_PIPE,ERROR_BROKEN_PIPE};
    {
        native_request_completion completion={NATIVE_REQUEST_VERSION,0,NATIVE_COMPLETION_CONSOLE_EMPTY};
        if(broker_native_completion_status(&completion))return 20;
        completion.flags=2;
        if(broker_native_completion_status(&completion)!=ERROR_INVALID_DATA)return 21;
        completion.flags=0;completion.version--;
        if(broker_native_completion_status(&completion)!=ERROR_INVALID_DATA)return 22;
    }
    if(!GetEnvironmentVariableW(L"COMSPEC",image,MAX_PATH))return 1;
    if(!GetCurrentDirectoryW(MAX_PATH,directory))return 1;
    environment=GetEnvironmentStringsW();if(!environment)return 1;
    peer_process=OpenProcess(SYNCHRONIZE,FALSE,GetCurrentProcessId());if(!peer_process)return 1;
    swprintf_s(command,2*MAX_PATH,L"\"%ls\" /d /c exit 37",image);
    start.application=image;start.command=command;start.directory=directory;start.environment=environment;
    for(scenario=0;scenario<6;++scenario){
        HANDLE target=NULL,receipt=NULL;DWORD error,result=0,request=0;
        peer_error=0;peer_thread=NULL;
        error=run16_native_request_submit((HANDLE)1,&start,&target,&receipt,&request);
        if(receipt)CloseHandle(receipt);
        if(!peer_thread || WaitForSingleObject(peer_thread,5000)!=WAIT_OBJECT_0){printf("FAIL admission case=%lu error=%lu\n",scenario,error);return 2;}
        CloseHandle(peer_thread);
        if(error!=expected[scenario] || peer_error){printf("FAIL case=%lu error=%lu peer=%lu\n",scenario,error,peer_error);return 3;}
        if(!error){
            if(!target || WaitForSingleObject(target,5000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(target,&result) || result!=37)return 4;
            CloseHandle(target);
        }else if(target)return 5;
    }
    for(scenario=6;scenario<=8;++scenario) {
        HANDLE target=NULL,receipt=NULL;
        DWORD result,error,request=0,completed=0,wanted=scenario==6 ? 0 : scenario==7 ? ERROR_WRITE_FAULT : ERROR_BROKEN_PIPE;
        error=run16_native_request_submit((HANDLE)1,&start,&target,&receipt,&request);
        if(error || WaitForSingleObject(target,5000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(target,&result) || result!=37)return 6;
        result=0;
        error=run16_native_request_finish(request,&result,&completed);
        if(error!=wanted || result!=37 || completed!=TRUE ||
            WaitForSingleObject(peer_thread,5000)!=WAIT_OBJECT_0 || peer_error)return 7;
        printf("PASS final presentation case=%lu status=%lu target=37\n",scenario,error);
        CloseHandle(peer_thread);CloseHandle(receipt);CloseHandle(target);
    }
    for(scenario=9;scenario<=11;++scenario) {
        DWORD error,wanted=scenario==9 ? 0 : scenario==10 ? ERROR_ACCESS_DENIED : ERROR_INVALID_DATA;
        peer_error=0;peer_thread=NULL;
        error=run16_native_request_resume((HANDLE)1);
        if(!peer_thread || WaitForSingleObject(peer_thread,5000)!=WAIT_OBJECT_0 || peer_error || error!=wanted)return 8;
        printf("PASS resume presentation case=%lu status=%lu no target\n",scenario,error);
        CloseHandle(peer_thread);
    }
    scenario=12;
    {
        DWORD result=99,completed=99;
        /* Broker failure must win over a missing final worker channel. */
        if(run16_native_request_finish(91,&result,&completed)!=
            ERROR_PROCESS_ABORTED || result || completed)return 9;
        puts("PASS broker worker-failure receipt returned without reading dead presentation channel");
    }
    FreeEnvironmentStringsW(environment);
    CloseHandle(peer_process);
    puts("PASS client-only link, worker execution only: actual target 37; rejection, version, contradictory reply, EOF and partial reply fail without UI ownership");
    return 0;
}

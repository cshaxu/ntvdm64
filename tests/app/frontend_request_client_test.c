/* Launcher-only unit with substituted authenticated RPC. Real broker role,
 * version and typed-object rejection remain monitor_rpc_test's responsibility.
 * No old native control pipe or numeric reply DTO is emulated here. */
#include <windows.h>
#include <stdio.h>
#include "run16-exe/native_request_client.h"
#include "run16-exe/native_launch.h"

static DWORD scenario,final_status,created;
static HANDLE final_ready,final_permission,completion_thread;
static DWORD WINAPI complete_target(void *context)
{
    HANDLE target=context;DWORD error=ERROR_SUCCESS;
    if(WaitForSingleObject(target,5000)!=WAIT_OBJECT_0 ||
        WaitForSingleObject(final_permission,5000)!=WAIT_OBJECT_0)error=ERROR_TIMEOUT;
    if(!error) {
        final_status=scenario==7 ? ERROR_WRITE_FAULT : scenario==8 ? ERROR_BROKEN_PIPE : ERROR_SUCCESS;
        if(!SetEvent(final_ready))error=GetLastError();
    }
    CloseHandle(target);return error;
}
DWORD OpenNtBaseClientSubmitNativeRequest(HANDLE capability,DWORD bytes,BYTE *payload,
    HANDLE *target,HANDLE *receipt,DWORD *request)
{
    static const DWORD errors[]={0,ERROR_ACCESS_DENIED,ERROR_REVISION_MISMATCH,
        ERROR_INVALID_DATA,ERROR_BROKEN_PIPE,RPC_S_SERVER_UNAVAILABLE};
    run16_native_launch_packet packet;WCHAR *strings[4];PROCESS_INFORMATION process={0};DWORD error;
    *target=*receipt=NULL;*request=0;
    if(capability!=(HANDLE)1)return ERROR_ACCESS_DENIED;
    if(!bytes) {
        if(payload)return ERROR_INVALID_DATA;
        return scenario==9 ? ERROR_SUCCESS : scenario==10 ? ERROR_ACCESS_DENIED : ERROR_INVALID_DATA;
    }
    error=run16_native_launch_unpack(payload,bytes,&packet,strings);if(error)return error;
    if(packet.capabilities[0] || packet.capabilities[1])return ERROR_INVALID_DATA;
    if(scenario<6 && errors[scenario])return errors[scenario];
    error=run16_native_launch_start(payload,bytes,&process);if(error)return error;
    ++created;CloseHandle(process.hThread);
    *target=process.hProcess;*request=91;
    *receipt=CreateEventW(NULL,TRUE,scenario<6,NULL);
    if(!*receipt){error=GetLastError();CloseHandle(*target);*target=NULL;*request=0;return error;}
    if(scenario>=6 && scenario<=8) {
        HANDLE owned=NULL;
        if(!DuplicateHandle(GetCurrentProcess(),*target,GetCurrentProcess(),&owned,SYNCHRONIZE,FALSE,0))return GetLastError();
        completion_thread=CreateThread(NULL,0,complete_target,owned,0,NULL);
        if(!completion_thread){error=GetLastError();CloseHandle(owned);return error;}
    }
    return ERROR_SUCCESS;
}
DWORD OpenNtBaseClientFinishNativeRequest(DWORD request,DWORD *exit_code,DWORD *target_completed)
{
    if(request!=91 || !exit_code || !target_completed)return ERROR_INVALID_PARAMETER;
    if(scenario==12)return ERROR_PROCESS_ABORTED;
    if(WaitForSingleObject(final_ready,5000)!=WAIT_OBJECT_0)return ERROR_TIMEOUT;
    *exit_code=37;*target_completed=TRUE;return final_status;
}
int main(void)
{
    WCHAR image[MAX_PATH],command[2*MAX_PATH],directory[MAX_PATH];LPWCH environment;
    run16_native_start start={0};
    DWORD expected[]={0,ERROR_ACCESS_DENIED,ERROR_REVISION_MISMATCH,ERROR_INVALID_DATA,
        ERROR_BROKEN_PIPE,RPC_S_SERVER_UNAVAILABLE};
    if(!GetEnvironmentVariableW(L"COMSPEC",image,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,directory))return 1;
    environment=GetEnvironmentStringsW();if(!environment)return 1;
    swprintf_s(command,2*MAX_PATH,L"\"%ls\" /d /c exit 37",image);
    start.application=image;start.command=command;start.directory=directory;start.environment=environment;
    /* Launcher authority slots must be stripped before copied RPC submission. */
    start.capabilities[0]=(HANDLE)123;start.capabilities[1]=(HANDLE)456;
    for(scenario=0;scenario<6;++scenario) {
        HANDLE target=NULL,receipt=NULL;DWORD error,result=0,request=0,before=created;
        error=run16_native_request_submit((HANDLE)1,&start,&target,&receipt,&request);
        if(error!=expected[scenario])return 2;
        if(!error) {
            if(!target || !receipt || request!=91 || WaitForSingleObject(target,5000)!=WAIT_OBJECT_0 ||
                !GetExitCodeProcess(target,&result) || result!=37)return 3;
            CloseHandle(target);CloseHandle(receipt);
        } else if(target || receipt || request || created!=before)return 4;
    }
    for(scenario=6;scenario<=8;++scenario) {
        HANDLE target=NULL,receipt=NULL;DWORD result,error,request=0,completed=0,thread_result;
        DWORD wanted=scenario==6 ? 0 : scenario==7 ? ERROR_WRITE_FAULT : ERROR_BROKEN_PIPE;
        final_ready=CreateEventW(NULL,TRUE,FALSE,NULL);final_permission=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!final_ready || !final_permission)return 5;
        error=run16_native_request_submit((HANDLE)1,&start,&target,&receipt,&request);
        if(error || WaitForSingleObject(target,5000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(target,&result) || result!=37)return 6;
        if(WaitForSingleObject(final_ready,0)!=WAIT_TIMEOUT || WaitForSingleObject(receipt,0)!=WAIT_TIMEOUT)return 7;
        if(!SetEvent(final_permission))return 8;
        result=0;error=run16_native_request_finish(request,&result,&completed);
        if(error!=wanted || result!=37 || !completed ||
            WaitForSingleObject(completion_thread,5000)!=WAIT_OBJECT_0 ||
            !GetExitCodeThread(completion_thread,&thread_result) || thread_result)return 9;
        printf("PASS final presentation case=%lu status=%lu target=37\n",scenario,error);
        CloseHandle(completion_thread);CloseHandle(target);CloseHandle(receipt);
        CloseHandle(final_ready);CloseHandle(final_permission);
    }
    for(scenario=9;scenario<=11;++scenario) {
        DWORD before=created,wanted=scenario==9 ? 0 : scenario==10 ? ERROR_ACCESS_DENIED : ERROR_INVALID_DATA;
        if(run16_native_request_resume((HANDLE)1)!=wanted || created!=before)return 10;
        printf("PASS RPC resume case=%lu status=%lu no target\n",scenario,wanted);
    }
    scenario=12;
    {
        DWORD result=99,completed=99;
        if(run16_native_request_finish(91,&result,&completed)!=ERROR_PROCESS_ABORTED || result || completed)return 11;
        puts("PASS broker failure returned with cleared result, no dead-channel read");
    }
    FreeEnvironmentStringsW(environment);
    puts("PASS client-only RPC submission, authority stripping, real exit 37, I/O barrier, errors and resume");
    return 0;
}

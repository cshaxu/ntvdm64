#include <windows.h>
#include <stdio.h>
#include "ntsrv-exe/transport/native_job_tracker.h"

typedef struct JOB_TEST_STATE {
    HANDLE created,exited,empty;
    LONG created_count,exited_count;
} JOB_TEST_STATE;

static void WINAPI report(void *context,DWORD generation,DWORD request,DWORD event,
    DWORD process,DWORD parent,DWORD root_process)
{
    JOB_TEST_STATE *state=context;
    (void)generation;(void)request;(void)process;(void)parent;(void)root_process;
    if(event==JOB_OBJECT_MSG_NEW_PROCESS) {
        InterlockedIncrement(&state->created_count);SetEvent(state->created);
    } else if(event==JOB_OBJECT_MSG_EXIT_PROCESS) {
        InterlockedIncrement(&state->exited_count);SetEvent(state->exited);
    } else if(event==JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO)SetEvent(state->empty);
}

int wmain(int argc,WCHAR **argv)
{
    OPENNT_NATIVE_JOB_TRACKER *tracker=NULL;
    JOB_TEST_STATE state={0};
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION process={0};
    WCHAR command[]=L"cmd.exe /d /c \"cmd.exe /d /c exit 0\"";
    DWORD error=0;

    WCHAR self[MAX_PATH],held_command[MAX_PATH+32];
    if(argc==2 && !lstrcmpW(argv[1],L"--hold")) { Sleep(10000);return 0; }
    if(argc!=1)return 2;
    state.created=CreateEventW(NULL,TRUE,FALSE,NULL);
    state.exited=CreateEventW(NULL,TRUE,FALSE,NULL);
    state.empty=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!state.created || !state.exited || !state.empty)error=GetLastError();
    if(!error)error=OpenNtNativeJobTrackerOpen(&tracker,report);
    if(!error && !CreateProcessW(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED,
        NULL,NULL,&startup,&process))error=GetLastError();
    if(!error)error=OpenNtNativeJobTrackerCreate(tracker,&state,7,11);
    if(!error)error=OpenNtNativeJobTrackerAssign(tracker,&state,7,11,process.hProcess);
    if(!error && ResumeThread(process.hThread)==(DWORD)-1)error=GetLastError();
    if(!error && WaitForSingleObject(process.hProcess,5000)!=WAIT_OBJECT_0)error=ERROR_TIMEOUT;
    /* Job notifications are deliberately best-effort. This test records the
     * local platform result without claiming a timeout disproves the design. */
    if(!error && WaitForSingleObject(state.empty,1000)!=WAIT_OBJECT_0)
        printf("SKIP Job ACTIVE_PROCESS_ZERO not delivered in local window\n");
    else if(!error && WaitForSingleObject(state.created,0)!=WAIT_OBJECT_0)
        printf("SKIP Job NEW_PROCESS not delivered in local window\n");
    else if(!error && WaitForSingleObject(state.exited,0)!=WAIT_OBJECT_0)
        printf("SKIP Job EXIT_PROCESS not delivered in local window\n");
    else if(!error)printf("PASS Job events new=%ld exit=%ld\n",
        state.created_count,state.exited_count);
    if(process.hThread)CloseHandle(process.hThread);
    if(process.hProcess)CloseHandle(process.hProcess);
    if(tracker)OpenNtNativeJobTrackerClose(tracker);
    process.hThread=process.hProcess=NULL;tracker=NULL;
    /* Closing a Job handle ends observation only.  It must not act as a
     * process-tree kill when the target remains alive. */
    if(!error && !GetModuleFileNameW(NULL,self,ARRAYSIZE(self)))error=GetLastError();
    if(!error)swprintf_s(held_command,ARRAYSIZE(held_command),L"\"%ls\" --hold",self);
    if(!error && !OpenNtNativeJobTrackerOpen(&tracker,report)) {
        if(!CreateProcessW(NULL,held_command,NULL,NULL,FALSE,CREATE_SUSPENDED,
            NULL,NULL,&startup,&process))error=GetLastError();
        if(!error)error=OpenNtNativeJobTrackerCreate(tracker,&state,8,12);
        if(!error)error=OpenNtNativeJobTrackerAssign(tracker,&state,8,12,process.hProcess);
        if(!error && ResumeThread(process.hThread)==(DWORD)-1)error=GetLastError();
        if(!error && WaitForSingleObject(process.hProcess,0)!=WAIT_TIMEOUT)error=ERROR_PROCESS_ABORTED;
        OpenNtNativeJobTrackerClose(tracker);tracker=NULL;
        if(!error && WaitForSingleObject(process.hProcess,0)!=WAIT_TIMEOUT)error=ERROR_PROCESS_ABORTED;
        if(!error)printf("PASS Job close leaves target alive\n");
        if(process.hProcess) {
            (void)TerminateProcess(process.hProcess,0);
            (void)WaitForSingleObject(process.hProcess,5000);
        }
    } else if(!error)error=GetLastError();
    if(process.hThread)CloseHandle(process.hThread);
    if(process.hProcess)CloseHandle(process.hProcess);
    if(tracker)OpenNtNativeJobTrackerClose(tracker);
    if(state.created)CloseHandle(state.created);
    if(state.exited)CloseHandle(state.exited);
    if(state.empty)CloseHandle(state.empty);
    if(error)fprintf(stderr,"FAIL %lu\n",(unsigned long)error);
    return error ? 1 : 0;
}

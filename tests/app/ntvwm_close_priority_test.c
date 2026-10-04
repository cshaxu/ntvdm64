/* Exercise the production exit-priority decision, not a second implementation.
 * The real Console close is replaced only in this fixture; shutdown/ack and
 * carrier termination use worker-base's production implementation. */
#define wmain ntvwm_unused_entry
#define ntvwm_console_close test_console_close
#include "../../src/ntvwm-exe/main.c"
#undef ntvwm_console_close
#undef wmain
#include <stdlib.h>

static HANDLE observed_close;
DWORD test_console_close(void)
{
    return SetEvent(observed_close) ? ERROR_SUCCESS : GetLastError();
}
int wmain(int argc,WCHAR **argv)
{
    unsigned failures=0,mode;
    if(argc==5) {
        native_membership state={0};CRITICAL_SECTION lock;
        mode=(unsigned)wcstoul(argv[1],NULL,10);
        observed_close=(HANDLE)(ULONG_PTR)_wcstoui64(argv[2],NULL,10);
        state.closed=(HANDLE)(ULONG_PTR)_wcstoui64(argv[3],NULL,10);
        state.quit=(HANDLE)(ULONG_PTR)_wcstoui64(argv[4],NULL,10);
        InitializeCriticalSection(&lock);state.lock=&lock;
        state.shutdown=CreateEventW(NULL,TRUE,mode==1,NULL);
        state.stop_requested=CreateEventW(NULL,TRUE,mode==2,NULL);
        if(!state.shutdown || !state.stop_requested)return ERROR_NOT_ENOUGH_MEMORY;
        /* quit is already set: cancellation must not hide a broker close. */
        honor_console_close(&state);
        CloseHandle(state.shutdown);CloseHandle(state.stop_requested);
        DeleteCriticalSection(&lock);
        return mode ? ERROR_INVALID_STATE : ERROR_PROCESS_ABORTED;
    }
    for(mode=0;mode<3;++mode) {
        SECURITY_ATTRIBUTES attributes={sizeof(attributes),NULL,TRUE};
        HANDLE close=CreateEventW(&attributes,TRUE,FALSE,NULL);
        HANDLE ack=CreateEventW(&attributes,TRUE,FALSE,NULL);
        HANDLE quit=CreateEventW(&attributes,TRUE,TRUE,NULL);
        WCHAR image[MAX_PATH],command[MAX_PATH+160];
        STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};
        DWORD result=STILL_ACTIVE;
        if(!close || !ack || !quit || !GetModuleFileNameW(NULL,image,MAX_PATH))return 1;
        swprintf_s(command,MAX_PATH+160,L"\"%s\" %u %Iu %Iu %Iu",image,mode,
            (ULONG_PTR)close,(ULONG_PTR)ack,(ULONG_PTR)quit);
        if(!CreateProcessW(image,command,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process))return 1;
        if(WaitForSingleObject(process.hProcess,5000)!=WAIT_OBJECT_0) {
            ++failures;TerminateProcess(process.hProcess,ERROR_TIMEOUT);
        }
        if(!GetExitCodeProcess(process.hProcess,&result) ||
            result!=(DWORD)(mode ? ERROR_CANCELLED : ERROR_PROCESS_ABORTED))++failures;
        if(WaitForSingleObject(close,0)!=(mode ? WAIT_OBJECT_0 : WAIT_TIMEOUT))++failures;
        if(WaitForSingleObject(ack,0)!=(mode ? WAIT_OBJECT_0 : WAIT_TIMEOUT))++failures;
        CloseHandle(process.hThread);CloseHandle(process.hProcess);
        CloseHandle(close);CloseHandle(ack);CloseHandle(quit);
    }
    printf("%s native close priority: three cancellation/close cases, %u failures\n",
        failures ? "FAIL" : "PASS",failures);
    return failures ? 1 : 0;
}

/* Exercise the production exit-priority decision, not a second implementation.
 * The real Console close is replaced only in this fixture; shutdown/ack and
 * carrier termination use worker-base's production implementation. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
static int ntvwm_unused_entry(int,WCHAR **);
#define wmain ntvwm_unused_entry
#define ntvwm_console_close test_console_close
#include "../../src/ntvwm-exe/main.c"
#undef ntvwm_console_close
#undef wmain
#include <stdlib.h>

static HANDLE observed_close;
static HANDLE observed_request;
static volatile LONG exit_requests;
DWORD OpenNtBaseClientFailure(DWORD error){return error;}
DWORD worker_base_request_exit(void)
{ InterlockedIncrement(&exit_requests);SetEvent(observed_request);return ERROR_SUCCESS; }
DWORD OpenNtBaseClientBrokerProcess(HANDLE *process)
{ return DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),process,
    SYNCHRONIZE,FALSE,0) ? ERROR_SUCCESS : GetLastError(); }
void ntvwm_trace_error(const char *phase,DWORD request,DWORD error)
{ (void)phase;(void)request;(void)error; }
void ntvwm_console_guard_stop(ntvwm_console_guard *guard){(void)guard;}
static DWORD WINAPI lose_service(void *context){service_lost(context);return 0;}
static DWORD WINAPI report_fault(void *context){request_fault_exit(context,ERROR_WRITE_FAULT);return 0;}
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
        state.console_close=CreateEventW(NULL,TRUE,mode==3,NULL);
        if(!state.shutdown || !state.stop_requested || !state.console_close)return ERROR_NOT_ENOUGH_MEMORY;
        if(mode==5 || mode==6) {
            HANDLE reporter;
            observed_request=state.quit;ResetEvent(observed_request);
            reporter=CreateThread(NULL,0,report_fault,&state,0,NULL);
            if(!reporter || WaitForSingleObject(observed_request,10000)!=WAIT_OBJECT_0 ||
                exit_requests!=1 || WaitForSingleObject(observed_close,0)!=WAIT_TIMEOUT ||
                WaitForSingleObject(reporter,0)!=WAIT_TIMEOUT)return ERROR_INVALID_STATE;
            /* Acceptance and the local admission stop are not permission. */
            ntvwm_close_for_worker_shutdown(&state);
            if(WaitForSingleObject(observed_close,0)!=WAIT_TIMEOUT)return ERROR_INVALID_STATE;
            SetEvent(mode==5 ? state.shutdown : state.console_close);
            WaitForSingleObject(reporter,10000);
            return ERROR_INVALID_STATE;
        }
        if(mode==4) {
            HANDLE losing;
            /* A real launch/rollback owns this lock. Confirmed service loss
             * must not close its Console while that operation is unfinished. */
            ResetEvent(state.quit);
            EnterCriticalSection(&lock);
            losing=CreateThread(NULL,0,lose_service,&state,0,NULL);
            if(!losing || WaitForSingleObject(state.quit,10000)!=WAIT_OBJECT_0 ||
                WaitForSingleObject(observed_close,0)!=WAIT_TIMEOUT)return ERROR_INVALID_STATE;
            LeaveCriticalSection(&lock);
            WaitForSingleObject(losing,10000);
            return ERROR_INVALID_STATE; /* Production shutdown must exit. */
        }
        /* quit is already set: cancellation must not hide a broker close. */
        ntvwm_close_for_console_shutdown(&state);
        ntvwm_close_for_worker_shutdown(&state);
        CloseHandle(state.shutdown);CloseHandle(state.stop_requested);CloseHandle(state.console_close);
        DeleteCriticalSection(&lock);
        return mode && mode!=2 ? ERROR_INVALID_STATE : ERROR_PROCESS_ABORTED;
    }
    for(mode=0;mode<7;++mode) {
        BOOL closes=mode!=0 && mode!=2;
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
        if(WaitForSingleObject(process.hProcess,10000)!=WAIT_OBJECT_0) {
            ++failures;TerminateProcess(process.hProcess,ERROR_TIMEOUT);
        }
        if(!GetExitCodeProcess(process.hProcess,&result) ||
            result!=(DWORD)(mode==4 ? ERROR_INVALID_HANDLE : closes ? ERROR_CANCELLED : ERROR_PROCESS_ABORTED))++failures;
        if(WaitForSingleObject(close,0)!=(closes ? WAIT_OBJECT_0 : WAIT_TIMEOUT))++failures;
        if(WaitForSingleObject(ack,0)!=(closes ? WAIT_OBJECT_0 : WAIT_TIMEOUT))++failures;
        CloseHandle(process.hThread);CloseHandle(process.hProcess);
        CloseHandle(close);CloseHandle(ack);CloseHandle(quit);
    }
    printf("%s native close priority: seven cancellation/permission/close/loss-fence cases, %u failures\n",
        failures ? "FAIL" : "PASS",failures);
    return failures ? 1 : 0;
}

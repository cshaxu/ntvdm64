/* Exercise the production exit-priority decision, not a second implementation.
 * The real Console close is replaced only in this fixture; shutdown/ack and
 * carrier termination use worker-base's production implementation. */
#define _WIN32_WINNT 0x0A00
#include <windows.h>
#include "worker-base/input_watch.h"
static int ntvwm_unused_entry(int,WCHAR **);
/* This initialization case must fail before creating either background
 * consumer. Do not link their unrelated I/O closure into this owner test. */
static unsigned unexpected_start;
static HANDLE unexpected_thread(void){++unexpected_start;return NULL;}
static worker_base_input_watch *unexpected_watch(void){++unexpected_start;return NULL;}
#define wmain ntvwm_unused_entry
#define ntvwm_console_close test_console_close
#define CreateThread(a,b,c,d,e,f) unexpected_thread()
#define worker_base_input_watch_create(a,b,c,d,e,f,g,h) unexpected_watch()
#include "../../src/ntvwm-exe/main.c"
#undef worker_base_input_watch_create
#undef CreateThread
#undef ntvwm_console_close
#undef wmain
#include <stdlib.h>

static HANDLE observed_close;
static HANDLE observed_request;
static volatile LONG exit_requests;
DWORD OpenNtBaseClientRegisterNativeBackend(HANDLE frontend)
{ (void)frontend;return ERROR_NOT_ENOUGH_MEMORY; }
DWORD worker_base_io_release_event(HANDLE *event)
{ (void)event;++unexpected_start;return ERROR_INVALID_STATE; }
DWORD OpenNtBaseClientFailure(DWORD error){return error;}
DWORD worker_base_request_exit(void)
{ InterlockedIncrement(&exit_requests);SetEvent(observed_request);return ERROR_SUCCESS; }
void ntvwm_trace_error(const char *phase,DWORD request,DWORD error)
{ printf("trace %s request=%lu error=%lu\n",phase,request,error);fflush(stdout); }
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
        /* Match Connect's real wait-only process handle, not a pseudo-handle. */
        if(!DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
            &state.server,SYNCHRONIZE,FALSE,0))return GetLastError();
        mode=(unsigned)wcstoul(argv[1],NULL,10);
        observed_close=(HANDLE)(ULONG_PTR)_wcstoui64(argv[2],NULL,10);
        state.closed=(HANDLE)(ULONG_PTR)_wcstoui64(argv[3],NULL,10);
        state.quit=(HANDLE)(ULONG_PTR)_wcstoui64(argv[4],NULL,10);
        InitializeCriticalSection(&lock);state.lock=&lock;
        state.shutdown=CreateEventW(NULL,TRUE,mode==1,NULL);
        state.stop_requested=CreateEventW(NULL,TRUE,mode==2,NULL);
        state.console_close=CreateEventW(NULL,TRUE,mode==3,NULL);
        if(!state.shutdown || !state.stop_requested || !state.console_close)return ERROR_NOT_ENOUGH_MEMORY;
        if(mode==5 || mode==6 || mode==7) {
            HANDLE reporter;
            if(mode==7) {
                HANDLE shutdown=state.shutdown,console_close=state.console_close;
                /* Exercise actual partial initialization, not just a fault
                 * reported by an otherwise fully initialized membership. */
                CloseHandle(state.stop_requested);state.stop_requested=NULL;
                state.closed=NULL;state.quit=NULL;
                if(membership_initialize(&state)!=ERROR_NOT_ENOUGH_MEMORY ||
                    state.shutdown!=shutdown || state.console_close!=console_close ||
                    WaitForSingleObject(shutdown,0)!=WAIT_TIMEOUT ||
                    WaitForSingleObject(console_close,0)!=WAIT_TIMEOUT || state.thread || unexpected_start)
                    return ERROR_INVALID_STATE;
                /* Observe the production close acknowledgement in the parent. */
                CloseHandle(state.closed);
                state.closed=(HANDLE)(ULONG_PTR)_wcstoui64(argv[3],NULL,10);
            }
            observed_request=state.quit;ResetEvent(observed_request);
            reporter=CreateThread(NULL,0,report_fault,&state,0,NULL);
            if(!reporter || WaitForSingleObject(observed_request,10000)!=WAIT_OBJECT_0 ||
                exit_requests!=1 || WaitForSingleObject(observed_close,0)!=WAIT_TIMEOUT ||
                WaitForSingleObject(reporter,0)!=WAIT_TIMEOUT)return ERROR_INVALID_STATE;
            /* Acceptance and the local admission stop are not permission. */
            ntvwm_close_for_worker_shutdown(&state);
            if(WaitForSingleObject(observed_close,0)!=WAIT_TIMEOUT)return ERROR_INVALID_STATE;
            SetEvent(mode==6 ? state.console_close : state.shutdown);
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
        CloseHandle(state.shutdown);CloseHandle(state.stop_requested);CloseHandle(state.console_close);CloseHandle(state.server);
        DeleteCriticalSection(&lock);
        return mode && mode!=2 ? ERROR_INVALID_STATE : ERROR_PROCESS_ABORTED;
    }
    {
        HANDLE waits[2]={GetCurrentProcess(),CreateEventW(NULL,TRUE,FALSE,NULL)},real=NULL;
        DWORD observed=WaitForMultipleObjects(2,waits,FALSE,0),error=GetLastError();
        printf("pseudo-process multiwait=%lu error=%lu\n",observed,error);
        if(!waits[1] || !DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
            &real,SYNCHRONIZE,FALSE,0))return 1;
        waits[0]=real;observed=WaitForMultipleObjects(2,waits,FALSE,0);
        printf("real-process multiwait=%lu\n",observed);
        if(observed!=WAIT_TIMEOUT)++failures;
        CloseHandle(real);CloseHandle(waits[1]);
    }
    for(mode=0;mode<8;++mode) {
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
        printf("mode=%u exit=%lu close=%lu ack=%lu\n",mode,result,
            WaitForSingleObject(close,0),WaitForSingleObject(ack,0));fflush(stdout);
        if(WaitForSingleObject(close,0)!=(closes ? WAIT_OBJECT_0 : WAIT_TIMEOUT))++failures;
        if(WaitForSingleObject(ack,0)!=(closes ? WAIT_OBJECT_0 : WAIT_TIMEOUT))++failures;
        CloseHandle(process.hThread);CloseHandle(process.hProcess);
        CloseHandle(close);CloseHandle(ack);CloseHandle(quit);
    }
    printf("%s native close priority: eight cancellation/permission/close/loss-fence/startup cases, %u failures\n",
        failures ? "FAIL" : "PASS",failures);
    return failures ? 1 : 0;
}

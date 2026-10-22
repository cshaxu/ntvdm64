/* MIT, Copyright (c) 2026 Neko. Narrowed from SoftPC test/lib/kvm-window/
 * win32/kvm_window_capture_contract_smoke.c lifetime cases. Exercise the real
 * imported startup/join/dispose code, substituting only OS failure boundaries
 * and a non-UI worker. No product fatal callback or timeout is introduced. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib/types/win32/scalar.h"
#include "lib/types/win32/sync.h"
#include "lib/kvm-window/window.h"

static unsigned fault,closed;
static lib_win32_dword LIB_WIN32_WINAPI idle_worker(void *opaque);
static lib_win32_handle LIB_WIN32_WINAPI idle_create(lib_win32_security_attributes *security,
    lib_size stack,lib_win32_thread_start_routine entry,void *opaque,
    lib_win32_dword flags,lib_win32_dword *identity)
{
    (void)entry;
    return lib_win32_create_thread(security,stack,idle_worker,opaque,flags,identity);
}
static lib_win32_dword LIB_WIN32_WINAPI startup_wait(lib_win32_dword count,
    const lib_win32_handle *handles,lib_win32_bool all,lib_win32_dword timeout)
{
    (void)count;(void)handles;(void)all;(void)timeout;
    return fault>=3 ? LIB_WIN32_WAIT_OBJECT_0 : LIB_WIN32_WAIT_FAILED;
}
static lib_win32_bool LIB_WIN32_WINAPI stop_signal(lib_win32_handle event)
{
    return fault==1 || fault==3 ? LIB_WIN32_FALSE : lib_win32_set_event(event);
}
static lib_win32_dword LIB_WIN32_WINAPI terminal_wait(lib_win32_handle worker,lib_win32_dword timeout)
{
    return fault==2 || fault==4 ? LIB_WIN32_WAIT_FAILED : lib_win32_wait_for_single_object(worker,timeout);
}
static lib_win32_bool LIB_WIN32_WINAPI close_handle(lib_win32_handle handle)
{
    lib_win32_bool result=lib_win32_close_handle(handle);
    ++closed;
    return fault==5 ? LIB_WIN32_FALSE : result;
}
static void idle_wait(lib_win32_handle stop)
{
    if(lib_win32_wait_for_single_object(stop,LIB_WIN32_INFINITE)!=LIB_WIN32_WAIT_OBJECT_0)
        lib_process_exit_immediate(91);
}
#undef lib_win32_create_thread
#define lib_win32_create_thread idle_create
#undef lib_win32_wait_for_multiple_objects
#define lib_win32_wait_for_multiple_objects startup_wait
#undef lib_win32_set_event
#define lib_win32_set_event stop_signal
#undef lib_win32_wait_for_single_object
#define lib_win32_wait_for_single_object terminal_wait
#undef lib_win32_close_handle
#define lib_win32_close_handle close_handle
#include "lib/kvm-window/win32/component.c"

static lib_win32_dword LIB_WIN32_WINAPI idle_worker(void *opaque)
{
    kvm_window *window=opaque;
    kvm_window_win32_state *state=window->worker_state;
    idle_wait(state->stop);
    return 0;
}
static lib_bool input(void *context,const kvm_input_event *event)
{ (void)context;(void)event;return LIB_TRUE; }
static void failure(void *context,lib_u64 source,lib_status status)
{ (void)context;(void)source;(void)status; }
static void unexpected_cleanup(void) { _Exit(99); }
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(int argc,char **argv)
{
    kvm_window *window=NULL;
    kvm_window_options options={.initial_title="",
        .component={.input_sink=input,.failure_sink=failure}};
    if(argc==2) {
        fault=(unsigned)atoi(argv[1]);
        CHECK(fault>=1 && fault<=4 && !atexit(unexpected_cleanup));
        (void)kvm_window_create(&window,&options);
        (void)kvm_window_destroy(window);
        _Exit(90);
    }
    CHECK(kvm_window_create(&window,&options)==LIB_STATUS_IO_ERROR);
    CHECK(!window && closed==3);
    fault=5;
    CHECK(kvm_window_create(&window,&options)==LIB_STATUS_OK);
    CHECK(kvm_window_destroy(window)==LIB_STATUS_IO_ERROR && closed==6);
    for(unsigned mode=1;mode<=4;++mode) {
        char executable[MAX_PATH],command[MAX_PATH+32];
        STARTUPINFOA startup={sizeof(startup)};PROCESS_INFORMATION process={0};DWORD code;
        CHECK(GetModuleFileNameA(NULL,executable,sizeof(executable))<sizeof(executable));
        CHECK(sprintf_s(command,sizeof(command),"\"%s\" %u",executable,mode)>0);
        CHECK(CreateProcessA(executable,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
        DWORD wait=WaitForSingleObject(process.hProcess,10000); /* Test budget only. */
        if(wait!=WAIT_OBJECT_0){TerminateProcess(process.hProcess,92);WaitForSingleObject(process.hProcess,INFINITE);}
        BOOL read=GetExitCodeProcess(process.hProcess,&code);
        CloseHandle(process.hThread);CloseHandle(process.hProcess);
        CHECK(wait==WAIT_OBJECT_0 && read && code==LIB_STATUS_INTERNAL_ERROR);
    }
    puts("PASS joined startup rollback, consumed cleanup-error return, four fatal child boundaries without atexit");
    return 0;
}

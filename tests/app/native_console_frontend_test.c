/* Production frontend/backend/view/host composition on a private desktop.
 * Only this executable's private helper is suspended or terminated for faults;
 * no production injection switch and no guest or broker substitute. */
#include "frontend-exe/native_console_frontend.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

static HANDLE target,helper,finished;
static HANDLE signal_received;
static volatile LONG signal_type;
static WCHAR prefix[96];
#define CHECK(x) do { if(!(x)) { \
    fprintf(stderr,"FAIL line=%u error=%lu: %s\n",(unsigned)__LINE__,GetLastError(),#x); \
    if(target) { HANDLE p=OpenProcess(PROCESS_TERMINATE,FALSE,GetProcessId(target)); \
        if(p) { TerminateProcess(p,1);CloseHandle(p); } } \
    if(helper)TerminateProcess(helper,1);ExitProcess(1); } } while(0)

static HANDLE named_event(const WCHAR *suffix,BOOL create)
{
    WCHAR name[128];HANDLE event;
    CHECK(swprintf_s(name,128,L"%ls-%ls",prefix,suffix)>0);
    event=create ? CreateEventW(NULL,TRUE,FALSE,name) :
        OpenEventW(SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,name);
    CHECK(event);return event;
}
static DWORD WINAPI watchdog(void *context)
{
    (void)context;
    CHECK(WaitForSingleObject(finished,45000)==WAIT_OBJECT_0);
    return 0;
}
static DWORD WINAPI stop_helper_thread(void *context)
{
    HANDLE main_thread=context;
    HANDLE stop=named_event(L"stall",FALSE),stalled=named_event(L"stalled",FALSE);
    HANDLE release=named_event(L"release",FALSE);
    CHECK(WaitForSingleObject(stop,30000)==WAIT_OBJECT_0);
    CHECK(SuspendThread(main_thread)!=(DWORD)-1);
    CHECK(SetEvent(stalled));
    CHECK(WaitForSingleObject(release,30000)==WAIT_OBJECT_0);
    CHECK(ResumeThread(main_thread)!=(DWORD)-1);
    CloseHandle(main_thread);CloseHandle(stop);CloseHandle(stalled);CloseHandle(release);
    return 0;
}
static int helper_main(void)
{
    WCHAR name[128];HANDLE mapping,thread,main_thread;
    DWORD *pid;
    if(GetEnvironmentVariableW(L"NTVDM_TEST_CONTROL",NULL,0))return (int)run16_native_console_host();
    CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix,96));
    CHECK(swprintf_s(name,128,L"%ls-pid",prefix)>0);
    mapping=OpenFileMappingW(FILE_MAP_WRITE,FALSE,name);CHECK(mapping);
    pid=MapViewOfFile(mapping,FILE_MAP_WRITE,0,0,sizeof(*pid));CHECK(pid);
    *pid=GetCurrentProcessId();UnmapViewOfFile(pid);CloseHandle(mapping);
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentThread(),GetCurrentProcess(),
        &main_thread,THREAD_SUSPEND_RESUME,FALSE,0));
    thread=CreateThread(NULL,0,stop_helper_thread,main_thread,0,NULL);CHECK(thread);
    CloseHandle(thread);
    return (int)run16_native_console_host();
}
static void exercise(DWORD expected,BOOL stalled,BOOL cancel)
{
    run16_native_frontend *frontend=NULL;
    run16_native_start start={0};
    WCHAR image[MAX_PATH],command[1024],directory[MAX_PATH],name[128],helper_image[MAX_PATH];
    HANDLE mapping,done,stall,ack,release;
    DWORD *pid,error,result=0xdeadbeef,actual,image_chars=MAX_PATH;
    ULONGLONG began,elapsed;
    CHECK(swprintf_s(prefix,96,L"Local\\NTVDMFrontend-%lu-%lu-%u-%u",
        GetCurrentProcessId(),expected,stalled,cancel)>0);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix));
    done=named_event(L"done",TRUE);stall=named_event(L"stall",TRUE);
    ack=named_event(L"stalled",TRUE);release=named_event(L"release",TRUE);
    CHECK(swprintf_s(name,128,L"%ls-pid",prefix)>0);
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sizeof(DWORD),name);
    CHECK(mapping);pid=MapViewOfFile(mapping,FILE_MAP_READ|FILE_MAP_WRITE,0,0,sizeof(*pid));CHECK(pid);
    *pid=0;
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH) && GetCurrentDirectoryW(MAX_PATH,directory));
    CHECK(swprintf_s(command,1024,L"\"%ls\" --target %lu",image,expected)>0);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=GetEnvironmentStringsW();start.console_mask=7;CHECK(start.environment);
    CHECK(!run16_native_frontend_create(&frontend));
    CHECK(!run16_native_frontend_launch(frontend,&start,&target));
    FreeEnvironmentStringsW((WCHAR *)start.environment);
    CHECK(*pid && *pid!=GetCurrentProcessId());
    helper=OpenProcess(PROCESS_TERMINATE|SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,*pid);CHECK(helper);
    CHECK(QueryFullProcessImageNameW(helper,0,helper_image,&image_chars) && !_wcsicmp(image,helper_image));
    CHECK(WaitForSingleObject(target,0)==WAIT_TIMEOUT);
    if(stalled) {
        CHECK(SetEvent(stall) && WaitForSingleObject(ack,5000)==WAIT_OBJECT_0);
        CHECK(SetEvent(done) && WaitForSingleObject(target,5000)==WAIT_OBJECT_0);
    } else if(cancel)run16_native_frontend_cancel(frontend);
    else CHECK(TerminateProcess(helper,99) && WaitForSingleObject(helper,5000)==WAIT_OBJECT_0);
    began=GetTickCount64();
    error=run16_native_frontend_wait(frontend,target,&result);
    elapsed=GetTickCount64()-began;
    printf("frontend wait expected=%lu stalled=%u cancel=%u error=%lu result=%lu elapsed=%llu ms\n",
        expected,stalled,cancel,error,result,elapsed);
    if(stalled)CHECK(!error && result==expected && elapsed<3000);
    else {
        CHECK(error && result==0xdeadbeef && WaitForSingleObject(target,0)==WAIT_TIMEOUT);
        CHECK(SetEvent(done) && WaitForSingleObject(target,5000)==WAIT_OBJECT_0);
    }
    CHECK(GetExitCodeProcess(target,&actual) && actual==expected);
    if(!(stalled && cancel))CHECK(SetEvent(release));
    began=GetTickCount64();
    run16_native_frontend_destroy(frontend);
    elapsed=GetTickCount64()-began;
    printf("frontend destroy stalled=%u retained-stall=%u elapsed=%llu ms\n",stalled,stalled && cancel,elapsed);
    CHECK(elapsed<10000);
    CHECK(WaitForSingleObject(helper,5000)==WAIT_OBJECT_0);
    CloseHandle(target);target=NULL;CloseHandle(helper);helper=NULL;
    UnmapViewOfFile(pid);CloseHandle(mapping);
    CloseHandle(done);CloseHandle(stall);CloseHandle(ack);CloseHandle(release);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",NULL));
}
static BOOL WINAPI signal_handler(DWORD event)
{
    if(event!=CTRL_C_EVENT && event!=CTRL_BREAK_EVENT)return FALSE;
    InterlockedExchange(&signal_type,(LONG)event);SetEvent(signal_received);return TRUE;
}
static int control_root(DWORD kind)
{
    run16_native_frontend *frontend=NULL;
    run16_native_start start={0};
    WCHAR image[MAX_PATH],command[1024],directory[MAX_PATH];
    HANDLE ready;
    DWORD result=0,event=(kind==1 || kind==3) ? CTRL_BREAK_EVENT : CTRL_C_EVENT;
    DWORD expected=kind==2 ? 0xc000013au : event==CTRL_C_EVENT ? 42 : 44;
    DWORD members[4];
    CHECK(swprintf_s(prefix,96,L"Local\\NativeRootControl-%lu",GetCurrentProcessId())>0);
    CHECK(SetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix) &&
        SetEnvironmentVariableW(L"NTVDM_TEST_CONTROL",L"1"));
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH) && GetCurrentDirectoryW(MAX_PATH,directory));
    CHECK(swprintf_s(command,1024,L"\"%ls\" --signal-target %lu",image,kind)>0);
    ready=named_event(L"ready",TRUE);
    start.application=image;start.command=command;start.directory=directory;
    start.environment=GetEnvironmentStringsW();start.console_mask=7;CHECK(start.environment);
    CHECK(!run16_native_frontend_create(&frontend));
    CHECK(!run16_native_frontend_launch(frontend,&start,&target));
    FreeEnvironmentStringsW((WCHAR *)start.environment);
    CHECK(WaitForSingleObject(ready,5000)==WAIT_OBJECT_0);
    /* Never broadcast into the observer/user Console. The parent creates this
     * root on its own hidden Console, and the helper/target use a second one. */
    CHECK(GetConsoleProcessList(members,4)==1 && members[0]==GetCurrentProcessId());
    if(kind==3)CHECK(!run16_native_frontend_dos_bind(frontend,&ready,TRUE));
    CHECK(GenerateConsoleCtrlEvent(event,0));
    CHECK(WaitForSingleObject(target,5000)==WAIT_OBJECT_0);
    CHECK(!run16_native_frontend_wait(frontend,target,&result));
    CHECK(result==expected);
    if(kind==3)run16_native_frontend_dos_forget(frontend,&ready);
    CloseHandle(target);target=NULL;CloseHandle(ready);
    run16_native_frontend_destroy(frontend);
    return 0;
}
static void controls(void)
{
    WCHAR image[MAX_PATH],command[1024];DWORD kind,code;
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
    for(kind=0;kind<4;++kind) {
        STARTUPINFOW startup={sizeof(startup)};
        PROCESS_INFORMATION process={0};
        startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
        CHECK(swprintf_s(command,1024,L"\"%ls\" --control-root %lu",image,kind)>0);
        CHECK(CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&startup,&process));
        CloseHandle(process.hThread);
        if(WaitForSingleObject(process.hProcess,15000)!=WAIT_OBJECT_0) {
            TerminateProcess(process.hProcess,ERROR_TIMEOUT);CHECK(FALSE);
        }
        CHECK(GetExitCodeProcess(process.hProcess,&code));CloseHandle(process.hProcess);
        printf("root control case=%lu exit=%lu\n",kind,code);CHECK(!code);
    }
    puts("PASS actual root Console Ctrl+C/Break forwards to hidden targets, default exit and paused-DOS I/O state");
}
int wmain(int argc,WCHAR **argv)
{
    HANDLE guard;
    if(argc==2 && !wcscmp(argv[1],L"--internal-native-console"))return helper_main();
    if(argc==3 && !wcscmp(argv[1],L"--control-root"))return control_root(wcstoul(argv[2],NULL,10));
    if(argc==3 && !wcscmp(argv[1],L"--signal-target")) {
        HANDLE ready;DWORD kind=wcstoul(argv[2],NULL,10);
        CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix,96));
        ready=named_event(L"ready",FALSE);
        signal_received=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(signal_received);
        if(kind!=2)CHECK(SetConsoleCtrlHandler(signal_handler,TRUE));
        CHECK(SetEvent(ready));CloseHandle(ready);
        if(WaitForSingleObject(signal_received,3000)!=WAIT_OBJECT_0)return 8;
        return signal_type==CTRL_C_EVENT ? 42 : 44;
    }
    if(argc==2 && !wcscmp(argv[1],L"--controls")) { controls();return 0; }
    if(argc==3 && !wcscmp(argv[1],L"--target")) {
        HANDLE done;DWORD wait;
        CHECK(GetEnvironmentVariableW(L"NTVDM_TEST_FRONTEND",prefix,96));
        done=named_event(L"done",FALSE);wait=WaitForSingleObject(done,30000);CloseHandle(done);
        return wait==WAIT_OBJECT_0 ? (int)wcstoul(argv[2],NULL,10) : (int)ERROR_TIMEOUT;
    }
    finished=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(finished);
    guard=CreateThread(NULL,0,watchdog,NULL,0,NULL);CHECK(guard);
    exercise(37,TRUE,FALSE);exercise(0,TRUE,FALSE);exercise(STILL_ACTIVE,TRUE,FALSE);
    puts("PASS production async frontend preserves completed 37/0/259 while actual helper is stalled");
    exercise(41,FALSE,FALSE);exercise(43,FALSE,TRUE);
    puts("PASS helper loss/cancellation is I/O failure, not live target completion or termination");
    exercise(47,TRUE,TRUE);
    puts("PASS production teardown cancels outstanding presentation and bounds permanently stalled helper cleanup");
    SetEvent(finished);CHECK(WaitForSingleObject(guard,5000)==WAIT_OBJECT_0);
    CloseHandle(guard);CloseHandle(finished);return 0;
}

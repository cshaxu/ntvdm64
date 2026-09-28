/* Private-desktop integration with the retained localized WINMINE media.
 * No keyboard/mouse injection and no switch of the user's desktop. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

typedef struct wow_thread { DWORD pid, tid; } wow_thread;
static BOOL CALLBACK find_wow_thread(HWND window,LPARAM context)
{
    wow_thread *found=(wow_thread *)context;
    WCHAR name[80]; DWORD pid,tid=GetWindowThreadProcessId(window,&pid);
    if (pid==found->pid && GetClassNameW(window,name,80) &&
        !_wcsicmp(name,L"WOWExecClass")) found->tid=tid;
    return TRUE;
}

/* Observe the real CheckVDM READY result, not merely a sleeping launcher. */
static BOOL await_reuse(DWORD pid)
{
    WCHAR path[MAX_PATH]; char data[65536],marker[80];
    ULONGLONG deadline=GetTickCount64()+10000;
    if (!GetEnvironmentVariableW(L"MVDM_S34_TRACE_PATH",path,MAX_PATH)) return FALSE;
    sprintf_s(marker,sizeof(marker),"%lu run16-vdm-state 4",pid);
    do {
        HANDLE file=CreateFileW(path,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE|
            FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
        DWORD bytes=0;
        if (file!=INVALID_HANDLE_VALUE) {
            BOOL read=ReadFile(file,data,sizeof(data)-1,&bytes,NULL);
            CloseHandle(file); data[bytes]=0;
            if (read && strstr(data,marker)) return TRUE;
        }
        Sleep(25);
    } while (GetTickCount64()<deadline);
    return FALSE;
}

typedef struct mine_windows {
    HWND windows[8];
    DWORD pids[8], count;
} mine_windows;

static BOOL CALLBACK diagnose_window(HWND window, LPARAM context)
{
    WCHAR name[128],title[256];
    DWORD pid=0;
    (void)context;
    name[0]=title[0]=0;
    GetClassNameW(window,name,128);
    GetWindowTextW(window,title,256);
    GetWindowThreadProcessId(window,&pid);
    fwprintf(stderr,L"WINDOW hwnd=%p pid=%lu visible=%u class=%ls title=%ls\n",
        window,pid,IsWindowVisible(window),name,title);
    return TRUE;
}

static BOOL CALLBACK collect(HWND window, LPARAM context)
{
    static const WCHAR mine_class[]={0xc9,0xa8,0xc0,0xd7,0};
    WCHAR name[80];
    mine_windows *found=(mine_windows *)context;
    if (found->count<8 && IsWindowVisible(window) &&
        GetClassNameW(window,name,80) && !wcscmp(name,mine_class)) {
        DWORD index=found->count++;
        found->windows[index]=window;
        GetWindowThreadProcessId(window,&found->pids[index]);
    }
    return TRUE;
}

static BOOL find_windows(HDESK desktop,DWORD expected,mine_windows *found)
{
    ULONGLONG deadline=GetTickCount64()+20000;
    do {
        ZeroMemory(found,sizeof(*found));
        SetLastError(ERROR_SUCCESS);
        if (!EnumDesktopWindows(desktop,collect,(LPARAM)found) && GetLastError())
            return FALSE;
        if (found->count==expected) return TRUE;
        Sleep(50);
    } while (GetTickCount64()<deadline);
    return FALSE;
}

static BOOL launch(const WCHAR *root,const WCHAR *image,const WCHAR *tail,
    WCHAR *desktop,PROCESS_INFORMATION *process)
{
    WCHAR path[MAX_PATH],command[1024];
    STARTUPINFOW startup={sizeof(startup)};
    startup.lpDesktop=desktop;
    if (swprintf_s(path,MAX_PATH,L"%ls\\%ls",root,image)<0 ||
        swprintf_s(command,1024,L"\"%ls\" %ls",path,tail)<0) return FALSE;
    return CreateProcessW(path,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,
        NULL,root,&startup,process);
}

static void dispose(PROCESS_INFORMATION *process)
{
    if (process->hProcess) {
        if (WaitForSingleObject(process->hProcess,0)==WAIT_TIMEOUT) {
            TerminateProcess(process->hProcess,97);
            WaitForSingleObject(process->hProcess,5000);
        }
        CloseHandle(process->hProcess);
    }
    if (process->hThread) CloseHandle(process->hThread);
}

/* Shell contexts are test callers, not product shell-context heuristics. */
static WCHAR caller_batch[MAX_PATH];
static WCHAR caller_report[MAX_PATH];
static BOOL report_contains(const WCHAR *suffix,const char *marker)
{
    WCHAR path[MAX_PATH]; char data[65536]; FILE *file=NULL; size_t size;
    if(swprintf_s(path,MAX_PATH,L"%ls%ls",caller_report,suffix)<0 ||
        _wfopen_s(&file,path,L"rb") || !file) return FALSE;
    size=fread(data,1,sizeof(data)-1,file); data[size]=0; fclose(file);
    return strstr(data,marker)!=NULL;
}
static BOOL launch_first(const WCHAR *root,BOOL wait_first,const WCHAR *caller,
    WCHAR *desktop,PROCESS_INFORMATION *process)
{
    WCHAR shell[MAX_PATH],command[2048],line[1024];
    STARTUPINFOW startup={sizeof(startup)};
    HANDLE input=NULL,writer=NULL,output=INVALID_HANDLE_VALUE;
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
    BOOL result=FALSE,pipe_mode=!wcscmp(caller,L"--cmd-input");
    DWORD count=GetEnvironmentVariableW(L"COMSPEC",shell,MAX_PATH),written;
    char bytes[2048]; BOOL loss=FALSE; int length;
    if(!wcscmp(caller,L"--dos-input")) {
        WCHAR observer[MAX_PATH]; size_t root_length=wcslen(root);
        DWORD n=GetEnvironmentVariableW(L"WOW_TEST_OBSERVER",observer,MAX_PATH);
        DWORD r=GetEnvironmentVariableW(L"WOW_TEST_REPORT",caller_report,MAX_PATH);
        if(!n || n>=MAX_PATH || !r || r>=MAX_PATH ||
            GetFileAttributesW(caller_report)!=INVALID_FILE_ATTRIBUTES) return FALSE;
        startup.lpDesktop=desktop;
        /* This observer is already placed on our private desktop. Do not let
         * it create a second desktop that hides the WINMINE assertion. */
        if(!SetEnvironmentVariableW(L"MVDM_OBSERVER_PRIVATE_DESKTOP",NULL)) return FALSE;
        if(swprintf_s(command,2048,L"\"%ls\" \"%ls%lsrun16.exe\" %ls \"%ls\" COMMAND.COM "
            L"--observe-console-input-text \"run16 %ls WINMINE.EXE\rmem\rexit\r\" "
            L"--observe-console-line-delay-ms 1500 --observation-timeout-ms 60000",
            observer,root,root_length && root[root_length-1]==L'\\' ? L"" : L"\\",
            root,caller_report,wait_first ? L"--wait" : L"")<0) return FALSE;
        return CreateProcessW(observer,command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,
            NULL,root,&startup,process);
    }
    if(!wcscmp(caller,L"--dos-c")) {
        size_t root_length=wcslen(root);
        /* DOS paths must not acquire the doubled separator tolerated by
         * native CreateProcess when the short package root ends in '\\'. */
        if(swprintf_s(command,2048,L"COMMAND.COM /c %ls%lsrun16.exe %ls WINMINE.EXE",
            root,root_length && root[root_length-1]==L'\\' ? L"" : L"\\",
            wait_first ? L"--wait" : L"")<0) return FALSE;
        return launch(root,L"run16.exe",command,desktop,process);
    }
    if(!count || count>=MAX_PATH) return FALSE;
    startup.lpDesktop=desktop;
    if(swprintf_s(line,1024,L"\"%ls\\run16.exe\" %ls WINMINE.EXE",
        root,wait_first ? L"--wait" : L"")<0) return FALSE;
    if(!wcscmp(caller,L"--cmd-c")) {
        if(swprintf_s(command,2048,L"\"%ls\" /d /s /c \"%ls\"",shell,line)<0) return FALSE;
    } else {
        WCHAR script[1200]; HANDLE file=INVALID_HANDLE_VALUE;
        if(swprintf_s(script,1200,L"@echo off\r\n%ls\r\nexit /b %%errorlevel%%\r\n",line)<0) return FALSE;
        length=WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,script,-1,bytes,sizeof(bytes),NULL,&loss);
        if(!length || loss) return FALSE;
        if(pipe_mode) {
            if(!CreatePipe(&input,&writer,&security,0) ||
                !SetHandleInformation(writer,HANDLE_FLAG_INHERIT,0)) goto done;
            output=CreateFileW(L"NUL",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
                &security,OPEN_EXISTING,0,NULL);
            if(output==INVALID_HANDLE_VALUE) goto done;
            startup.dwFlags=STARTF_USESTDHANDLES;
            startup.hStdInput=input; startup.hStdOutput=startup.hStdError=output;
            if(!WriteFile(writer,bytes,length-1,&written,NULL) || written!=(DWORD)length-1) goto done;
            if(swprintf_s(command,2048,L"\"%ls\" /d /q",shell)<0) goto done;
        } else {
            if(swprintf_s(caller_batch,MAX_PATH,L"%ls\\tests\\W8-%lu.cmd",root,GetCurrentProcessId())<0) goto done;
            file=CreateFileW(caller_batch,GENERIC_WRITE,0,NULL,CREATE_NEW,0,NULL);
            if(file==INVALID_HANDLE_VALUE){caller_batch[0]=0;goto done;}
            result=WriteFile(file,bytes,length-1,&written,NULL) && written==(DWORD)length-1;
            CloseHandle(file); if(!result) goto done;
            result=FALSE;
            if(swprintf_s(command,2048,L"\"%ls\" /d /s /c \"\"%ls\"\"",shell,caller_batch)<0) goto done;
        }
    }
    result=CreateProcessW(shell,command,NULL,NULL,pipe_mode,CREATE_NO_WINDOW,
        NULL,root,&startup,process);
done:
    if(input) CloseHandle(input);
    if(writer) CloseHandle(writer);
    if(output!=INVALID_HANDLE_VALUE) CloseHandle(output);
    return result;
}

#define REQUIRE(value) do { if (!(value)) { \
    fprintf(stderr,"FAIL line=%u error=%lu\n",__LINE__,GetLastError()); goto done; } } while (0)

int wmain(int argc,WCHAR **argv)
{
    WCHAR desktop_name[80],expected[MAX_PATH],actual[MAX_PATH];
    HDESK desktop=NULL;
    PROCESS_INFORMATION broker={0},first={0},second={0};
    HANDLE worker=NULL,suspended_thread=NULL;
    mine_windows found;
    HWND first_window=NULL;
    DWORD worker_pid=0,code=0,length=MAX_PATH;
    BOOL wait_first,worker_fault,broker_fault;
    int result=1;
    if (argc<3 || argc>5) return 87;
    wait_first=argc>=4 && !wcscmp(argv[3],L"--wait-first");
    worker_fault=argc>=4 && !wcscmp(argv[3],L"--worker-fault");
    broker_fault=argc>=4 && !wcscmp(argv[3],L"--broker-fault");
    if (argc>=4 && !wait_first && !worker_fault && !broker_fault && wcscmp(argv[3],L"--async")) return 87;
    if (argc==5 && (worker_fault || broker_fault ||
        (wcscmp(argv[4],L"--cmd-c") && wcscmp(argv[4],L"--cmd-input") &&
         wcscmp(argv[4],L"--batch") && wcscmp(argv[4],L"--dos-c") &&
         wcscmp(argv[4],L"--dos-input")))) return 87;
    REQUIRE(SetEnvironmentVariableW(L"SystemRoot",argv[1]));
    swprintf_s(desktop_name,80,L"NTVDMWowLaunch-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(desktop_name,NULL,NULL,0,DESKTOP_CREATEWINDOW |
        DESKTOP_ENUMERATE | DESKTOP_READOBJECTS | DESKTOP_WRITEOBJECTS,NULL);
    REQUIRE(desktop);
    REQUIRE(launch(argv[1],L"ntsrv.exe",L"",desktop_name,&broker));
    if(argc==5) {
        REQUIRE(launch_first(argv[1],wait_first,argv[4],desktop_name,&first));
        wprintf(L"WOW-CALLER %ls wait=%u\n",argv[4],wait_first);
    } else REQUIRE(launch(argv[1],L"run16.exe",wait_first ? L"--wait WINMINE.EXE" : L"WINMINE.EXE",desktop_name,&first));
    if (!wait_first) {
        REQUIRE(WaitForSingleObject(first.hProcess,caller_report[0] ? 60000 : 20000)==WAIT_OBJECT_0);
        REQUIRE(GetExitCodeProcess(first.hProcess,&code) && code==0);
        if(caller_report[0]) {
            REQUIRE(report_contains(L"", "scripted-console-input=delivered"));
            REQUIRE(report_contains(L".console.txt", "bytes total conventional memory"));
            puts("PASS original COMMAND prompt executed MEM while WINMINE remains open");
        }
    }
    if(!find_windows(desktop,1,&found)) {
        EnumDesktopWindows(desktop,diagnose_window,0);
        fprintf(stderr,"FAIL first WINMINE window missing; caller exit=%lu\n",code);
        goto done;
    }
    first_window=found.windows[0]; worker_pid=found.pids[0];
    worker=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE |
        PROCESS_TERMINATE,FALSE,worker_pid);
    REQUIRE(worker && QueryFullProcessImageNameW(worker,0,actual,&length));
    REQUIRE(GetFullPathNameW(argv[2],MAX_PATH,expected,NULL));
    REQUIRE(wcscat_s(expected,MAX_PATH,L"\\ntvdm.exe")==0);
    /* Second argument is the physical package corresponding to the alias. */
    REQUIRE(!_wcsicmp(actual,expected));
    REQUIRE(WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
    if (wait_first) {
        REQUIRE(WaitForSingleObject(first.hProcess,caller_report[0] ? 6000 : 200)==WAIT_TIMEOUT);
        if(caller_report[0]) REQUIRE(!report_contains(L".line-02.console.txt",
            "bytes total conventional memory"));
        printf("PASS explicit-wait launcher remains pending; WINMINE worker=%lu\n",worker_pid);
    } else printf("PASS async launcher exited; WINMINE worker=%lu remains alive\n",worker_pid);
    if (worker_fault || broker_fault) {
        wow_thread thread={worker_pid,0};
        REQUIRE(EnumDesktopWindows(desktop,find_wow_thread,(LPARAM)&thread));
        REQUIRE(thread.tid);
        suspended_thread=OpenThread(THREAD_SUSPEND_RESUME|THREAD_QUERY_LIMITED_INFORMATION,
            FALSE,thread.tid);
        REQUIRE(suspended_thread && GetProcessIdOfThread(suspended_thread)==worker_pid);
        /* Freeze only the guest dispatcher; the broker-death watcher stays live. */
        REQUIRE(SuspendThread(suspended_thread)!=(DWORD)-1);
        REQUIRE(launch(argv[1],L"run16.exe",L"WINMINE.EXE",desktop_name,&second));
        REQUIRE(await_reuse(second.dwProcessId));
        REQUIRE(WaitForSingleObject(second.hProcess,0)==WAIT_TIMEOUT);
        REQUIRE(TerminateProcess(worker_fault ? worker : broker.hProcess,97));
        REQUIRE(WaitForSingleObject(second.hProcess,15000)==WAIT_OBJECT_0);
        REQUIRE(GetExitCodeProcess(second.hProcess,&code));
        printf("FAULT launcher result=%lu expected=%lu\n",code,
            worker_fault ? (DWORD)ERROR_PROCESS_ABORTED : (DWORD)RPC_S_SERVER_UNAVAILABLE);
        REQUIRE(code==(worker_fault ? ERROR_PROCESS_ABORTED : RPC_S_SERVER_UNAVAILABLE));
        REQUIRE(WaitForSingleObject(worker,10000)==WAIT_OBJECT_0);
        if (broker_fault) {
            REQUIRE(GetExitCodeProcess(worker,&code) && code==RPC_S_SERVER_UNAVAILABLE);
        }
        puts("WOW-SHARED-STARTUP-FAULT-PASS"); result=0; goto done;
    }
    REQUIRE(launch(argv[1],L"run16.exe",L"--wait WINMINE.EXE",desktop_name,&second));
    /* Original windows/ep/winmine/winmine.c MMain WIN16 branch activates
     * hPrevInstance's existing window and returns FALSE (0). A second
     * window is not the source-defined success criterion. */
    if (WaitForSingleObject(second.hProcess,20000)!=WAIT_OBJECT_0) {
        GetExitCodeProcess(second.hProcess,&code);
        fprintf(stderr,"SECOND launcher=%lu exit=%lu\n",second.dwProcessId,code);
        EnumDesktopWindows(desktop,diagnose_window,0);
        goto done;
    }
    REQUIRE(GetExitCodeProcess(second.hProcess,&code) && code==0);
    REQUIRE(find_windows(desktop,1,&found) && found.windows[0]==first_window &&
        found.pids[0]==worker_pid);
    REQUIRE(WaitForSingleObject(worker,0)==WAIT_TIMEOUT);
    if (wait_first) REQUIRE(WaitForSingleObject(first.hProcess,0)==WAIT_TIMEOUT);
    puts("PASS original single-instance WINMINE returns 0; first task remains alive");
    dispose(&second);
    ZeroMemory(&second,sizeof(second));
    REQUIRE(launch(argv[1],L"run16.exe",L"system32\\wowexec.exe",desktop_name,&second));
    REQUIRE(WaitForSingleObject(second.hProcess,20000)==WAIT_OBJECT_0);
    REQUIRE(GetExitCodeProcess(second.hProcess,&code) && code==0);
    REQUIRE(find_windows(desktop,1,&found) && found.windows[0]==first_window &&
        found.pids[0]==worker_pid);
    if (wait_first) REQUIRE(WaitForSingleObject(first.hProcess,0)==WAIT_TIMEOUT);
    puts("PASS original redundant WOWEXEC completes without closing WINMINE");
    REQUIRE(PostMessageW(first_window,WM_CLOSE,0,0));
    REQUIRE(find_windows(desktop,0,&found));
    if (wait_first) {
        REQUIRE(WaitForSingleObject(first.hProcess,caller_report[0] ? 60000 : 10000)==WAIT_OBJECT_0);
        REQUIRE(GetExitCodeProcess(first.hProcess,&code) && code==0);
        if(caller_report[0]) {
            REQUIRE(report_contains(L"", "scripted-console-input=delivered"));
            REQUIRE(report_contains(L".console.txt", "bytes total conventional memory"));
            puts("PASS original COMMAND prompt executed MEM after explicit GUI wait");
        }
        puts("PASS first task completion returns 0 only after its window closes");
    }
    puts("WOW-SHARED-LAUNCH-PASS");
    result=0;
done:
    dispose(&second); dispose(&first);
    if(caller_batch[0] && !DeleteFileW(caller_batch)) result=1;
    if (worker) {
        if (WaitForSingleObject(worker,0)==WAIT_TIMEOUT) {
            TerminateProcess(worker,97); WaitForSingleObject(worker,5000);
        }
        CloseHandle(worker);
    }
    if (suspended_thread) CloseHandle(suspended_thread);
    dispose(&broker);
    if (desktop) CloseDesktop(desktop);
    return result;
}

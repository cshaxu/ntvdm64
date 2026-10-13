/* Actual immutable guests on an unswitched desktop. SOL/WRITE's retained
 * baseline reaches an error dialog; that is a title witness, not gameplay. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
static HWND mine,dialog,native;
static DWORD worker;
static DWORD shell_worker;
static HANDLE retained_launcher;
static DWORD retained_pid;
static BOOL CALLBACK collect(HWND window,LPARAM unused)
{
    WCHAR cls[80];DWORD pid;
    (void)unused;
    GetWindowThreadProcessId(window,&pid);
    if(!IsWindowVisible(window) || !GetClassNameW(window,cls,80))return TRUE;
    if(!wcscmp(cls,L"Minesweeper")){mine=window;worker=pid;}
    else if(!wcscmp(cls,L"WOWExecClass")){shell_worker=pid;}
    else if(!wcscmp(cls,L"NativeStyleReturn"))native=window;
    else if(!wcscmp(cls,L"#32770") && (!worker || pid==worker))dialog=window;
    return TRUE;
}
static BOOL await_window(HDESK desktop,int kind)
{
    ULONGLONG deadline=GetTickCount64()+20000;
    do {
        dialog=NULL;EnumDesktopWindows(desktop,collect,0);
        if(kind==2?native!=NULL:kind==1?dialog!=NULL:mine!=NULL)return TRUE;
        Sleep(50);
    }while(GetTickCount64()<deadline);
    return FALSE;
}
static BOOL marked_owner(HWND window,BOOL expected,DWORD owner)
{
    WCHAR title[256];DWORD pid;
    if(!window || !GetWindowTextW(window,title,256))title[0]=0;
    GetWindowThreadProcessId(window,&pid);
    wprintf(L"WITNESS pid=%lu hwnd=%p title=[%ls] expected-marker=%u\n",pid,window,title,expected);fflush(stdout);
    return window && pid==owner && (!wcsncmp(title,L"(Win16 Style) ",14))==expected;
}
static BOOL marked(HWND window,BOOL expected){return marked_owner(window,expected,worker);}
static BOOL launch(PCWSTR root,PCWSTR options,PCWSTR guest,PWSTR desktop,DWORD wait_ms)
{
    WCHAR app[MAX_PATH],command[1024],image[MAX_PATH];DWORD code=STILL_ACTIVE,wait,error=0,type;BOOL result,classified;
    STARTUPINFOW start={sizeof(start)};PROCESS_INFORMATION process={0};
    swprintf_s(app,MAX_PATH,L"%ls\\system32\\run16.exe",root);
    swprintf_s(command,1024,L"\"%ls\" %ls \"%ls\\%ls\"",app,options,root,guest);
    start.lpDesktop=desktop;
    swprintf_s(image,MAX_PATH,L"%ls\\%ls",root,guest);
    type=0xffffffff;classified=GetBinaryTypeW(image,&type);error=classified?0:GetLastError();
    printf("HOST CLASSIFICATION success=%u type=%lu error=%lu\n",classified,type,error);error=0;
    wprintf(L"LAUNCH %ls\n",command);fflush(stdout);
    if(!CreateProcessW(app,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,root,&start,&process)){
        fprintf(stderr,"CreateProcess failed error=%lu\n",GetLastError());return FALSE;
    }
    if(!wait_ms){
        retained_launcher=process.hProcess;retained_pid=process.dwProcessId;
        CloseHandle(process.hThread);
        printf("LAUNCH WAIT RETAINED pid=%lu\n",retained_pid);fflush(stdout);return TRUE;
    }
    wait=WaitForSingleObject(process.hProcess,wait_ms);
    if(wait==WAIT_FAILED)error=GetLastError();
    result=wait==WAIT_OBJECT_0 && GetExitCodeProcess(process.hProcess,&code) && !code;
    printf("LAUNCH RESULT pid=%lu wait=%lu exit=%lu wait-error=%lu\n",process.dwProcessId,wait,code,error);fflush(stdout);
    if(!result)TerminateProcess(process.hProcess,98);
    CloseHandle(process.hThread);CloseHandle(process.hProcess);return result;
}
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL line=%d error=%lu %s\n",__LINE__,GetLastError(),#x);result=1;goto done;}}while(0)
int wmain(int argc,WCHAR **argv)
{
    WCHAR name[80];HDESK desktop=NULL;int result=0;BOOL reverse;
    CHECK(argc==2 || argc==3);
    reverse=argc==3 && !wcscmp(argv[2],L"off-on");
    swprintf_s(name,80,L"WowStyle-%lu",GetCurrentProcessId());
    desktop=CreateDesktopW(name,NULL,NULL,0,GENERIC_ALL,NULL);CHECK(desktop);
    if(argc==3 && (!wcsncmp(argv[2],L"child-",6) || !wcsncmp(argv[2],L"relay-",6) ||
        !wcsncmp(argv[2],L"separate-",9) || !wcsncmp(argv[2],L"separate-a-",11))){
        BOOL separate=!wcsncmp(argv[2],L"separate-",9);
        BOOL ansi=separate && !wcsncmp(argv[2],L"separate-a-",11);
        PCWSTR policy=argv[2]+(ansi?11:separate?9:6);
        DWORD original_worker=0;
        BOOL on=wcscmp(policy,L"off")!=0,relay=separate || !wcsncmp(argv[2],L"relay-",6);
        PCWSTR options=!wcscmp(policy,L"off")?L"--wait":
            !wcscmp(policy,L"on")?L"--wait --win16-style":
            !wcscmp(policy,L"all")?L"--wait --win16-style-all":NULL;
        CHECK(options);
        CHECK(launch(argv[1],options,L"tests\\STYLEPAR.EXE",name,relay?0:20000));
        if(relay){
            DWORD_PTR reply=0;DWORD pid,idle;HANDLE process;
            CHECK(await_window(desktop,2));
            CHECK(GetWindowThreadProcessId(native,&pid));
            process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_INFORMATION,FALSE,pid);CHECK(process);
            idle=WaitForInputIdle(process,10000);CloseHandle(process);
            printf("NATIVE INPUT IDLE pid=%lu result=%lu\n",pid,idle);fflush(stdout);CHECK(idle==0);
            original_worker=shell_worker;
            CHECK(!separate || original_worker);
            CHECK(SendMessageTimeoutW(native,WM_APP+0x31,separate?(ansi?2:1):0,0,SMTO_BLOCK|SMTO_ABORTIFHUNG,60000,&reply) && reply==1);
        }
        {
            BOOL appeared=await_window(desktop,FALSE);
            if(separate && !appeared){DWORD_PTR code=MAXDWORD;
                SendMessageTimeoutW(native,WM_APP+0x33,0,0,SMTO_BLOCK|SMTO_ABORTIFHUNG,1000,&code);
                printf("SEPARATE LAUNCHER status=%lu\n",(DWORD)code);fflush(stdout);
            }
            CHECK(appeared && marked(mine,on));
        }
        if(separate){
            DWORD_PTR reply=0;
            printf("SEPARATE WOW parent-worker=%lu child-worker=%lu ansi=%u\n",original_worker,worker,ansi);fflush(stdout);
            CHECK(worker!=original_worker);
            CHECK(PostMessageW(mine,WM_CLOSE,0,0));
            CHECK(SendMessageTimeoutW(native,WM_APP+0x32,0,0,SMTO_BLOCK|SMTO_ABORTIFHUNG,15000,&reply) && reply==1);
        }
        if(relay){
            DWORD pid,code;HANDLE process;
            CHECK(native && GetWindowThreadProcessId(native,&pid) && pid!=worker);
            CHECK(marked_owner(native,!wcscmp(policy,L"all"),pid));
            process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);CHECK(process);
            CHECK(PostMessageW(native,WM_CLOSE,0,0));
            if(WaitForSingleObject(process,10000)!=WAIT_OBJECT_0 || !GetExitCodeProcess(process,&code) || code){CloseHandle(process);CHECK(FALSE);}
            CloseHandle(process);if(!separate)CHECK(marked(mine,on));
            code=STILL_ACTIVE;
            {
                DWORD wait=WaitForSingleObject(retained_launcher,30000);
                if(wait==WAIT_OBJECT_0)CHECK(GetExitCodeProcess(retained_launcher,&code));
                printf("PARENT COMPLETION pid=%lu wait=%lu exit=%lu\n",retained_pid,wait,code);fflush(stdout);
                CHECK(wait==WAIT_OBJECT_0 && !code);
            }
        }
        puts(relay?"PASS real Win16/native/Win16 relay, both parent completions and native close":
            "PASS real Win16 WinExec child inherits policy after parent completion");
        goto done;
    }
    CHECK(launch(argv[1],reverse?L"":L"--win16-style",L"WINMINE.EXE",name,20000));
    CHECK(await_window(desktop,FALSE) && marked(mine,!reverse));
    CHECK(launch(argv[1],reverse?L"--win16-style":L"",L"SOL.EXE",name,20000));
    CHECK(await_window(desktop,TRUE) && marked(dialog,reverse) && marked(mine,!reverse));
    puts("PASS real shared WOW: launcher retirement, mixed task policies and modal title");
done:
    if(retained_launcher){
        if(WaitForSingleObject(retained_launcher,0)!=WAIT_OBJECT_0)TerminateProcess(retained_launcher,98);
        CloseHandle(retained_launcher);
    }
    if(desktop)CloseDesktop(desktop);
    return result;
}

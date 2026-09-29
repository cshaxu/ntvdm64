/* Run through the private-desktop observer from an isolated package.
 * No pointer/focus changes. Launcher completion/death must preserve accepted
 * guest work; independent frontend death closes only its associated worker. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>
static const WCHAR *markers[]={L"logs\\NIOREADY",L"logs\\NIOGO",L"logs\\NIODONE",L"logs\\NIOPID",L"logs\\NIOROOT",L"logs\\NIOMEM"};
static BOOL CALLBACK fault_window(HWND window,LPARAM context)
{
    DWORD pid;WCHAR title[512]={0},kind[96]={0};DWORD_PTR copied=0;
    GetWindowThreadProcessId(window,&pid);
    if(pid!=(DWORD)context)return TRUE;
    GetClassNameW(window,kind,96);
    SendMessageTimeoutW(window,WM_GETTEXT,512,(LPARAM)title,
        SMTO_ABORTIFHUNG|SMTO_BLOCK,200,&copied);
    fwprintf(stderr,L"fault-window class=%ls visible=%d text=%ls\n",kind,IsWindowVisible(window),title);
    if(!wcscmp(kind,L"#32770"))EnumChildWindows(window,fault_window,context);
    return TRUE;
}
static HANDLE find_child(DWORD parent,PCWSTR name)
{
    PROCESSENTRY32W entry={sizeof(entry)};
    HANDLE snapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0),worker=NULL;
    if(snapshot==INVALID_HANDLE_VALUE) return NULL;
    if(Process32FirstW(snapshot,&entry)) do {
        if(entry.th32ParentProcessID==parent && !_wcsicmp(entry.szExeFile,name)) {
            worker=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_TERMINATE,FALSE,entry.th32ProcessID);
            break;
        }
    } while(Process32NextW(snapshot,&entry));
    CloseHandle(snapshot);return worker;
}
typedef struct test_window { DWORD pid;HWND window; } test_window;
static BOOL CALLBACK find_test_window(HWND window,LPARAM context)
{
    test_window *state=(test_window *)context;DWORD pid;WCHAR kind[64];
    GetWindowThreadProcessId(window,&pid);
    if(pid==state->pid && IsWindowVisible(window) &&
        GetClassNameW(window,kind,64) && !wcscmp(kind,L"LibKvmWindow")){
        state->window=window;return FALSE;
    }
    return TRUE;
}
static BOOL select_test_window(HANDLE frontend)
{
    char desktop[96];DWORD needed,written;INPUT_RECORD keys[2]={0};
    test_window state={0};ULONGLONG deadline;
    if(!GetUserObjectInformationA(GetThreadDesktop(GetCurrentThreadId()),UOI_NAME,
        desktop,sizeof(desktop),&needed) || strncmp(desktop,"NTVDMConsoleTest-",17))return FALSE;
    keys[0].EventType=KEY_EVENT;keys[0].Event.KeyEvent.bKeyDown=TRUE;
    keys[0].Event.KeyEvent.wRepeatCount=1;keys[0].Event.KeyEvent.wVirtualKeyCode='F';
    keys[0].Event.KeyEvent.wVirtualScanCode=0x21;
    keys[0].Event.KeyEvent.dwControlKeyState=LEFT_CTRL_PRESSED|LEFT_ALT_PRESSED;
    keys[1]=keys[0];keys[1].Event.KeyEvent.bKeyDown=FALSE;
    if(!WriteConsoleInputW(GetStdHandle(STD_INPUT_HANDLE),keys,2,&written) || written!=2)return FALSE;
    state.pid=GetProcessId(frontend);deadline=GetTickCount64()+8000;
    do {
        EnumWindows(find_test_window,(LPARAM)&state);
        if(state.window)return TRUE;
        Sleep(20);
    } while(GetTickCount64()<deadline && WaitForSingleObject(frontend,0)==WAIT_TIMEOUT);
    return FALSE;
}
int main(int argc,char **argv)
{
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION child={0},bystander={0};
    WCHAR command[1024]=L"run16.exe tests\\NOIO.COM",image[MAX_PATH];
    HANDLE file,nested=NULL,worker=NULL,frontend=NULL;
    HANDLE peer_ready=NULL,peer_go=NULL,peer_frontend=NULL;
    WCHAR ready_name[96],go_name[96],peer_command[1536];
    DWORD i,code=1,bytes,pid=0;
    BOOL isolated=argc==3 && !strcmp(argv[2],"--isolation");
    BOOL normal=argc>=2 && !strcmp(argv[1],"--normal");
    BOOL spawn=argc==2 && !strcmp(argv[1],"--spawn");
    BOOL launcher_loss=argc>=2 && !strcmp(argv[1],"--launcher-loss");
    BOOL worker_loss=argc>=2 && !strcmp(argv[1],"--worker-loss");
    BOOL frontend_loss=argc>=2 && !strcmp(argv[1],"--frontend-loss");
    BOOL window_verified=FALSE;
    DWORD report_mode=0;BOOL have_report_mode=GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),&report_mode);
    const char *verdict=NULL;
    const char *phase="guest-ready";
    ULONGLONG deadline;
    if(argc==4 && !strcmp(argv[1],"--bystander")) {
        HANDLE ready=OpenEventA(EVENT_MODIFY_STATE,FALSE,argv[2]);
        HANDLE go=OpenEventA(SYNCHRONIZE,FALSE,argv[3]);
        DWORD result;
        if(!ready || !go || !SetEvent(ready))return 65;
        result=WaitForSingleObject(go,45000)==WAIT_OBJECT_0 ? 53 : 66;
        if(result==53)puts("UNRELATED-NATIVE-SESSION-SURVIVED");
        CloseHandle(ready);CloseHandle(go);return (int)result;
    }
    if(argc!=1 && !normal && !spawn && !launcher_loss && !worker_loss && !frontend_loss) {
        for(i=0;i<(DWORD)argc;++i) fprintf(stderr,"argument[%lu]=%s\n",i,argv[i]);
        return 64;
    }
    for(i=0;i<sizeof(markers)/sizeof(markers[0]);++i) if(GetFileAttributesW(markers[i])!=INVALID_FILE_ATTRIBUTES) {
        fprintf(stderr,"Existing marker; refusing test\n");return 2;
    }
    if(isolated) {
        STARTUPINFOW peer_startup={sizeof(peer_startup)};
        phase="independent-session-start";
        if(!GetModuleFileNameW(NULL,image,MAX_PATH))goto cleanup;
        swprintf_s(ready_name,96,L"Local\\NTVDM-S5-%lu-ready",GetCurrentProcessId());
        swprintf_s(go_name,96,L"Local\\NTVDM-S5-%lu-go",GetCurrentProcessId());
        peer_ready=CreateEventW(NULL,TRUE,FALSE,ready_name);
        peer_go=CreateEventW(NULL,TRUE,FALSE,go_name);
        if(!peer_ready || !peer_go)goto cleanup;
        swprintf_s(peer_command,1536,L"run16.exe \"%ls\" --bystander %ls %ls",image,ready_name,go_name);
        peer_startup.dwFlags=STARTF_USESHOWWINDOW;peer_startup.wShowWindow=SW_HIDE;
        if(!CreateProcessW(L"run16.exe",peer_command,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,
            NULL,NULL,&peer_startup,&bystander) ||
            WaitForSingleObject(peer_ready,10000)!=WAIT_OBJECT_0)goto cleanup;
        peer_frontend=find_child(bystander.dwProcessId,L"ntkvm.exe");
        if(!peer_frontend)goto cleanup;
    }
    if(normal) {
        if(!GetModuleFileNameW(NULL,image,MAX_PATH) ||
           swprintf_s(command,1024,L"run16.exe \"%s\" --spawn",image)<0) return 4;
    }
    /* The nested launcher must inherit the authenticated frontend capability,
     * just as CMD does. The external controller has no such capability. */
    if(!CreateProcessW(L"run16.exe",command,NULL,NULL,spawn,0,NULL,NULL,&startup,&child))
        return 3;
    deadline=GetTickCount64()+15000;
    while(GetFileAttributesW(markers[0])==INVALID_FILE_ATTRIBUTES &&
          WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT && GetTickCount64()<deadline)
        Sleep(20);
    if(GetFileAttributesW(markers[0])==INVALID_FILE_ATTRIBUTES) goto cleanup;
    phase="root-completion";
    if(spawn) {
        DWORD members[64],count=GetConsoleProcessList(members,64);
        if(!count || count>64)goto cleanup;
        file=CreateFileW(markers[5],GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,0,NULL);
        if(file==INVALID_HANDLE_VALUE)goto cleanup;
        if(!WriteFile(file,members,count*sizeof(DWORD),&bytes,NULL) || bytes!=count*sizeof(DWORD)){
            CloseHandle(file);goto cleanup;
        }
        CloseHandle(file);
        file=CreateFileW(markers[3],GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,0,NULL);
        if(file==INVALID_HANDLE_VALUE) goto cleanup;
        pid=child.dwProcessId;
        if(WriteFile(file,&pid,sizeof(pid),&bytes,NULL) && bytes==sizeof(pid)) code=37;
        CloseHandle(file);
        deadline=GetTickCount64()+15000;
        while(GetFileAttributesW(markers[4])==INVALID_FILE_ATTRIBUTES && GetTickCount64()<deadline) Sleep(20);
        if(GetFileAttributesW(markers[4])==INVALID_FILE_ATTRIBUTES) code=1;
        CloseHandle(child.hThread);CloseHandle(child.hProcess);
        return (int)code; /* Deliberately leave the successfully handed-off child running. */
    }
    phase="nested-identity";
    if(normal) {
        deadline=GetTickCount64()+5000;
        while(GetFileAttributesW(markers[3])==INVALID_FILE_ATTRIBUTES && GetTickCount64()<deadline) Sleep(20);
        do {
            file=CreateFileW(markers[3],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
            if(file!=INVALID_HANDLE_VALUE || GetLastError()!=ERROR_SHARING_VIOLATION) break;
            Sleep(20); /* Wait for the writer to close the complete PID record. */
        } while(GetTickCount64()<deadline);
        if(file==INVALID_HANDLE_VALUE) goto cleanup;
        if(!ReadFile(file,&pid,sizeof(pid),&bytes,NULL) || bytes!=sizeof(pid)) {CloseHandle(file);goto cleanup;}
        CloseHandle(file);
        nested=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
        if(!nested || WaitForSingleObject(nested,0)!=WAIT_TIMEOUT) goto cleanup;
    }
    worker=find_child(normal ? pid : child.dwProcessId,L"ntvdm.exe");
    frontend=find_child(child.dwProcessId,L"ntkvm.exe");
    if(!worker || !frontend) goto cleanup;
    if(peer_frontend && GetProcessId(peer_frontend)==GetProcessId(frontend))goto cleanup;
    if(GetEnvironmentVariableA("MVDM_LIFETIME_WINDOW",NULL,0)) {
        phase="actual-window-before-fault";
        if(!select_test_window(frontend))goto cleanup;
        window_verified=TRUE;
    }
    if(normal){
        DWORD members[64];
        phase="worker-native-console-isolation";
        file=CreateFileW(markers[5],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
        if(file==INVALID_HANDLE_VALUE)goto cleanup;
        if(!ReadFile(file,members,sizeof(members),&bytes,NULL)){CloseHandle(file);goto cleanup;}
        CloseHandle(file);
        for(i=0;i<bytes/sizeof(DWORD);++i)if(members[i]==GetProcessId(worker)){
            fprintf(stderr,"worker=%lu is attached to native hidden Console\n",members[i]);goto cleanup;
        }
    }
    phase="root-close";
    if(normal) {
        file=CreateFileW(markers[4],GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,0,NULL);
        if(file==INVALID_HANDLE_VALUE) goto cleanup;
        CloseHandle(file);
    } else if(!TerminateProcess(launcher_loss ? child.hProcess : worker_loss ? worker : frontend,91)) goto cleanup;
    code=1;
    if(normal || launcher_loss){
        DWORD result;
        if(WaitForSingleObject(child.hProcess,5000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(child.hProcess,&result) || result!=(normal ? 37u : 91u))goto cleanup;
        phase="guest-survives-launcher";
        if(WaitForSingleObject(worker,250)!=WAIT_TIMEOUT ||
            WaitForSingleObject(frontend,0)!=WAIT_TIMEOUT)goto cleanup;
        file=CreateFileW(markers[1],GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,0,NULL);
        if(file==INVALID_HANDLE_VALUE)goto cleanup;
        CloseHandle(file);
        phase="guest-file-completion";
        deadline=GetTickCount64()+8000;
        while(GetFileAttributesW(markers[2])==INVALID_FILE_ATTRIBUTES && GetTickCount64()<deadline)Sleep(20);
        if(GetFileAttributesW(markers[2])==INVALID_FILE_ATTRIBUTES)goto cleanup;
        phase="nested-dos-result";
        if(normal && (WaitForSingleObject(nested,5000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(nested,&result) || result!=7))goto cleanup;
        phase="frontend-resource-lifetime";
        if(WaitForSingleObject(frontend,8000)!=WAIT_OBJECT_0)goto cleanup;
        verdict=normal ? "PASS direct native result 37; orphan nested DOS survives and returns 7; empty frontend retires" :
            "PASS killed launcher preserves frontend and guest file work; frontend retires after last task";
    }else{
        DWORD result;
        phase="frontend-closes-worker";
        if(WaitForSingleObject(worker,8000)!=WAIT_OBJECT_0 ||
            WaitForSingleObject(child.hProcess,5000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(child.hProcess,&result) || result!=ERROR_PROCESS_ABORTED)goto cleanup;
        if(GetFileAttributesW(markers[2])!=INVALID_FILE_ATTRIBUTES)goto cleanup;
        if(worker_loss && WaitForSingleObject(frontend,8000)!=WAIT_OBJECT_0)goto cleanup;
        verdict=worker_loss ? "PASS worker failure completes direct task with 1067 and releases frontend" :
            "PASS independent frontend death closes associated worker without guest Console I/O; task fails 1067";
    }
    phase="worker-retirement-before-test-cleanup";
    /* The completed task result is checked before frontend retirement. Its
     * subsequent Console close ends the associated DOS worker, not vice versa. */
    if(WaitForSingleObject(worker,8000)!=WAIT_OBJECT_0)goto cleanup;
    if(isolated) {
        DWORD peer_code;
        phase="unrelated-session-survival";
        if(WaitForSingleObject(bystander.hProcess,0)!=WAIT_TIMEOUT ||
            WaitForSingleObject(peer_frontend,0)!=WAIT_TIMEOUT || !SetEvent(peer_go) ||
            WaitForSingleObject(bystander.hProcess,8000)!=WAIT_OBJECT_0 ||
            !GetExitCodeProcess(bystander.hProcess,&peer_code) || peer_code!=53 ||
            WaitForSingleObject(peer_frontend,8000)!=WAIT_OBJECT_0)goto cleanup;
    }
    /* All lifecycle assertions are complete. Restore only this test observer's
     * diagnostic output: killing a frontend can leave its former raw Console
     * mode behind. This is not evidence of product Console-mode restoration. */
    if(have_report_mode) {
        COORD origin={0,0};
        if(!SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),report_mode) ||
           !SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),origin))goto cleanup;
    }
    puts(verdict);
    if(isolated)puts("PASS distinct unrelated character frontend and native target survive tested fault, return 53 and retire when empty");
    if(window_verified)
        puts("PASS tested frontend has actual visible Window before lifecycle transition");
    code=0;
cleanup:
    if(code) {
        DWORD child_code=0;
        GetExitCodeProcess(child.hProcess,&child_code);
        fprintf(stderr,"FAIL phase=%s spawn=%d normal=%d child=%lu error=%lu\n",
            phase,spawn,normal,child_code,GetLastError());
        {
            HANDLE observed[]={nested,worker,frontend,peer_frontend};
            const char *names[]={"nested","worker","frontend","peer-frontend"};
            for(i=0;i<sizeof(observed)/sizeof(observed[0]);++i)if(observed[i]) {
                DWORD status=0;
                if(GetExitCodeProcess(observed[i],&status))
                    fprintf(stderr,"fault-observation %s pid=%lu exit=%lu\n",names[i],GetProcessId(observed[i]),status);
                EnumWindows(fault_window,(LPARAM)GetProcessId(observed[i]));
            }
        }
    }
    if(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT) {
        TerminateProcess(child.hProcess,99);WaitForSingleObject(child.hProcess,5000);
    }
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    if(nested) CloseHandle(nested);
    if(worker){if(WaitForSingleObject(worker,0)==WAIT_TIMEOUT)TerminateProcess(worker,99);CloseHandle(worker);}
    if(frontend){if(WaitForSingleObject(frontend,0)==WAIT_TIMEOUT)TerminateProcess(frontend,99);CloseHandle(frontend);}
    if(peer_go)SetEvent(peer_go);
    if(bystander.hProcess){WaitForSingleObject(bystander.hProcess,5000);CloseHandle(bystander.hThread);CloseHandle(bystander.hProcess);}
    if(peer_frontend){if(WaitForSingleObject(peer_frontend,0)==WAIT_TIMEOUT)TerminateProcess(peer_frontend,99);CloseHandle(peer_frontend);}
    if(peer_ready)CloseHandle(peer_ready);if(peer_go)CloseHandle(peer_go);
    /* Controller retains markers until external test-owned worker cleanup,
     * so a late guest cannot be accidentally released by the next test. */
    return (int)code;
}

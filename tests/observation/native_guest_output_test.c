/* Authored native parent: write through real ConPTY while original COMMAND
 * is executing a DOS batch. File gates avoid synthetic frontend callbacks. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

static BOOL CALLBACK observe_window(HWND window,LPARAM context)
{
    FILE *trace=(FILE *)context;WCHAR kind[80],text[512]={0};DWORD pid;DWORD_PTR copied;
    GetWindowThreadProcessId(window,&pid);GetClassNameW(window,kind,80);
    SendMessageTimeoutW(window,WM_GETTEXT,512,(LPARAM)text,SMTO_ABORTIFHUNG,100,&copied);
    fprintf(trace,"window pid=%lu class=%ls visible=%d text=%ls\n",pid,kind,IsWindowVisible(window),text);
    if(!lstrcmpW(kind,L"#32770"))EnumChildWindows(window,observe_window,context);
    return TRUE;
}

static BOOL output(PCWSTR text)
{
    DWORD count,length=(DWORD)lstrlenW(text);
    return WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE),text,length,&count,NULL) && count==length;
}
int main(int argc,char **argv)
{
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION child={0};
    WCHAR command[1024];
    HANDLE file;DWORD result=1;ULONGLONG deadline;const char *phase="guest-ready";
    FILE *trace=NULL;WCHAR directory[MAX_PATH];
    if(argc!=2 || fopen_s(&trace,argv[1],"wx"))return 79;
    GetCurrentDirectoryW(MAX_PATH,directory);
    fprintf(trace,"cwd=%ls\n",directory);fflush(trace);
    swprintf_s(command,1024,L"run16.exe \"%lsCOMMAND.COM\" /c %lstests\\CG.BAT",directory,directory);
    if(GetFileAttributesW(L"tests\\CGREADY")!=INVALID_FILE_ATTRIBUTES ||
       GetFileAttributesW(L"tests\\CGDONE")!=INVALID_FILE_ATTRIBUTES)return 80;
    if(!output(L"NATIVE-BASE\r\n"))return 81;
    if(!CreateProcessW(L"run16.exe",command,NULL,NULL,TRUE,0,NULL,NULL,&startup,&child))return 82;
    CloseHandle(child.hThread);
    deadline=GetTickCount64()+45000;
    while(GetFileAttributesW(L"tests\\CGREADY")==INVALID_FILE_ATTRIBUTES &&
          GetTickCount64()<deadline && WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT)Sleep(10);
    if(GetFileAttributesW(L"tests\\CGREADY")==INVALID_FILE_ATTRIBUTES ||
       WaitForSingleObject(child.hProcess,0)!=WAIT_TIMEOUT)goto done;
    /* COMMAND has printed DOS-INTERVAL and is waiting on CGDONE. Native output
     * therefore arrives while the actual DOS task is still active. */
    phase="native-output";
    if(!output(L"NATIVE-DURING-DOS\r\n"))goto done;
    file=CreateFileW(L"tests\\CGDONE",GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,0,NULL);
    if(file==INVALID_HANDLE_VALUE)goto done;
    CloseHandle(file);
    phase="guest-result";
    if(WaitForSingleObject(child.hProcess,15000)!=WAIT_OBJECT_0 ||
       !GetExitCodeProcess(child.hProcess,&result) || result) {result=1;goto done;}
    if(!output(L"NATIVE-AFTER-DOS\r\nPASS REAL-GUEST-CONCURRENT result=0\r\n"))result=1;
done:
    if(result) {
        DWORD code=0;GetExitCodeProcess(child.hProcess,&code);
        printf("FAIL phase=%s child=%lu error=%lu\n",phase,code,GetLastError());fflush(stdout);
        fprintf(trace,"FAIL phase=%s child=%lu error=%lu\n",phase,code,GetLastError());fflush(trace);
        {
            WCHAR desktop[128];DWORD needed;
            if(GetUserObjectInformationW(GetThreadDesktop(GetCurrentThreadId()),UOI_NAME,desktop,sizeof(desktop),&needed) &&
               !wcsncmp(desktop,L"NTVDMConsoleTest-",17))EnumWindows(observe_window,(LPARAM)trace);
        }
    }
    /* Bounded test housekeeping only; no production tree-termination policy. */
    if(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT) {
        TerminateProcess(child.hProcess,99);WaitForSingleObject(child.hProcess,5000);
    }
    CloseHandle(child.hProcess);fclose(trace);return result ? 83 : 37;
}
